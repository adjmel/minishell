/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex4.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melissaadjogoua <marvin@42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/05 11:08:21 by melissaadjogo     #+#    #+#             */
/*   Updated: 2023/06/05 14:54:47 by melissaadjogo    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

sig_atomic_t g_sigsigint = 0;
sig_atomic_t g_sigexit_status = 0;
sig_atomic_t g_sigpid = 0;
sig_atomic_t g_sigsigquit = 0;

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

static char	*ft_malloc(char const *s, unsigned int start, size_t len)
{
	char	*s2;

	if (ft_strlen(s) == 1 && start > ft_strlen(s))
		s2 = (char *)malloc(sizeof(char));
	else if (len > ft_strlen(s))
		s2 = (char *)malloc(sizeof(char) * (ft_strlen(s) + 1));
	else if (start > ft_strlen(s))
		s2 = (char *)malloc(sizeof(char));
	else if (len == ft_strlen(s) && start < len && len != 0)
		s2 = (char *)malloc(sizeof(char) * (len - start) + 1);
	else
		s2 = (char *)malloc(sizeof(char) * (len + 1));
	return (s2);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	char	*s2;

	i = 0;
	if (!s)
		return (NULL);
	s2 = ft_malloc(s, start, len);
	if (!s2)
		return (NULL);
	while (i < len && s[i] && start < ft_strlen(s))
	{
		s2[i] = s[start];
		i++;
		start++;
	}
	s2[i] = '\0';
	return (s2);
}

char	*ft_strjoin(char *s1, char *s2)
{
	char	*join;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	join = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!join)
		return (NULL);
	while (s1[i])
	{
		join[i] = s1[i];
		i++;
	}
	while (s2[j])
	{
		join[i + j] = s2[j];
		j++;
	}
	join[i + j] = '\0';
	return (join);
}

static int	count_words(char const *s, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i] && s)
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] != c && s[i])
				i++;
		}
		else
			i++;
	}
	return (count);
}

void	ft_allocate(char **s2, char const *s, char c)
{
	char	**tab_s2;
	char	*tmp;

	tmp = (char *)s;
	tab_s2 = s2;
	while (*tmp)
	{
		while (*s == c)
			++s;
		tmp = (char *)s;
		while (*tmp && *tmp != c)
			++tmp;
		if (*tmp == c || tmp > s)
		{
			*tab_s2 = ft_substr(s, 0, tmp - s);
			s = tmp;
			++tab_s2;
		}
	}
	*tab_s2 = NULL;
}

char	**ft_split(char const *s, char c)
{
	char	**s2;
	int		size;

	if (!s)
		return (NULL);
	size = count_words(s, c);
	s2 = (char **) malloc (sizeof(char *) * (size + 1));
	if (!s2)
		return (NULL);
	ft_allocate(s2, s, c);
	return (s2);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
	{
		return (0);
	}
	while ((unsigned char)*s1 == (unsigned char)*s2 && *s1 != '\0' && n - 1 > 0)
	{
		s1++;
		s2++;
		n--;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

char	**ft_recup_envp(char **envp)
{
	char	*env;
	char	**new_env;
	int		i;

	env = NULL;
	new_env = NULL;
	i = 0;
	if (envp)
	{
		while (envp[i] != NULL)
		{
			if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			{
				env = envp[i] + 5; 
				break ;
			}
			i++;
		}
	}
	if (!env)
		return (new_env);
	else
		new_env = ft_split(env, ':');
	return (new_env);
}

void	free_tab(char **tab)
{
	int	i;

	i = 0;
	if (tab == NULL)
		return ;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

char	*ft_recup_path(char *command, char **envp)
{
	char	**allpath;
	char	*allpathnew;
	char	*cmdpath;
	int		j;

	allpath = ft_recup_envp(envp);
	j = 0;
	if (!allpath)
		return (command);
	while (allpath[++j])
	{
		allpathnew = ft_strjoin(allpath[j], "/");
		cmdpath = ft_strjoin(allpathnew, command); 
		if (access(cmdpath, R_OK) != -1)
		{
			free_tab(allpath);
			free(allpathnew);
			//vars->flag = 1;
			return (cmdpath); 
		}	  
		free(allpathnew);
		free(cmdpath); 
	} 
	free_tab(allpath);
	return (command);
}

char	*ft_command(char *av)
{
	int		i;
	int		j;
	char	*command;

	i = 0;
	j = 0;
	while (av[i] == ' ')
		i++;
	j = i;
	while (av[j] != ' ' && av[j])
		j++;
	command = ft_substr(av, i, j);
	return (command);
}

// Si ce n'est pas la dernière commande, créez un nouveau tuyau
void not_last_command(int i, int num_commands, int *next_pipe)
{
	if (i < num_commands - 1) 
	{
		pipe(next_pipe);
	}
}

void	process_child(int i, int *prev_pipe, int *next_pipe, int num_commands)
{
	// Code du processus enfant
	// Redirection des descripteurs de fichier pour l'entrée et la sortie
	if (i > 0) 
	{
		dup2(prev_pipe[0], STDIN_FILENO);
		close(prev_pipe[1]);
		close(prev_pipe[0]);
	}

	if (i < num_commands - 1) 
	{
		dup2(next_pipe[1], STDOUT_FILENO);
		close(next_pipe[0]);
		close(next_pipe[1]);
	}
}

void	process_father(int i, int *prev_pipe, int *next_pipe, int num_commands)
{
	if (i > 0) 
	{
		close(prev_pipe[0]);
		close(prev_pipe[1]);
	}

	if (i < num_commands - 1) 
	{
		prev_pipe[0] = next_pipe[0];
		prev_pipe[1] = next_pipe[1];
	}
}

// Fermeture des descripteurs de fichier restants dans le processus parent
// et attente de la terminaison de tous les processus enfants
void	close_and_wait(int *prev_pipe, int num_commands)
{
	close(prev_pipe[0]);
	close(prev_pipe[1]);

	int i = 0;
	while (i++ < num_commands) 
	{
		wait(NULL);
	}
}

int	ft_execve(char *path, char **args, char *envp[], char **av)
{
	//access a modifier apres selon le parsing et ou peut/se situera l'infile
	/*if (access(av[1], R_OK) == -1)
	  {
	  write(2, "zsh: permission denied\n", 23);
	  exit(EXIT_FAILURE);
	  }*/
	(void)av;

	if (execve(path, args, envp) == -1)
	{
		free(path);
		free_tab(args);
		write(2, "zsh: command not found\n", 23);
		exit(EXIT_FAILURE);
	}
	return (0);
}

void sigint(int signal) 
{
		if (g_sigpid == 0) 
		{
			write(STDERR_FILENO, "\n\033[0;36m\033[1m👍 minishell> \033[0m", 29);
			g_sigexit_status = 1;
		} 
		else 
		{
			printf("\n");
			g_sigexit_status = 130;
		}
		g_sigsigint = 1;
}

void sigquit(int signal) 
{
		if (g_sigsigint) 
		{
			g_sigsigint = 1;
		} 
		else 
		{
			write(STDERR_FILENO, "\b\b  \b\b", 6);		
		}
}

void allsignals()
{
	signal(SIGINT, sigint);
	signal(SIGQUIT, sigquit);
	//signal(SIGQUIT, SIG_IGN);
	//signals_d();
}

int main(int argc, char* argv[], char* envp[]) 
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
				//printf("args = %s\n", args[0]);

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
			main(argc,argv, envp);
		}

		return 0;
	}
}
