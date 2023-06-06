/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   supp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melissaadjogoua <marvin@42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 14:13:39 by melissaadjogo     #+#    #+#             */
/*   Updated: 2023/06/06 14:19:52 by melissaadjogo    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

static int openfile_sign_redir(char *file, int sign_redir)
{
	int fdfinal;

	if (sign_redir == REDIR_HEREDOC)
	{
		int pipe_fd[2];
		if (pipe(pipe_fd) != -1) // ⬇️  permet d'établir une communication entre 2 processus : l'un qui écrit le contenu de file dans le tube (une fois ecris on close car plus besoin), et l'autre qui peut lire ce contenu à partir du tube en utilisant le descripteur fdfinal avec le stdin
		{
			write(pipe_fd[1], file, ft_strlen(file));
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



