// This file contains various the functions to execute requests and send data to client.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "excecute.h"
#include "queue.h"
#include <arpa/inet.h>
#include <pwd.h>

extern char *log_file;
extern int log_s;
extern int debug;
extern FILE *log_f;


void write_log(int s,char *in_time,char *th_time,char *wh_req, int status, int length,char clientstr[])
{


	if ( log_s ==1){
		log_f=fopen(log_file,"a");
		fprintf(log_f,"%s - [%s ] [%s ] \"%s\" %d %d\r\n",clientstr,in_time,th_time,wh_req,status,length);
		fclose(log_f);}
	if ( debug == 1){
		fprintf(stdout,"\n**The log of the request received is as follows:\n");
				fflush(stdout);
		fprintf(stdout,"%s - [%s ] [%s ] \"%s\" %d %d\r\n",clientstr,in_time,th_time,wh_req,status,length);
		fflush(stdout);}
	
}


void tilda_handler(char *file_or_dir)
{
	char path[40];
	char user[40];
	char *cont1=NULL;
	int tpos, i,c;
	int sl,st;
	char *tilda=NULL;
	struct passwd *u_name;

	strcpy(path,file_or_dir);
	char *til=strstr (path, "~");
	if (til){
		char *slash=strstr(til+1,"/");
		strcpy(user,til+1);
		cont1=strtok(user,"/");
		u_name=getpwnam((const char*) cont1);
		if (u_name){
			sprintf(file_or_dir,"%s/myhttpd%s",u_name->pw_dir,slash);}

	}					  
}
char * content_t(char *file_path)
{
	char *ext;
	ext=strrchr(file_path,'.');
	char *content_type=(char *)calloc(20,sizeof(char));
	ext=ext+1;
	if (!ext)
	{

		strcpy(content_type,"text/plain");
	}
	else if ( strcmp(ext,"html") == 0)
	{
		strcpy(content_type,"text/html");
	}
	else if ( strcmp(ext, "gif") == 0 ){
		strcpy(content_type,"image/gif");}
	else if ( strcmp(ext, "jpg") == 0 ){
		strcpy(content_type,"image/jpeg");}
	else if ( strcmp(ext, "jpeg") == 0 ){
		strcpy(content_type,"image/jpeg");
	}
	else {
		strcpy(content_type,"text/plain");
	}
	return content_type;
}

void GET_req(char *file_path, int client_fd,char *in_time, char *th_time,char *wh_req,char clientstr[])
{
	FILE *fout;
	char *fcontent;
	char *server_s="Swapnesh's Server for CSE521 V1.0";
	char *content_type;
	time_t now;
	struct stat st;
	stat(file_path,&st);
	char *m_time;
	int status=200;
	m_time=calloc(BUF_LEN,sizeof(char));
	strftime(m_time,100,"%c",gmtime(&st.st_mtime));
	m_time=strtok(m_time,"\n");
	now=time(NULL);
	fout=fopen(file_path,"r");

	size_t byt_read;
	size_t byt_sent;
	char *date=asctime(gmtime(&now));
	date=strtok(date,"\n");
	fcontent=(char *)calloc(BUF_LEN,sizeof(char));
	content_type=content_t(file_path);

	sprintf(fcontent,"HTTP/1.0 200 OK\r\nDate: %s GMT\r\nServer: %s\r\nLast-Modified: %s GMT\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", date, server_s,m_time,content_type,(int)st.st_size);
	send(client_fd,fcontent,strlen(fcontent),0);
	if (debug ==1 ){fprintf(stdout,"\nThe response to client was:\n");
	fflush(stdout);
	fprintf(stdout,fcontent);
	fflush(stdout);}
	while ( !feof(fout))
	{
		memset((void *)fcontent, 0, sizeof(fcontent));
		fgets(fcontent,BUF_LEN,fout);

		send(client_fd,fcontent,strlen(fcontent),0);
		if (debug ==1 ) {fprintf(stdout,fcontent);
		fflush(stdout);}

	}

	fclose(fout);
	close(client_fd);
	if ( (log_s == 1 && log_file != NULL) || debug ==1 )
	{
		write_log(client_fd,in_time,th_time,wh_req,status,(int)st.st_size,clientstr);
	}

}

void HEAD_req(char *file_path, int client_fd,char *in_time, char *th_time,char *wh_req,char clientstr[])
{
	char *fcontent;
	char *server_s="Swapnesh's Server for CSE521 V1.0";
	char *content_type;
	time_t now;
	struct stat st;
	int status=200;
	char *m_time;
	stat(file_path,&st);
	m_time=calloc(BUF_LEN,sizeof(char));
	strftime(m_time,100,"%c",gmtime(&st.st_mtime));
	m_time=strtok(m_time,"\n");
	now=time(NULL);
	char *date=asctime(gmtime(&now));
	date=strtok(date,"\n");

	fcontent=(char *)calloc(BUF_LEN,sizeof(char));
	content_type=content_t(file_path);
	sprintf(fcontent,"HTTP/1.0 200 OK\r\nDate: %s GMT\r\nServer: %s\r\nLast-Modified: %s GMT\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", date, server_s,m_time,content_type,(int)st.st_size);

	send(client_fd,fcontent,strlen(fcontent),0);


	close(client_fd);
	if ( (log_s == 1 && log_file != NULL) || debug ==1 )
	{
		write_log(client_fd,in_time,th_time,wh_req,status,(int)st.st_size,clientstr);
	}
	if (debug ==1 ){fprintf(stdout,"\nThe response to client was:\n");
	fflush(stdout);
	fprintf(stdout,fcontent);
	fflush(stdout);}

}


