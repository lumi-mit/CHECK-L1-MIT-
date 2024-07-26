#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>
#include <locale.h>

void clean_buffer()
{
	char tampon;
	while((tampon=getchar())!='\n');
}

void pause()
{
	clean_buffer();
	getchar();
}

int get_number(FILE *file)
{
	int number=0;
	char letter;
	rewind(file);
	while((letter=fgetc(file))!=EOF)
	{
		if(letter=='\n')
		{
			number++;
		}
	}
	return (number);
}

FILE *open_file(char *filename,char *mode)
{
	FILE *fic;
	fic=fopen(filename,mode);;
	if(fic==NULL)
	{
		printf("Failure opening %s \n",filename);
	}
	return (fic);
}

int name_count(FILE *file)
{
	int number=0;
	char letter;
	while((letter=fgetc(file))!=';')
	{
		number++;
	}
	fseek(file,-number,SEEK_CUR);
	return (number+1);
}

int firstname_count(FILE *file)
{

	int number=0;
	char letter;
	while((letter=fgetc(file))!='\n')
	{
		number++;
	}
	fseek(file,-number,SEEK_CUR);
	return (number+1);
}

Etudiant *get_student(FILE *file,FILE *fic,int student_number)
{
	Etudiant *student;
	int i,nbr_name;
	student=(Etudiant*)malloc(student_number*sizeof(Etudiant));
	rewind(file);
	for(i=0;i<student_number;i++)
	{
		nbr_name=name_count(file);
		student[i].nom=(char*)malloc	(nbr_name*sizeof(char));
		fscanf(file,"%[^;]",student[i].nom);
		nbr_name=firstname_count(file);
		student[i].prenom=(char*)malloc(nbr_name*sizeof(char));
		fscanf(file,"%[^\n]",student[i].prenom);
		student[i].check=3;
    }
	return (student);
}

int already_check(FILE *fic,int number,char *Date_d)
{
	int line_date;
	char texte[80];
	if(!date_repeat(fic,&line_date,Date_d))
	{
		move_cur(fic,number+line_date);
		fgets(texte,80,fic);	
		if(strstr(texte,"OUI")||strstr(texte,"NON")||strstr(texte,"PERSO")||strstr(texte,"ABS"))
		{
			mvprintw(5,0,"	Student already check \n        Press [Enter]: \n");
			getch();
			return (1);
		}
	}
	return (0);
}

void print_check()
{
	mvprintw(7,0,"+===========================================================================================+\n"
				"|   Choose  with border [number] to check:           |                                      |\n"
				"|                    NON                   [0        |      *Use Key: UP and DOWN           |\n"	
				"|                    OUI                   [1        |      *Press [Enter] to valid         |\n"
				"|                    PERSONNEL             [2        |                                      |\n"
				"|                                                    |                                      |\n"
				"+===========================================================================================+\n");
}

