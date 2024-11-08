# include <errno.h>
# include <fcntl.h>
# include <stddef.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

typedef struct s_apa
{
    char    *cmd;
    char    **paths;
    pid_t   *pid;
    char    **envp;
    char    **av;
    char    **cmd_args;
    int     **pipes;
    int     status;
    int     flag;
    int     ac;
} t_apa;

// couvert ( ./a.out infile "ls -la" cat outfile ) to ( ./a.out "ls -la" cat )
// remove infile and outfile from av


void		ft_putstr_fd(char *s, int fd);
int			werror(char *err);
char		**ft_split(char const *s, char c);
size_t		ft_strlen(const char *s);
char		*ft_strdup(const char *s);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
char		*ft_strjoin(char const *s1, char const *s2);
void		*ft_calloc(size_t nmemb, size_t size);
size_t		ft_strlcpy(char *dst, const char *src, size_t size);
void		check_infile(t_apa *pipex);
char		**get_args(char *cmd);
void		find_cmd(t_apa *pipex, int i);
void		ft_free(char **str);
void		check_outfile(t_apa *pipex);
void		ft_process(t_apa *pipex, int i);
void		get_cmd_paths(t_apa *pipex);
void		get_cmd(t_apa *pipex);
void		p_werror(char **err, int flag);
void		find_lite(t_apa *pipex, int i);
void		ft_bzero(void *s, size_t n);

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
	{
		i++;
	}
	return (i);
}

char	*ft_strdup(const char *s)
{
	char	*str;
	size_t	ls;

	ls = ft_strlen(s) + 1;
	str = (char *)ft_calloc(ls, sizeof(char));
	if (!str)
	{
		return (NULL);
	}
	ft_strlcpy(str, s, ls);
	return (str);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && (s1[i] || s2[i]))
	{
		if (s1[i] != s2[i])
		{
			return (s1[i] - s2[i]);
		}
		i++;
	}
	return (0);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	size_t	lent;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (!s1 || !s2)
		return (NULL);
	lent = ft_strlen(s1) + ft_strlen(s2);
	str = malloc(lent + 1);
	if (!str)
		return (NULL);
	while (s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	while (s2[j])
		str[i++] = s2[j++];
	str[i] = '\0';
	return (str);
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*ptr;
	size_t	s;

	s = nmemb * size;
	if (s > 0)
	{
		if (SIZE_MAX / nmemb < size)
			return (NULL);
		s = nmemb * size;
		ptr = malloc(s);
		if (!ptr)
			return (NULL);
		ft_bzero(ptr, s);
		return (ptr);
	}
	ptr = malloc(1);
	ft_bzero(ptr, 1);
	return (ptr);
}


void	ft_bzero(void *s, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		((unsigned char *)s)[i] = '\0';
		i++;
	}
}

void	get_cmd(t_apa *pipex)
{
	int		i;
	char	*tmp;
	char	*tmp1;

	i = 0;
	while (pipex->paths && pipex->paths[i])
	{
		tmp1 = ft_strjoin(pipex->paths[i], "/");
		tmp = ft_strjoin(tmp1, pipex->cmd_args[0]);
		free(tmp1);
		if (!access(tmp, F_OK & X_OK))
		{
			pipex->cmd = tmp;
			return ;
		}
		free(tmp);
		i++;
	}
	ft_putstr_fd("command not found\n", 2);
	exit(127);
}

void	p_werror(char **err, int flag)
{
	if (flag)
	{
		while(*err && *(err + 1) && err++);
		perror(*err);
		exit(1);
	}
	else
	{
		perror(err[1]);
		exit(1);
	}
}

// void	check_infile(t_apa *pipex)
// {
// 	pipex->infile = open(pipex->av[1], O_RDONLY);
// 	if (pipex->infile < 0)
// 		p_werror(pipex->av, 0);
// 	else
// 	{
// 		dup2(pipex->infile, 0);
// 		dup2(pipex->fd[1], 1);
// 		close(pipex->infile);
// 		close(pipex->fd[1]);
// 		close(pipex->fd[0]);
// 	}
// }

char	**get_args(char *cmd)
{
	int		i;
	char	**args;

	i = 0;
	args = NULL;
	args = ft_split(cmd, ' ');
	return (args);
}


int	found_slash(char *str)
{
	while (str && *str && *str != '/' && str++)
		;
	if (!(*str))
		return (0);
	return (1);
}

void	find_lite(t_apa *pipex, int i)
{
	if (open(pipex->av[i + 1], O_DIRECTORY) >= 0)
	{
		ft_putstr_fd("Is a directory\n", 2);
		ft_free(pipex->paths);
		ft_free(pipex->cmd_args);
		exit(126);
	}
}

void	find_cmd(t_apa *pipex, int i)
{
	pipex->cmd_args = get_args(pipex->av[i + 1]);
	if (!pipex->cmd_args)
	{
		ft_free(pipex->paths);
		exit(20);
	}
	if (found_slash(pipex->av[i + 1]))
	{
		pipex->cmd = pipex->cmd_args[0];
		pipex->flag = 1;
		return ;
	}
	find_lite(pipex, i);
	if (!access(pipex->av[i + 1], X_OK))
	{
		pipex->cmd = pipex->cmd_args[0];
		pipex->flag = 1;
		return ;
	}
	get_cmd(pipex);
}

void	ft_free(char **str)
{
	int	i;

	i = 0;
	if (!str || !*str)
		return (free(str));
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}

