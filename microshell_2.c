# include <stdlib.h>
# include <string.h>
# include <unistd.h>

void	ft_cmd(char** av, char** env);
void	ft_cd(char** av);
void	ft_exit(char* str1, char* str2);
int		ft_strlen(char* str);
int		ft_exec(char**av, int* fd, int pipe_fd[2], char** env);

int	main(int ac, char** av, char** env)
{
	int	i		= 1;
	int	start	= 1;

	if (ac < 2)
		return (0);
	while(av[i])
	{
		if(!strcmp(av[i], ";"))
		{
			av[i] = NULL;
			ft_cmd(&av[start], env);
			i ++;
			while(av[i] && !strcmp(av[i], ";"))
				i ++;
			start = i;
		}
		else
			i ++;
	}
	ft_cmd(&av[start], env);
	return(0);
}

void	ft_cmd(char** av, char** env)
{
	int	i	= 0;
	int	fd	= 0;
	int	pipe_fd[2];

	if(!av[i])
		return ;
	if(!strcmp(av[i], "cd"))
		return(ft_cd(&av[i]));
	while(av[i])
		i += ft_exec(&av[i], &fd, pipe_fd, env);
	close(fd);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
}

void	ft_cd(char** av)
{
	if(!av[1] || av[2])
		ft_exit("error: cd: bad arguments", NULL);
	else
		if(chdir(av[1]))
			ft_exit("error: cd: cannot change directory to ", av[1]);
}

void	ft_exit(char* str1, char* str2)
{
	write(2, str1, ft_strlen(str1));
	if(str2)
		write(2, str2, ft_strlen(str2));
	write(2, "\n", 1);
}

int	ft_strlen(char* str)
{
	int	i = 0;

	while(str[i])
		i ++;
	return (i);
}

int	ft_exec(char** av, int* fd, int pipe_fd[2], char** env)
{
	int		i		= 0;
	int		last	= 0;
	pid_t	pid;

	while(av[i] && strcmp(av[i], "|"))
		i ++;
	if (!av[i])
		last = 1;
	av[i] = NULL;
	if (pipe(pipe_fd) == -1)
		ft_exit("error: fatal", NULL);
	if ((pid = fork()) == -1)
		ft_exit("error: fatal", NULL);
	if(!pid)
	{
		close(pipe_fd[0]);
		dup2(*fd, STDIN_FILENO);
		if (!last)
			dup2(pipe_fd[1], STDOUT_FILENO);
		if(execve(av[0], &av[0], env) == -1)
			ft_exit("error: cannot execute ", av[0]);
	}
	else
	{
		close(pipe_fd[1]);
		if (*fd)
			close(*fd);
		*fd = pipe_fd[0];
		waitpid(-1, NULL, 0);
	}
	if(last)
		return (i);
	return (i + 1);
}