int machine(Etudiant *l1,FILE *fic,int *start,int student_number,char *Date_d)
{ 
	int number=0,choix,i,k,j=0;
	int letter;
	char choice[50],string[30];
	initscr();
	nodelay(stdscr,false);
	erase();
	cbreak();
	keypad(stdscr,true);
	printw("+===========================================================================================+");
	attron(A_BOLD);
	printw("\n|[%s]                            STUDENT CHECKING                             |\n",Date_d);
	attroff(A_BOLD);
	printw("+===========================================================================================+");
	printw("\n--->Enter student's number or name [exit<0>]: ");
	print_check();
	refresh();
	label:
	mvgetstr(3,45,choice);
	if(atoi(choice)== 0 &&strlen(choice)<3&&choice[0]<65)
	{
		*start=0;
	}
	else if(strlen(choice)<3&&choice[0]<65)
	{
		number=atoi(choice);
		if(number > student_number)
		{
			mvprintw(3,0,"-->Student number max is : %d,Try again: ",student_number);
			refresh();
			goto label;
		}
		else
		{
			twist:
			if(number==0)
			{
				mvprintw(3,0,"--->Student not found , Retry:                    ");
				goto label;
			}
			mvprintw(4,0,"=>| %s %s |                           	",l1[number-1].nom,l1[number-1].prenom);
		    if(!already_check(fic,number-1,Date_d)&&number!=0)
			{
				number--;
				choix=0;
				nodelay(stdscr,true);
				while((letter=getch())!='\n')
				{
					if(letter==KEY_UP)
					{
						choix--;
							if(choix<0)
								choix++;
					}
					else if(letter==KEY_DOWN)
					{
						choix++;
							if(choix>2)
								choix--;
					}
					print_check();				
					attron(A_BLINK|A_BOLD);
					mvprintw(9+choix,18,"--[");
					mvprintw(9+choix,45,"]--");
					attroff(A_BLINK|A_BOLD);
					refresh();
				}
				nodelay(stdscr,false);
				switch(choix)
				{
					case NON:
						l1[number].check=NON;
						break;
					case OUI:
						l1[number].check=OUI;
						break;
					case PERSONNEL:
						l1[number].check=PERSONNEL;
						break;
					default:
						break;
				}
				mvprintw(9,60,"Check successful ...    ");
				attron(A_BLINK);
				mvprintw(10,60,"Press [Enter]:         ");
				attroff(A_BLINK);
				refresh();
				getch();
				endwin();
				return (number);
			}
		}
	}
	else
	{
		for(i=0;i<student_number;i++)
		{
			while(j<student_number)
			{
				lower(l1[j].nom);
				j++;
			}
			if(strstr(lower(l1[i].nom),lower(choice))||strstr(lower(l1[i].prenom),lower(choice)))
			{
				mvprintw(4,0,"=>| %s %s |                           ",l1[i].nom,l1[i].prenom);
				refresh();
				again:
				letter=getch();
				if(letter==KEY_DOWN)
				{
					continue;
				}
				else if(letter==KEY_UP)
				{
					if(i>0)
					{
						i--;
						while(!strstr(lower(l1[i].nom),lower(choice))&&!strstr(lower(l1[i].prenom),lower(choice)))
						{
							i--;
						}
						i--;
					}
				}
				else if(letter=='\n')
				{
					number=i+1;
					goto twist;
					break;
				}
				else
				{
					printw("Invalid choice,Retry [y/n]:");
					refresh();
					goto again;
				}
				refresh();
			}
		}
		mvprintw(5,0," Failure: Student not in database < %s >!!",choice);
		mvprintw(6,0,"          Press any Key: ");
		nodelay(stdscr,false);
		cbreak();
		getch();
		nocbreak();
	}
	endwin();
	return (-1);
}

int date_repeat(FILE *fic,int *line_date,char *Date_d)
{
	char test[DIM];
	rewind(fic);
	int line=0;
	while(!feof(fic))
	{
		*line_date=line;
		fgets(test,DIM,fic);
		if(strstr(test,Date_d))
		{
				return (0);
		}
		line++;
	}
	return (1);
}

void move_cur(FILE *fic,int pos)
{
	int i=-3;
	char line[DIM];
	rewind(fic);
	while(i!=pos)
	{
		fgets(line,DIM,fic);
		i++;
	}
}

void print_row(FILE* fic,Etudiant *l1,int i,char *Date_h)
{
	switch(l1[i].check)
	{
		case NON:
			fprintf(fic,"%02d |%-30s |%-30s\t|  NON  | %s  |\n",i+1,l1[i].nom,l1[i].prenom,Date_h);
			break;
		case OUI:
			fprintf(fic,"%02d |%-30s |%-30s\t|  OUI  | %s  |\n",i+1,l1[i].nom,l1[i].prenom,Date_h);
			break;
		case PERSONNEL:
			fprintf(fic,"%02d |%-30s |%-30s\t| PERSO | %s  |\n",i+1,l1[i].nom,l1[i].prenom,Date_h);
			break;
		default:
			fprintf(fic,"%02d |%-30s |%-30s\t|       |           |\n",i+1,l1[i].nom,l1[i].prenom);
			break;
	}
}

void print_file(FILE *fic,Etudiant *l1,int student_number,int numero,char *Date_h,char *Date_d)
{
	int i,line_date=0;
	if(date_repeat(fic,&line_date,Date_d))
	{
		fseek(fic,0,SEEK_END);
		fprintf(fic,"\n\n\t\t\t\t====> %s <====\n",Date_d);
		fprintf(fic,"N° |              NOM              |               PRENOM               |MACHINE|   HEURE   |\n");
		fprintf(fic,"---|-------------------------------|------------------------------------|-------|-----------|\n");
		for(i=0;i<student_number;i++)
		{
			print_row(fic,l1,i,Date_h);
		}	
	}
	else
	{
		if(numero>-1)
		{
			move_cur(fic,numero+line_date);
			print_row(fic,l1,numero,Date_h);
		}
	}
}

