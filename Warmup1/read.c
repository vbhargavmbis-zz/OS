#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct data
{
	char op;
	long ltime;
	int amt;
	char desc[80];//needs change
} Transact;

int main()
{
	Transact t;
	char address[100]="test.tfile";
	FILE *fptr;
	char buf[80];
	fptr=fopen(address, "rt"); 
    if (fptr==NULL) 
    { 
        printf("%s",address);
        printf("Could not open file!\n"); // error in opening file
        // getch();
        return 1; 
    }
    else
    {
		int i;
		char buf[1026];
		char *buf2;
	//	printf("+-----------------+--------------------------+----------------+----------------+\n");
	//	printf("|       Date      | Description              |         Amount |        Balance |\n");
	//	printf("+-----------------+--------------------------+----------------+----------------+\n");
		while(1){
		if (fgets(buf, sizeof(buf), fptr) == NULL)
		{
		  /* end of file */
		  break;
		} 
		else 
		{
			long timeint;
			char time[30];//change size 
			char op;
			float amt;
			double a=0.0;
			char desc[40];//change size!!!
			//else 
			{
			
				//printf("BUF: %s\n",buf);
				int i;
				char *start_ptr = buf;
				char *tab_ptr = strchr(start_ptr, '\t');
				if (tab_ptr != NULL) 
				{
				  *tab_ptr++ = '\0';
				 // printf("%s\t",start_ptr);
				  op=start_ptr[0];
						//printf("OPERATOR= %c\n",op);
						t.op=op;
						printf("%c\t",t.op);
				}
				for(i=0;i<3;i++)
				{
					start_ptr = tab_ptr;
					tab_ptr = strchr(start_ptr, '\t');
					if (tab_ptr != NULL) 
					{
					  *tab_ptr++ = '\0';
					  
					}
					if(i==0)
					{
						//printf("INSIDE 1\n");
						timeint=atol(start_ptr);
						t.ltime=timeint;
						//printf("--TIME: %lu\t",timeint);
						strcpy(time,ctime(&timeint));//time is stored in array time
						//printf("Converted Time: %s\t",time);
						//printf("%s\t",time);
						printf("%d\t",t.ltime);
					}
					else if(i==1)
					{
						
						a=atof(start_ptr);
						t.amt=a*100;
						
						//printf("AMOUNT: %f--\t",a);
						//printf("%f\t",a);
						printf("%d\t",t.amt);
					}
					else if(i==2)
					{
						strcpy(desc,start_ptr);
						strcpy(t.desc,desc);
						//printf("Description%s",start_ptr);
						printf("%s\n",t.desc);
					}
				//	printf("%s\t",start_ptr);
				}
				printf("\n");
				
				//printf("CTIME:%s",ctime(&time));						
			}
		 }
	  }//while loop
	}
 }
 