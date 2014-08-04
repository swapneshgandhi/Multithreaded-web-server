/* This is the main file where every other file and functions are called */

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <ctype.h>
#include        <sys/types.h>
#include        <sys/socket.h>
#include        <netdb.h>
#include        <netinet/in.h>
#include        <inttypes.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include "pthread.h"
#include "unistd.h"
#include "excecute.h"
#include "queue.h"
#include "que_func.h"
#include "th.h"
#include "serv_th.h"
#include "server_in.h"
#include "schd_th.h"
//static char svnid[] = "$Id: soc.c 6 2009-07-03 03:18:54Z kensmith $";

char * file_type(char *f);
void usage();
//void schedule_sjf(int ele);
struct queue *start=NULL;


//char [] get_first_queue();
int port_num;
struct sockaddr_in serv, remote;
struct servent *se;
int newsock, len,sort;
int NUM_THREADS;
int fd=-1;
int *fd_ptr=&fd;
char *progname;
char buf[BUF_LEN];
char request[BUF_LEN];
char *def_dir=NULL;
char *log_file=NULL;
int sleep_t;
int debug,log_s;
FILE *log_f;
int s, sock, ch, server,r;
int soctype = SOCK_STREAM;
char *host = NULL;
char *port = NULL;
extern char *optarg;
extern int optind;
struct th *assign_r;
pthread_mutex_t mut_que=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_th_status=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_assign=PTHREAD_MUTEX_INITIALIZER;
pthread_t listening_th, schedlr_th;
pthread_t *serving_th;
pthread_cond_t *thread_wake;
pthread_cond_t new_in_que=PTHREAD_COND_INITIALIZER;
pthread_cond_t served=PTHREAD_COND_INITIALIZER;
int *thread_status;

void usage_summary();

int daemon_init()
{
        pid_t pid;
        if ((pid=fork())<0) return (-1);

        else if (pid!=0) exit (0); //parent goes away
        fprintf(stderr,"\n***To kill the daemon use 'kill %d'***\n",getpid());
        setsid(); //becomes session leader
        umask(0); //clear file creation mask
        close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);
        return (0) ;
}
// This function will listen to requets continuosly nad adds requests to ready queue.
void *listen_fn(void *arg)
{
        FILE *fpin;


        int i;
        while(1)
        {

                listen(s, 10);
                newsock = s;
                if (debug ==1) {
                        while (i != 0){
                                sleep(3);
                                pthread_mutex_lock (&mut_que);
                                i=traverse_queue_wh();
                                pthread_mutex_unlock (&mut_que);
                        }}
                        if (soctype == SOCK_STREAM) {

                                fd = accept(s, (struct sockaddr *) &remote, &len);

                        }
                        fpin = fdopen(fd, "r" );
                        fgets(request,BUF_LEN,fpin);
                        if (debug ==1 ){
                        printf("\nListning thread received a request = %s", request);
                        fflush(stdout);}
                        while( fgets(buf,BUF_LEN,fpin) != NULL && strcmp(buf,"\r\n") != 0 );
                        pthread_mutex_lock (&mut_que);
                        add_queue(request,fd);                        
                        i=traverse_queue_wh();
                        pthread_cond_signal(&new_in_que);
                        pthread_mutex_unlock (&mut_que);

        }
}