void search_student(FILE *fic,int *start,Etudiant *l1,int student_number,char *Date_d)  
{
	char name[50],test[DIM],iname[150];
	int letter;
	int i=0,j=0,k;
	int oui=0,non=0,perso=0,abs=0;
	initscr();
	nodelay(stdscr,false);
	erase();
	cbreak();
	keypad(stdscr,true);
	rewind(fic);
	erase();
	printw(	"+===========================================================================================+");
	attron(A_BOLD);
	printw(	"\n|[%s]                            SEARCH CHECKING                              |\n",Date_d);
	attroff(A_BOLD);
	printw(	"+===========================================================================================+");
	printw("\n--->Enter student's number or name [exit<0>]: ");
	mvprintw(6,0,"+===========================================================================================+\n");
	mvprintw(3,65,"|  *Use: Key up and down   | ");
	mvprintw(4,65,"|        To scroll student |");
	mvprintw(5,65,"|  *Press [Enter] to valid |");
	mvprintw(20,0,"+===========================================================================================+");
	mvgetstr(3,48,name);
	refresh();	
	if(strcmp(name,"0")==0)
	{
		*start=0;
	}
	else
	{
		while(j<student_number)
		{
			lower(l1[j].nom);
			j++;
		}
		for(j=0;j<student_number;j++)
		{
			if( j+1==atoi(name) || strstr(lower(l1[j].nom),lower(name)) ||  strstr(lower(l1[j].prenom),lower(name)))
			{	
				i++;
				k=j;	
				nodelay(stdscr,false);
				mvprintw(4,1," %02d | %s %s                       ",j+1,l1[j].nom,l1[j].prenom);
				again:
				letter=getch();
				if(letter==KEY_DOWN)
				{
					continue;
				}
				else if(letter==KEY_UP)
				{
					if(j>0)
					{
						j--;
						while(!strstr(lower(l1[j].nom),lower(name)) &&  !strstr(lower(l1[j].prenom),lower(name)))
						{
							j--;
						}
						j--;
					}
				}
				else if(letter=='\n')
				{
					i=1;
					break;
				}
				else
				{
					printw("Invalid choice,Retry [y/n]:");
					refresh();
					goto again;
				}	
			}
			refresh();
		}
		if(i==1)
		{
			mvprintw(7,2,"\n\tName         : %s\n\tFirstname    : %s \n\tNumero       : %d \nSTAT:\n"
					"                                      +------------------+-------+-----------+\n"
					"                                      |       DATE       |MACHINE|    HOUR   |\n"
					"                                      +------------------+-------+-----------+\n",l1[k].nom,l1[k].prenom,k+1);	
			rewind(fic);
			while(!feof(fic))
			{ 
				fgets(test,DIM,fic);
				if(strstr(test,"====>"))
				{
					sscanf(test,"				====> %[^<]",iname);
					printw("                                      | %s",iname);
				}
				if(strstr(test,l1[k].prenom))
				{
					if(strstr(test,"|  OUI  |"))
					{
						oui++;
					}
					else if(strstr(test,"|  NON  |"))
					{
						non++;
					}
					else if(strstr(test,"| PERSO |"))
					{
						perso++;
					}
					else if(strstr(test,"|       |"))
					{
						abs++;
					}
					fseek(fic,-22,SEEK_CUR);
					while((letter=fgetc(fic))!='\n')
					{
						printw("%c",letter);		
					}
					printw("\n");
				}
			}
			printw("                                      +------------------+-------+-----------+\n");
			mvprintw(12,1,	"+-----------------------------+");
			mvprintw(13,1,	"|    CHECK-PC   |    Number   |");
			mvprintw(14,1,	"+-----------------------------+");
			mvprintw(15,1,	"|      OUI      |     %02d      |",oui);
			mvprintw(16,1,	"|      NON      |     %02d      |",non);
			mvprintw(17,1,	"|      PERSO    |     %02d      |",perso);
			mvprintw(18,1,	"|      ABSENT   |     %02d      |",abs);
			mvprintw(19,1,	"+-----------------------------+");
			refresh();
		}
		if(i==0)
		{
			printw("Failed to search student  : %s \n",name);
		}
	}
	if(strcmp(name,"0")!=0)
	{
		mvprintw(5,0,"Press [Enter] to continue: ");
		nodelay(stdscr,false);
		getch();
	}	
	endwin();
}

