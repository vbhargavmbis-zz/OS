/*
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * @(#)$Id: listtest.c,v 1.1 2013/08/22 04:14:30 william Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

#include "cs402.h"

#include "my402list.h"

typedef struct data
{
	char op;
	long ltime;
	int amt;
	char desc[30];//needs change
} Transact;
char b[15],finamt[15],cnvAmt[15];

int checkTime(My402List *pList,long chktime)
{
	 My402ListElem *elem=NULL;
	 Transact *x;
	  elem=malloc(sizeof(struct tagMy402ListElem));
	  for (elem=My402ListFirst(pList);elem != NULL;elem=My402ListNext(pList, elem)) 
	  {
		
		x=(Transact*)elem->obj;
		//printf("--%d--",(int*)elem->obj);
		if(x->ltime==chktime)
		{
			return(1);//similar time exists
		}
	  }
	  return(0);
}


char * processDate(char *a)
{
	int i,ctr=0;
	//char b[15];
	for(i=0;i<24;i++)
	{
		if((i<11)||(i>19))
		{
			b[ctr]=a[i];
			ctr++;
		}
	}
	b[ctr]='\0';
	//printf("THE FINAL DATE IS: %s",b);
	return(b);
}
char* reverse(char *str)
{
	int i,ctr=0;
	//char cnvAmt[15];
	for(i=strlen(str)-1;i>=0;i--)
	{
		cnvAmt[ctr]=str[i];
		ctr++;
	}
	cnvAmt[ctr]='\0';
	//printf("FINAL CONV AMT: %s",cnvAmt);
	return(cnvAmt);
}
char* processNum(int a)
{
	char x[15]="\0";
	char finamt2[15];
	int ctr=0,i;
	sprintf(x, "%d",a);
	//printf("No.TO STR IS: %s",x);
	for(i=strlen(x)-1;i>=0;i--)
	{
		finamt2[ctr]=x[i];
		ctr++;
		if(ctr==2)
		{
			finamt2[ctr]='.';
			ctr++;
		}
		if(((ctr==6)||(ctr==10))&&(x[i-1]>='0'))
		{
			finamt2[ctr]=',';
			ctr++;
		}
		finamt2[ctr]='\0';
	}
	strcpy(cnvAmt,reverse(finamt2));
	//printf("CONVERTED AMT: %s",cnvAmt);
	return(cnvAmt);
}
static
void PrintTestList(My402List *pList, int num_items)
{
    My402ListElem *elem=NULL;
	int i=0,bal=0;
	Transact *x;
	printf("+-----------------+--------------------------+----------------+----------------+\n");
	printf("|       Date      | Description              |         Amount |        Balance |\n");
	printf("+-----------------+--------------------------+----------------+----------------+\n");
		
	
    if (My402ListLength(pList) != num_items)
	{
        fprintf(stderr, "List length is not %1d in PrintTestList().\n", num_items);
        exit(1);
    }
    for (elem=My402ListFirst(pList); elem != NULL; elem=My402ListNext(pList, elem),i++)
	{//Transact *x;
		x=(Transact*)elem->obj;
		char op=(char)(x->op);
		long time=(long)(x->ltime);
        int amt=(int)(x->amt);
		char *desc=x->desc;
		char t[24],amount[15],balance[15];
		strcpy(t,ctime(&time));
		
		char fintime[15];
		strcpy(fintime,processDate(t));
		//int i;
		if(op=='+')
		{
			bal=bal+amt;
		}
		else
		{
			bal=bal-amt;
		}
		
		strcpy(amount,processNum(amt));
		strcpy(balance,processNum(bal));
		
		//printf("%lu\t",(long)(((Transact*)(elem->obj))->ltime));
		// printf("%c\t%lu\t%d\t%s\n",op,time,amt,desc);
      
//	  printf("|%lu\t%s\t\t%0.2f\t%0.2f\n",time,desc,amount,balance);
	if(op=='-')
		printf("| %s | %-24s | (%12s) | %14s |\n",b,desc,amount,balance);
	else
		printf("| %s | %-24s | %14s | %14s |\n",b,desc,amount,balance);
		//printf("FINTIME %s",fintime);
    }
	printf("+-----------------+--------------------------+----------------+----------------+");
	
    fprintf(stdout, "\n");
}
static
void BubbleForward(My402List *pList, My402ListElem **pp_elem1, My402ListElem **pp_elem2)
    /* (*pp_elem1) must be closer to First() than (*pp_elem2) */
{
    My402ListElem *elem1=(*pp_elem1), *elem2=(*pp_elem2);
    void *obj1=elem1->obj, *obj2=elem2->obj;
    My402ListElem *elem1prev=My402ListPrev(pList, elem1);
/*  My402ListElem *elem1next=My402ListNext(pList, elem1); */
/*  My402ListElem *elem2prev=My402ListPrev(pList, elem2); */
    My402ListElem *elem2next=My402ListNext(pList, elem2);

    My402ListUnlink(pList, elem1);
    My402ListUnlink(pList, elem2);
    if (elem1prev == NULL) {
        (void)My402ListPrepend(pList, obj2);
        *pp_elem1 = My402ListFirst(pList);
    } else {
        (void)My402ListInsertAfter(pList, obj2, elem1prev);
        *pp_elem1 = My402ListNext(pList, elem1prev);
    }
    if (elem2next == NULL) {
        (void)My402ListAppend(pList, obj1);
        *pp_elem2 = My402ListLast(pList);
    } else {
        (void)My402ListInsertBefore(pList, obj1, elem2next);
        *pp_elem2 = My402ListPrev(pList, elem2next);
    }
}

