#include "header.h"

//built in commands
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help","jobs","fg","bg",NULL};

char cmd_buf[MAX_CMD];

char* get_cmd(char* input_string)
{
	int i = 0;
	while(input_string[i] != ' ')//copying till space
	{
		if(i >= MAX_CMD - 1)
		break;
		cmd_buf[i] = input_string[i];
		i++;
	}
	cmd_buf[i] = '\0';
	return cmd_buf;
}


int check_command_type(char* command)
{
	int i = 0;
	while(builtins[i] !=  NULL)
	{
		if(strcmp(builtins[i],command) == 0)
		return BUILTIN;
		i++;
	}
	i = 0;
	while(ext_cmd[i] != NULL)// in the scan input function the first line executed is extract external commands during that time external commands from ext_cmds.txt is extracted into the array of strings that is ext_cmds
	{
		if(strcmp(ext_cmd[i],command) == 0)
		return EXTERNAL;
		i++;
	}
	return NO_COMMAND;
}


void extract_external_commands(char **external_commands)
{
	int i = 0;
	char buff[100];
	FILE* fp = fopen("ext_cmds.txt","r");
	if(fp == NULL)
	{
		perror(" ");
		return;
	}
	while(fscanf(fp,"%99s",buff) != EOF)
	{
		external_commands[i] = malloc(strlen(buff)+1);//in case of mkdir len is 5(5+1)
		if(external_commands[i] == NULL)
		{
			perror("malloc");
			break;
		}
	        strcpy(external_commands[i],buff);
		i++;
	}
	external_commands[i] = NULL;
	fclose(fp);
}

void execute_internal_commands(char* input_string)
{
	if(strcmp(input_string,"exit") == 0)
	{
		printf("Exiting mini shell!!!!!\n");
		exit(0);
	}
	else if(strcmp(input_string,"pwd") == 0)
	{
 		char buff[500];
		if(getcwd(buff,sizeof(buff)) != NULL)//getcwd returns buffer on successful execution else on failure NULL
		printf("%s\n",buff);
		else
		perror("pwd");
		return;
	}
	else if(strncmp(input_string,"cd",2) == 0)//strncmp is used as cd can come aloong with some othe directory name as in cd ASSIG
	{
		char *path = strtok(input_string + 2, " ");

        if(path == NULL)
        path = getenv("HOME");//now path contains pointer to the environmental variable HOME(pointer to string "/home/madhavi")

        if(chdir(path) != 0)//chdir changes the directory to the path it return 0 forn successful execution and -1 on failure
        perror("cd");
		return;
	}
	else if(strcmp(input_string,"echo $?") == 0)
	{
		if(WIFEXITED(g_status))//prints status of last command 1 in case by return or by exit and WEXITSTATUS gives the value for in case of exit(5) WEXITSTATUS is 5
		{
			printf("%d",WEXITSTATUS(g_status));//g_status contains information like did child exit normally or it was killed by some signal ,which signal and what is the exit code
		}
		else if(WIFSIGNALED(g_status))
		{
        printf("%d\n", WTERMSIG(g_status));
	    }
	}
	else if(strcmp(input_string,"echo $SHELL") == 0)
	{
		const char* sh = getenv("SHELL");// shell is an environment variable and when its called it gives pointer to string (/bin/bash)
		if(sh)
		printf("%s\n",sh);
		else
		{
			printf("minishell\n");
		}
		return;
	}
	else if(strcmp(input_string,"jobs") == 0)
	{
		print_list(head);
		return;
	}
	else if(strcmp(input_string,"fg") == 0)
    {
		if(head == NULL)//no jobs in the background
		{
			printf("fg: No such jobs\n");
            return;
        }
        
			pid_t fg_pid = head->pid;//pid of fg is the pid of the most recent job
            char cmd_copy[MAX_INPUT];
            strcpy(cmd_copy, head->command);//copying the command in the list to buffer before deleting the node

            delete_first();

            g_child_pid = fg_pid;
            kill(fg_pid, SIGCONT);//continuing the process

            waitpid(fg_pid, &g_status, WUNTRACED);//by using WUNTRACED it will return from wait if child is stopped in between or else it might not return and keep waiting
            g_child_pid = 0;
			if(WIFSTOPPED(g_status))
			{
				insert_at_first(fg_pid, cmd_copy);
                printf("[Stopped] PID: %d Command: %s\n", fg_pid, cmd_copy);
            }
			return;
	}
	else if(strcmp(input_string,"bg") == 0)
	 {
			if(head == NULL)
			{
				printf("bg: No such jobs\n");
				return;
			}
			pid_t bg_pid = head->pid;
			kill(bg_pid, SIGCONT);//continuing background ,process
			printf("[bg] PID %d resumed in background\n", bg_pid);
			return;
        }
	    printf("%s: command not found\n",input_string);
}


void execute_external_commands(char* input_string)  // executes single external command or multiple piped commands
{
	char *cmds[10];
    int cmd_count = 0;
	if(strchr(input_string, '|') == NULL) // if there is no pipe (single command)
    {
        char *argv[MAX_ARGS];
        int i = 0;

        char *token = strtok(input_string, " ");
        while(token != NULL && i < MAX_ARGS - 1)
        {
            argv[i++] = token;
            token = strtok(NULL, " ");
        }
        argv[i] = NULL;

        execvp(argv[0], argv);
        perror("execvp");
        exit(1);
    }
    // Split by pipe
    char *pipe_token = strtok(input_string, "|");
    while(pipe_token != NULL && cmd_count < 10)
    {
        cmds[cmd_count++] = pipe_token;
        pipe_token = strtok(NULL, "|");
    }

    int prev_fd = -1;// initializing it to -1 since -1 is an invalid descriptor

    for(int i = 0; i < cmd_count; i++)
    {
        int fd[2];

        if(i < cmd_count - 1)
        {
            if(pipe(fd) == -1)
            {
                perror("pipe");
                exit(1);
            }
        }

        pid_t pid = fork();

        if(pid == 0) // child
        {
            if(prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            if(i < cmd_count - 1)
            {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }

            // tokenize command into argv
            char *argv[MAX_ARGS];
            int j = 0;

            char *token = strtok(cmds[i], " \n");
            while(token != NULL && j < MAX_ARGS - 1)
            {
                argv[j++] = token;
                token = strtok(NULL, " \n");
            }
            argv[j] = NULL;

            execvp(argv[0], argv);
            perror("execvp");
            exit(1);
        }

        else if(pid > 0) // parent
        {
            if(prev_fd != -1)//c,losing all used file descriptors 
                close(prev_fd);

            if(i < cmd_count - 1)
            {
                close(fd[1]);
                prev_fd = fd[0];
            }
        }
        else
        {
            perror("fork");
            exit(1);
        }
    }
}