void search_date(FILE *fic,int *start,Etudiant *l1,char *Date_d)
{
	char test[DIM];
	int i=0,j,a=0,date=1,voir,letter;
	initscr();
	curs_set(false);
	keypad(stdscr,true);
	nodelay(stdscr,true);
	cbreak();
	erase();
	printw(	"+===========================================================================================+");
	attron(A_BOLD);
	printw(	"\n|[%s]                            SEARCH DATE                                  |\n",Date_d);
	attroff(A_BOLD);
	printw(	"+===========================================================================================+");
	printw(	"\n|                                                                                           |\n");
	while((letter=getch())!='\n')
	{
		if(letter==KEY_UP)
		{
			date--;
				if(date<1)
					date++;
		}
		else if(letter==KEY_DOWN)
		{
			date++;
				if(date>i+1)
					date--;
		}
		move(4,0);
		clrtobot();
		i=0;
		rewind(fic);
		while(!feof(fic))
		{
			fgets(test,DIM,fic);
			if(strstr(test,"====>"))
			{
				mvprintw(i+5,0,"|                                        [exit]                                             |"
				"\n+===========================================================================================+\n");
				i++;
				j=0;
				a=0;
				mvprintw(i+3,0,"|                       ");
				while(test[j]!='<')
				{
					j++;
					if(a==1&&test[j]!='<')
					{
						printw("%c",test[j]);
					}
					if(test[j]=='>')
					{
						a=1;	
					}
				}
				printw("]-----------------[%02d                             |\n",i);
			}
		}
		attron(A_BLINK);
		mvprintw(date+3,22,"--[");
		mvprintw(date+3,63,"]--");
		attroff(A_BLINK);
		refresh();
	}
	rewind(fic);
	if(date==i+1)
	{
		*start=0;
	}
	else
	{
		voir=1;
		while(voir)
		{
			display_voir(fic,date,&voir,l1);
		}
	}
}

void update_HTML()
{
	FILE *file,*fic;
	char machine[DIM];
	char line[DIM];
	int nb=0;
	int j=0;
	int i;
	char *pj=NULL;
	file=open_file("file.html","w");
	fic=open_file("file.csv","r");
	fprintf(file,	"<HTML>\n"
    				"\t<HEAD>\n"
        			"\t\t<TITLE>\n"
         			"\t\t\tCHECK MACHINE\n"
        			"\t\t</TITLE>\n"
   					"\t</HEAD>\n"
					"\t<BODY BACKGROUND=\"ressource/background.JPG\">\n"
					"\t\t<FONT SIZE=\"6\"  COLOR=\"red\"><CENTER> ETUDIANT MIT LIC</FONT><FONT SIZE=\"6\"  COLOR=\"#FFFFFF\">ENCE 1 </CENTER></FONT> <BR>");
	fprintf(file,"<CENTER><TABLE  BGCOLOR=\"#E0FFFF\" BORDER=\"1\">");
	while(!feof(fic))
	{
		fgets(line, DIM, fic);
		nb=strlen(line);	
		fprintf(file, "\n\t\t\t<TR>\n\t\t\t\t<TD>");
		j=0;
		for(i=0; i<(nb-1); i++)
		{
			if(line[i] == ';' && (i<60))
			{
				if(strstr(line," // "))
					fprintf(file, "</TD>\n\t\t\t\t<TD BGCOLOR=\"#FFFF00\">");
				else
					fprintf(file, "</TD>\n\t\t\t\t<TD>");
			}
			else if(line[i] != ';'&& (i<68))
			{
				fprintf(file, "%c", line[i]);
			}
			else
			{
				if(strstr(line, " OUI "))
				{
					fprintf(file, "</TD>\n <TD BGCOLOR=\"#00FF00\"> OUI </TD>");
				}
				else if(strstr(line, " NON "))
				{
					fprintf(file, "</TD>\n <TD BGCOLOR=\"#FF0000\"> NON</TD>");
				}
				else if(strstr(line, " PERSO "))
				{
					fprintf(file, "</TD>\n <TD BGCOLOR=\"#0000FF\"> PERSO</TD>");
				}
				else if(strstr(line, "MACHINE"))
				{
					fprintf(file, "</TD>\n <TD> MACHINE</TD>");
				}
				else if(strstr(line, "      "))
				{
					fprintf(file, "</TD>\n <TD BGCOLOR=\"#000000\">  ABSENT </TD>");
				}
				i+=25;
			}
		}
		fprintf(file, "\n\t\t\t</TR>");
	}
	fprintf(file,"</TABLE></CENTER>");
	fprintf(file,	"\t</BODY>\n"
					"</HTML>");
	mvprintw(15	,0,"Update success....\nPress [Enter] to continue: \n");
	close_file(file);
	close_file(fic);
}

