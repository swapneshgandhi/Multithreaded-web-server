/* This file contains functions add_queue traverse queue and sorting of queue based on file sizes.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "que_func.h"
#include "queue.h"
#include "excecute.h"
#include "time.h"

#define BUF_LEN 1024
extern struct queue *start;
extern char *def_dir;
extern int r;
struct que_arr
{
        char wh_req[50];
        char req_type[10];
        char file_or_dir_name[40];
        long size;
        int fdc;
        char in_time[50];
};


void add_queue(char *wh_command,int file_disc)
{
        struct queue *p, *temp=NULL;
        struct stat st;
        char cmd[BUF_LEN], argt[BUF_LEN];
        temp=start;
        p=(struct queue*) calloc(1,sizeof(struct queue));
        char tmp[40];
        char temp_v[40];
        char *m_time;
        m_time=calloc(BUF_LEN,sizeof(char));
        strcpy(argt,"");
        sscanf(wh_command, "%s%s", cmd, argt);
        //              printf("\n cmd:%s",cmd);
        wh_command=strtok(wh_command,"\r\n");
        strcpy(p->wh_req,wh_command);
        strncpy(p->req_type,cmd,(10-1));
        //      printf("\n argt:%s",argt);
        if ( strchr(argt, '~') ) {
                tilda_handler(argt);
                //printf("\nfile name after handling tilda is:%s",argt);
                goto jump1;
        }

        else{
                strcpy(tmp,argt);
                if( tmp[0] != '/'){
                        strcpy(temp_v,tmp);
                        strcpy(tmp,"/");
                        strcat(tmp,temp_v);}
                //printf("**1 tmp:%s,argt:%s",tmp,argt);
                if ( r==1 ){
                        strcpy(argt,def_dir);
                        if ( argt[strlen(argt)-1] == '/' ){
                                argt[strlen(argt)-1]=0;
                        }
                }
                else {
                        strcpy(argt,".");
                        //printf("**2tmp:%s,argt:%s",tmp,argt);
                }

                sprintf(argt+strlen(argt),tmp);
                //printf("**3tmp:%s,argt:%s",tmp,argt);
        }
        jump1:                                                          //printf("\n argt after everything:%s",argt);
        strncpy(p->file_or_dir_name,argt,(40-1));
        if (stat(argt, &st) == 0 && strcmp (cmd,"GET"))
        {
                if( st.st_mode & S_IFREG )
                        p->size=st.st_size;
        }
        else {p->size=0;}

        p->fdc=file_disc;
        time_t now;
        now=time(NULL);
        strftime(m_time,100,"%d/%b/%Y:%H:%M:%S GMT",gmtime(&now));

        m_time=strtok(m_time,"\n");
        //printf("time is %s",m_time);
        strcpy(p->in_time,m_time);
        p->next=NULL;
        //printf("req is %s:",p->wh_req);
        if (start==NULL)
        {
                start=p;
        }
        else
        {
                while(temp->next != NULL)
                {
                        temp=temp->next;
                }
                temp->next=p;
        }
}


int traverse_queue_wh()
{
        struct queue *tra=NULL;
        int c;
        tra=start;
        //      printf("\nin traverse,the requets after breaking command are:");
        c=0;

        if (tra == NULL){
                return c;}
        while(tra->next != NULL)
        {
                if (tra != NULL)
                {
                        //printf("\n req %s type %s file name %s size %ld file dicriptor %d time %s",tra->wh_req, tra->req_type,tra->file_or_dir_name,tra->size, tra ->fdc, tra-> in_time);
                        tra=tra->next;
                        c++;
                        //  printf("c is %d ",c);
                }
        }
        if (tra != NULL)
        {
                //printf("\n req %s type %s file name %s size %ld file dicriptor %d time %s",tra->wh_req, tra->req_type,tra->file_or_dir_name,tra->size, tra ->fdc, tra-> in_time);
                c++;
                //        printf("c is %d ",c);
                return c;
        }

}


void schedule_sjf(int ele)
{
        struct queue *sch_t=NULL;
        struct que_arr arr[ele],sch_t_ar;
        sch_t=(struct queue*) calloc(1,sizeof(struct queue));
        int i_s, j, c_s;
        sch_t=start;
        //printf("hello sorthin...");
        for (c_s=0; sch_t->next != NULL ; c_s++)
        {
                strcpy(arr[c_s].wh_req,sch_t->wh_req);
                strcpy(arr[c_s].req_type,sch_t->req_type);
                strcpy(arr[c_s].file_or_dir_name,sch_t->file_or_dir_name);
                arr[c_s].size=sch_t->size;
                arr[c_s].fdc=sch_t->fdc;
                strcpy(arr[c_s].in_time,sch_t->in_time);
                sch_t=sch_t->next;
        }
        strcpy(arr[c_s].wh_req,sch_t->wh_req);
        strcpy(arr[c_s].req_type,sch_t->req_type);
        strcpy(arr[c_s].file_or_dir_name,sch_t->file_or_dir_name);
        arr[c_s].size=sch_t->size;
        arr[c_s].fdc=sch_t->fdc;
        strcpy(arr[c_s].in_time,sch_t->in_time);


        for (i_s=0;i_s<= c_s-1; i_s++)
        {
                for (j=i_s+1;j<= c_s; j++)
                {
                        if( arr[i_s].size > arr[j].size )
                        {
                                sch_t_ar=arr[i_s];
                                arr[i_s]=arr[j];
                                arr[j]=sch_t_ar;
                        }
                }
        }
        sch_t=start;
        for (c_s=0; sch_t->next != NULL ; c_s++)
        {
                strcpy(sch_t->wh_req,arr[c_s].wh_req);
                strcpy(sch_t->req_type,arr[c_s].req_type);
                strcpy(sch_t->file_or_dir_name,arr[c_s].file_or_dir_name);
                sch_t->size=arr[c_s].size;
                sch_t->fdc=arr[c_s].fdc;
                strcpy(sch_t->in_time,arr[c_s].in_time);
                sch_t=sch_t->next;
        }
        strcpy(sch_t->wh_req,arr[c_s].wh_req);
        strcpy(sch_t->req_type,arr[c_s].req_type);
        strcpy(sch_t->file_or_dir_name,arr[c_s].file_or_dir_name);
        sch_t->size=arr[c_s].size;
        sch_t->fdc=arr[c_s].fdc;
        strcpy(sch_t->in_time,arr[c_s].in_time);
}
