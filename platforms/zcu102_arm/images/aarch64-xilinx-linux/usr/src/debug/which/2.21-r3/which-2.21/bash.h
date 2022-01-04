/* From bash-4.3 / general.h / line 228 */
/* Some defines for calling file status functions. */
#define FS_EXISTS         0x1
#define FS_EXECABLE       0x2
#define FS_EXEC_PREFERRED 0x4
#define FS_EXEC_ONLY      0x8
#define FS_DIRECTORY      0x10
#define FS_NODIRS         0x20
#define FS_READABLE       0x40

/* From bash-4.3 / general.h / line 69 */
#define savestring(x) (char *)strcpy(xmalloc(1 + strlen (x)), (x))

extern int file_status(const char *name);
extern int absolute_program(const char *string);
extern char *get_next_path_element(char const* path_list, int *path_index_pointer);
extern char *make_full_pathname(const char *path, const char *name, int name_len);
extern int uidget();
extern char* sh_get_home_dir(void);