void update_CSV(FILE *fic)
{
	FILE *file;
	char letter;
	file=open_file("file.csv","w");
	rewind(fic);
	while((letter=fgetc(fic))!=EOF)
	{
		if(letter=='|'||letter=='<'||letter=='>')
		{
			fprintf(file,";");
		}
		else if(letter=='-'||letter=='=')
		{
			continue;
		}
		else{
			fprintf(file,"%c",letter);
		}
	}
	close_file(file);
}

void menu(FILE *file,FILE *fic,Etudiant *l1,int student_number)
{
	int choix,open=OUI,numero,start;
	char Date_h[30],Date_d[30];
	time_t rawtime;
	int  letter;
	setlocale(LC_ALL,"");
	initscr();
	curs_set(false);
	keypad(stdscr,true);
	time(&rawtime);
	tm *local=localtime(&rawtime);
	sprintf(Date_h,"%02d:%02d:%02d",local->tm_hour,local->tm_min,local->tm_sec);
	sprintf(Date_d,"%02d // %02d // %2d",local->tm_mday,local->tm_mon+1,local->tm_year+1900);
	rewind(fic);
	while(open)
	{
		start=1;
		nodelay(stdscr,true);
		keypad(stdscr,true);
		choix=1;
		while((letter=getch())!='\n')
		{
			if(letter==KEY_UP)
			{
				choix--;
				if(choix<1)
					choix++;
			}
			else if(letter==KEY_DOWN)
			{
				choix++;
				if(choix>5)
					choix--;
			}
			erase();
			printw(	"+===========================================================================================+");
			attron(A_BOLD);
			printw(	"\n|[%s]                            MAIN MENU                                    |\n",Date_d);
			attroff(A_BOLD);
			printw(	"+===========================================================================================+\n"
					"|                                                                                           |\n"
					"|          	Checking a student-----------------------------------------[1               |\n"
					"|               Search for student data------------------------------------[2               |\n"
					"|               Search Date------------------------------------------------[3               |\n"
					"|               Update HTML file-------------------------------------------[4               |\n"          
					"|               Quit-------------------------------------------------------[5               |\n"
					"|                                                                                           |\n"
					"+===========================================================================================+");
			attron(A_BLINK|A_BOLD);
			mvprintw(choix+3,12,"--[");
			mvprintw(choix+3,77,"]--");
			attroff(A_BLINK|A_BOLD);
			refresh();
		}
		switch(choix)
		{
			case 1:
				while(start)
				{	
					numero=machine(l1,fic,&start,student_number,Date_d);
					print_file(fic,l1,student_number,numero,Date_h,Date_d);
				}
				break;
			case 2:
				endwin();
				while(start)
				{
					search_student(fic,&start,l1,student_number,Date_d);
				}
				break;
			case 3:
				endwin();
				while(start)
				{
					search_date(fic,&start,l1,Date_d);
				}
				break;
			case 4:
				update_CSV(fic);
				update_HTML();
				nodelay(stdscr,false);
				getch();
				break;
			case 5:
				endwin();
				open=NON;
				break;
			default: 
				break;
		}
	}
	system("clear");
}

void free_student(Etudiant *l1,int student_number)
{
	int i;
	for(i=0;i<student_number;i++)
	{
		free(l1[i].nom);
		free (l1[i].prenom);
	}
}

void close_file(FILE *file)
{
	if(file!=NULL)
	{
		fclose(file);	
	}
}

char *lower(char *sentence)
{
	char *phrase;
	int i;
	phrase=(char*)malloc(strlen(sentence)*sizeof(char));
	for(i=0;i<strlen(sentence);i++)
	{	if(sentence[i]<97)
		{
			phrase[i]=sentence[i]+32;
		}
		else{
			phrase[i]=sentence[i];
		}
	}
	return (phrase);
}

