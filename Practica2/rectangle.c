/*#include <stdio.h>
int main(char toDo, int base, int altura){
	if (toDo == 'a'){
		int area = 0;
		area  = base * altura;
		printf("Area = ", area, "\n" );
		}
	if (toDo == 'p'){
		int perimetro = 0;
		perimetro  = base + base + altura + altura;
		printf("Perimeter = ", perimetro, "\n" );
	}
	return 0;
}
*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	/*
	int i = 0;
	for (i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
	}
	*/
	if (*argv[1] == 'a'){
		int area = 0;
		area  = atoi(argv[3]) * atoi(argv[5]);
		printf("Area = %d \n", area);
		}
	if (*argv[1] == 'p'){
		int perimetro = 0;
		perimetro  = atoi(argv[3]) + atoi(argv[3]) + atoi(argv[5]) + atoi(argv[5]);
		printf("Perimeter = %d \n", perimetro);
	}

 		return 0;
}
