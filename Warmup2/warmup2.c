#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <sys/resource.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>

#include "cs402.h"

#include "my402list.h"
//-----------------------------------------------------
double lambda = 0.5;// 2;
double mu = 0.35;
double r = 1.5;//4;
int b = 10;
int p_tok = 3;
int num = 3;//default value is 20!!!;
#define LAMBDA 0.5
#define MU 0.35
#define R 1.5
#define B 10
#define P_TOK 3
#define NUM 3	//number of packets

//------------------------------------------------------
pthread_t packet, token, server;
FILE *fptr;
double xsq=0.0;
long emul_time=0;// emulation start time
struct timeval emul_start;
struct timespec timeout, remaining_time;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int q2_empty = 1; //1 if q2 is empty, 0 if its not empty
My402List q1,q2;
int token_bucket = 0;
int tok_req = P_TOK;//number of tokens required
int pkt_inter = (1000000/LAMBDA);
int tok_inter = (1000000/R);
int service_time = (1000000/MU);
int leaveq1=0,leaveq2=0,leaveS=0;
double inter_arrival_time=0.0;
int rff=0;
int ctrlc = 0;
double inc_pkt_serv=0.0,inc_avg_pkt_q1=0.0,inc_avg_pkt_q2=0.0,inc_avg_pkt_s=0.0,inc_avg_pkt_sys=0.0;
//------------------------------------------------------
//STATISTICS VARIABLES:
int completed=0;
double avg_pkt_inter=0.0;
double avg_pkt_service=0.0;
double avg_pkt_q1=0.0;
double avg_pkt_q2=0.0;
double avg_pkt_s=0.0;
double avg_pkt_sys=0.0;
double std_deviation=0.0;
double tok_drop_prob=0.0;
double pkt_drop_prob=0.0;
double emul_time_end=0.0;
double dropped_tok=0,tot_tok=0,dropped_pkt=0,tot_pkt=0;

//------------------------------------------------------
typedef struct data
{
	int no;
	double p_pkt_inter, p_serv_time, p_tok_inter;
	int p_tok_req, b;//need to add value of r
	struct timeval arrival;
	struct timeval q1_enter;
	struct timeval q1_leave;
	struct timeval q2_enter;
	struct timeval service_enter;
	struct timeval service_leave;
} Packet;





