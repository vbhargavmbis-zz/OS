#include "my402list.h"
#include<stdio.h>
#include<stdlib.h>

extern int  My402ListLength(My402List*list)
{
	//printf("%d",list->num_members);
	return list->num_members;
}
extern int  My402ListEmpty(My402List*list)
{
	//if(list->anchor.next==NULL)
	if(list->num_members==0)
	{//printf("%d",list->num_members);
		return 1;
		}
	else
		return 0;
}
extern int  My402ListAppend(My402List*list, void*item)	//DRAW AND CHECK THIS------------------------------------------
{
		My402ListElem *elem;
		elem=malloc(sizeof(struct tagMy402ListElem));
		elem->obj=item;
	if(My402ListEmpty(list)==1) //if list is empty
	{
		elem->next=&list->anchor;
		elem->prev=&list->anchor;
		list->anchor.next=elem;
		list->anchor.prev=elem;
		list->num_members++;
		//printf("appended %d when list is empty\n",(int*)elem->obj);
		return(1);
	}
	else
	{
		elem->next=&list->anchor;
		elem->prev=list->anchor.prev;
		list->anchor.prev->next=elem;//last element 
		list->anchor.prev=elem;
		list->num_members++;
		//printf("appended %d when list is not empty\n",(int*)elem->obj);
		return(1);
	}
	return(0);
}
extern int  My402ListPrepend(My402List*list, void*item)
{
	My402ListElem *elem;
	elem=malloc(sizeof(struct tagMy402ListElem));
	elem->obj=item;
	if(My402ListEmpty(list)==1) //if list is empty
	{
		elem->next=&list->anchor;
		elem->prev=&list->anchor;
		list->anchor.next=elem;
		list->anchor.prev=elem;
		list->num_members++;
		return(1);
	}
	else
	{		
		elem->next=list->anchor.next;
		elem->prev=&list->anchor;
		list->anchor.next->prev=elem;
		list->anchor.next=elem;
		list->num_members++;
		return(1);
	}
	return(0);
}
extern void My402ListUnlink(My402List*list, My402ListElem*elem)
{
	if(list->num_members==0)
	{
		printf("No elements in the list\n");
	}
	if(list->num_members==1)
	{
		list->anchor.next=NULL;
		list->anchor.prev=NULL;
		list->num_members=0;
		free(elem);
	}
	else
	{
	  elem->prev->next=elem->next;
	  elem->next->prev=elem->prev;
	  list->num_members--;
	  free(elem);
	}
}
extern void My402ListUnlinkAll(My402List*list)
{
	 My402ListElem *curr=NULL;
	 My402ListElem *elem=NULL;
	  for (elem=My402ListFirst(list);elem != NULL;elem=My402ListNext(list, elem)) 
	  {
		curr=elem;
		free(curr);
	  }
	 list->anchor.next=NULL;
	 list->anchor.prev=NULL;
	 list->num_members=0;
}
extern int  My402ListInsertAfter(My402List*list, void*item, My402ListElem*elem)
{
	if(elem==NULL)
	{
		int x;
		x=My402ListAppend(list, item);
		if(x==1)
			return(1);
		else
			return(0);//not req
	}
	else
	{
		My402ListElem *curr=NULL;//element that is to be inserted
		curr=malloc(sizeof(struct tagMy402ListElem));
		curr->obj=item;
		curr->prev=elem;
		curr->next=elem->next;
		elem->next->prev=curr;
		elem->next=curr;
		list->num_members++;
		return(1);
	}
	return(0);
}
extern int  My402ListInsertBefore(My402List*list, void*item, My402ListElem*elem)
{
	if(elem==NULL)
	{
		int x;
		x=My402ListPrepend(list, item);
		if(x==1)
			return(1);
		else
			return(0);
	}
	else
	{
		My402ListElem *curr=NULL;//element that is to be inserted
		curr=malloc(sizeof(struct tagMy402ListElem));
		curr->obj=item;
		curr->prev=elem->prev;
		curr->next=elem;
		elem->prev->next=curr;
		elem->prev=curr;
		list->num_members++;
		return(1);
	}
	return(0);
}

extern My402ListElem *My402ListFirst(My402List*list)
{
	if(My402ListEmpty(list)==1) //if list is empty
	{
		return(NULL);
	}
	else
	{
		return(list->anchor.next);
	}
}
extern My402ListElem *My402ListLast(My402List*list)
{
	if(My402ListEmpty(list)==1) //if list is empty
	{
		return(NULL);
	}
	else
	{
		return(list->anchor.prev);
	}
}
extern My402ListElem *My402ListNext(My402List*list, My402ListElem*elem)
{
	if(elem->next==&list->anchor)
	{
		return(NULL);
	}
	else
	{
		return(elem->next);
	}
}
extern My402ListElem *My402ListPrev(My402List*list, My402ListElem*elem)
{
	if(elem->prev==&list->anchor)
	{
		return(NULL);
	}
	else
	{
		return(elem->prev);
	}
}

extern My402ListElem *My402ListFind(My402List*list, void*item)
{
	  My402ListElem *elem=NULL;
	  elem=malloc(sizeof(struct tagMy402ListElem));
	  for (elem=My402ListFirst(list);elem != NULL;elem=My402ListNext(list, elem)) 
	  {
		//printf("--%d--",(int*)elem->obj);
		if(elem->obj==item)//IS THIS CORRECT??? (item is in the list having some addr, check if its the same addr
		{
			return(elem);
		}
	  }
	  return(NULL);
}

extern int My402ListInit(My402List*list)
{
	//struct tagMy402List *list;
	list=malloc(sizeof(struct tagMy402List));//needed???
	list->num_members=0;
	list->anchor.next=NULL;
	list->anchor.prev=NULL;
	list->anchor.obj=NULL;
	
	//printf("Initialized List:\n");
	//printf("OBJ: %p",list->anchor.obj);
	return(1);
}
/*int main()
{
	printf("Helloworld\n");
}*/
