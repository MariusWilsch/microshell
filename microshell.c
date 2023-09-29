/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: verdant <verdant@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 18:49:25 by verdant           #+#    #+#             */
/*   Updated: 2023/07/18 20:15:16 by verdant          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libc.h>

#define READ 0
#define WRITE 1

int	ft_strlen(char *str)
{
	int	x = 0;
	
	if (!str)
		return (x);
	while (str[x])
		x++;
	return (x);
}

void	print_error(char *str, char *str2)
{
	write(2, str, ft_strlen(str));
	if (str)
		write(2, str2, ft_strlen(str2));
	write(2, "\n", 1);
}

void	execute(char **argv, int fd_in, int fd_out, char **envp, int i)
{
	pid_t	pid = fork();
	if (pid < 0)
	{
		print_error("error: fatal", NULL);
		exit(1);
	}
	if (pid == 0)
	{
		dup2(fd_in, STDIN_FILENO);
		dup2(fd_out, STDOUT_FILENO);
		close(fd_in);
		close(fd_out);
		argv[i] = NULL;
		execve(argv[0], argv, envp);
		print_error("error: cannot execute ", argv[0]);
	}
	else
		waitpid(pid, NULL, 0);
}

int	main(int argc, char **argv, char **envp)
{
	int	i, fd[2], fd_in, fd_out, pipe_before;
	
	if (argc == 1)
		return (1);
	argv++;
	pipe_before = 0;
	while(*argv)
	{
		i = 0;
		while (*argv && (!strcmp(*argv, "|") || !strcmp(*argv, ";")))
			argv++;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (i == 0)
			continue;
		if (argv[0] && !strcmp(argv[0], "cd"))
		{
			if (i != 2)
				print_error("error: cd: bad arguments", NULL);
			else if (chdir(argv[1]) == -1)
				print_error("error: cd: cannot change directory to ", argv[1]);
		}
		else
		{
			if (pipe_before)
			{
				fd_in = fd[READ];
				close(fd[WRITE]);
			}
			else
				fd_in = dup(READ);
			if (argv[i] && !strcmp(argv[i], "|"))
			{
				if (pipe(fd) == -1)
				{
					print_error("error: fatal", NULL);
					exit(1);
				}
				fd_out = fd[WRITE];
			}
			else
				fd_out = dup(WRITE);
			execute(argv, fd_in, fd_out, envp, i);
			if (pipe_before == 1)
				close(fd_in);
			if (argv[i] && !strcmp(argv[i], "|"))
				pipe_before = 1;
			else
				pipe_before = 0;
		}
		argv += i;
	}
}

