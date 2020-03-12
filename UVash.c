#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>

/**
 * Función que comprueba el formato de una linea y devuelve 0 si es correcto
 * y 1 si no lo es.
 */
int lineformat(char *line) {

	int last_char = strlen(line) - 1;

	if (line[0] == '&')
		return 1;

	for (int i = 0; i < last_char-1; i++)

		if (line[i] == '&' && line[i+1] == '&')
			return 1;		

	return 0;
}

int main(int argc, char **argv) {

	//COMPRUEBA LOS ARGUMENTOS DEL MAIN
	if (argc > 2) {
		fprintf(stderr, "An error has occurred\n");
		exit(1);
	}
	
	//DECALARA VARIABLES
	int mode = 0;
	FILE *stream = stdin;
	char *line = NULL;
	char *comand = NULL;
	char **argument = NULL;
	size_t n = 0;
	int n_arg = 0;
	int child_number = 0;
	pid_t pid;

	//RESERVAMOS MEMORIA DINAMICA
	argument = (char**) malloc(sizeof(char*));

	//COMPRUEBA SI ESTÁ EN MODO BATCH
	if (argc == 2) {
		if ((stream = fopen(argv[1], "r")) == NULL) {
			fprintf(stderr, "An error has occurred\n");
			exit(1);
		}

		mode = 1;
	}

	//INICIO DEL BUCLE CONTINUO
	while(1) {
		
		if (mode == 0)
			printf("UVash> ");

		//LEE UNA LINEA DEL STREAM Y ELIMINA EL '\n'
		if (getline(&line, &n, stream) == -1) exit(0);
		line[strlen(line) - 1] = '\0';

		//COMPRUEBA EL FORMATO PARA EVITAR CASOS NO VALIDOS
		if (lineformat(line)) {
			fprintf(stderr, "An error has occurred\n");
			continue;
		}
			

		//DIVIDE LA LINEA EN COMANDOS
		while((comand = strsep(&line, "&")) != NULL) {

			if (!strcmp(comand, "")) {
				continue;
			}

			//DIVIDE CADA COMANDO EN ARGUMENTOS
			while((argument[n_arg] = strsep(&comand, " ")) != NULL) {

				if (!strcmp(argument[n_arg], "")) {
					continue;
				}

				n_arg++;
				argument = (char**) realloc(argument, sizeof(char*) + sizeof(char*) * n_arg);
			}

			//AÑADE UN NULL AL FINAL DEL ARRAY DE ARGUMENTOS
			argument[n_arg] = NULL;

			//COMPRUEBA QUE EL COMANDO NO STUVIESE VACIO
			if (n_arg == 0)
				break;

			//EJECUTA EL BUILD-IN COMAND
			if (!strcmp(argument[0], "cd")) {
				if (n_arg != 2)
					fprintf(stderr, "An error has occurred\n");

				else
					if(chdir(argument[1]) == -1)
						fprintf(stderr, "An error has occurred\n");

			}

			else if (!strcmp(argument[0], "exit")) {
				if (n_arg != 1)
					fprintf(stderr, "An error has occurred\n");

				else
					exit(0);
			}

			//SI NO ES UN BUILD-IN COMAND
			else{
				
				//CREAMOS UN PROCESO HIJO
				if((pid = fork()) == -1)
					return 1;

				if (pid == 0) {

					//COMPROBAMOS SI HAY REDIRECCION
					for (int i = 0; argument[i] != NULL; i++)

						if (!strcmp(argument[i], ">")){

							if(i != 0 && i == n_arg-2) {

								FILE *fout = fopen(argument[i+1], "w");
								dup2(fileno(fout), STDOUT_FILENO);
								dup2(fileno(fout), STDERR_FILENO);
								argument[i] = NULL;
							}

							else {
								fprintf(stderr, "An error has occurred\n");
								exit(0);
							}
						}
					
					//EJECUTAMOS EL COMANDO
					execvp(argument[0], argument);
					fprintf(stderr, "An error has occurred\n");
					exit(0);
				}

				child_number++;
			}

			n_arg = 0;
		}//del while((comand = ...)

		for (int j = 0; j < child_number; j++)
			waitpid(0, NULL, 0);

		child_number = 0;
	
	}//del while(1)
}
