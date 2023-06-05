/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melissaadjogoua <marvin@42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/05 15:48:41 by melissaadjogo     #+#    #+#             */
/*   Updated: 2023/06/05 15:48:45 by melissaadjogo    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*plus tard, mettre les variables redirin etc dans une structure*/

static void    clean_redir(char **av, int savestdin, int savestdout, int savestderr)
{
    //t_data *mini;
char *redirin = "<";
char *redirout1[] = {">", ">>"};
char *redirout2[] = {"2>>", "2>"};
int i = 0;

while(i <= 2)
{
    if (strcmp(av[1], redirin) == 0)
            {
                dup2(savestdin, 0);
                break;
            }
    else if (strcmp(av[1], redirout1[i]) == 0)
            {
                dup2(savestdout, 1);
                break;
            }
    else if (strcmp(av[1], redirout2[i]) == 0)
            {
                dup2(savestderr, 2);
                break;
            }
       i++;
}
   /*test verificaion : 
    if (savestdin == 0)
        printf("ok\n");*/

}

static void    redir_dup(char **av, char *redirin, char **redirout1, char **redirout2, char **env_real)
{
    int         fd = 0; //pour l'instant on le met a NULL juste pour les tests, on le changera avec execve
    int        savestdin = 0;//pareil
    int        savestdout = 0;//pareil
    int        savestderr = 0;//pareil
    int i = 0;

    while(i++ <= 2)//4 symboles de redirections existants
{        
    if (strncmp(av[1], redirin, strlen(redirin)) == 0) //attention les if sont a modifie car il ne regarde pas tous le tableau de tableau mais ce qu'il y a unl'interieur est bon
    {           
        savestdin = dup(0);
        close(0); 
        dup2(fd, 0);
        break;
    }
   else if (strncmp(av[1], redirout1[i], strlen(redirout1[i])) == 0)
    {
        savestdout = dup(1);
        close(1);
        dup2(fd, 1);
        break;
    }
   else if (strncmp(av[1], redirout2[i], strlen(redirout2[i])) == 0)
   /*((strncmp(av[1], redirout2[0], strlen(redirout2[0])) == 0 && strlen(redirout2[0]) == strlen(av[1])) ||
         (strncmp(av[1], redirout2[1], strlen(redirout2[1])) == 0 && strlen(redirout2[1]) == strlen(av[1])))*/
    {
        savestderr = dup(2);
        close(2);
        //write(2, "ok\n", 3); //ne s'ecrit pas car sortie stderr fermee
        dup2(fd, 2);
       //write(2, "ok\n", 3); //s'ecrit car sterr redirigee vers fd
        break;
    } 
    else
        break;
}
    exec(env_real);
    clean_redir(av, savestdin, savestdout, savestderr);
}

int  main(int ac, char **av, char **env_real)
{
//t_data *mini;
(void)ac;
int	fd[2];

if (pipe(fd) == -1)
		return (1);

/*futur structure, ici juste pour les tests*/
char *redirin = "<";
char *redirout1[] = {">", ">>"};
char *redirout2[] = {"2>>", "2>"};

redir_dup(av, redirin, redirout1, redirout2, env_real);
}