// void	check_outfile(t_apa *pipex)
// {
// 	pipex->outfile = open(pipex->av[pipex->ac - 1], O_TRUNC | O_CREAT | O_RDWR, 0644);
// 	if (pipex->outfile < 0)
// 		p_werror(pipex->av, 1);
// 	else
// 	{
// 		dup2(pipex->outfile, 1);
// 		close(pipex->outfile);
// 		close(pipex->fd[1]);
// 		close(pipex->fd[0]);
// 	}
// }


void	ft_putstr_fd(char *s, int fd)
{
	if (!s || fd < 0)
		return ;
	write(fd, s, ft_strlen(s));
}

void	j_werror(char *err)
{
	perror(err);
	exit(1);
}

int	werror(char *err)
{
	write(2, err, ft_strlen(err));
	return (1);
}

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	len;

	i = 0;
	len = ft_strlen(src);
	if (size < 1)
	{
		return (len);
	}
	while (src && src[i] && i < (size - 1))
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (len);
}


static char	**free_str(char **str)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
	return (NULL);
}

static int	cword(char const *s, char c)
{
	int	i;
	int	t;

	t = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			while (s[i] && s[i] != c)
				i++;
			t++;
		}
		if (s[i])
			i++;
	}
	return (t);
}

static char	**strt(char const *s, char c, char **str)
{
	int	i;
	int	j;
	int	t;

	t = 0;
	i = 0;
	j = 0;
	str[cword(s, c)] = NULL;
	while (s[i])
	{
		if (s[i] != c)
		{
			while (s[i] && s[i++] != c)
				t++;
			i--;
			str[j] = ft_calloc(t + 1, sizeof(char));
			if (!str[j++])
				return (free_str(str));
			t = 0;
		}
		i++;
	}
	return (str);
}

static char	**strmalloc(char const *s, char c)
{
	char	**str;

	if (!s)
		return (NULL);
	str = malloc(sizeof(char *) * (1 + cword(s, c)));
	if (!str)
		return (NULL);
	str = strt(s, c, str);
	return (str);
}

char	**ft_split(char const *s, char c)
{
	int		i;
	int		j;
	int		g;
	char	**str;

	i = 0;
	j = 0;
	g = 0;
	str = strmalloc(s, c);
	while (s && s[i])
	{
		while (s[i] != c && s[i] != '\0')
		{
			str[j][g] = s[i];
			g++;
			if (s[i++ + 1] == c)
			{
				j++;
				g = 0;
			}
		}
		if (s[i++] == '\0')
			return (str);
	}
	return (str);
}

void ft_processing(t_apa *pipex, int i)
{
    if (i == 0) {
        // First command: output to pipe
        dup2(pipex->pipes[i][1], STDOUT_FILENO);
    } else if (i == pipex->ac - 2) {
        // Last command: input from pipe
        dup2(pipex->pipes[i-1][0], STDIN_FILENO);
    } else {
        // Middle commands: input from previous pipe, output to next pipe
        dup2(pipex->pipes[i-1][0], STDIN_FILENO);
        dup2(pipex->pipes[i][1], STDOUT_FILENO);
    }

    // Close all pipe file descriptors
    for (int j = 0; j < pipex->ac - 2; j++) {
        close(pipex->pipes[j][0]);
        close(pipex->pipes[j][1]);
    }

    find_cmd(pipex, i);
}

void ft_process(t_apa *pipex, int i)
{
    pipex->pid[i] = fork();
    if (pipex->pid[i] < 0)
        exit(20);
    if (pipex->pid[i] == 0)
    {
        ft_processing(pipex, i);
        execve(pipex->cmd, pipex->cmd_args, pipex->envp);
        perror(pipex->cmd);
        ft_free(pipex->paths);
        ft_free(pipex->cmd_args);
        if (!pipex->flag)
            free(pipex->cmd);
        exit(126);
    }
}

void	get_cmd_paths(t_apa *pipex)
{
	int	i;

	i = 0;
	pipex->paths = NULL;
	if (!pipex->envp && !pipex->envp[0])
		return ;
	while (pipex->envp[i])
	{
		if (!ft_strncmp(pipex->envp[i], "PATH=", 5))
		{
			pipex->paths = ft_split(pipex->envp[i], ':');
			break ;
		}
		i++;
	}
}

void get_start(t_apa *pipex)
{
    int i;

    // Create pipes
    pipex->pipes = malloc(sizeof(int *) * (pipex->ac - 2));
    for (i = 0; i < pipex->ac - 2; i++) {
        pipex->pipes[i] = malloc(sizeof(int) * 2);
        if (pipe(pipex->pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    // Create processes
    pipex->pid = malloc(sizeof(pid_t) * (pipex->ac - 1));
    for (i = 0; i < pipex->ac - 1; i++) {
        ft_process(pipex, i);
    }

    // Close all pipe file descriptors in parent
    for (i = 0; i < pipex->ac - 2; i++) {
        close(pipex->pipes[i][0]);
        close(pipex->pipes[i][1]);
    }
}


int main(int ac, char **av, char **envp)
{
    t_apa pipex;
    int i;

    pipex.envp = envp;
    pipex.av = av;
    pipex.ac = ac;
    pipex.cmd_args = NULL;
    pipex.cmd = NULL;
    pipex.paths = NULL;
    pipex.flag = 0;
    if (ac < 3)
        return (werror("Not enough arguments\n"));
    get_cmd_paths(&pipex);
    get_start(&pipex);

    // Wait for all child processes
    for (i = 0; i < pipex.ac - 1; i++) {
        waitpid(pipex.pid[i], &pipex.status, 0);
    }

    // Free allocated memory
    for (i = 0; i < pipex.ac - 2; i++) {
        free(pipex.pipes[i]);
    }
    free(pipex.pipes);
    free(pipex.pid);
    ft_free(pipex.paths);

    exit(WEXITSTATUS(pipex.status));
}