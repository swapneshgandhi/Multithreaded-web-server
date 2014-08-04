//This thread responds to user requets and calls appropriate functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pthread.h"
#include "unistd.h"
#include "th.h"
#include "serv_th.h"
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "excecute.h"

extern pthread_mutex_t mut_th_status;
extern pthread_mutex_t mutex_assign;
extern pthread_t *serving_th;
extern pthread_cond_t *thread_wake;
extern pthread_cond_t served;
extern int *thread_status;
extern int NUM_THREADS;
extern struct th *assign_r;
extern int debug;

void *serve_requests(void *threadid)

{
        while (1)
        {
                char clientstr[40];
                long tid;
                struct stat st;
                struct stat st_dir;
                tid = (long)threadid;
                pthread_mutex_lock (&mutex_assign);
                pthread_cond_wait(&thread_wake[tid],&mutex_assign);
                if (debug ==1 ){
                printf("\nThread %ld has woken up and will serve the requets now.",tid);
                fflush(stdout);}
                char *m_time;
                m_time=calloc(BUF_LEN,sizeof(char));
                time_t now;
                now=time(NULL);
                strftime(m_time,100,"%d/%b/%Y:%H:%M:%S GMT",gmtime(&now));
                m_time=strtok(m_time,"\n");
                int fd=assign_r[tid].fdc;
                struct sockaddr_storage client;
                socklen_t leng;
                leng = sizeof (client);
                getpeername(fd, (struct sockaddr*)&client, &leng);

                if (client.ss_family == AF_INET) {
                        struct sockaddr_in *fd = (struct sockaddr_in *)&client;
                        inet_ntop(AF_INET, &fd->sin_addr, clientstr, sizeof clientstr);
                }
                //   printf("***req %s type %s file name %s size %ld file discriptor %d time %s",assign_r[tid].wh_req,assign_r[tid].req_type,assign_r[tid].file_or_dir_name,assign_r[tid].size, assign_r[tid].fdc,assign_r[tid].in_time);

                //      printf("thread %ld released mutex assign",tid);
                if (strcmp (assign_r[tid].req_type,"GET") != 0 && strcmp (assign_r[tid].req_type,"HEAD") != 0)
                {
                        http_501(assign_r[tid].fdc,assign_r[tid].req_type,assign_r[tid].in_time,m_time,assign_r[tid].wh_req,clientstr);

                        goto last; }
               else if (stat(assign_r[tid].file_or_dir_name, &st) != 0)
                {
                        http_404(assign_r[tid].file_or_dir_name,assign_r[tid].fdc,assign_r[tid].in_time,m_time,assign_r[tid].wh_req,clientstr);

                        goto last; }
               else if( st.st_mode & S_IFDIR )
                {
                        char dir_t[40];

                        strcpy(dir_t,assign_r[tid].file_or_dir_name);

                        if ( dir_t[strlen(dir_t)-1] != '/' ){
                                sprintf(dir_t+strlen(dir_t),"/index.html");
                        }
                        else {
                                sprintf(dir_t+strlen(dir_t),"index.html");
                        }

                        if (stat(dir_t, &st_dir) == 0 && strcmp (assign_r[tid].req_type,"GET") == 0){
                                //printf("\nnew dir is %s",dir_t);
                                GET_req(dir_t, assign_r[tid].fdc,assign_r[tid].in_time,m_time,assign_r[tid].wh_req,clientstr);

                                goto last;
                        }
                        else if (stat(dir_t, &st_dir) == 0 && strcmp (assign_r[tid].req_type,"HEAD") == 0){
                                HEAD_req(dir_t,assign_r[tid].fdc,assign_r[tid].in_time,m_time,assign_r[tid].wh_req,clientstr);

                                goto last;
                        }
                        else if (stat(dir_t, &st_dir) != 0 && strcmp (assign_r[tid].req_type,"GET") == 0){
                        dir_ls(assign_r[tid].file_or_dir_name,assign_r[tid].fdc,assign_r[tid].in_time,m_time,assign_r[tid].wh_req,clientstr);

						goto last;}	
						else if (stat(dir_t, &st_dir) != 0 && strcmp (assign_r[tid].req_type,"HEAD") == 0){
						HEAD_req(assign_r[tid].file_or_dir_name,assign_r[tid].fdc,assign_r[tid].in_time,m_time,assign_r[tid].wh_req,clientstr);

						goto last;                        
                        }
                }
		
                else if (strcmp (assign_r[tid].req_type,"GET") == 0)
                {
                        GET_req(assign_r[tid].file_or_dir_name, assign_r[tid].fdc,assign_r[tid].in_time,m_time,assign_r[tid].wh_req,clientstr);

                        goto last;
                }
                else if (strcmp (assign_r[tid].req_type,"HEAD") == 0)
                {
			
                        HEAD_req(assign_r[tid].file_or_dir_name,assign_r[tid].fdc,assign_r[tid].in_time,m_time,assign_r[tid].wh_req,clientstr);

                        goto last; }
		else {
		http_404(assign_r[tid].file_or_dir_name,assign_r[tid].fdc,assign_r[tid].in_time,m_time,assign_r[tid].wh_req,clientstr);

		goto last;}

		last:	if(debug ==1){
				printf("\nThe request has been served");
				fflush(stdout);}
		
				pthread_mutex_unlock (&mutex_assign);
                pthread_mutex_lock(&mut_th_status);
                //              dprintf("locked status array from thread");
                thread_status[tid]=0;
                pthread_cond_signal(&served);
                pthread_mutex_unlock(&mut_th_status);
        }
}
