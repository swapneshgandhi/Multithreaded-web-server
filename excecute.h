#ifndef EXCECUTE_H_INCLUDED
#define EXCECUTE_H_INCLUDED
#define BUF_LEN 1024
void write_log(int s,char *in_time,char *th_time,char *wh_req, int status, int length,char clientstr[]);
void tilda_handler(char file_or_dir[]);
char * content_t(char *file_path);
void GET_req(char *file_path, int client_fd,char *in_time, char *th_time,char *wh_req,char clientstr[]);
void HEAD_req(char *file_path, int client_fd,char *in_time, char *th_time,char *wh_req,char clientstr[]);
void dir_ls(char *dirname, int client_fd,char *in_time, char *th_time,char *wh_req,char clientstr[]);
void http_501(int client_fd,char * cmd,char *in_time, char *th_time,char *wh_req,char clientstr[]);
void http_404(char *file_req, int client_fd,char *in_time, char *th_time,char *wh_req,char clientstr[]);

#endif
