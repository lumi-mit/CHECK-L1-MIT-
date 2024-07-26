#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"

int main(int argc,char *argv[])
{	
	FILE *file,*fic;
	Etudiant *l1;
	int student_number;
	file=open_file("L1.csv","r");
	fic=open_file("file.txt","r+");
	student_number=get_number(file);
	l1=get_student(file,fic,student_number);
	menu(file,fic,l1,student_number);
	free_student(l1,student_number);
	close_file(fic);
	close_file(file);
	return (0);
}