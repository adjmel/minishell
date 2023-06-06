/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_fd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melissaadjogoua <marvin@42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 17:47:33 by melissaadjogo     #+#    #+#             */
/*   Updated: 2023/06/06 19:13:32 by melissaadjogo    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int good_fd_for_redir(char *num_fd, int sign_redir) 
{
    int fd;
	
 	fd = atoi(num_fd);
    if (ft_isdigit(num_fd[0])) 
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


/*#define REDIR_OUT 1
#define REDIR_OUT_APP 2
#define REDIR_IN 3
#define REDIR_HEREDOC 4

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
    if (ft_isdigit(num_fd[0])) 
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
	int fdfinal;

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

	return (fdfinal);
}


int main(int argc, char *argv[]) 
{
	if (argc != 3) 
	{
		printf("mettre le signe de reidrection puis le file : '>>' 'file' \n");
		return 1;
	}

	char *redirection = argv[1];
	char *filename = argv[2];
	int type = found_sign_redir(redirection);

	if (type != -1) //cas d'erreur, je rentre dans le if sinon je printf l'erreur du else en dessous
	{
		int fd = openfile_sign_redir(filename, type);
		if (fd != -1) 
		{
			int redirect_fd = good_fd_for_redir(redirection, type);
			if (redirect_fd != -1) 
			{
				printf("File opened successfully with file descriptor: %d\n", fd);
				printf("Redirect file descriptor: %d\n", redirect_fd);
				close(fd);
				close(redirect_fd);
			}
		}
	} 
else 
{
	printf("Invalid redirection sign.\n");
}
	return 0;
}
*/
