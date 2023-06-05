#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define REDIR_OUT 1
#define REDIR_OUT_APP 2
#define REDIR_IN 3
#define REDIR_HEREDOC 4

int redir_type(char *redir) {
    if (strcmp(redir, ">") == 0) {
        return REDIR_OUT;
    } else if (strcmp(redir, ">>") == 0) {
        return REDIR_OUT_APP;
    } else if (strcmp(redir, "<") == 0) {
        return REDIR_IN;
    } else if (strcmp(redir, "<<") == 0) {
        return REDIR_HEREDOC;
    } else {
        return -1;
    }
}

int redir_process(char *redir, char *file) {
    int fd[2];
    int type = redir_type(redir);

    if (type == -1) {
        printf("Type de redirection invalide.\n");
        return -1;
    }

    if (type == REDIR_HEREDOC) {
        if (pipe(fd) == -1) {
            printf("Erreur lors de la création du pipe : %s\n", strerror(errno));
            return -1;
        }
        printf("Entrez le contenu pour l'opérateur '<<' (appuyez sur Ctrl+D pour terminer) :\n");
        char buffer[100];
        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            write(fd[1], buffer, strlen(buffer));
        }
        close(fd[1]);
    }

    int open_flags = 0;
    if (type == REDIR_OUT || type == REDIR_OUT_APP) {
        open_flags = O_WRONLY | O_CREAT | (type == REDIR_OUT_APP ? O_APPEND : O_TRUNC);
    } else if (type == REDIR_IN || type == REDIR_HEREDOC) {
        open_flags = O_RDONLY;
    }

    int fd_file = open(file, open_flags, 0644);
    if (fd_file == -1) {
        printf("Erreur lors de l'ouverture du fichier : %s\n", strerror(errno));
        return -1;
    }

    if (type == REDIR_OUT || type == REDIR_OUT_APP) {
        if (dup2(fd_file, STDOUT_FILENO) == -1) {
            printf("Erreur lors de la redirection de la sortie standard : %s\n", strerror(errno));
            close(fd_file);
            return -1;
        }
    } else if (type == REDIR_IN || type == REDIR_HEREDOC) {
        if (dup2(fd_file, STDIN_FILENO) == -1) {
            printf("Erreur lors de la redirection de l'entrée standard : %s\n", strerror(errno));
            close(fd_file);
            return -1;
        }
    }

    close(fd_file);

    return 0;
}

int main() {
    char redir[] = ">";
    char file[] = "output.txt";

    if (redir_process(redir, file) == 0) {
        printf("Redirection effectuée avec succès.\n");
    }

    if (isatty(STDIN_FILENO)) {
        printf("Contenu de l'entrée standard :\n");
        char buffer[100];
        fgets(buffer, sizeof(buffer), stdin);
        printf("Entrée standard : %s", buffer);
    }

    return 0;
}

