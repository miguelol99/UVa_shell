#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>

int main (int argc, char **argv) {

	//COMPRUEBA LOS ARGUMENTOS DEL MAIN
	if (argc > 2) {

		fprintf(stderr, "An error has occurred\n");
		return 1;
	}

	//DECALARA VARIABLES
	FILE *stream	= stdin;
	char *line		= NULL;
	char *word		= NULL;
	char **args		= NULL;
	int mode		= 0;
	size_t n		= 0;
	int index;
	pid_t pid;

	//COMPRUEBA SI ESTÁ EN MODO BATCH
	if (argc == 2) {

		if ((stream = fopen(argv[1], "r")) == NULL) {
			fprintf(stderr, "An error has occurred\n");
			return 1;
		}

		mode = 1;
	}

	//INCIA EL BUCLE
	while(1) {

		if (mode == 0)
			printf("UVash> ");

		//LEE UNA LINEA DEL STREAM Y AÑADE \0 AL FINAL
		if (getline(&line, &n, stream) == -1)
			exit(0);

		line[strlen(line) - 1] = '\0';

		//INICIALIZA VARIABLES
		args = (char**) malloc(sizeof(char*) * strlen(line));
		index = 0;

		//SEPARA LA LINEA EN SUS ARGUMENTOS
		while ((word = strsep(&line, " ")) != NULL) 

			if (strcmp(word, "") != 0){
				args[index] = word;
				index++;
			}

		//COLOCA UN NULL AL FINAL DEL ARRAY DE STRINGS
		args[index] = NULL;

		//COMPRUEBA QUE NO ESTÉ VACIO
		if (index == 0){
			free(args);
			continue;
		}

		//BUILD-IN COMAND: CD
		if (strcmp(args[0], "cd") == 0){

			if (index != 2)

				fprintf(stderr, "An error has occurred\n");
			
			else {

				if (chdir(args[1]) == -1)

					fprintf(stderr, "An error has occurred\n");
			 }
			
			free(args);
			continue;
		}

		//BUILD-IN COMAND: EXIT
		if (strcmp(args[0], "exit") == 0) {

			if (index != 1) {

				fprintf(stderr, "An error has occurred\n");
				free(args);
				continue;
			}
			
			exit(0);	
		}	
	
		//CREA UN PROCESO HIJO
		if ((pid = fork()) == -1) 
			return 1;

		if (pid == 0) {

			//COMPRUEBA SI HAY REDIRECCION
			for (int i = 0; i < index; i++) 

				if (strcmp(args[i], ">") == 0){

					if ((i != 0) && (i == (index - 2))) {
					
						FILE *fout = fopen(args[index - 1], "w");
							
						dup2(fileno(fout), STDOUT_FILENO);
						dup2(fileno(fout), STDERR_FILENO);

						args[i] = NULL;
					}

					else {
						fprintf(stderr, "An error has occurred\n");
						exit(0);
					}
				}

			//EJECUTA EL COMANDO
			execvp(args[0], args);

			fprintf(stderr, "An error has occurred\n");
			exit(0);
		}

		wait(NULL);
		free(args);
	}







}
