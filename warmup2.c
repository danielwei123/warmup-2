#include<stdio.h>
#include<pthread.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/time.h>
#include<string.h>
#include<signal.h>
#include"my402list.h"

pthread_mutex_t lock;
pthread_cond_t cond;

sigset_t new;
struct sigaction act;
pthread_t ttoken,tarrival,tserver,tsignal;
long count,num,pc1,pc,cn,B,pac_dropped,to_dropped,tp1,tp2,nu1;
long long tim_sys;
long start_tim;
float sl_tim,ia_tim,st_tim,em_tim,q1_tim,q2_tim,s_tim,em2_tim;
float r;
FILE *fp;
My402List *q2,*q3;
My402List *q1,*q4;
struct timeval start;
int flag;

typedef struct stat
{
	float tim_g;
}PS;

typedef struct packet
{
	float lambda,mu;
	long P,no;
	long long tim_a,tim_s,tim_ia;
	
}P;
P *first;

	
void *ret_pac()
{
	My402ListElem *temp=My402ListFirst(q3);
	My402ListUnlink(q3,temp);
	if(q3->num_members==0)
		fclose(fp);
	return temp->obj;	
}

void *arrival_func(void * pac_struct)
{
	struct timeval te;
	My402ListElem *elem;
	P *p2;
	long r1=0,r_tim=0,re=0,i=0,re_a=0,pa=0;
	long qw=start_tim;
	
	nu1=num;
	
	P *p1=(struct packet*)malloc(sizeof(struct packet));
	for(i=0;i<nu1;i++)
	{
			
		if(fp!=NULL)
			p1=(P *)ret_pac();
		else
			p1=(P *)(pac_struct);

		r1=p1->lambda*1000;
		gettimeofday(&te,NULL);
		r_tim=(te.tv_sec*1000000+te.tv_usec);
		if((r1-r_tim+qw)>0)
		usleep(r1-r_tim+qw);
		else
		usleep(r1);
		
		
		gettimeofday(&te,NULL);
		qw=(te.tv_sec*1000000+te.tv_usec);
		
		r_tim=(te.tv_sec*1000000+te.tv_usec);
	
		p1->no=++pc;
		r_tim=r_tim-start_tim;
		re_a=r_tim-pa;
				
		ia_tim=ia_tim+(float)re_a/1000;

		
		pthread_mutex_lock(&lock);
		++tp1;
		
		if(p1->P >B)
		{
			pa=r_tim;
			printf("%012.3fms: p%ld arrives,needs %ld,dropped\n",(float)r_tim/1000,p1->no,p1->P);
		
			++pac_dropped;
			num--;
			
		}
		else {
		printf("%012.3fms: p%ld arrives,needs %ld tokens,inter-arrival time:%0.3fms\n",(float)r_tim/1000,p1->no,p1->P,(float)re_a/1000);

		pa=r_tim;
		

		gettimeofday(&te,NULL);
		r_tim=(te.tv_sec*1000000+te.tv_usec);
		p1->tim_a=r_tim;		
		r_tim=r_tim-start_tim;
		

		P *p4=(struct packet*)malloc(sizeof(struct packet));
		p4->tim_a=p1->tim_a;	
		p4->tim_s=p1->tim_s;	
		p4->lambda=p1->lambda;
		p4->mu=p1->mu;
		p4->P=p1->P;
		p4->no=p1->no;
		
		
		My402ListAppend(q1,p4);
		printf("%012.3fms: p%ld Enters q1\n",(float)r_tim/1000,p1->no);
		num=num-1;

		
		elem=My402ListFirst(q1);
		p2=(P *)malloc(sizeof(P));
		p2=((struct packet *)(elem->obj));
		
		
		
		if(count>=p2->P)
		{
				
			gettimeofday(&te,NULL);
			r_tim=(te.tv_sec*1000000+te.tv_usec);
			re=r_tim-p2->tim_a;
			r_tim=r_tim-start_tim;
			q1_tim+=(float)re/1000;
			
			My402ListUnlink(q1,My402ListFirst(q1));
			count=count-p2->P;
			printf("%012.3fms: p%ld leaves Q1,Time spent in Q1:%0.3fms,token bucket now has %ld token\n",(float)r_tim/1000,p2->no,(float)re/1000,count);


			

			gettimeofday(&te,NULL);
			r_tim=(te.tv_sec*1000000+te.tv_usec);
			p2->tim_s=r_tim;
			r_tim=r_tim-start_tim;
			
			
			My402ListAppend(q2,p2);
			pc1=pc1+1;

			if(pc1>0)
				pthread_cond_signal(&cond);
			printf("%012.3fms: p%ld enters Q2\n",(float)r_tim/1000,p2->no);
			
			
		}
		
		
		}
                pthread_mutex_unlock(&lock);
		
		
			
	}
	if(num==0)
		pthread_exit(0);
	return 0;	
}

