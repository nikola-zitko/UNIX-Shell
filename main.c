#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "functions.h"


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


int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int readHistory(char **args);
int clearHistory(char **args);

char *lsh_read_line(void){
	char *line = NULL;
	ssize_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	return line;
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
