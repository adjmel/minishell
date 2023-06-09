/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parfait.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melissaadjogoua <marvin@42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/08 12:50:53 by melissaadjogo     #+#    #+#             */
/*   Updated: 2023/06/09 15:55:11 by melissaadjogo    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
/*quelques repetitions dans les bibli*/
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

#define REDIR_NUM 0
#define REDIR_FILE 1
#define REDIR_OUT 1
#define REDIR_OUT_APP 2
#define REDIR_IN 3
#define REDIR_HEREDOC 4

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

/*int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Mettre le signe de redirection puis le nom du fichier.\n");
        return 1;
    }

    char *redirection = argv[1];  // Récupère le signe de redirection passé en argument
    char *filename = argv[2];     // Récupère le nom du fichier passé en argument

    int sign_redir = found_sign_redir(redirection);
    if (sign_redir == -1)
    {
        printf("Signe de redirection invalide : %s\n", redirection);
        return 1;
    }

    t_redir_undo *undo = NULL;    // Pointeur vers la structure de stockage des informations de redirection
    int result = redir_process(redirection, filename, &undo);  // Effectue la redirection

    if (result == -1)
    {
        printf("Erreur lors de la redirection.\n");
        return 1;
    }

    printf("Redirection effectuée avec succès.\n");

    // Utiliser les descripteurs de fichiers redirigés ici

    result = undo_redir(undo);  // Annule la redirection
    if (result == -1)
    {
        printf("Erreur lors de l'annulation de la redirection.\n");
        return 1;
    }

    printf("Redirection annulée avec succès.\n");

    cleanup_redir_undo(undo);  // Nettoie la structure de stockage des informations de redirection

    return 0;
}*/