void display_voir(FILE* fic,int date,int *voir,Etudiant *l1)
{
	int search_datee=1,letter ;
	while((letter=getch())!='\n')
	{
		move(8,0);
		clrtobot();
		if(letter==KEY_UP)
		{
			search_datee--;
				if(search_datee<1)
					search_datee++;
		}
		else if(letter==KEY_DOWN)
		{
			search_datee++;
			if(search_datee>6)
				search_datee--;
		}
		printw(	"+===========================================================================================+\n");
		printw(	"|                                                                                           |\n"
				"|             Display check \"OUI\"     :-----------------------[1                            |\n"
				"|             Display check \"NON\"     :-----------------------[2                            |\n"
				"|             Display check \"ABSENT\"  :-----------------------[3                            |\n"
				"|             Display check \"PERSO\"   :-----------------------[4                            |\n"
				"|             Display ALL List        :-----------------------[5                            |\n"
				"|%d                                         [EXIT]                                           |\n",search_datee);
		printw("+===========================================================================================+\n");
		attron(A_BLINK);
		mvprintw(9+search_datee,10,"--[");
		mvprintw(9+search_datee,64,"]--");
		attroff(A_BLINK);
	}
	mvprintw(18,0,	"+===========================================================================================+");
	
	if(search_datee==6)
	{
		*voir=0;
		move(8,0);
		clrtobot();
	}
	else
	{	
		endwin();
		switch(search_datee)
		{
			case 1:	
				check_value(l1,fic,"|  OUI  |",date);
				break;

			case VNON:
				check_value(l1,fic,"|  NON  |",date);
				break;

			case VABSENT:
				check_value(l1,fic,"|       |",date);
				break;
			case VPERSONNEL:
				check_value(l1,fic,"| PERSO |",date);
				break;
			case LIST:

				chose_date(fic,date);
				break;
			default:
				break;
		}
		if(date!=0)
		{
			printf("Press a touch to continue: ");
			pause();
		}
		initscr();
		erase();
	}
}

void check_value(Etudiant *l1,FILE *fic,char *value,int date)
{
	int i=0,j=0,number=0;
	char test[100];
	char letter;
	rewind(fic);
	system("clear");
	printf("     +---------------------------------------------------------------------------------+\n"
		"     | N |                NAME            |            FIRSTNAME           |  HOUR     |\n"
			"     +---------------------------------------------------------------------------------+\n");
	while(!feof(fic))
	{
		fgets(test,DIM,fic);
		if(strstr(test,"====>"))
		{
			i++;
			if(i==date)
			{
				while(j<79	)
				{
					j++;
					fgets(test,DIM,fic);
					if(strstr(test,value))
					{
						printf("     |%02d | %-30s | %-30s",j-2,l1[j-3].nom,l1[j-3].prenom);
						fseek(fic,-15,SEEK_CUR);
						while((letter=fgetc(fic))!='\n')
						{
							printf("%c",letter);
						}
						printf("\n");
						number++;		
					}
				}
			}
		}
	}
	printf("     +---------------------------------------------------------------------------------+\n");
	printf("\tStudent Number : %d / 77 \n\tPercentage     : %g%% \n",number,(float)number*(float)100/(float)77);
	printf(	"+===========================================================================================+\n");
}

void chose_date(FILE *fic,int date)
{
	char test[100];
	rewind(fic);
	int i=0,j;
	int oui=0,non=0,perso=0,abs=0;
	system("clear");
	while(!feof(fic))
	{
		fgets(test,DIM,fic);
		if(strstr(test,"====>"))
		{
			i++;
			if(i==date)
			{
				j=0;
				printf("%s",test);
				printf("--------------------------------------------------------------------------------------------|\n");
				while(j<79)
				{
					j++;
					fgets(test,DIM,fic);
					printf("%s",test);
					if(strstr(test,"|  OUI  |"))
					{
						oui++;
					}
					else if(strstr(test,"|  NON  |"))
					{
						non++;
					}
					else if(strstr(test,"| PERSO |"))
					{
						perso++;
					}
					else if(strstr(test,"|       |"))
					{
						abs++;
					}
				}
				printf("--------------------------------------------------------------------------------------------|\n");
			}
		}
	}
	printf("STAT :\n");
	printf(	"\n\t\t\t+------------------------------------------------+\n"
			"\t\t\t|    CHECK-PC   |   Student   |    Percentage    |\n"
			"\t\t\t+------------------------------------------------+\n");
	printf(	"\t\t\t|      OUI      |     %02d      |       %02d%%        |\n",oui,oui*100/77);
	printf(	"\t\t\t|      NON      |     %02d      |       %02d%%        |\n",non,non*100/77);
	printf(	"\t\t\t|      PERSO    |     %02d      |       %02d%%        |\n",perso,perso*100/77);
	printf(	"\t\t\t|      ABSENT   |     %02d      |       %02d%%        |\n",abs,abs*100/77);
	printf(	"\t\t\t+------------------------------------------------+\n");
	printf(	"+===========================================================================================+\n");
}