#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
 

#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND      3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*char prompt[25] = "minishell$: ";
char input_string[50];
int status;
char* ext_cmds[152];*/

//buffers
#define MAX_INPUT 256
#define MAX_PROMPT 64
#define MAX_CMD 64
#define MAX_ARGS 64
#define MAX_EXT_CMD 300

typedef struct Slist
{
	int pid;
	char command[MAX_INPUT];
	struct Slist *next;
}Slist;

extern char   prompt[MAX_PROMPT];
extern char   input_string[MAX_INPUT];
extern int    g_status;
extern char  *ext_cmd[MAX_EXT_CMD];
extern pid_t  g_child_pid;
extern Slist *head;
//scan_input.c
void scan_input(char *prompt, char *input_string);
void signal_handler(int sig_num);

//commands.c
char *get_cmd(char *input_string);
int check_command_type(char *command);
void extract_external_commands(char **external_commands);
void execute_internal_commands(char *input_string);
void execute_external_commands(char* input_string);

//list.c
void insert_at_first(pid_t pid,const char* cmd);
void delete_node(pid_t pid);
void delete_first(void);
void print_list(Slist* node);

//void copy_change(char *prompt, char *input_string);


//void echo(char *input_string, int status);

#endif