void calculate_statistics()
{
//	if(leaveS == num)
	
	{
		// struct timeval t2,tot_emul_time;
		// double g=0.0;
		//completed = tot_pkt - dropped_pkt;
		//completed = leaveS;
		// if(emul_time_end==0)
		// {
			// gettimeofday(&t2,NULL);
			// tot_emul_time.tv_sec = t2.tv_sec - emul_start.tv_sec;
			// tot_emul_time.tv_usec= t2.tv_usec - emul_start.tv_usec;	
			// g = ((tot_emul_time.tv_sec*1000000) + (tot_emul_time.tv_usec));
			// emul_time_end = g/1000;
		// }
		// printf("tot pkt time in Q1 = %.3fms\n",avg_pkt_q1);
		// printf("tot pkt time in Q2 = %.3fms\n",avg_pkt_q2);
		// printf("tot pkt time in S = %.3fms\n",avg_pkt_s);
		// printf("tot number of packets = %.0f\n",tot_pkt);
		// printf("no of completed packets = %d\n",completed);
		// printf("tot number of tokens = %.3f\n",tot_tok);
		// printf("toks dropped = %.3f\n",dropped_tok);
		// printf("squares added value = %.6g\n",xsq);
		
		printf("\nStatistics:\n\n");
		
		if(tot_pkt==0)
		{
			printf("\taverage packet inter-arrival time = N/A, total number of packets produced = 0\n");
		}
		else
		{
			avg_pkt_inter = (avg_pkt_inter/tot_pkt)/1000000;
			printf("\taverage packet inter-arrival time = %.6gs\n",avg_pkt_inter);
		}
		if(completed==0)
		{
			printf("\taverage packet service time = N/A, number of completed packets = 0\n\n");
		}
		else
		{
			avg_pkt_service = (avg_pkt_s/completed)/1000;
			printf("\taverage packet service time = %.6gs\n\n",avg_pkt_service);	
		}
		if((completed==0)||(emul_time_end==0))
		{
			printf("\taverage number of packets in Q1 = N/A, number of completed packets = 0\n");
			printf("\taverage number of packets in Q2 = N/A, number of completed packets = 0\n");
			printf("\taverage number of packets in S = N/A, number of completed packets = 0\n\n");
			//printf("EMUL TIME END = %.6g\n",emul_time_end);
		}
		else
		{
			avg_pkt_q1 = (avg_pkt_q1/emul_time_end);
			avg_pkt_q2 = (avg_pkt_q2/emul_time_end);
			avg_pkt_s = (avg_pkt_s/emul_time_end);
			printf("\taverage number of packets in Q1 = %.6g\n",avg_pkt_q1);
			printf("\taverage number of packets in Q2 = %.6g\n",avg_pkt_q2);
			printf("\taverage number of packets at S = %.6g\n\n",avg_pkt_s);
		}
		
		if(completed==0)
		{
			printf("\taverage time a packet spent in system = N/A, number of completed packets = 0\n");
			printf("\tstandard deviation for time spent in system = N/A, number of completed packets = 0\n\n");
		}
		else
		{
			avg_pkt_sys = (avg_pkt_sys/completed)/1000;//avg time pkt spends in system
			printf("\taverage time a packet spent in system = %.6gs\n",avg_pkt_sys);
			// EX = avg_pkt_sys;
			// std_deviation = ((((EX * completed)*(EX * completed))/completed) - (EX * EX));
			xsq = xsq/completed;
			xsq = xsq - (avg_pkt_sys*1000)*(avg_pkt_sys*1000);
			std_deviation = sqrt(xsq);
			std_deviation = std_deviation/1000;
			printf("\tstandard deviation for time spent in system = %.6gs\n\n",std_deviation);
		}
		if(tot_tok==0)
		{
			printf("\ttoken drop probability = N/A, number of tokens produced = 0\n");
		}
		else
		{
			tok_drop_prob = dropped_tok/tot_tok;
			printf("\ttoken drop probability = %.6g\n",tok_drop_prob);
		}
		if(tot_pkt==0)
		{
			printf("\tpacket drop probability = N/A, number of pakcets produced = 0\n");
		}
		else
		{
			pkt_drop_prob = dropped_pkt/tot_pkt;
			printf("\tpacket drop probability = %.6g\n",pkt_drop_prob);
		}
		exit(0);
		// double EX=0.0;
		// printf("tot pkt time in Q1 = %.3fms\n",avg_pkt_q1);
		// printf("tot pkt time in Q2 = %.3fms\n",avg_pkt_q2);
		// printf("tot pkt time in S = %.3fms\n",avg_pkt_s);
		// printf("tot number of packets = %.0f\n",tot_pkt);
		
		
		// //std_deviation = ((avg_pkt_sys * avg_pkt_sys)/NUM);//  sqr(E[X] x 8) in milliseconds
		// //printf("std dev : %.6g\n",std_deviation);
		// avg_pkt_inter = (avg_pkt_inter/num)/1000000;
		// avg_pkt_service = (avg_pkt_s/num)/1000;
		// avg_pkt_q1 = (avg_pkt_q1/emul);
		// avg_pkt_q2 = (avg_pkt_q2/emul);
		// avg_pkt_s = (avg_pkt_s/emul);
		// avg_pkt_sys = (avg_pkt_sys/num)/1000;//avg time pkt spends in system
		// EX = avg_pkt_sys;
		// std_deviation = ((((EX * num)*(EX * num))/num) - (EX * EX));
		// std_deviation = sqrt(std_deviation);
		// //std_deviation = std_deviation - ((avg_pkt_sys * avg_pkt_sys)*1000);//because 1000 is being squared
		// //printf("2nd part: %.6g\n",(avg_pkt_sys * avg_pkt_sys));
		// tok_drop_prob = dropped_tok/tot_tok;
		// pkt_drop_prob = dropped_pkt/tot_pkt;
		
		// printf("\nStatistics:\n\n");
		
		// printf("\taverage packet inter-arrival time = %.6gs\n",avg_pkt_inter);
		// printf("\taverage packet service time = %.6gs\n\n",avg_pkt_service);
		
		// printf("\taverage number of packets in Q1 = %.6g\n",avg_pkt_q1);
		// printf("\taverage number of packets in Q2 = %.6g\n",avg_pkt_q2);
		// printf("\taverage number of packets at S = %.6g\n\n",avg_pkt_s);
		
		// printf("\taverage time a packet spent in system = %.6gs\n",avg_pkt_sys);
		// printf("\tstandard deviation for time spent in system = %.6gs\n\n",std_deviation);
		
		// printf("\ttoken drop probability = %.6g\n",tok_drop_prob);
		// printf("\tpacket drop probability = %.6g\n",pkt_drop_prob);
	}
	//else
	{
		//double temp = ((p->q1_leave.tv_sec - p->q1_enter.tv_sec)*1000000) + (p->q1_leave.tv_usec - p->q1_enter.tv_usec);
		//avg_pkt_q1 = avg_pkt_q1 + (temp/1000);
		
	}
	pthread_mutex_lock(&m);
	My402ListUnlinkAll(&q1);
	My402ListUnlinkAll(&q2);
	pthread_mutex_unlock(&m);
}

