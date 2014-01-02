#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include  <stdio.h>
#include  <signal.h>
#include  <stdlib.h>

void     INThandler(int);

int  main(void)
{
     signal(SIGINT, INThandler);
     while (1)
          pause();
     return 0;
}

void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     printf("OUCH, did you hit Ctrl-C?\n"
            "Do you really want to quit? [y/n] ");
     c = getchar();
     if (c == 'y' || c == 'Y')
          exit(0);
     else
          signal(SIGINT, INThandler);
     getchar(); // Get new line character
}







// void *packet_arrival()
// {
	// // printf("Hello world before sleep.");
	// // usleep(2000000);
	// //printf("Hello world after sleep.");
	// int i=0;
	// while(i < 10 )
	// {
		// usleep(2000000);
		// printf("threadFunc says: YO..processing..\n");
		// ++i;
	// }
// }
// int main(void) 
// {
	// pthread_t packet;
   
   // pthread_create(&packet,0,packet_arrival,NULL);
   // pthread_join(packet, NULL /* void ** return value could go here */);
   // //packet_arrival();
   
// }






// double display_time(struct timeval x)
// {
	// double tot = (x.tv_sec*1000000)+(x.tv_usec);
	 // double conv = tot/1000; 
   // printf("%012.3f\n",conv);
   
   // return(conv);
// }
// int main()
	
// {
	// printf("Start\n");
	 // struct timeval x;
	 // x.tv_sec=2;
	 // x.tv_usec=1346;
	 // display_time(x);
	// return(0);
// }







// #include <stdio.h>
// #include <pthread.h>

// /* This is our thread function.  It is like main(), but for a thread */
// void *threadFunc(void *arg)
// {
	// char *str;
	// int i = 0;

	// str=(char*)arg;

	// while(i < 10 )
	// {
		// usleep(2000000);
		// printf("threadFunc says: %s\n",str);
		// ++i;
	// }

	// return NULL;
// }

// int main(void)
// {
	// pthread_t pth;	// this is our thread identifier
	// int i = 0;

	// /* Create worker thread */
	// pthread_create(&pth,NULL,threadFunc,"processing...");

	// /* wait for our thread to finish before continuing */
	// pthread_join(pth, NULL /* void ** return value could go here */);

	// while(i < 10 )
	// {
		// usleep(1);
		// printf("main() is running...\n");
		// ++i;
	// }

	// return 0;
// }