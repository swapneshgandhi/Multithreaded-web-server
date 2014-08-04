#ifndef QUEUE_H
#define QUEUE_H
struct queue
{
	char wh_req[50];               
	char req_type[10];
	char file_or_dir_name[40];
	long size;
	int fdc;
	char in_time[50];
	struct queue *next;
};
extern struct queue *start;						
#endif

