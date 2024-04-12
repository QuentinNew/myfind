#ifndef ERROR_H
#define ERROR_H

void error_arg(char msg[], char *text);

void error_exit(char msg[]);
int file_exist(const char *path, int follow);
int is_symbolic_link(const char *path);
void error_file(const char *path);
#endif /* !ERROR_H */
