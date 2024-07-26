#ifndef MENU_H
#define MENU_H

#define DIM 100
typedef enum
{
	NON,OUI,PERSONNEL,ABSENT
}Machine;

typedef enum
{
	CHECK,STUDENT,DATE,CVS,QUIT
}Choix;

typedef enum
{
	VOUI=1,VNON=2,VABSENT=3,VPERSONNEL=4,LIST=5,STATISTIC=6
}Voir;

typedef struct
{
	char *nom;
	char *prenom;
	Machine check;
}Etudiant;

typedef struct tm tm;

void pause();
Etudiant *get_student(FILE *file,FILE *fic,int student_number);
int get_number(FILE *file);
FILE *open_file(char *filename,char *mode);
void menu(FILE *file,FILE *fic,Etudiant *l1,int student_number);
int machine(Etudiant *l1,FILE *fic,int *start,int student_number,char *Date_d);
void search_student(FILE *fic,int *start,Etudiant *l1,int student_number,char *Date_d); 
void search_date(FILE *fic,int *start,Etudiant *l1,char *Date_d);
void update_HTML();
void update_CSV(FILE *fic);

int name_count(FILE *file);
int firstname_count(FILE *file);
void free_student(Etudiant *l1,int student_number);
int date_repeat(FILE *fic,int *line_date,char *Date_d);
void move_cur(FILE *fic,int pos);
void print_file(FILE *fic,Etudiant *l1,int student_number,int numero,char *Date_h,char *Date_d);
void print_row(FILE* fic,Etudiant *l1,int i,char *date_h);
void clean_buffer();
void close_file(FILE *file);
char *lower(char *sentence);
void display_voir(FILE* fic,int date,int *voir,Etudiant *l1);
void chose_date(FILE *fic,int date);
void check_value(Etudiant *l1,FILE *fic,char *value,int date);

#endif
