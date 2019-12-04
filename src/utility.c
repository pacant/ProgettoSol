#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <errno.h>
#include <sys/select.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
int timeval_subtract (result, x, y)
     struct timeval *result, *x, *y;{

  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}
int MCD(long x, long y){
    if (x == 0) return y;
    return MCD(y%x, x);
}
int min(int a,int b){
  return a<b ? a : b;
}
int stimate(long x, long y){  
    long z;
    if(x==1) return y;
    if(y==1) return x;
    z=MCD(x,y);
    if(z==1) z = x < y ? x:y;
    return z;       
}