void display_time(struct timeval x)
{
	double tot = (x.tv_sec*1000000)+(x.tv_usec);
	double conv = tot/1000; 
    printf("%012.3fms: ",conv);
}
void *packet_arrival()
{
	double x,z;
	int y;
	char line[50];
	struct timeval pkt_time,disp_time;
	double earlier=0.0;
	int i=0;
	
	//printf("PKT INTER ARRIVAL: %d\n",pkt_inter);
	//for(i=0;i<num;i++)
	while((i<num)&&(ctrlc==0))
	{	
		//printf("looping in pkt while\n");
		Packet *p;
		p = malloc(sizeof(struct data));
		p->no = i+1;
		p->p_pkt_inter =  pkt_inter;
		p->p_tok_inter = tok_inter;
		p->p_serv_time = service_time;
		p->b = b;
		p->p_tok_req = tok_req;
		if(pkt_inter>=10000)
		{
			usleep(pkt_inter);
		}
		//usleep(0);
		//printf("pkt inter is: %d",pkt_inter);
		//avg_pkt_inter = avg_pkt_inter + pkt_inter;
		
		if(p->p_tok_req > p->b)
		{
			leaveq1++;
			leaveq2++;
			leaveS++;
			//printf("Packet will be dropped\n");
			gettimeofday(&pkt_time,NULL);
			disp_time.tv_sec = pkt_time.tv_sec - emul_start.tv_sec;
			disp_time.tv_usec= pkt_time.tv_usec - emul_start.tv_usec;
			inter_arrival_time = ((1000000*disp_time.tv_sec)+(disp_time.tv_usec));
			inter_arrival_time = (inter_arrival_time/1000) - earlier;
			earlier = (((1000000*disp_time.tv_sec)+(disp_time.tv_usec))/1000);
			avg_pkt_inter = avg_pkt_inter + (inter_arrival_time*1000);
			display_time(disp_time);
			printf("p%d arrives, needs %d tokens, dropped\n",p->no,p->p_tok_req);
			dropped_pkt++;
			tot_pkt=i+1;
			if(i==num)
				ctrlc=1;
			
		}
		else
		{
				
			tot_pkt=i+1;
			gettimeofday(&pkt_time,NULL);
			disp_time.tv_sec = pkt_time.tv_sec - emul_start.tv_sec;
			disp_time.tv_usec= pkt_time.tv_usec - emul_start.tv_usec;
			//printf("Packet Arrived at: %lu s, %lu us\n",disp_time.tv_sec,disp_time.tv_usec);
			
			display_time(disp_time);
			inter_arrival_time = ((1000000*disp_time.tv_sec)+(disp_time.tv_usec));
			inter_arrival_time = (inter_arrival_time/1000) - earlier;
			//inter_arrival_time =
			avg_pkt_inter = avg_pkt_inter + (inter_arrival_time*1000);
			earlier = (((1000000*disp_time.tv_sec)+(disp_time.tv_usec))/1000);
			printf("p%d arrives, needs %d tokens, inter arrival time = %.3fms\n",p->no,p->p_tok_req,inter_arrival_time);
			My402ListElem *elem=NULL;
					
			p->arrival.tv_sec = disp_time.tv_sec;
			p->arrival.tv_usec = disp_time.tv_usec;
			
			pthread_mutex_lock(&m);			
					My402ListAppend(&q1,p);			
			pthread_mutex_unlock(&m);
				
				gettimeofday(&pkt_time,NULL);
				p->q1_enter.tv_sec = pkt_time.tv_sec - emul_start.tv_sec;
				p->q1_enter.tv_usec= pkt_time.tv_usec - emul_start.tv_usec;			
				display_time(p->q1_enter);
				printf("p%d enters Q1\n",p->no);
				 pthread_mutex_lock(&m);
				// printf("locked mutex\n");
			//if(My402ListEmpty(&q1))
			if(q1.num_members==1)
			{
				if(ctrlc==1)
					break;
				
				 if(token_bucket>=p->p_tok_req)
				 {	
				//	printf("YES, ENOUGH TOKENS\n");		
					elem=My402ListFirst(&q1);
					//p=(Packet *)elem->obj;
							
					gettimeofday(&pkt_time,NULL);
					p->q1_leave.tv_sec = pkt_time.tv_sec - emul_start.tv_sec;
					p->q1_leave.tv_usec= pkt_time.tv_usec - emul_start.tv_usec;
					display_time(p->q1_leave);
					leaveq1++;//no of packets that have left q1
					
					double time_q1 = (((p->q1_leave.tv_sec - p->q1_enter.tv_sec)*1000000) + (p->q1_leave.tv_usec - p->q1_enter.tv_usec));
					time_q1 = time_q1/1000;
					avg_pkt_q1 = avg_pkt_q1 + time_q1;
					token_bucket = token_bucket - p->p_tok_req;
					
					printf("p%d leaves Q1, ",p->no);
					printf("time in Q1 = %.3fms, ",time_q1); 
					printf("token bucket now has %d tokens\n",token_bucket);
							
							
					//printf("P1 leaves Q1 at %lus, %luus\n",p->q1_leave.tv_sec,p->q1_leave.tv_usec);
					My402ListUnlink(&q1,elem);
					My402ListAppend(&q2,p);
					
					
					if(!My402ListEmpty(&q2))
					{					
						pthread_cond_signal(&cv);
					}
							
					gettimeofday(&pkt_time,NULL);
					p->q2_enter.tv_sec = pkt_time.tv_sec - emul_start.tv_sec;
					p->q2_enter.tv_usec= pkt_time.tv_usec - emul_start.tv_usec;		
					display_time(p->q2_enter);
					printf("p%d enters Q2\n",p->no);				  
				 }
				// printf("####ctrl c = %d####",ctrlc);
			}
					
			pthread_mutex_unlock(&m);
		}
		if(rff!=0)
		{
			if(fgets(line, sizeof(line), fptr) != NULL)
			{
				//fgets(line, sizeof(line), fptr);
				//printf("%s",line);
				sscanf( line, "%lf %d %lf", &x, &y, &z );
				//printf("%lf %d %lf\n",x,y,z);
				pkt_inter = x*1000;
				tok_req = y;
				service_time = z*1000;
			}
		}
		i++;
	}
	//PrintTestList(&q1,q1.num_members);
	//printf("End of pkt thread\n");
	pthread_exit(NULL);
	return(0);
}

