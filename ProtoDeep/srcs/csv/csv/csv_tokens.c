#include "pd_main.h"

void        pd_csv_free_tokens_list(pd_tokens_list *tokens)
{
    pd_tokens_list   *tmp;

    while (tokens)
    {
            tmp = tokens;
            tokens = tokens->next;
            pd_dast_free((void**)&(tmp->tokens.val));
            pd_dast_free((void**)&tmp);
    }
}

pd_char_a pd_csv_retrieve_line(pd_char_a *content)
{
    pd_char_a     line;
    size_t      len;

    if ((len = pd_str_chr(*content, '\n')))
    {
       line = pd_str_dup(*content, len - 2);
        *content = pd_str_fsub(*content, len, content->len);
    }
    else
    {
        len = pd_str_len(*content);
        line = pd_str_dup(*content, len);
        *content = pd_str_fsub(*content, len, content->len);
    }
    return (line);
}

int     pd_csv_get_line(int fd, pd_char_a *line)
{
    pd_char_a             buf;
    size_t              ret;
    static pd_char_a      content;

    buf = pd_arrInit(PD_T_CHAR, 64);
    if (fd < 0 || line->val == NULL || read(fd, buf.val, 0) < 0)
        return (-1);
    if (!content.val)
        content = pd_arrInit(PD_T_CHAR, 0);
    while ((ret = read(fd, buf.val, 64)))
    {
        ((char *)buf.val)[ret] = '\0';
        content = pd_str_fjoin(content, pd_str_dup(buf, buf.len));
        if (!content.val)
            return (-1);
        if (pd_str_chr(content, '\n'))
            break;
    }
	pd_dast_free((void**)&buf.val);
    if (ret < 64 && !str_len(content))
    {
		free(content.val);
		content.val = NULL;
        return (0);
    }
	pd_dast_free((void**)&line->val);
	line->val = NULL;
    *line = pd_csv_retrieve_line(&(content));
    if (!line->val)
        return (-1);
	return (1);
}

pd_tokens_list   *pd_csv_add_tokens(pd_arr tokens)
{
    pd_tokens_list   *elem;

    elem = (pd_tokens_list *)malloc(sizeof(pd_tokens_list));
    elem->tokens = tokens;
    elem->next = NULL;
    return (elem);
}

pd_tokens_list   *pd_csv_wrong_width(size_t i, size_t first_width, size_t width)
{
    write(2, "read_csv error: ", 16);
    if (first_width > width)
        write(2, "Not enough values on line ", 26);
    else if (first_width < width)
        write(2, "Too much values on line ", 24);
    pd_arr line = pd_math_itoa(i + 1);
    write(2, (char *)line.val, line.len);
    write(2, ".\n", 2);
    return (NULL);
}

pd_tokens_list   *pd_csv_create_tokens_list(int fd, char separator, size_t *height, size_t *width)
{
    pd_arr           *line = NULL;
    pd_tokens_list   *list = NULL;
    pd_tokens_list   *curr = NULL;
    pd_arr           tokens;
    size_t          first_width = 0;

    line = (pd_arr *)malloc(sizeof(pd_arr));
    *line = pd_arrInit(PD_T_CHAR, 0);
    ((char *)line->val)[0] = 0;
    while (pd_csv_get_line(fd, line))
    {
        if (!line || line->len == 0)
            continue;
        tokens = pd_str_split(*line, separator, 1);
        if (tokens.len == 0)
            continue;
        if ((!first_width && !(first_width = tokens.len)) || (first_width != tokens.len))
            return (pd_csv_wrong_width(*height, first_width, tokens.len));
        if ((!list && !(list = pd_csv_add_tokens(tokens)))
            || (curr && !(curr->next = pd_csv_add_tokens(tokens))))
            return (NULL);
        curr = curr ? curr->next : list;
        *height = *height + 1;
    }
    *width = first_width;
	pd_dast_free((void**)&line->val);
	pd_dast_free((void**)&line);
    close(fd);
    return (list);
}