void  dir_ls(char *dirname, int client_fd,char *in_time, char *th_time,char *wh_req,char clientstr[])
{
	FILE *pipe_ls;
	char *fcontent;
	size_t total;
	size_t byt_sent;
	time_t now;
	now=time(NULL);
	char *content_type="text/plain";
	char *server_s="Swapnesh's Server for CSE521 V1.0";
	struct stat st;
	char *m_time;
	int status=200;
	char cmd [40];
	sprintf(cmd,"ls -1 ");
	sprintf(cmd+strlen(cmd),dirname);
	stat(dirname,&st);
	m_time=calloc(BUF_LEN,sizeof(char));
	strftime(m_time,100,"%c",gmtime(&st.st_mtime));
	m_time=strtok(m_time,"\n");
	char *date=asctime(gmtime(&now));
	date=strtok(date,"\n");
	fcontent=(char *)calloc(BUF_LEN,sizeof(char));
	sprintf(fcontent,"HTTP/1.0 200 OK\r\nDate: %s GMT\r\nServer: %s\r\nLast-Modified: %s GMT\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", date, server_s,m_time,content_type,0);

	byt_sent=send(client_fd,fcontent,strlen(fcontent),0);
	if (debug ==1 ){fprintf(stdout,"\nThe response to client was:\n");
	fflush(stdout);
	fprintf(stdout,fcontent);
	fflush(stdout);}
	pipe_ls=popen(cmd,"r");
	total=0;
	while (!feof(pipe_ls))
	{
		strcpy(fcontent,"");
		fgets(fcontent,BUF_LEN,pipe_ls);
		byt_sent=send(client_fd,fcontent,strlen(fcontent),0);
		total=total+byt_sent;
	}
	pclose(pipe_ls);
	int z=1;
	z=(int) total;

	if ( !z)
	{
		sprintf(fcontent,"The directory you requested is empty\r\n");
		byt_sent=send(client_fd,fcontent,strlen(fcontent),0);
	}
	close(client_fd);
	if ( (log_s == 1 && log_file != NULL) || debug ==1 )
	{
		write_log(client_fd,in_time,th_time,wh_req,status,(int)st.st_size,clientstr);
	}
	if ( debug ==1 ){

		fprintf(stdout,fcontent);
		fflush(stdout);

	}

}


void http_501(int client_fd,char * cmd,char *in_time, char *th_time,char *wh_req,char clientstr[])
{

	char *fcontent;
	time_t now;
	now=time(NULL);
	int status=501;
	size_t byt_sent;
	char *server_s="Swapnesh's Server for CSE521 V1.0";
	fcontent=malloc(BUF_LEN*sizeof(char));
	char *date=asctime(gmtime(&now));
	date=strtok(date,"\n");
	sprintf(fcontent, "HTTP/1.0 501 Not Implemented\r\nDate: %s GMT\r\nServer: %s\r\nContent-type: text/plain\r\nThe command %s is not yet implemented\r\n\r\n",date,server_s,cmd);

	byt_sent=send(client_fd,fcontent,strlen(fcontent),0);
	if ( (log_s == 1 && log_file != NULL) || debug ==1 )
	{
		write_log(client_fd,in_time,th_time,wh_req,status,0,clientstr);
	}
	if ( debug ==1 ){
		fprintf(stdout,"\nThe response to client was:\n");
		fflush(stdout);
		fprintf(stdout,fcontent);
		fflush(stdout);

	}
	close(client_fd);

}
void http_404(char *file_req, int client_fd,char *in_time,char *th_time,char *wh_req,char clientstr[])
{
	char *fcontent;
	size_t byt_sent;
	time_t now;
	int status=404;
	now=time(NULL);
	char *server_s="Swapnesh's Server for CSE521 V1.0";
	char *date=asctime(gmtime(&now));
	date=strtok(date,"\n");
	fcontent=malloc(BUF_LEN*sizeof(char));
	sprintf(fcontent, "HTTP/1.0 404 Not Found\r\nDate: %s GMT\r\nServer: %s\r\nContent-type: text/plain\r\nThe item you requested: %s is not found\r\n\r\n",date,server_s,file_req);
	byt_sent=send(client_fd,fcontent,strlen(fcontent),0);
	if ( (log_s == 1 && log_file != NULL) || debug ==1 )
	{
		write_log(client_fd,in_time,th_time,wh_req,status,0,clientstr);
	}
	if ( debug ==1 ){
		fprintf(stdout,"\nThe response to client was:");
		fflush(stdout);
		fprintf(stdout,fcontent);
		fflush(stdout);

	}

	close(client_fd);
}																		
