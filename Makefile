all:
	gcc main.c commands.c scan_input.c list.c -o minishell
clean:
	rm -f minishell

