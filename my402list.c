#include<stdlib.h>
#include"my402list.h"

int My402ListInit(My402List *list)
{   
    list->num_members=0;
    list->anchor.next=&list->anchor;
    list->anchor.prev=&list->anchor;
    list->anchor.obj=NULL;
    return 1;
   
}
int My402ListLength(My402List* list)
{
    if(My402ListEmpty(list))
        return 0;
    return list->num_members;
}
int My402ListEmpty(My402List * list)
{
    if(list->anchor.next == &list->anchor && list->anchor.prev == &list->anchor)
        return 1;
    else
        return 0;
}

My402ListElem *My402ListFirst(My402List *list)
{
       
    if(My402ListEmpty(list))
        return NULL;
    else
        return list->anchor.next;
}

My402ListElem *My402ListLast(My402List *list)
{

   
    if(My402ListEmpty(list))
        return NULL;
    else
        return list->anchor.prev;
}

My402ListElem *My402ListNext(My402List *list,My402ListElem *item)
{
    if(My402ListLast(list) == item)
        return NULL;
    else
        return item->next;
}

My402ListElem *My402ListPrev(My402List *list,My402ListElem *item)
{
    if(My402ListFirst(list) == item)
        return NULL;
    else
        return item->prev;
       
}

int My402ListAppend(My402List *list,void *o)
{
    My402ListElem *node=NULL,*temp=NULL;
    node= (My402ListElem *) malloc (sizeof(struct tagMy402ListElem));   
    if(node!=NULL)
    {
   
        if(My402ListLength(list)==0)
        {
            list->anchor.next=node;
            node->prev=&list->anchor;
            node->next=&list->anchor;
            list->anchor.prev=node;
            node->obj=o;
            list->num_members++;
        }
        else
        {
            temp=My402ListLast(list);
            temp->next=node;
            node->prev=temp;
            node->next=&list->anchor;
            list->anchor.prev=node;
            node->obj=o;
            list->num_members++;
           
        }
   
    return 1;
    }
    else
        return 0;
}

int My402ListPrepend(My402List *list,void *o)
{
    My402ListElem *node,*temp=NULL;   
    node= (My402ListElem *) malloc (sizeof(struct tagMy402ListElem));   
    if(node!=NULL)
    {
       
        if(My402ListLength(list)==0)
        {
            list->anchor.next=node;
            node->prev=&list->anchor;
            node->next=&list->anchor;
            list->anchor.prev=node;
            node->obj=o;
            list->num_members++;       
        }
        else
        {
            temp=My402ListFirst(list);
            list->anchor.next= node;
            node->next=temp;
            temp->prev=node;
            node->prev=&list->anchor;
            node->obj=o;
            list->num_members++;
        }
       
    return 1;
    }
    else
        return 0;
}

void My402ListUnlink(My402List* list, My402ListElem *item)
{
    if(!My402ListEmpty(list))
    {
        item->prev->next=item->next;
        item->next->prev=item->prev;
        list->num_members--;
    }   
}

void My402ListUnlinkAll(My402List *list)
{   
    My402ListElem *temp=NULL;
    if(!My402ListEmpty(list))   
        for(temp=My402ListFirst(list);temp->obj!=NULL;temp=My402ListNext(list,temp))
        {
            temp->prev->next=temp->next;
            temp->next->prev=temp->prev;
            list->num_members--;
        }   
       
       
       
}

int My402ListInsertBefore(My402List *list,void *o,My402ListElem *item)
{
   
    if(My402ListLength(list)==0)
        My402ListPrepend(list,o);
    My402ListElem *node=NULL;
    node= (My402ListElem *) malloc (sizeof(struct tagMy402ListElem));
    if(node!= NULL)
    {
        item->prev->next=node;
        node->prev=item->prev;
        item->prev=node;
        node->next=item;
        node->obj=o;
        list->num_members++;
    }
   
    return 1;
}

int My402ListInsertAfter(My402List *list,void *o,My402ListElem *item)
{
    if(My402ListLength(list)==0)
        My402ListAppend(list,o);
    My402ListElem *node=NULL;   
    node= (My402ListElem *) malloc (sizeof(struct tagMy402ListElem));
    if(node!= NULL)
    {       
        item->next->prev=node;
        node->next=item->next;
        item->next=node;
        node->prev=item;
        node->obj=o;
        list->num_members++;   
    }
   
    return 1;
   
}

My402ListElem *My402ListFind (My402List *list, void *o)
{
    My402ListElem *temp=NULL,*p=NULL;
    for(temp=My402ListFirst(list);temp!=NULL;temp=My402ListNext(list,temp))
        if(temp->obj==o)
            p=temp;
    if(p==NULL)
        return NULL;
    else
        return p;
}     
