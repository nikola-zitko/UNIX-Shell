#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>


int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int readHistory(char **args);


#define COLOR_RED "\x1b[0;31m"
#define COLOR_RESET "\x1b[1;37m"
#define COLOR_BLUE "\x1b[0;33m"
#define BACK_COLOR "\x1b[0;47m"
#define BLINKING "\x1b[5m"
#define STOP_BLINKING "\x1b[0m"
#define BOLD "\x1b[1m"
#define RESET "\x1b[0m"

#define MAX_CMD_LEN 128
#define HISTORY_COUNT 30
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line){
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if(!tokens){
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, LSH_TOK_DELIM);
	while(token != NULL){
		tokens[position] = token;
		position++;

		if(position >= bufsize){
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if(!tokens){
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, LSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

int lsh_launch(char **args){

	pid_t pid, wpid;
	int status;

	pid = fork();
	if(pid == 0){
		if(execvp(args[0], args) == -1){
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	}
	else if(pid < 0){
		perror("lsh");
	}
	else{
		do{
			wpid = waitpid(pid, &status, WUNTRACED);
		}while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

char *builtin_str[] = {"cd", "help", "exit", "history"};

int (*builtin_func[]) (char **) = {&lsh_cd, &lsh_help, &lsh_exit, &readHistory};

int lsh_num_builtins(){
	return sizeof(builtin_str) / sizeof(char *);
}

int lsh_cd(char **args){
	if(args[1] == NULL){
		fprintf(stderr, "%sERROR: Expected argument to \"cd\"%s\n", COLOR_RED, RESET);
	}
	else{
		if(chdir(args[1]) != 0){
			perror("lsh");
		}
	}
	return 1;
}

int lsh_help(char **args){
	int i;
	printf("\nNikola Žitko's LSH\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are built in:\n");

	for(i = 0; i< lsh_num_builtins(); i++){
		printf(" %s\n", builtin_str[i]);
	}
	printf("Use the man command for information on other programs.\n\n");
	return 1;
}

int lsh_exit(char **args){
	return 0;
}



int lsh_execute(char **args){
	int i;

	if(args[0] == NULL){
		return 1;
	}

	for (i = 0; i < lsh_num_builtins(); i++){
		if(strcmp(args[0], builtin_str[i]) == 0){
			return (*builtin_func[i])(args);
		}
	}
	return lsh_launch(args);
}

char *lsh_read_line(void){
	char *line = NULL;
	ssize_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	return line;
}


int readHistory(char** args){
    char buffer[1024];
    char path[1024];
    strcpy(path, getenv("HOME"));
    strcat(path, "/history.log");
    FILE* historyFile = fopen(path, "r");
    while(fgets(buffer, 1024, historyFile) != NULL){
        fprintf(stdout, "%s", buffer);
    }
    fclose(historyFile);
}


void history(char* input){
    char path[1024];
    strcpy(path, getenv("HOME"));
    strcat(path, "/history.log");
    FILE* historyFile = fopen(path, "a");
    if(historyFile == NULL){
        fprintf(stdout, "ERROR: File couldn't be opened.\n");
    }
    else{
        fprintf(historyFile, "%s\n", input);
        fclose(historyFile);
    }
}




void lsh_loop(void){

	char *line;
	char **args;
	int status;
    char* username = getenv("USER");
  
	do{
		printf("%s@%s%s %s-->%s ",COLOR_BLUE, username, RESET, BOLD, RESET);
		line = lsh_read_line();
        history(line);
		args = lsh_split_line(line);
		status = lsh_execute(args);
		free (line);
		free (args);
	}while (status);

}


#define clear() printf("\033[H\033[J")
void init(){
    clear();
    printf("\n\n\n********************************************************");
    printf("\n\n\n\tPrograminski alati na Unix Računalima");
    printf("\n\n\n\tUnix Shell - Projektni zadatak");
    printf("\n\n\n\t%sKorištenje na vlastitu odgovornost!%s", COLOR_RED, COLOR_RESET);
    printf("\n\n\n********************************************************");
    char* username = getenv("USER");
    printf("\n\n\nUser is: @%s\n", username);
    printf("%sPress ENTER to continue...%s\n", BLINKING, STOP_BLINKING);
    getchar();
    clear();
}


int main(int argc, char **argv){
    

    init();
	lsh_loop();

	return EXIT_SUCCESS;
}
