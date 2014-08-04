// this thread function schedules the requets in the ready queue


#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include <time.h>
#include <sys/stat.h>
#include "pthread.h"
#include "unistd.h"
#include "queue.h"
#include "que_func.h"
#include "th.h"
#include "schd_th.h"

extern struct th *assign_r;
extern struct queue *start;
extern pthread_mutex_t mut_que;
extern pthread_mutex_t mut_th_status;
extern pthread_mutex_t mutex_assign;
extern pthread_cond_t *thread_wake;
extern pthread_cond_t new_in_que;
extern pthread_cond_t served;
extern int *thread_status;
extern int sleep_t;
extern int sort;
extern int debug;
extern int NUM_THREADS;

void *schedlr_fn(void *arg)
{
        struct queue *assign=NULL;
        int i;
        int tc;
        int count;
        assign=(struct queue*) calloc(1,sizeof(struct queue));
        sleep(sleep_t);
        if (debug ==1 ){
        printf("Schedular will start scheduling now after initial %d seconds wait",sleep_t);
        fflush(stdout);}
        while(1)
        {
                //frr_th=5;
                //                      printf("***locking..\n");
                pthread_mutex_lock (&mut_que);
                i=traverse_queue_wh();
                if (i == 0){
                        //                      printf("waiting..\n");
                        pthread_cond_wait(&new_in_que, &mut_que);}

                //printf("i value before sorting is %d",i);
                i=traverse_queue_wh();
                if (sort ==1 && i !=0 ){
                        schedule_sjf(i);}

                //printf("i value after sorting is %d",i);
                pthread_mutex_unlock (&mut_que);
                //              printf("schedular freed mutex");

                while ( i != 0)
                {
                        

                        for (tc =0; tc <=NUM_THREADS-1 ; tc++)
                        {
						pthread_mutex_lock(&mut_th_status);
						
                                if ( thread_status[tc] == 0)
                                {
                                        if (debug ==1 ){
                                        printf("\nSchedular found a free thread %d",tc);
                                        fflush(stdout);}
                                        pthread_mutex_lock(&mut_que);
                                        pthread_mutex_lock(&mutex_assign);
                                        assign=start;
                                        strcpy(assign_r[tc].wh_req,assign->wh_req);
                                        strcpy(assign_r[tc].req_type,assign->req_type);
                                        strcpy(assign_r[tc].file_or_dir_name,assign->file_or_dir_name);
                                        assign_r[tc].size=assign->size;
                                        assign_r[tc].fdc=assign->fdc;
                                        strcpy(assign_r[tc].in_time,assign->in_time);
                                        start=start->next;
                                        free(assign);
                                        pthread_mutex_unlock(&mut_que);
                                        pthread_cond_signal(&thread_wake[tc]);	
                                        pthread_mutex_unlock(&mutex_assign);
                                        thread_status[tc]=1;
                                        pthread_mutex_unlock(&mut_th_status);
                                        break;
                                }
                                else if ( thread_status[tc] != 0 && tc == NUM_THREADS-1 )
                                {

                                          pthread_mutex_unlock(&mut_th_status);   
//                                        pthread_cond_wait(&served,&mut_th_status);
										  sleep(2);
										
                                              //       printf("\n**schedular will try schedulling again..");
                                        tc=0;
                                }
								else {pthread_mutex_unlock(&mut_th_status);}
                        }
                        i--;
                }
        }
        pthread_exit(0);
}

