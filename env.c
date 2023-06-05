/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melissaadjogoua <marvin@42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/05 15:24:25 by melissaadjogo     #+#    #+#             */
/*   Updated: 2023/06/05 15:51:17 by melissaadjogo    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ssize_t find_good_env(char **env_real, const char *env_param) 
{
    if (!env_real || !env_param)
        return (-1);

    size_t len = strlen(env_param);
    size_t i = 0;

    while (env_real[i++]) 
    {
        if (strncmp(env_real[i], env_param, len) == 0 && env_real[i][len] == '=') 
        {    
            return (i);
        }
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

int			exec(char **env_real)
{   
	char    *path;

	path = recup_env(env_real, "PWD");
    printf("PWD = %s\n", path);
    return (0);
}

int main(int ac, char **av, char **envp)
{
	(void)ac;
	(void)av;
	exec(envp);
}
