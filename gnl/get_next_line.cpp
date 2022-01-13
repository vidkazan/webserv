#include "get_next_line.hpp"

static int	get_next_line_gen(char **s, char **line)
{
	int		i;
	char	*tmp;

	i = 0;
	while ((*s)[i] != '\n')
		i++;
	*line = ft_substr(*s, 0, i);
	if (*line)
	{
		tmp = ft_strdup(ft_strchr(*s, '\n') + 1);
		if (tmp)
		{
			free(*s);
			*s = tmp;
			return (0);
		}
	}
	free(*s);
	return (-1);
}

static int	get_next_line_res(char **s, char **line)
{
	if (ft_strchr(*s, '\n'))
	{
		if (get_next_line_gen(s, line) == -1)
			return (-1);
		return (1);
	}
	else if (*s)
	{
		*line = ft_strdup(*s);
		free(*s);
		*s = NULL;
		if (!(*line))
			return (-1);
		return (0); 
	}
	else
		return (-1);
}

static int	get_next_line_join(char **s, char *buf)
{
	char		*tmp;

	tmp = ft_strjoin(*s, buf);
	free(*s);
	if (!(*tmp))
		return (-1);
	*s = tmp;
	return (0);
}

int	get_next_line(int fd, char **line)
{
	int			bytes_read;
	char		buf[BUFFER_SIZE + 1];
	static char	*s;

	bytes_read = 1;
	if (BUFFER_SIZE <= 0 || fd < 0 || !line)
		return (-1);
	while (bytes_read)
	{
		bytes_read = read(fd, buf, BUFFER_SIZE);
		if (bytes_read < 0)
			return (-1);
		buf[bytes_read] = 0;
		if (!s)
			s = ft_strdup(buf);
		else if (*buf)
		{
			if (get_next_line_join(&s, buf) == -1)
				return (-1);
		}
		if (!*s || ft_strchr(s, '\n') || bytes_read < BUFFER_SIZE)
			break ;
	}
	return (get_next_line_res(&s, line));
}