void *token_arrival()
{
	struct timeval tkn_time,disp_time;
	double time_q1=0.0;
	int i=0,temp;
	//printf("TOKEN INTER: %d\n",tok_inter);
	//for(i=0;i<12;i++)
	while((leaveq1<num)&&(ctrlc==0))//it was leaveS before<num
	{
		i++;//token number
		gettimeofday(&tkn_time,NULL);
		disp_time.tv_sec = tkn_time.tv_sec - emul_start.tv_sec;
		disp_time.tv_usec= tkn_time.tv_usec - emul_start.tv_usec;
		
		if(tok_inter>=10000)
		{
			usleep(tok_inter);
		}
		gettimeofday(&tkn_time,NULL);
		disp_time.tv_sec = tkn_time.tv_sec - emul_start.tv_sec;
		disp_time.tv_usec= tkn_time.tv_usec - emul_start.tv_usec;
		//display_time(disp_time);
		//printf("token t%d arrives, ",i);
		//printf("Token Arrived at: %lu s, %lu us\n",disp_time.tv_sec,disp_time.tv_usec);
		My402ListElem *elem=NULL;
		Packet *p;
		elem=malloc(sizeof(struct tagMy402ListElem));
		
		pthread_mutex_lock(&m);
			if(token_bucket < b)//if token bucket is not full
			{
				token_bucket++;//add token to bucket
				display_time(disp_time);
				printf("token t%d arrives, ",i);
				printf("token bucket now has %d tokens\n",token_bucket);
				tot_tok=i;
			}
			else
			{
				display_time(disp_time);
				printf("token t%d arrives, dropped\n",i);
				dropped_tok++;
				tot_tok=i;
			}
			
			//if(!My402ListEmpty(&q1))	
			elem=My402ListFirst(&q1);
			if(elem!=NULL)				
			{				
				p=(Packet *)elem->obj;
				if(token_bucket>=p->p_tok_req)//if there are enough tokens available
				{																							
					gettimeofday(&tkn_time,NULL);
					p->q1_leave.tv_sec = tkn_time.tv_sec - emul_start.tv_sec;
					p->q1_leave.tv_usec= tkn_time.tv_usec - emul_start.tv_usec;
					display_time(p->q1_leave);
					leaveq1++;//no of packets that have left q1
					
					time_q1 = (((p->q1_leave.tv_sec - p->q1_enter.tv_sec)*1000000) + (p->q1_leave.tv_usec - p->q1_enter.tv_usec));
					time_q1 = time_q1/1000;
					avg_pkt_q1 = avg_pkt_q1 + time_q1;
					token_bucket = token_bucket - p->p_tok_req;
					printf("p%d leaves Q1, ",p->no);
					printf("time in Q1 = %.3fms, ",time_q1); 
					printf("token bucket now has %d tokens\n",token_bucket);
										
					//printf("P1 leaves Q1 at %lus, %luus\n",p->q1_leave.tv_sec,p->q1_leave.tv_usec);
					My402ListUnlink(&q1,elem);
					My402ListAppend(&q2,p);
					
					if(!My402ListEmpty(&q2))
					{
						//printf("No.of elements in Q2: %d\n",q2.num_members);
						pthread_cond_signal(&cv);
						//printf("Signaled\n");
					}
					
					gettimeofday(&tkn_time,NULL);
					p->q2_enter.tv_sec = tkn_time.tv_sec - emul_start.tv_sec;
					p->q2_enter.tv_usec= tkn_time.tv_usec - emul_start.tv_usec;		
					display_time(p->q2_enter);
					printf("p%d enters Q2\n",p->no);
					//printf("Packet Entered Q2 at %lus, %luus\n",p->q2_enter.tv_sec,p->q2_enter.tv_usec);
										
				}
			}
			if(!My402ListEmpty(&q2))
			{
				//printf("No.of elements in Q2: %d\n",q2.num_members);
				pthread_cond_signal(&cv);
				//printf("Signaled\n");
			}
		pthread_mutex_unlock(&m);
	}
	
		
			while(!My402ListEmpty(&q2))
			{
				//printf("No.of elements in Q2: %d\n",q2.num_members);
				
				pthread_cond_signal(&cv);
				
				//printf("Signaled\n");
			}
	
	
	pthread_mutex_lock(&m);
	temp = My402ListEmpty(&q2);
	pthread_mutex_unlock(&m);
	if((temp==1)&&((dropped_pkt+completed)==num))
	{
		pthread_cancel(server);
	}
	if((temp==1)&&(ctrlc==1))
	{
		pthread_cancel(server);
	}
	//printf("End of token thread\n");
	//pthread_exit(NULL);
	return(0);
}

