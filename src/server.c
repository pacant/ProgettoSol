#define _POSIX_SOURCE 200809L
#define _XOPEN_SOURCE 700


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <errno.h>
#include <sys/select.h>
#include <string.h>
#include <limits.h>
#include <arpa/inet.h>
#include <sys/signal.h>
#include <signal.h>
#include <sys/time.h>
#include "mac.h"
#include "utility.h"
static volatile int running=1;
typedef struct _clientargs{
    int fdpipe;
    int fdclient;
    int fdserver;
}clientargs;

int mandastima(int pipefd, int fdserver, int estimate, uint32_t id){
    char message[MAXTOWRITE] = {0};
    if (id==0) return 0;
    sprintf(message, "%x,%d", id, estimate);
    ssize_t scritti = write(pipefd, message, MAXTOWRITE);
    fprintf(stdout,"SERVER %d CLOSING %x ESTIMATES %d\n",fdserver,id,estimate);
    if (scritti == -1) return -1;
    fflush(stdout);
    return 0;
}

static void* workerclient(void * arg){
    clientargs* client=(clientargs*) arg;
    int fdclient= client->fdclient;
    int fdserver= client->fdserver;
    int fdpipe=client->fdpipe;
    struct timeval curr;
    struct timeval diff;
    struct timeval prec;
    prec.tv_sec=INT_MAX;
    int letti;
    int count=0;
    uint32_t * id;
    ec_null(id=malloc(sizeof(uint32_t)),"Errore malloc");
    uint32_t result;
    long min=INT_MAX;
    char* est=calloc(1,32);
    ec_meno1((letti=read(fdclient,id,sizeof(uint32_t))),"errore readn");
    do{
        count++;
        gettimeofday(&curr,NULL);
        result=ntohl(*id);
    
        timeval_subtract(&diff,&curr,&prec); //FUNZIONE TROVATA SU INTERNET, CALCOLA LA DIFFERENZA TRA I TEMPI©
        est= realloc(est,32);
        sprintf(est,"%ld%03d",labs(diff.tv_sec),abs(diff.tv_usec/1000));
        long min_est=strtol(est,NULL,10);
        if(min>min_est) min=min_est;
        prec=curr;
        fprintf(stdout,"SERVER %d INCOMING FROM %x @ %ld.%ld\n",fdserver,result,curr.tv_sec%60,(long)curr.tv_usec/1000);
    }
    while((letti=read(fdclient,id,sizeof(uint32_t))));
    close(fdclient);
    free(id);
    if (count==1){
        min= -1;
    }
    mandastima(fdpipe,fdserver,min,result);
    free(est);
    pthread_exit(NULL);
}
void sigterm_handler(int signum){
    if (signum==SIGTERM)running=0;
}
int main(int argc, char* argv[]){
    int fd= atoi(argv[1]);
    int fdpipe = atoi(argv[2]);
    signal(SIGINT, SIG_IGN);
    struct sigaction sg;
    sg.sa_handler= sigterm_handler;
    sigemptyset(&sg.sa_mask);
    ec_meno1(sigaction(SIGTERM,&sg,NULL),"Errore nella sigaction\n");
    sg.sa_flags = SA_RESTART;

    fprintf(stdout,"SERVER %d ACTIVE\n",fd);
    pthread_t worker; 

    // creo la socket
    int fd_skt;
    ec_meno1(fd_skt=socket(AF_UNIX,SOCK_STREAM,0),"Errore creando la socket\n");
    struct sockaddr_un sa;
    memset(&sa, '0', sizeof(sa));
    sa.sun_family=AF_UNIX;
    char * sockname;
    ec_null(sockname=malloc(sizeof(char)*14),"Errore malloc");
    sprintf(sockname,"OOB-server-%d",fd);
    strncpy(sa.sun_path,sockname,14);
    unlink(sockname);
    ec_meno1(bind(fd_skt,(struct sockaddr *)&sa,sizeof(sa)),"Errore bind socket\n");
    ec_meno1(listen(fd_skt,SOMAXCONN),"Errore listen socket\n");

    //inizio con le accept dei client, facendo partire per ogni client un thread worker
    int cfd;
    clientargs * client;
    while(running){
        if((cfd=accept(fd_skt,NULL,0))==-1){
         perror("Errore nell'accept del socket\n");
        }
        fprintf(stdout,"SERVER %d CONNECT FROM CLIENT\n",fd);
        //pthread_mutex_lock(&lock);
        client = malloc(sizeof(clientargs));
        client->fdclient=cfd;
        client->fdpipe=fdpipe;
        client->fdserver=fd;
        //pthread_mutex_unlock(&lock);
        if(pthread_create(&worker,NULL,workerclient,client)!=0){
            perror("Errore creando il thread dispatcher\n");
            close(cfd);
        }
    }
    close(fdpipe);
    free(client);
}