void * token_func()
{
	
	struct timeval te;
	long re=0,r_tim=0,ts=0;
	My402ListElem *elem;
	long qw=start_tim;
	P *p1;
	
	
	
	for(;;)
	{
		pthread_testcancel();
		float r1= (1/r);
		ts=1000000*r1;
		gettimeofday(&te,NULL);
		r_tim=(te.tv_sec*1000000+te.tv_usec);
		if((ts-r_tim+qw)>0)
		usleep(ts-r_tim+qw);
		
		else
		usleep(ts);

		gettimeofday(&te,NULL);
		qw=(te.tv_sec*1000000+te.tv_usec);
		r_tim=(te.tv_sec*1000000+te.tv_usec);
		r_tim=r_tim-start_tim;

		pthread_mutex_lock(&lock);
		cn=cn+1;
		if(B<=count)
		{
			
			printf("%012.3fms: token t%ld arrives,dropped\n",(float)r_tim/1000,cn); 
			++to_dropped;
		}
	
		if(B>count)
		{

			count=count+1;
			printf("%012.3fms: token t%ld has arrived, token bucket now has %ld  tokens\n",(float)r_tim/1000,cn,count); 
		}
	
		if(q1->num_members>0)
		{
			elem=My402ListFirst(q1);
			p1=(struct packet*)elem->obj;
			
			if(p1->P<=count)
			{
				gettimeofday(&te,NULL);
				r_tim=(te.tv_sec*1000000+te.tv_usec);
				re=r_tim-p1->tim_a;
				r_tim=r_tim-start_tim;
				My402ListUnlink(q1,My402ListFirst(q1));
				
				pc1=pc1+1;
				
				count=count-p1->P;
				q1_tim+=(float)re/1000;
				printf("%012.3fms: p%ld leaves Q1,Time spent in Q1:%0.3fms,token bucket now has %ld token\n",(float)r_tim/1000,p1->no,(float)re/1000,count);

											
				gettimeofday(&te,NULL);
				r_tim=(te.tv_sec*1000000+te.tv_usec);
				p1->tim_s=r_tim;
				r_tim=r_tim-start_tim;
	
				
				My402ListAppend(q2,p1);
				
				if(pc1>0)
					pthread_cond_signal(&cond);
				printf("%012.3fms: p%ld enters Q2\n",(float)r_tim/1000,p1->no);
				
			
			}

		}
		pthread_mutex_unlock(&lock);
		if(num==0 && q1->num_members==0)
		{
			if(q2->num_members==0)
				pthread_cancel(tserver);
			pthread_exit(0);
		}
	}
	
}


void interrupt(int sig)
{
	flag=1;
	long r_tim;
	struct timeval te;
	gettimeofday(&te,NULL);
	r_tim=(te.tv_sec*1000000+te.tv_usec);
	r_tim=r_tim-start_tim;
	em2_tim=(float)r_tim/1000;
	if(q1->num_members!=0)
		My402ListUnlinkAll(q1);
	if(q2->num_members!=0)
		My402ListUnlinkAll(q2);
	pthread_cancel(ttoken);
	pthread_cancel(tarrival);
	pthread_cond_signal(&cond);
	
}