int
main(int argc,char *argv[])
{
        signal(SIGPIPE, SIG_IGN);
        fd_set ready;
        int     sock, fd, i;
        char    buf[BUF_LEN];



        struct sockaddr_in msgfrom;

        int msgsize;
        union {
                uint32_t addr;
                char bytes[4];
        } fromaddr;


        if ((progname = rindex(argv[0], '/')) == NULL)
                progname = argv[0];
        else
                progname++;

        log_s=0;
        debug=0;
        def_dir="./";
        sleep_t=60;
        NUM_THREADS=4;
        sort=0;

        while ((ch = getopt(argc, argv, "dl:p:hr:n:s:t:")) != -1)
                switch(ch) {

                case 'd':
                        debug=1;
                        break;
                case 'l':
                        log_s=1;
                        if (optarg == NULL){
                                printf("With option -l log file name mut be entered now exitng..");
                                usage_summary();}

                        log_file=optarg;
                        log_f=fopen(log_file,"a");
                        if (log_f == NULL){
                                fprintf(stderr, "Invalid log file name given please check log file name.\n**exiting now**");
                                exit(1);}
                                fclose(log_f);
                        break;
                case 'p':
                        port = optarg;
                        if (!isdigit(*optarg))
                        {
                                port_num=8080;
                        }
                        else{
                                port_num=(atoi(optarg));}
                                break;
                case 'h':
                        usage_summary();
                        break;
                case 'r':
                        r=1;
                        def_dir=optarg;
                        break;
                case 'n':
                        if (isdigit(*optarg)){
                                NUM_THREADS=atoi(optarg);}
                        else {
                                NUM_THREADS=4;}
                        break;

                case 's':
                        if ( optarg != NULL && strcmp(optarg,"SJF") == 0){
                                sort=1;}
                        else{
                                sort=0;}
                        break;

                case 't':
                        if (isdigit(*optarg)){
                                sleep_t=atoi(optarg);}
                        else {
                                sleep_t=60;}
                        break;

                case '?':
                        printf("\nUnrecognized option -%c\n",optopt);
                        usage_summary();

                default:
                        usage_summary();
                }
        /*                       * Create socket on local host.
         */

        if ((s = socket(AF_INET, soctype, 0)) < 0) {
                perror("socket");
                exit(1);
        }
        long t;
        setup_server();
        fprintf(stderr, "Welcome to myhttpd the server will now undergo setup...\n");
        fprintf(stderr, "The server will listen to port number %d.\n",port_num);
        fprintf(stderr, "Number of worker/excecution threads is:%d\n",NUM_THREADS);
        if ( sort ==1 )
                fprintf(stderr, "The server's sceduling policy is SJF\n");
        else
                fprintf(stderr, "The server's sceduling policy is FCFS\n");
        if (debug ==0 ){
                fprintf(stderr, "The server will daemonize now...\n");
                sleep(1);
                daemon_init();
           }
        // allocate variables dynamically
        assign_r=(struct th*)calloc(NUM_THREADS,sizeof(struct th));
        serving_th=(pthread_t *)calloc(NUM_THREADS,sizeof(pthread_t));
        thread_wake=(pthread_cond_t *)calloc(NUM_THREADS,sizeof(pthread_cond_t));
        thread_status=(int *)calloc(NUM_THREADS,sizeof(int));

        pthread_attr_t in_attr;
        pthread_attr_init(&in_attr);
        i=1;
        pthread_create(&listening_th, &in_attr, listen_fn , &i);
        pthread_create(&schedlr_th, &in_attr, schedlr_fn , &i);
        for (t=0;t< NUM_THREADS; t++)
        {
                pthread_cond_init(&thread_wake[t],NULL);
                thread_status[t]=0;
                pthread_create(&serving_th[t], NULL, serve_requests,(void *) t);
        }
        pthread_join(listening_th,NULL);
        pthread_join(schedlr_th,NULL);
}


/*       * usage - print usage string and exit
 */
void
usage_summary()
{
        fprintf(stderr, "\nUsage summary for myhttpd:\n -d \t\t: Enter debugging mode." );
        fprintf(stderr, "\n -h \t\t: Print this usage summary with all options and exit." );
        fprintf(stderr, "\n -l file\t: Log all the request in given file." );
        fprintf(stderr, "\n -p port\t: Listen on the given port. If not provided myhttpd will listen on port 8080." );
        fprintf(stderr, "\n -r dir\t\t: Set the root directory for the server to dir." );
        fprintf(stderr, "\n -t time\t: Set the queuing time to time seconds. Default is 60seconds." );
        fprintf(stderr, "\n -n threadnum\t: Set number of threads waiting ready in the execution thread pool. Default is 4 thread." );
        fprintf(stderr, "\n -s sched\t: Set the scheduling policy. It can be either FCFS or SJF. The default will be FCFS.\n" );
        exit(1);
}


