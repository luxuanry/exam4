#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int	ft_error(char *str)
{
	while(*str)
		write(2, str, 1);
	str++;
	return (1);
}

void fatal_error()
{
	ft_error("error: fatal error\n");
	exit(1);
}

int ft_cd(char **av, int ac)
{
	if(ac != 2)
		return(ft_error("error: cd: bad arguments\n"));
	else if(chdir(av[1]) == -1)
		return(ft_error("error: cd: cannot change directory to "), 
		ft_error(av[1]),
		ft_error("\n"));
	return (0);
}

int exec(char **av, char **envp, int i)
{
	int fd[2];
	int pid;
	int status;
	int has_pipe;

	has_pipe  = av[i] && (strcmp(av[i], "|") == 0);
	if(has_pipe &&pipe(fd) == -1)
		fatal_error();
	if((pid = fork()) == -1)
		fatal_error();
	if(pid == 0)
	{
		av[i] = 0;
		if(has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
			fatal_error();
		if(execve(*av, av, envp) == -1)
		{
			ft_error("error: can not execute ");
			ft_error(*av);
			ft_error("\n");
			exit(1);
		}
	}
	if (waitpid(pid, &status, 0) == -1)
		fatal_error();
	if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		fatal_error();
	return(WIFEXITED(status) && WEXITSTATUS(status));
}

int main(int ac, char **av, char **envp)
{
	int status = 0;
	int i = 0;

	if(ac > 1)
	{
		while(av[i] && av[++i])
		{
			av += i;
			i = 0;
			while(av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
				i++;
			if(strcmp(*av, "cd") == 0)
				status = ft_cd (av, i);
			else if(i)
				status = exec(av, envp, i);
		}
	}
	return (status);
}
