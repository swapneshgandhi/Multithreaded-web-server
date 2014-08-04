#ifndef TH_H
#define TH_H

struct th
{
	char wh_req[50];
	char req_type[10];
	char file_or_dir_name[40];
	long size;
	int fdc;
	char in_time[50];
};
extern struct th *assign_r;						
#endif

