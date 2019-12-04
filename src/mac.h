#define MAC_H
#define MAX_COMMAND 255
#define MAXTOWRITE \
        sizeof(long long int)+sizeof(int)+(sizeof(char)*3)
#define UNIX_PATH_MAX 108
#define MAX_HEADER 40
#define MAX_BUFFSIZE 1024 
#define MAX_NAME 101
#define NTOMSEC 1e6
#define ec_meno1(s,m) \
 if ( (s) < 0 ) {perror(m); exit(EXIT_FAILURE);}
#define ec_meno1zero(s,m) \
 if ( (s) <= 0 ) {perror(m); exit(EXIT_FAILURE);}
#define ec_null(s,m) \
 if((s)==NULL) {perror(m); exit(EXIT_FAILURE);}
#define ec_notzero(s,m) \
 if ( (s)!= 0) {perror(m); exit(EXIT_FAILURE);}
 
