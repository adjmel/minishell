void sigint(int signal) 
{
        (void)signal;
			write(STDERR_FILENO, "\n\033[0;36m\033[1m👍 minishell> \033[0m", 29);
}

void allsignals()
{
    
	signal(SIGINT, sigint);
	//signal(SIGQUIT, sigquit);
	signal(SIGQUIT, SIG_IGN);
}

void signals_d()
{
    char buffer[1];
    ssize_t bytesRead = read(STDIN_FILENO, buffer, 0);
    if (bytesRead == 0) 
    {
        write(STDERR_FILENO, "exit\n", 5);
        exit(0);
    }
}

int ft_pipex(int argc, char* argv[], char* envp[]) 
{
	allsignals();

	char *line;
	while(1)
	{
		line = readline("\033[0;36m\033[1m minishell> \033[0m");
		if (line != NULL)
		{
			int num_commands = 1;
			int prev_pipe[2]; // Descripteurs de fichier pour le tuyau précédent
			int next_pipe[2]; // Descripteurs de fichier pour le tuyau suivant

			// Initialisation du tuyau précédent
			pipe(prev_pipe);
			int i = 0;
			while(i < num_commands) 
			{
				char *command = ft_command(line);
				//printf("cmd = %s\n", command);
				char *path = ft_recup_path(command, envp);
				//printf("pth = %s\n", path);
				char **args = ft_split(line, ' ');
				//printf("1 args = %s\n", args[0]);
				//printf("2 args = %s\n", args[1]);
				//printf("3 args = %s\n", args[2]);
					
 		   if (strcmp(args[0], "exit") == 0)
  			{
    		    execute_exit(args);
    		}
				if (path != NULL) 
				{
					not_last_command(i, num_commands, next_pipe);
					pid_t child_pid = fork();
					if (child_pid == 0) 
					{
						process_child(i, prev_pipe, next_pipe, num_commands);
						ft_execve(path, args, envp, argv);
						exit(EXIT_FAILURE);
					} 
					else if (child_pid > 0) 
					{
						process_father(i, prev_pipe, next_pipe, num_commands);
					}
					else 
					{
						printf("Erreur lors de la création du processus enfant.\n");
						exit(EXIT_FAILURE);
					}
				}
				i++;
			}
			close_and_wait(prev_pipe, num_commands);
			//line = readline("\033[0;36m\033[1m minishell> \033[0m");
             ft_pipex(argc,argv, envp);
			//signals_c_backslash();
		}
		return 0;
	}
}

int main(int ac, char **av, char **envp)
{
	ft_pipex(ac, av, envp);
    signals_d();
}