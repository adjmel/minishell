/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex5.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melissaadjogoua <marvin@42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 13:29:24 by melissaadjogo     #+#    #+#             */
/*   Updated: 2023/06/15 12:38:28 by melissaadjogo    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdio.h>
#include <string.h>
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
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


sig_atomic_t g_sigsigint = 0;
sig_atomic_t g_sigexit_status = 0;
sig_atomic_t g_sigpid = 0;
sig_atomic_t g_sigsigquit = 0;

#define REDIR_NUM 0
#define REDIR_FILE 1
#define REDIR_OUT 1
#define REDIR_OUT_APP 2
#define REDIR_IN 3
#define REDIR_HEREDOC 4

#include <limits.h>

typedef struct 
{
	int fd_repl;         // Descripteur de fichier à rediriger
	int fd_repl_dup;     // Copie du descripteur de fichier pour pouvoir le restaurer plus tard
} t_redir_undo;

int	found_sign_redir(char *redir) 
{
	if (redir == 0)
		return(-1);

	if (strcmp(redir, ">") == 0) 
		return (REDIR_OUT);

	else if (strcmp(redir, ">>") == 0) 
		return (REDIR_OUT_APP);

	else if (strcmp(redir, "<") == 0) 
		return (REDIR_IN);

	else if (strcmp(redir, "<<") == 0) 
		return (REDIR_HEREDOC);

	else 
		return -1;
}

int good_fd_for_redir(char *num_fd, int sign_redir) 
{
	int fd;

	fd = atoi(num_fd);
	if (isdigit(num_fd[0])) 
	{
		return (fd);
	} 
	else if (sign_redir == REDIR_OUT || sign_redir == REDIR_OUT_APP) 
	{
		fd = STDOUT_FILENO;
	} 
	else if (sign_redir == REDIR_IN || sign_redir == REDIR_HEREDOC) 
	{
		fd = STDIN_FILENO;
	}
	return (fd);
}

static int openfile_sign_redir(char *file, int sign_redir)
{
	int fdfinal = 0;

	if (sign_redir == REDIR_HEREDOC)
	{
		int pipe_fd[2];
		if (pipe(pipe_fd) != -1) // ⬇️  permet d'établir une communication entre 2 processus : l'un qui écrit le contenu de file dans le tube (une fois ecris on close car plus besoin), et l'autre qui peut lire ce contenu à partir du tube en utilisant le descripteur fdfinal avec le stdin
		{
			write(pipe_fd[1], file, strlen(file));
			close(pipe_fd[1]);
			fdfinal = pipe_fd[0];
		}
	}
	else
	{
		int flags = 0;	
		if (sign_redir == REDIR_IN)
			flags = O_RDONLY;
		else if (sign_redir == REDIR_OUT)
			flags = O_WRONLY | O_CREAT | O_TRUNC;
		else if (sign_redir == REDIR_OUT_APP)
			flags = O_WRONLY | O_CREAT | O_APPEND;

		fdfinal = open(file, flags, 0644);
	}

	if (fdfinal == -1)
		printf("Error: Failed to open file or create file descriptor.\n");
	else
		printf("fichier ouvert %d\n", fdfinal);

	return (fdfinal);
}

static int redir_process(char *redir, char *file, t_redir_undo **undo)
{
	int status = 0;
	int fd = good_fd_for_redir(redir, found_sign_redir(redir));  // Obtient le descripteur de fichier associé à la redirection

	if (fd == -1)
		return (-1);

	int tmp = dup(fd);  // Duplique le descripteur de fichier pour pouvoir le restaurer plus tard
	if (tmp == -1)  // Vérifie si la duplication a échoué
		return (printf("Bad file descriptor\n"));

	fd = openfile_sign_redir(file, found_sign_redir(redir));  // Ouvre le fichier spécifié par 'file' en fonction du type de redirection
	if (fd == -1)
	{
		close(tmp);  // Ferme la copie du descripteur de fichier
		return (-1);
	}

	if (dup2(fd, fd) == -1)  // Redirige le descripteur de fichier 'fd' vers le descripteur du fichier ouvert
	{
		status = -1; 
		printf("Bad file descriptor\n");//print_error_errno(SHELL_NAME, redir, NULL);
	}
	close(fd);  // Ferme le descripteur de fichier redirigé

	if (status != -1)
	{
		t_redir_undo *new_undo = malloc(sizeof(t_redir_undo));  // Alloue de la mémoire pour stocker les informations de la redirection
		if (new_undo == NULL)
		{
			close(tmp);
			return (printf("Bad file descriptor\n"));
		}
		new_undo->fd_repl = fd;  // Stocke les informations de redirection
		new_undo->fd_repl_dup = tmp;
		*undo = new_undo;  // Stocke les informations de redirection dans le pointeur vers la structure 'undo'
	}
	else
	{
		close(tmp);  // Ferme la copie du descripteur de fichier
	}
	return (status);
}

static int undo_redir(t_redir_undo *undo)
{
	if (undo == NULL)
		return 0;

	int status = 0;

	if (dup2(undo->fd_repl_dup, undo->fd_repl) == -1)  // Restaure le descripteur de fichier d'origine
		status = -1;

	close(undo->fd_repl_dup);  // Ferme la copie du descripteur de fichier
	free(undo);  // Libère la mémoire allouée pour les informations de redirection
	return (status);
}

static void cleanup_redir_undo(t_redir_undo *undo)
{
	if (undo != NULL)
	{
		close(undo->fd_repl_dup);  // Ferme la copie du descripteur de fichier
								   //free(undo);  // Libère la mémoire allouée pour les informations de redirection
	}
}

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

#define MAX_ARGS 1000

ssize_t find_good_env(char **env_real, const char *env_param)
{
	if (!env_real || !env_param)
		return (-1);

	size_t len = strlen(env_param);
	size_t i = 0;

	while (env_real[i])
	{
		if (strncmp(env_real[i], env_param, len) == 0 && env_real[i][len] == '=')
		{
			return (i);
		}
		i++;
	}
	return (-1);
}

char *recup_env(char **env_real, const char *env_param)
{
	ssize_t i = find_good_env(env_real, env_param);

	if (i != -1)
	{
		return (strdup(env_real[i] + strlen(env_param) + 1));
	}
	return (NULL);
}

int set_env(char **env_real, const char *env_param)
{
	char *path = recup_env(env_real, env_param);
	if (path)
	{
		printf("%s = %s\n", env_param, path);
		free(path);
		return 0;
	}
	else
	{
		printf("%s not found\n", env_param);
		return -1;
	}
}

static int perform_cd(const char *path, char **envp) 
{
	if (chdir(path) == 0) 
	{
		char *cwd = getcwd(NULL, 0);
		setenv("PWD", cwd, 1);
		free(cwd);
		return 0;
	} 		else 
	{
		perror("Erreur lors du changement de répertoire");
		return 1;
	}
}

int run_cd(char **args, char **envp) 
{
	if (args[1] && args[2] || !args[1]) 
		return 1;
	
	else 
	{
		// Cas où un chemin est spécifié
		if (args[1][0] == '/') 
		{
			perform_cd(args[1], envp);
			// Chemin absolu
			/*if (perform_cd(args[1], envp) == 0) 
			{
				printf("minishell> PWD = %s\n", getenv("PWD"));
			}*/
		} 
		else 
		{
			// Chemin relatif, ici je constru artificielment un chemin absolu
			char *cwd = getcwd(NULL, 0);
			size_t cwd_len = strlen(cwd);
			size_t arg_len = strlen(args[1]);
			char *new_path = malloc(cwd_len + arg_len + 2); // +2 pour le slash et le caractère nul
			strcpy(new_path, cwd);
			new_path[cwd_len] = '/';
			strcpy(new_path + cwd_len + 1, args[1]);

			perform_cd(new_path, envp);
			/*if (perform_cd(new_path, envp) == 0) 
			{
				printf("minishell> PWD = %s\n", getenv("PWD"));
			}*/

			free(new_path);
			free(cwd);
		}
	}
	return 0;
}


