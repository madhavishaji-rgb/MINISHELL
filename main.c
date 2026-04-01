
#include "header.h"

char prompt[MAX_PROMPT] = "myshell> ";
char input_string[MAX_INPUT];
int g_status = 0;
char* ext_cmd[MAX_EXT_CMD];
pid_t g_child_pid = 0;
Slist* head = NULL;

int main()
{
	system("clear");
	Slist* head = NULL;

	printf(ANSI_COLOR_MAGENTA
		"-----------------------------------------\n"
		"       Welcome to MINISHELL               \n"
		"-----------------------------------------\n"
	ANSI_COLOR_RESET);

	scan_input(prompt,input_string);
	return 0;
}

