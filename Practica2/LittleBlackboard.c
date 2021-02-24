#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
	
	int i = 0, maximo = 0;
	char nombre[20];
	char matricula[20];
	char dato[40];
	char opcion[1];
	/*
	for (i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
	}*/
	maximo = atoi(argv[2]);
	//printf("CANTIDAD MAXIMA = %d \n", maximo);
	FILE *file = fopen("dataBase.txt", "w");
	if (file==NULL){
		perror ("Error al abrir dataBase.txt");
		return -1;
	}
	
	for( i = 0; i < maximo; i++){
		//Pedir los datos y escribirlos en un archivo
		printf("Enter the Name: \n");
		scanf("%s", nombre);
		//printf("Nombre Guardado = %s \n", nombre);
		printf("Enter the ID: \n");
		scanf("%s", matricula);
		strcpy(dato, nombre);
		strcat(dato, " ");
		strcat(dato, matricula);
		strcat(dato, "\n");
		fputs(dato, file);
		printf("Do you want to continue? y-n \n");
		scanf("%s", opcion);
		if (*opcion == 'n'){
			i = maximo;
		}
	}
	printf("Students information stored in dataBase.txt \n");
	fclose(file);

	return 0;
}
