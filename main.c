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


char* tilde(char *orig){
    char* rep = (char*)malloc(sizeof(char)*2);
    rep = strcpy(rep, "~");
    char* with = getenv("HOME");
    char *result, *ins, *tmp;
    int len_rep, len_with, len_front, count;

    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if(len_rep == 0)
        return NULL;
    if(!with)
        with = "";
    len_with = strlen(with);

    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count){
        ins = tmp + len_rep;
    }
    
    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);
    if(!result)
        return NULL;
    
    while(count--){
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep;
    }
    strcpy(tmp, orig);
    return result;
}
void printdir(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf(": %s", cwd);
}

void lsh_loop(void){

	char *line;
	char **args;
	int status;
    char* username = getenv("USER");
  
	do{
        printf("%s@%s%s",COLOR_BLUE, username, RESET);
        printdir();
		printf("%s ->%s ", BOLD, RESET);
		line = lsh_read_line();
        history(line);
        line = tilde(line);
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
