#include "header.h"

void signal_handler(int sig_num)//handles SIGINT,SIGTSTP,SIGCHLD for the shell
{
	if(sig_num == SIGINT)
	{
		if(g_child_pid == 0)
		printf("\n%s\n",prompt);
	}
	if(sig_num == SIGTSTP)
	{
	if(g_child_pid != 0)
    {
        kill(g_child_pid, SIGTSTP);
    }
	}
	if(sig_num == SIGCHLD)
	{
		pid_t pid;
		int status;
		while((pid = waitpid(-1,&status,WNOHANG)) > 0)
		{
			delete_node(pid);
			printf("\n[bg done] PID %d exited with status %d\n",pid,WEXITSTATUS(status));
		}
	}
}


void scan_input(char* prompt,char* input)// mini shell read evaluation loop
{
	extract_external_commands(ext_cmd);// load external commands from file


	signal(SIGINT,signal_handler);
	signal(SIGTSTP,signal_handler);
	signal(SIGCHLD,signal_handler);
	
	while(1)
	{
		printf(ANSI_COLOR_YELLOW"%s"ANSI_COLOR_RESET,prompt);
		if(fgets(input,MAX_INPUT,stdin) == NULL)  //handling empty input
		{
			printf("\n");// end of file 
			exit(0);
		}

		//trailing new line
		int len = strlen(input);
		if(len > 0 && input[len-1] == '\n')//last character being new line
		input[--len] = '\0';//storing in len -1 and reducing len by 1

		if(len == 0)
		continue;

		if(strncmp(input,"PS1=",4) == 0)//considering PS1 where in we get to change the prompt
		{
			if((strchr(input+ 4,' ') != NULL))//reject if there are spaces value portion
			{
				printf("PS1: command not found\n");
				return;
			}
			else
			{
			
				strncpy(prompt,input + 4,MAX_PROMPT - 1);
				prompt[MAX_PROMPT - 1] = '\0';
			}
			continue;// goes to next iteration of while loop and new prompt is printed
			
		}
		
			char* cmd = get_cmd(input);
			int type = check_command_type(cmd);
			if(type == BUILTIN)
			{
				execute_internal_commands(input);
			}
			else if(type == EXTERNAL)
			{
				int background = 0;
				if(len > 0 && input[len - 1] == '&')//detects backfround execution eg sleep 30 &
				{
					background = 1;
					input[len - 1] = '\0';
				}

				pid_t pid = fork();
				
				if(pid < 0)
				{
					perror("");
					continue;
				}
				if(pid == 0)
				{
					signal(SIGINT,SIG_DFL);//as we had previously mentioned signal handler to change behaviour of SIGINT, SIGTSTP but while executing external commands like sleep ctrl + z should stop process of child hence getting back the default behaviour of SIGINT etc
					signal(SIGTSTP,SIG_DFL);
					signal(SIGCHLD,SIG_DFL);
					execute_external_commands(input);
					exit(0);
				}
				if(background)
				{
					
					insert_at_first(pid,input);//inserting the background job into jobs
					printf("[bg] PID %d : %s\n",pid,input);
				}
				else
				{
					g_child_pid = pid;
					waitpid(pid,&g_status,WUNTRACED);
					g_child_pid = 0;
				
				if(WIFSTOPPED(g_status))
				{
					insert_at_first(pid,input);
					printf("[Stopped] PID: %d Command: %s\n",pid,input);
				}
			}
		}
			else
			{
				printf("%s: command not found\n",cmd);//for unknown commands
			}
		}
}