void *server_func()
{
		
	My402ListElem *elem;
	struct timeval te;
	P *p1;
	long r1=0,re=0,pc=0,re1=0;
	long r_tim=0;
	
	PS *sw;
	
	for(;;)
	{
		pthread_mutex_lock(&lock);
		
		
		while(q2->num_members==0)
		
			{if(flag==1){ pthread_exit(0);} pthread_cond_wait(&cond,&lock);}
			
		pc=pc+1;
				
		elem=My402ListFirst(q2);
		tp2++;
		p1=(struct packet*)malloc(sizeof(struct packet));
		p1=((struct packet*)(elem->obj));
		r1=p1->mu*1000;
		gettimeofday(&te,NULL);
		r_tim=(te.tv_sec*1000000+te.tv_usec);
		re1=r_tim;		
		re=r_tim-p1->tim_s;
		r_tim=r_tim-start_tim;
		q2_tim+=(float)re/1000;
		printf("%012.3fms: p%ld begins service S, time in Q2:%0.3fms\n",(float)r_tim/1000,p1->no,(float)re/1000);
				
		pthread_mutex_unlock(&lock);
		

		usleep(r1);
		gettimeofday(&te,NULL);
		
		
		pthread_mutex_lock(&lock);
		
		r_tim=(te.tv_sec*1000000+te.tv_usec);
		tim_sys+=(r_tim-p1->tim_a);
			
		re=r_tim-p1->tim_a;
		re1=r_tim-re1;
		st_tim+=(float)re1/1000;
		r_tim=r_tim-start_tim;
		em_tim=(float)r_tim/1000;
		s_tim+=(float)re1/1000;

		sw =(PS *)malloc(sizeof(PS));
		sw->tim_g=(float)re/1000;
		My402ListAppend(q4,sw);

		My402ListUnlink(q2,elem);
		printf("%012.3fms: p%ld leaves S,service time is:%0.3fms,time in system is %0.3fms\n",(float)r_tim/1000,p1->no,(float)re1/1000,(float)re/1000);
		
		pthread_mutex_unlock(&lock);
		
		if((q2->num_members==0 && num==0 && q1->num_members==0)||flag==1)
		{
			
			My402ListUnlinkAll(q2);
			pthread_exit(0);
		}

		
		
	}
	
	
}
void stats()
{
	My402ListElem *ele;
	
	PS *sw1=(PS *)malloc(sizeof(PS));
	float sys =(float)tim_sys/1000,temp=0,m=0,var=0;
	printf("\n\nSTATISTICS:\n");
	
	if(flag==1)
	{
		
		printf("\tThe average inter-arrival time:%.6gs\n",(ia_tim/tp1)/1000);
		if(tp2==0)
			printf("\tThe average service time:0s\n\n");
		else
		printf("\tThe average service time:%.6gms\n\n",(st_tim/tp2)/1000);
		printf("\tThe average No of packets spent in Q1:%.6g\n",q1_tim/em2_tim);
		printf("\tThe average No of packets spent in Q2:%.6g\n",q2_tim/em2_tim);
		printf("\tThe average No of packets spent in S:%.6g\n\n",s_tim/em2_tim);
		
		if(tp2!=0)
		{
			printf("\tThe average time a packet spent in system:%.6g\n",(sys/tp2)/1000);
			m=sys/tp2;
			for(ele=My402ListFirst(q4);ele!=NULL;ele=My402ListNext(q4,ele))
			{
				sw1=(PS *)ele->obj;
				temp+=pow(((sw1->tim_g-m)/1000),2);
			}
			var=temp/tp2;
			printf("\tThe standard deviation for time spent in system:%.6g\n\n",sqrt(var));
		}
		else
		{
			printf("\tThe average time a packet spent in system:0\n");
			printf("\tThe average time a packet spent in system:0\n");
			printf("\tThe standard deviation for time spent in system:0\n");
		}
	
		printf("\tPacket drop Propability: %.6g\n",(float) pac_dropped/tp1);
		if(cn!=0)
		printf("\tToken drop Propability: %.6g\n",(float) to_dropped/cn);
		else
			printf("\tToken drop Propability:0");


	}
	else if(tp1== pac_dropped)
	{
		printf("\tThe average inter-arrival time:%.6gs\n",(ia_tim/tp1)/1000);
		printf("\tThe average service time:0\n\n");
		printf("\tThe average No of packets spent in Q1:0\n");
		printf("\tThe average No of packets spent in Q2:0\n");
		printf("\tThe average No of packets spent in S:0\n\n");
		printf("\tThe average time a packet spent in system:0\n");
		printf("\tThe standard deviation for time spent in system:0\n\n");
		printf("\tPacket drop Propability: %.6g\n",(float) pac_dropped/tp1);
		printf("\tToken drop Propability: %.6g\n",(float) to_dropped/cn);
	}
	else
	{
		printf("\tThe average inter-arrival time:%.6gs\n",(ia_tim/tp1)/1000);
		printf("\tThe average service time:%.6gs\n\n",(st_tim/tp2)/1000);
		printf("\tThe average No of packets spent in Q1:%.6g\n",q1_tim/em_tim);
		printf("\tThe average No of packets spent in Q2:%.6g\n",q2_tim/em_tim);
		printf("\tThe average No of packets spent in S:%.6g\n\n",s_tim/em_tim);
		printf("\tThe average time a packet spent in system:%.6gs\n",(sys/tp2)/1000);
	
		m=sys/tp2;
		for(ele=My402ListFirst(q4);ele!=NULL;ele=My402ListNext(q4,ele))
		{
			sw1=(PS *)ele->obj;
			temp+=pow(((sw1->tim_g-m)/1000),2);
		}
		var=temp/tp2;
		
		printf("\tThe standard deviation for time spent in system:%.6g\n\n",sqrt(var));
		printf("\tPacket drop Propability: %.6g\n",(float) pac_dropped/tp1);
		printf("\tToken drop Propability: %.6g\n",(float) to_dropped/cn);
	}
	
}


