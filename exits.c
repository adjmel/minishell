int check_correct_exit(char **args)
{
    int i = 1;
    int num_args = 1;
    while (args[i])
    {
        num_args++;
        i++;
    }
    if (num_args > 2)
    {
        printf("exit: too many arguments\n");
        return 1;
    }
    char *arg = args[1];
    int j = 0;
    while (arg[j] != '\0')
    {
        if (isalpha(arg[j]))
        {
            printf("invalid exit code\n");
            return 1;
        }
        j++;
    }
    return 0;
}

int check_exit_signs(char **args)
{
    char *arg = args[1];
    int i = 0;
    int count = 0;

    while (arg[i] != '\0')
    {
        if (arg[i] == '+' || arg[i] == '-')
        {
            count = 1;
            while (arg[i + 1] == arg[i])
            {
                count++;
                i++;
            }
            if (count > 2)
            {
                printf("Invalid exit code\n");
                return (1);
            }
        }
        i++;
    }
    return 0;
}

int execute_exit(char **args) 
{
        if (args[1] != NULL) 
        {
            // Si un argument est présent, convertissez-le en un nombre entier	
			if (check_correct_exit(args) == 1 || 
			check_exit_signs(args) == 1)
				exit(-1);

            long long exit_code = atoll(args[1]);
			if (strcmp(args[1], "9223372036854775808") == 0 || 
				strcmp(args[1], "-9223372036854775808") == 0)
			{
					printf("exit: %s numeric argument required\n", args[1]);
					exit(0);
			}
	else 
		   {
			 	printf("exit 💙\n");
            	exit(exit_code);
		   }
        } 
    else 
        {
            // Sinon, sortir avec le code de sortie par défaut (0)
            printf("exit 💚\n");
            exit(0);
        }
     return (1);  
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