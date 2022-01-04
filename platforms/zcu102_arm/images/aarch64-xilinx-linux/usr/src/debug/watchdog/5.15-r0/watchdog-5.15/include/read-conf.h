#ifndef READ_CONF_H
#define READ_CONF_H

typedef struct read_list_s {
	const char	*name;
	int			value;
} read_list_t;

#define READ_LIST_ADD(name, value) {name, value},
#define READ_LIST_ENUM(enumv) {#enumv, enumv},
#define READ_LIST_END() {NULL, 0}

typedef enum {
	Read_allow_blank = 1,
	Read_string_only = 2
} string_read_e;

/** read-conf.c **/
void trim_white(char *buf);
char *str_start(char *p);

int read_int_func(char *arg, char *val, const char *name, int imin, int imax, int *iv);
int read_string_func(char *arg, char *val, const char *name, string_read_e mode, char **str);
int read_enumerated_func(char *arg, char *val, const char *name, const read_list_t list[], int *iv);

int read_list_func(char *arg, char *val, const char *name, int version, struct list **list);

void add_list(struct list **list, const char *name, int version);
void free_list(struct list **list);

#endif /*READ_CONF_H*/
