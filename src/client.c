#define _POSIX_SOURCE 199309L
#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include "mac.h"
struct s{
    int fd;
    int in;
};
typedef struct s elencoserver;

int main(int argc, char* argv[]){
    if(argc!=4){
        perror("Errore: argomenti non validi, passare 3 valori");
        exit(EXIT_FAILURE);
    }
    srand(time(0)*getpid());

    int p= atoi(argv[1]); // numero di server con cui comunicare
    int k= atoi(argv[2]); // numero di server attivi
    int w= atoi(argv[3]); // messaggi da inviare
    if(p>k||p<1||w<=3*p){
        perror("Errore, rispettare i vincoli: 1<= p < k, w>3p");
        exit(EXIT_FAILURE);
    }
    // p, k e w, con 1 ≤ p < k e w > 3p

    srand48((long int)time(0)*getpid()); //imposto seed rand48

    uint32_t id= lrand48(); //genero id
    int secret= rand()%3000 + 1; //genero il secret fra 1 e 3000

    printf("CLIENT %x SECRET %d\n",id,secret); // stampo id e secret

    int i,aux,sfd;
    elencoserver* serv;
    ec_null(serv=malloc(sizeof(elencoserver)*k),"Errore malloc"); // creo una struct per tenere elenco dei server con i loro fd
    for(i=0;i<k;i++){
        serv[i].in=0;
        serv[i].fd=0;
    }
    for(i=0;i<p;i++){
        do {
            aux=rand()%k;
        }
        while(serv[aux].in);
        serv[aux].in=1;
        if ((sfd=socket(AF_UNIX,SOCK_STREAM,0))<= 0) {
            perror("Errore nella creazione del socket");
            exit(EXIT_FAILURE);
        }
        serv[aux].fd=sfd;
        struct sockaddr_un sa;
        memset(&sa,'0',sizeof(sa));
        sa.sun_family=AF_UNIX;
        sprintf(sa.sun_path,"OOB-server-%d",aux);
        //fprintf(stdout,"%d - Server\n",aux);
        ec_meno1(connect(sfd,(struct sockaddr*)&sa,sizeof(sa)),"Errore nella connessione al socket");
    }

    //for(i=0;i<k;i++) if(serv[i].in) fprintf(stdout,"Server fd %i :%d\n",i,serv[i].fd);
    // qui sono connesso ai server

    int serverconn[p]; //array di appoggio
    int j= -1;
    for(i=0;i<p;i++) serverconn[i] = 0;
    for(i=0;i<k&&j<p;i++){
        if(serv[i].in){
            j++;
            serverconn[j]=serv[i].fd; //serverconn contiene i fd dei p server connessi
        }
    }

    uint32_t nbo=htonl(id); //converto in Network Byte Order per inviarlo al server
    uint32_t * tosend;
    ec_null(tosend=malloc(sizeof(uint32_t)),"Errore malloc");
    memcpy(tosend,&nbo,sizeof(uint32_t)); // metto l'id in nbo nel buffer

    struct timespec nqtp;
    nqtp.tv_sec=secret/1000;  // imposto la nanosleep
    nqtp.tv_nsec=(secret%1000)*NTOMSEC;

    int towrite;
    for(i=0;i<w;i++){
        towrite=rand()%p;
        ec_meno1(write(serverconn[towrite],tosend,sizeof(uint32_t)),"Errore writen");
        nanosleep(&nqtp,NULL);
    }

    //da millisecondi a nanosecondi -> * 10 ^ 6  usare nanosleep()
    return 0;
}