int main(int argc, char * argv[])
{
	
	P *pac;
	long i;
	char buff[1024];
	float lam=0,m=0;
	sigemptyset(&new);
	sigaddset(&new,SIGINT);
	
	act.sa_handler=interrupt;
	sigaction(SIGINT,&act,NULL);
	

	q1=(My402List*)malloc(sizeof(My402List));
	q2=(My402List*)malloc(sizeof(My402List));
	q4=(My402List*)malloc(sizeof(My402List));
	
	
	
	pac=(struct packet *)malloc(sizeof(struct packet));
	pac->lambda=0;
	pac->mu=0;
	pac->P=0;
	if(argc>1)
	{
		for(i=1;i<argc;i=i+2)
		{
			if(argv[i][0]!='-')
			{
				printf("Enter the input in way\n");
				exit(0);
			}
		
			else if(strcmp(argv[i],"-t")==0)
			{
				fp=fopen(argv[i+1],"r");
				if(fp==NULL)
				{	
					printf("Invalid file\n");
					exit(0);
				}
				q3=(My402List*)malloc(sizeof(My402List));
				fgets(buff,1024,fp);
				num=atol(buff);
				fgets(buff,1024,fp);
				while(!feof(fp))
				{
					first=(P *)malloc(sizeof(P));
					sscanf(buff,"%f %ld %f",&first->lambda,&first->P,&first->mu);
					My402ListAppend(q3,first);
					fgets(buff,1024,fp);
				}
					
			}
			else if(strcmp(argv[i],"-n")==0)
			{
				sscanf(argv[i+1],"%ld",&num);
				if(num<0|| num >0x7fffffff)
				{
					printf("Enter a correct value\n");
					exit(0);
				}
			}
			else if(strcmp(argv[i],"-r")==0)
			{
				sscanf(argv[i+1],"%f",&r);
				if(r<0.1)
					r=0.1;
				
				if(r<0|| r >0x7fffffff)
				{
					printf("Enter a correct value\n");
					exit(0);
				}
			}
	
			else if(strcmp(argv[i],"-P")==0)
			{
				sscanf(argv[i+1],"%ld",&pac->P);
				if(pac->P<0|| pac->P>0x7fffffff)
				{
					printf("Enter a correct value\n");
					exit(0);
				}
			}
			else if(strcmp(argv[i],"-B")==0)
			{
				sscanf(argv[i+1],"%ld",&B);
				if(B<0 || B >0x7fffffff)
				{
					printf("Enter a correct value\n");
					exit(0);
				}
			}
		
			else if(strcmp(argv[i],"-lambda")==0)
			{
				sscanf(argv[i+1],"%f",&pac->lambda);
				
				if(pac->lambda<0.1)
					pac->lambda=0.1;
				lam=pac->lambda;
				pac->lambda=(1/pac->lambda)*1000;		
				printf("%f\n",pac->lambda);
				if(pac->lambda<0|| pac->lambda >0x7fffffff)
				{
					printf("Enter a correct value\n");
					exit(0);
				}
			}
			else if(strcmp(argv[i],"-mu")==0)
			{
				sscanf(argv[i+1],"%f",&pac->mu);
				
				if(pac->mu<0.1)
					pac->mu=0.1;
				m=pac->mu;
				pac->mu=(1/pac->mu)*1000;
				if(pac->mu<0||pac->mu >0x7fffffff)
				{
					printf("Enter a correct value\n");
					exit(0);
				}
			}
			else
			{
				printf("Wrong Input\n");
				exit(0);
			}
		}
	
	}
	
	if(pac->mu==0)
		pac->mu=(1/0.35)*1000;
	if(pac->lambda==0)
		pac->lambda=(1/0.5)*1000;
	if(pac->P==0)
		pac->P=3;
	if(r==0)
		r=1.5;
	if(B==0)
		B=10;
	if(num==0)
		num=20;
	
	
	if(fp==NULL)
		printf("Emulation parameters:\n\t\tlambda=%0.3f\n\t\tmu=%0.3f\n\t\tP=%ld\n\t\tr=%0.3f\n\t\tB=%ld\n\t\tnumber to arrive=%ld\n",lam,m,pac->P,r,B,num);
	else
		printf("Emulation parameters:\n\t\tB=%ld\n\t\tr=%f\n\t\tFilename:%s\n",B,r,argv[2]);
	gettimeofday(&start,NULL);
	start_tim=start.tv_sec*1000000+start.tv_usec;
	sl_tim=start_tim;
	printf("00000000.000ms:Emulation begins\n");
	pthread_create(&tarrival,NULL,arrival_func,(void *)pac);
	pthread_create(&ttoken,NULL,token_func,NULL);
	pthread_create(&tserver,NULL,server_func,NULL);

	
	pthread_join(ttoken,NULL);
	pthread_join(tarrival,NULL);
	pthread_join(tserver,NULL);
	
	stats();
	return 0;
		
}
