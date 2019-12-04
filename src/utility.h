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
int timeval_subtract (struct timeval * result, struct timeval *x, struct timeval * y);
int MCD(int x, int y);
int stimate(int x, int y);
int min(int a,int b);
