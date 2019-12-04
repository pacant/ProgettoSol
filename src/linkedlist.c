#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "linkedlist.h"
void add(lista * list,uint32_t id,int stima){
    if(*list ==NULL){
        *list=(lista)malloc(sizeof(lis));
        (*list)->id=id;
        (*list)->n=1;
        (*list)->stima=stima;
        (*list)->next=NULL;
    }
    else{
        add(&(*list)->next,id,stima);
    }
}
lista* search(lista * list,uint32_t id){ // mi restituisce la stima per il client "id"
    if(*list!=NULL){
        if((*list)->id==id) return list;
        else return search(&(*list)->next,id);
    }
    return NULL;
}
void replace(lista * list,uint32_t id,int stima){
    if(*list!=NULL&&(*list)->id==id){
        (*list)->stima=stima;
    }
}
void stampa(lista head,int control){
    if(head==NULL)return;
    if(control) fprintf(stderr,"SUPERVISOR ESTIMATE %d FOR %x BASED ON %d\n",head->stima,head->id,head->n);
    else fprintf(stdout,"SUPERVISOR ESTIMATE %d FOR %x BASED ON %d\n",head->stima,head->id,head->n);
    stampa(head->next,control);
}
void del_list(lista * list){
    if((*list)->next!=NULL)del_list(&(*list)->next);
    free(*list);
}
/*int main(){
    int id1,id2,id3,stima1,stima2,stima3,stima4;
    scanf("%d",&id1);
    scanf("%d",&stima1);
    scanf("%d",&id2);
    scanf("%d",&stima2);
    scanf("%d",&id3);
    scanf("%d",&stima3);
    scanf("%d",&stima4);
    lista head;
    add(&head,id1,stima1);
    add(&head,id2,stima2);
    add(&head,id3,stima3);
    replace(&head,id3,stima4);
    stampa(head,0);
    return 0;
}*/