void *server_proc()
{
	struct timeval srvr_time,t1,t2,tot_emul_time;
	double a=0.0;
	double time_q2 = 0.0;
	double show_srvtime=0.0, tis=0.0;//tis=time in system
	My402ListElem *elem=NULL;
	Packet *p;
	elem=malloc(sizeof(struct tagMy402ListElem));
	//printf("---%d---\n",service_time);
	
		while((leaveq2<num)&&(ctrlc==0))// empty=1,not empty=0
		{
			pthread_mutex_lock(&m);
			
			if((My402ListEmpty(&q2))&&(ctrlc==0))
			{//printf("COMING HERE!!\n");	
				pthread_cond_wait(&cv,&m);		
						
				//printf("IN COND WAIT---");
			}
			elem=My402ListFirst(&q2);
			p=(Packet *)elem->obj;
			My402ListUnlink(&q2,elem);
			leaveq2++;
			pthread_mutex_unlock(&m);
			gettimeofday(&srvr_time,NULL);
			p->service_enter.tv_sec = srvr_time.tv_sec - emul_start.tv_sec;
			p->service_enter.tv_usec= srvr_time.tv_usec - emul_start.tv_usec;	
			time_q2 = (((p->service_enter.tv_sec - p->q2_enter.tv_sec)*1000000) + (p->service_enter.tv_usec - p->q2_enter.tv_usec));
			time_q2 = time_q2/1000;	
			avg_pkt_q2 = avg_pkt_q2 + time_q2;
			display_time(p->service_enter);
			printf("p%d begins service at S, time in Q2 = %.3fms\n",p->no,time_q2);
			
			gettimeofday(&t1,NULL);
			// p->service_enter.tv_sec = srvr_time.tv_sec - emul_start.tv_sec;
			// p->service_enter.tv_usec= srvr_time.tv_usec - emul_start.tv_usec;	
			
			if(p->p_serv_time>10000)
			{
				usleep(p->p_serv_time);
			}
			//printf("pkt no: %d\t",p->no);
			//printf("p_serv_time: %.3f\n",p->p_serv_time);
			gettimeofday(&t2,NULL);
		    p->service_leave.tv_sec = t2.tv_sec - emul_start.tv_sec;
			p->service_leave.tv_usec= t2.tv_usec - emul_start.tv_usec;	
			a = (((t2.tv_sec - srvr_time.tv_sec)*1000000) + (t2.tv_usec - srvr_time.tv_usec));//calculates the amount of time slept
			//a = (((t2.tv_sec - t1.tv_sec)*1000000) + (t2.tv_usec - t1.tv_usec));//calculates the amount of time slept
			//a = a/1000;	//convert into millisecond format
			//printf("Time slept for %.3fms \n",a);
			//show_srvtime = service_time/1000;
			show_srvtime = a/1000;
			a = (((p->service_leave.tv_sec - p->arrival.tv_sec)*1000000) + (p->service_leave.tv_usec - p->arrival.tv_usec));//calculates the amount of time slept
			tis = a/1000;	
			avg_pkt_sys = avg_pkt_sys + tis;
			xsq = xsq + (tis*tis);
			//printf("XSQ VALUE: %.6g\n",xsq);
			display_time(p->service_leave);
			avg_pkt_s = avg_pkt_s + show_srvtime;
			leaveS++;
			printf("p%d departs from S, service time = %.3fms, time in system = %.3fms\n",p->no,show_srvtime,tis); //changed LEAVEQ2 TO LEAVES
			completed++;
			
		}
		
	gettimeofday(&t2,NULL);
	tot_emul_time.tv_sec = t2.tv_sec - emul_start.tv_sec;
	tot_emul_time.tv_usec= t2.tv_usec - emul_start.tv_usec;	
	a = ((tot_emul_time.tv_sec*1000000) + (tot_emul_time.tv_usec));
	emul_time_end = a/1000;
	//printf("ctrlc = %d",ctrlc);
	if(ctrlc==1)
	{
	 calculate_statistics();
	 //pthread_exit(NULL);
	 
	 //return 0;
	}
	// printf("Emulation took %.3fms\n",emul_time_end);
//printf("End of server thread\n");
	pthread_exit(NULL);
	return(0);
}

