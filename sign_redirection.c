#define REDIR_OUT 1
#define REDIR_OUT_APP 2
#define REDIR_IN 3
#define REDIR_HEREDOC 4

#include <stdio.h>
#include <string.h>

int	sign_redirection(char *redir) 
{
	if (redir == 0)
		return(-1);

if (ft_strcmp(redir, ">") == 0) 
		return (REDIR_OUT);

 else if (ft_strcmp(redir, ">>") == 0) 
		return (REDIR_OUT_APP);

else if (ft_strcmp(redir, "<") == 0) 
		return (REDIR_IN);

else if (ft_strcmp(redir, "<<") == 0) 
		return (REDIR_HEREDOC);

	else 
		return -1;
}

/*
Pour tester la fonction

void	sign_redirection(char *redir) 
{

	if (redir == 0)
		printf("error avec un return pour eviter le segfault\n");

	if (strcmp(redir, ">") == 0) 
	{
		printf(">\n");//return REDIR_OUT;
	}
 else if (strcmp(redir, ">>") == 0) 
	{
		printf(">>\n");//return REDIR_OUT_APP;
	} 
else if (strcmp(redir, "<") == 0) 
	{
		printf("<\n");//return REDIR_IN;
	} 
else if (strcmp(redir, "<<") == 0) 
	{
		printf("<<\n");//return REDIR_HEREDOC;
	} 
	else 
	{
		printf("rien trouve\n");//return -1;
	}
}

int main(int ac, char **av)
{
	(void)ac;
	sign_redirection(av[1]);
}*/
