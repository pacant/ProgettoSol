#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
struct elemento{
    int stima;
    uint32_t id;
    int n;
    struct elemento* next;
};
typedef struct elemento lis;
typedef lis* lista;
void add(lista * list,uint32_t id,int stima);
lista* search(lista * list,uint32_t id);
void replace(lista * list,uint32_t id,int stima);
void stampa(lista head,int control);
void del_list(lista * list);
