#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define REDIR_NUM 0
#define REDIR_FILE 1
#define ERROR -1

int redir_type(char *redir);
int redir_process(char *redir, char *file);

int main() {
    char redir[] = "2>>";
    char file[] = "output.txt";

    int result = redir_process(redir, file);
    if (result == ERROR) {
        printf("Erreur lors de la redirection.\n");
    } else {
        printf("Redirection réussie.\n");
    }

    return 0;
}

int redir_type(char *redir) {
    if (redir[0] == '1' || redir[0] == '2') {
        if (redir[1] == '>') {
            if (redir[2] == '>') {
                return REDIR_FILE;
            } else {
                return REDIR_NUM;
            }
        }
    }
    return ERROR;
}

int redir_process(char *redir, char *file) {
    int fd[2];
    int type = redir_type(redir);

    if (type == ERROR) {
        printf("Type de redirection invalide.\n");
        return ERROR;
    }

    if (type == REDIR_NUM) {
        char *endptr;
        fd[REDIR_NUM] = strtol(redir, &endptr, 10);
        if (*endptr != '\0') {
            printf("Numéro de descripteur de fichier invalide.\n");
            return ERROR;
        }
    } else if (type == REDIR_FILE) {
        fd[REDIR_NUM] = STDOUT_FILENO;
    }

    fd[REDIR_FILE] = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd[REDIR_FILE] == -1) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return ERROR;
    }

    if (dup2(fd[REDIR_FILE], fd[REDIR_NUM]) == -1) {
        printf("Erreur lors de la redirection du descripteur de fichier.\n");
        return ERROR;
    }

    close(fd[REDIR_FILE]);

    return 0;
}