void  INThandler(int sig)
{
	struct timeval t2,tot_emul_time;
	double g;
    signal(sig, SIG_IGN);
    ctrlc++;
	gettimeofday(&t2,NULL);
	tot_emul_time.tv_sec = t2.tv_sec - emul_start.tv_sec;
	tot_emul_time.tv_usec= t2.tv_usec - emul_start.tv_usec;	
	g = ((tot_emul_time.tv_sec*1000000) + (tot_emul_time.tv_usec));
	emul_time_end = g/1000;
	
	// pthread_cancel(packet);
	// pthread_cancel(token);
	// pthread_cancel(server);
	// calculate_statistics();
    //return 1;
}

int main(int argc, char *argv[])
{
	int j=0;
	char line[50];
	
	struct timeval tv;
	double x,z;
	int y;
	signal(SIGINT, INThandler);
	if(argc>1)
	{								
		for(j=0;j<argc;j++)
		{
			//printf("%s argc = %d\n",argv[j],argc);
			if(!(strcmp(argv[j],"-n")))//if second argument is "-n" yes its supposed to be !strcmp
			{
				//printf("This means number of pkts argc = %d\n",argc);
				if((atoi(argv[j+1])>0)&&(atol(argv[j+1])<=2147483647))
				{
					num = atoi(argv[j+1]);
				}
				//printf("Num is : %d\n",num);
				//exit(0);
			}
			if(!(strcmp(argv[j],"-r")))
			{
				double rate;
				//printf("this is the token rate\n");
				
				sscanf(argv[j+1], "%lf", &rate);
				if((rate>0)&&((1/rate)<=10))
				{
					r = rate;
					tok_inter = (1000000/r);
				//	printf("%.3f\n",r);
				
				}
				if((1/rate)>10)
				{
					r = 0.1;
					tok_inter = (1000000/r);
				}
			}
			if(!(strcmp(argv[j],"-B")))
			{
				//printf("this is the size of the token bucket\n");
				b = atoi(argv[j+1]);
				//printf("%d\n",b);
			
			}
			if(!(strcmp(argv[j],"-P")))
			{
				//printf("no of tokens require for transmission\n");
				p_tok = atoi(argv[j+1]);
				tok_req = p_tok;
				//printf("%d\n",p_tok);
				
			}
			if(!(strcmp(argv[j],"-lambda")))
			{
				//printf("this is the value of lambda\n");
				double lam;
				sscanf(argv[j+1], "%lf", &lam);
				if((lam>0)&&((1/lam)<=10))
				{
					lambda = lam;
					pkt_inter = (1000000/lambda);
					//printf("%.3f\n",lambda);
				}
				if((1/lam)>10)
				{
					lambda = 0.1;
					pkt_inter = (1000000/lambda);
				}
				
			}
			if(!(strcmp(argv[j],"-mu")))
			{
				//printf("this is the value of mu\n");
				double m;
				sscanf(argv[j+1], "%lf", &m);
				if((m>0)&&((1/m)<=10))
				{
					mu = m;
					service_time = (1000000/mu);
					//printf("%.3f\n",mu);
				}
				if((1/m)>10)
				{
				//	printf("Its exceeding\n");
					mu = 0.1;
					service_time = (1000000/mu); 
				}
			}
			if(!(strcmp(argv[j],"-t")))
			{
				
				//printf("Tracefile has been specified\n");
				if(!strstr(argv[j+1],".txt"))
				{
					printf("Could not open file: %s\n",argv[2]);
					printf("Input file either a directory or not in correct format\n");
					exit(0);
				}
			
				fptr=fopen(argv[j+1], "rt");
				if (fptr==NULL) 
				{ 
					printf("Could not open file: %s!\n",argv[j+1]); // error in opening file
					printf("%s\n", strerror(errno));
					exit(0);
					
				}
				else
				{
					rff++;
					fgets(line, sizeof(line), fptr);
					//printf("%s",line);
					num = atoi(line);
					//printf("%d",num);
					fgets(line, sizeof(line), fptr);
					//printf("%s",line);
					sscanf( line, "%lf %d %lf", &x, &y, &z );
					//printf("%lf %d %lf\n",x,y,z);
					pkt_inter = x*1000;
					tok_req = y;
					service_time = z*1000;
					
					printf("r = %.3f\n",r);
					printf("B = %d\n",b);
					printf("tsfile = %s\n\n",argv[j+1]);
				}
			}
		}
	}
	
	gettimeofday(&tv,NULL);
	gettimeofday(&emul_start,NULL);
	//printf("Emulation Starts: %lu s, %lu us\n",emul_start.tv_sec,emul_start.tv_usec); 
	
	memset(&q1, 0, sizeof(My402List));
    (void)My402ListInit(&q1);
	memset(&q2, 0, sizeof(My402List));
    (void)My402ListInit(&q2);
	
	if(rff==0)
	{
		printf("Emulation Parameters:\n");
		printf("lambda = %.3f\n",lambda);
		printf("mu = %.3f\n",mu); 
		printf("r = %.3f\n",r);
		printf("B = %d\n",b);
		printf("P = %d\n",p_tok);
		printf("number to arrive = %d\n\n",num);
	}
	else
	{
	}
	printf("00000000.000ms: emulation begins\n");
	pthread_create(&packet,NULL,packet_arrival,NULL);
	
	pthread_create(&token,NULL,token_arrival,NULL);
	pthread_create(&server,NULL,server_proc,NULL);
	
	pthread_join(packet, NULL /* void ** return value could go here */);
	pthread_join(token, NULL /* void ** return value could go here */);
	pthread_join(server, NULL /* void ** return value could go here */);
	
	//if(ctrlc==0)
	{
		calculate_statistics();
	}
	return(0);
}