int ft_execve(char *path, char **args, char *envp[], char **av)
{
	(void)av;
	int i = 0;
	int redirsign = 0;
	int positionsign = -1;

	while (args[i])
	{
		if (found_sign_redir(args[i]) != -1)
		{
			redirsign = found_sign_redir(args[i]);
			positionsign = i;
			break;
		}
		i++;
	}

	if (positionsign != -1)
	{
		char *filename = args[positionsign + 1];
		int fd_repl = openfile_sign_redir(filename, redirsign);
		if (fd_repl == -1)
		{
			exit(EXIT_FAILURE);
		}

		t_redir_undo *undo = malloc(sizeof(t_redir_undo));
		/*if (undo == NULL)
		  {
		  perror("Failed to allocate memory for t_redir_undo");
		  exit(EXIT_FAILURE);
		  }*/

		undo->fd_repl = good_fd_for_redir(args[positionsign - 1], redirsign);
		undo->fd_repl_dup = dup(undo->fd_repl);
		/*if (undo->fd_repl_dup == -1)
		  {
		  perror("Failed to duplicate file descriptor");
		  free(undo);
		  exit(EXIT_FAILURE);
		  }*/

		// Rediriger le descripteur de fichier approprié vers fd_repl
		if (dup2(fd_repl, undo->fd_repl) == -1)
		{
			perror("Failed to redirect file descriptor");
			close(fd_repl);
			free(undo);
			exit(EXIT_FAILURE);
		}

		// Fermer le descripteur de fichier supplémentaire
		close(fd_repl);

		// Supprimer le signe de redirection et le nom de fichier de args
		args[positionsign] = NULL;
		args[positionsign + 1] = NULL;
	}

	// Vérifie la commande
	if (strcmp(args[0], "cd") == 0)
	{
		run_cd(args, envp);
	}
	else if (strcmp(args[0], "pwd") == 0)
	{
		set_env(envp, "PWD");
	}

	if (execve(path, args, envp) == -1)
	{
		free(path);
		free(args);
		write(2, "zsh: command not found\n", 23);
		exit(EXIT_FAILURE);
	}
	return 0;
}


void sigint(int signal) 
{
	(void)signal;
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
	(void)signal;
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

				/*ATTENTION PWD ICI			//exec(envp, command);*/


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
						if (strcmp(args[0], "cd") == 0)
						{
							run_cd(args, envp);
						}
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
}
