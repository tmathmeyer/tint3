#include <stdio.h>
#include <signal.h>
#include <sys/time.h>



int limit = 10;
/* signal process */
void timeout_info(int signo)
{
   if(limit == 0)
   {
       printf("Sorry, time limit reached.\n");
       exit(0);
   }
   printf("only %d senconds left.\n", limit--);
}

/* init sigaction */
void init_sigaction(void)
{
    struct sigaction act;

    act.sa_handler = timeout_info;
    act.sa_flags   = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGPROF, &act, NULL);
} 

/* init */
void init_time(void)
{
    struct itimerval val;

    val.it_value.tv_sec = 1;
    val.it_value.tv_usec = 0;
    val.it_interval = val.it_value;
    setitimer(ITIMER_PROF, &val, NULL);
}


int main(void)
{
    char *str;
    char c;

    init_sigaction();
    init_time();
    printf("You have only 10 seconds for thinking.\n");

    while(1);
    exit(0);
}