/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melissaadjogoua <marvin@42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:10:11 by melissaadjogo     #+#    #+#             */
/*   Updated: 2023/06/09 18:10:21 by melissaadjogo    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int ft_execve(char *path, char **args, char *envp[], char **av)
{
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

    if (execve(path, args, envp) == -1)
    {
        free(path);
        free(args);
        write(2, "zsh: command not found\n", 23);
        exit(EXIT_FAILURE);
    }
    return 0;
}