static
void BubbleSortForwardList(My402List *pList, int num_items)
{
    My402ListElem *elem=NULL;
    int i=0;
	Transact *x,*y;
	
	
    if (My402ListLength(pList) != num_items) {
        fprintf(stderr, "List length is not %1d in BubbleSortForwardList().\n", num_items);
        exit(1);
    }
    for (i=0; i < num_items; i++) 
	{
        int j=0, something_swapped=FALSE;
        My402ListElem *next_elem=NULL;

        for (elem=My402ListFirst(pList), j=0; j < num_items-i-1; elem=next_elem, j++)
		{
			x=(Transact*)elem->obj;
			
            //int cur_val=(int)(elem->obj), next_val=0;
			long cur_val=(long)(x->ltime), next_val=0;
            next_elem=My402ListNext(pList, elem);
			y=(Transact*)next_elem->obj;
			
            next_val = (long)(y->ltime);
			
            if (cur_val > next_val) {
                BubbleForward(pList, &elem, &next_elem);
                something_swapped = TRUE;
            }
        }
        if (!something_swapped) break;
    }
}


int main(int argc, char *argv[])
{
		My402List list;
   // My402ListElem *elem=NULL;

    memset(&list, 0, sizeof(My402List));
    (void)My402ListInit(&list);
	
	//Transact t;
	
	
			FILE *fptr;
			//char address[100]="test.tfile";
		char buf[200];
	//printf("ARGC: %d\n",argc);
	if(argc<2)//if sort is missing
	{
		printf("Malformed command\n");
		exit(0);
	}
	if((strcmp(argv[1],"sort")))//if second argument is not 'sort'
	{
		printf("Malformed command\n");
		exit(0);
	}
	if(argc==2)
	{
			fptr=stdin;			
	}
	if(argc==3)
	{		
		if(!strstr(argv[2],".tfile"))
		{
			printf("Could not open file: %s\n",argv[2]);
			printf("Input file either a directory or not in correct format\n");
			exit(0);
		}
		
		fptr=fopen(argv[2], "rt"); 		
	}
    if (fptr==NULL) 
    { //perror("Error:");
       // printf("%s",argv[2]);
        printf("Could not open file: %s!\n",argv[2]); // error in opening file
		printf("%s\n", strerror(errno));
		exit(0);
		//perror("Error:");
        // getch();
        return 1; 
    }
    else
    {
	
		while(fgets(buf, sizeof(buf), fptr) != NULL)
		{
			// if (fgets(buf, sizeof(buf), fptr) == NULL)
		// {
		  // /* end of file */
		  // break;
		// } 
	//else 
	//	{
			//printf("%s",argv[3]);
			//printf("==%s==",buf);
			//printf("COMING TILL HERE!!!\n");
			if(strlen(buf)>1024)
			{
				printf("Invalid input, line is too long\n");
				exit(0);
			}
			long timeint;
			char timec[30];//change size 
			char op;
			//float amt;
			double a=0.0;
			char desc[40];//change size!!!
			//else 
			{
				
				Transact *t;
				t=malloc(sizeof(struct data));
				//printf("BUF: %s\n",buf);
				int i;
				
				char *start_ptr = buf;
				if(!strchr(start_ptr,'\t'))
				{
					printf("Invalid input, not enough info\n");
					exit(0);
				}
				//char *nline_ptr;
				if(strchr(start_ptr,'\r'))
				{
				char *nline_ptr = strchr(start_ptr,'\r');
				*nline_ptr++='\0';
				}
				if(strchr(start_ptr,'\n'))
				{
				char *nline_ptr = strchr(start_ptr,'\n');
				*nline_ptr++='\0';
				}
				char *tab_ptr = strchr(start_ptr, '\t');
				if (tab_ptr != NULL) 
				{
				  *tab_ptr++ = '\0';
				 // printf("%s\t",start_ptr);
				  op=start_ptr[0];
						//printf("OPERATOR= %c\n",op);
						t->op=op;
						if(((op!='+')&&(op!='-')))
						{
							printf("Invalid input with Operator: %c\n",t->op);
							exit(1);
						}
					//	printf("%c\t",t->op);
				}
				for(i=0;i<3;i++)
				{
					start_ptr = tab_ptr;
					// if(!strchr(start_ptr,'\t'))
					// {
						// printf("Invalid input, not enough info2\n");
						// //exit(0);
					// }
						tab_ptr = strchr(start_ptr, '\t');
					//nline_ptr= strchr(start_ptr, '\n');
					
					if ((tab_ptr != NULL)) 
					{
					  *tab_ptr++ = '\0';
					  
					}
					if(i==0)
					{
												
						if(strlen(start_ptr)>=11)
						{
							printf("Invalid input in timestamp: %s\n",start_ptr);
							exit(0);
						}
						if(!((atoi(start_ptr)>0)&&(atoi(start_ptr)<time(NULL))))
						{
							printf("Invalid input in timestamp\n");
							exit(0);
						}
					
						 timeint=atol(start_ptr);
						t->ltime=timeint;
						//printf("--TIME: %lu\t",timeint);
						strcpy(timec,ctime(&timeint));//time is stored in array time
						
					}
					else if(i==1)
					{
						//if(start_ptr[strlen(start_ptr)-3]
					  if(strlen(start_ptr)<4)//min should have one digit before decimal, 2 after and decimal itself
					  {
						printf("Invalid input, not enough digits or decimal\n");
						exit(0);
					  }
					 // if((start_ptr[strlen(start_ptr)-3]!='.')&&(start_ptr[0]=='.'))
					// printf("%c",start_ptr[strlen(start_ptr)-3]);
					  if((start_ptr[strlen(start_ptr)-3]!='.'))
					  {
						printf("Invalid input, does not contain decimal or enough digits after decimal: %s\n",start_ptr);
						exit(0);
					   }
					  if(start_ptr[0]=='.')
					  {
						printf("Invalid input, no digit before decimal\n");
						exit(0);
					  }
						a=atof(start_ptr);
						
						if((a>10000000)||(a<0))
						{
							printf("Invalid input with Amount\n");
							exit(1);
						}
						t->amt=a*100;
						
						
	
						// sprintf(chk, "%0.2f",a);
						// printf("STRING: %s\n",chk);
						// printf("Invalid input, does not contain decimal: %c\n",chk[strlen(chk)-2]);
						// // if(chk[strlen(chk)-3]!='.')
							// // printf("Invalid input, does not contain decimal: %c\n",chk[strlen(chk)-2]);
						
						//printf("AMOUNT: %f--\t",a);
						//printf("%f\t",a);
						//printf("%d\t",t->amt);
					}
					else if(i==2)
					{
						while(isspace(*start_ptr)) 
							start_ptr++;

					    if(*start_ptr == 0)  // All spaces?
						{
							printf("Invalid input..No description\n");
							exit(0);
						}
						if(strlen(start_ptr)>24)
							strncpy(desc,start_ptr,23);
						else
							strcpy(desc,start_ptr);
						
						strcpy(t->desc,desc);
						//t->desc=desc;
						//printf("Description%s",start_ptr);
						//printf("%s\n",t->desc);
					}
				//	printf("%s\t",start_ptr);
				}
				//printf("\n");
				if(checkTime(&list,t->ltime))//1 returned if same exists
				{
					printf("Two same timestamps detected\n");
					exit(0);
				}
				//printf("CTIME:%s",ctime(&time));	
				//printf("Appending\n");
				My402ListAppend(&list,t);
			
			}
		 //}
	  }//while loop
	}
	BubbleSortForwardList(&list,list.num_members);
	PrintTestList(&list,list.num_members);
  
 
    return(0);
}
