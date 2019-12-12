#define _POSIX_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/select.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include "mac.h"
#include "linkedlist.h"
#include "utility.h"

static volatile int supup=1;
pid_t * pids;
static time_t prectime;
lista list;

void createservers(int k, int pipes[k][2]){
    ec_null(pids=calloc(k,sizeof(pid_t)),"Errore allocando pids");
    int i,j;
    for(i=0;i<k;i++){
        if((pids[i]=fork())==-1){
            perror("Errore creando processo figlio\n");
            exit(EXIT_FAILURE);
        }
        if(pids[i]==0){
            fflush(stdout);
            //PROCESSO FIGLIO
            //chiudo la pipe in lettura e quelle che non mi servono
            for(j=0;j<k;j++){
                if(j!=i){
                    ec_meno1(close(pipes[j][0]),"Errore chiudendo la pipe in lettura (altre pipes)");
                    ec_meno1(close(pipes[j][1]),"Errore chiudendo la pipe in scrittura (altre pipes)");
                }
                else ec_meno1(close(pipes[j][0]),"Errore chiudendo la pipe in lettura");
            }
            char fdserver[4]; // stringhe per passare argomento alla exec
            char fdpipes[4];
            sprintf(fdserver,"%d",i);
            fflush(stdout);
            sprintf(fdpipes,"%d",pipes[i][1]);
            execl("server","server",fdserver,fdpipes,NULL);
            perror("Errore nella exec");
        }
    }
}
void* parsebuf(int *id, int* stima, char* msg){
    char * tok=NULL;
    tok = strtok(msg,",");
    if (tok==NULL) return NULL;
    *id=strtol(tok,NULL,16);
    tok=strtok(NULL,",");
    if(tok==NULL) return NULL;
    *stima= atoi(tok);
    return (void*)1;
}
void sigint_handler(int signum){
    if(time(NULL)-prectime <=1) supup=0; //metto ad 0 il flag del ciclo principale del supervisor
    else{
        stampa(list,1); // con un SIGINT stampo su stderr
    }
    prectime=time(NULL);
}
int main(int argc,char* argv[]){
    if(argc!=2){
        perror("Errore: argomenti non validi, passare 1 valore\n");
    }
    int k= atoi(argv[1]); //numero dei server da creare
    fprintf(stdout,"SUPERVISOR STARTING %d\n",k);
    fflush(stdout);
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler= sigint_handler;
    sigemptyset(&sa.sa_mask);
    ec_meno1(sigaction(SIGINT,&sa,NULL),"Errore nella sigaction\n");
    sa.sa_flags = SA_RESTART;

    int pipes[k][2];
    int i;
    fd_set set;
    fd_set rdset;
    FD_ZERO(&rdset);
    for(i=0;i<k;i++){
        ec_meno1(pipe(pipes[i]),"Errore creando pipe");
    }

    createservers(k,pipes);

    //chiudo le pipes anche per il supervisor, in scrittura
    for(i=0;i<k;i++){
        ec_meno1(close(pipes[i][1]),"Errore chiudendo pipes in scrittura");
    }
    FD_ZERO(&set);
    for (int i = 0; i < k; i++){
        FD_SET(pipes[i][0], &set);
    }
    int letti;
    lista *aux;
    int *id;
    ec_null(id=malloc(sizeof(int)),"Errore malloc");
    int * stima;
    ec_null(stima=malloc(sizeof(int)),"Errore malloc");
    char * msg;
    ec_null(msg=malloc(sizeof(MAXTOWRITE)),"Errore malloc");
    while(supup){ //esco dal while solo con i segnali
        // controllo le pipe pronte
        rdset=set;
        errno=0;
        if((select(FD_SETSIZE,&rdset,NULL,NULL,NULL)<0)){
            if(errno!=EINTR){
                perror("Errore nella select");
                exit(EXIT_FAILURE);
            }
            else continue;
        }
            for(i=0;i<k;i++){
                if(FD_ISSET(pipes[i][0],&rdset)){
                    letti=0;
                    if((letti=read(pipes[i][0],msg,MAXTOWRITE))<0){
                        perror("Errore leggendo dalla pipe");
                        exit(EXIT_FAILURE);
                    }
                    if(letti>0){
                        parsebuf(id,stima,msg); // ricava id e stima dal messaggio
                        fprintf(stdout,"SUPERVISOR ESTIMATE %d FOR %x FROM %d\n",*stima,*id,i);
                        fflush(stdout);
                        if(*stima!=-1){
                            if((aux=search(&list,*id))==NULL) add(&list,*id,*stima);
                            else{
                                (*aux)->n++;
                                *stima=min(*stima,(*aux)->stima);
                                replace(aux,*id,*stima);
                            }
                        }
                        msg= realloc(msg,sizeof(MAXTOWRITE));
                    }
                }
            }
    }
    for (i=0; i <k; i++) kill(pids[i], SIGTERM); // chiudo i processi server
    stampa(list,0); //ultima stampa della lista su stdout
    free(id);
    free(stima);
    free(msg);
    del_list(&list);
    if (pids) free(pids);
    fprintf(stdout,"SUPERVISOR EXITING");
    return 0;
}
