#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
/*car thread variable and truck thread varible*/
pthread_t vehicle_thread;
pthread_t truck_thread;
pthread_t car_thread;
pthread_t captain_thread;
/*global varibale*/
int seed;
int car_probability;
int truck_probability;
int max_length;
int K;
int terminate=0;

/*this is the 8 counter*/
int cars_queued_counter;
int trucks_queued_counter;
int cars_loaded_counter;
int trucks_loaded_counter;
int cars_sailing_counter;
int trucks_sailing_counter;
int cars_unloaded_counter;
int trucks_unloaded_counter;
/*end of counter*/
/*extra counter*/
int cars_exit_counter;
int trucks_exit_counter;


/*this is initialize the semaphores and mutexes*/
sem_t cars_waiting_line;
sem_t trucks_waiting_line;
sem_t cars_loaded_ferry;
sem_t trucks_loaded_ferry;
sem_t cars_sailing;
sem_t trucks_sailing;
sem_t cars_unloaded_ferry;
sem_t trucks_unloaded_ferry;
sem_t cars_exit;
sem_t trucks_exit;

pthread_mutex_t protect_cars_queued_counter;
pthread_mutex_t protect_trucks_queued_counter;
pthread_mutex_t protect_cars_loaded_counter;
pthread_mutex_t protect_trucks_loaded_counter;
pthread_mutex_t protect_cars_sailing_counter;
pthread_mutex_t protect_trucks_sailing_counter;
pthread_mutex_t protect_cars_unloaded_counter;
pthread_mutex_t protect_trucks_unloaded_counter;
pthread_mutex_t protect_trucks_exit_counter;
pthread_mutex_t protect_cars_exit_counter;
/*The pshared argument indicates whether this semaphore is to be shared
  between the threads of a process, or between processes.
  If pshared has the value 0, then the semaphore is shared between the
  threads of a process.
  sem_init() returns 0 on success; on error, -1 is returned, and errno
  is set to indicate the error.
*/
int check_sem_init(sem_t *sem, int pshared, unsigned int value);
/*here we use the defult value, so mattr=NULL;*/
int check_pthread_mutex_init(pthread_mutex_t *mp,const pthread_mutexattr_t *mattr);


/*create function*/
void *create_vehicle_thread(void *arg);
void *create_car(void *arg);
void *create_truck(void *arg);
void *create_captain(void *arg);


/*time function*/
int timeChange( const struct timeval startTime);


/*sem check and post function*/
int check_sem_wait(sem_t *sem);
int check_sem_post(sem_t *sem);

/*destory*/
int check_pthread_mutex_destroy(pthread_mutex_t *mutex);
int check_sem_destroy(sem_t *sem);


int main(int argc, char *argv[])
{	

	/*part(a) this function is used to initialize all needed mutexes and semaphores*/
	check_sem_init(&cars_waiting_line,0,0);
	check_sem_init(&trucks_waiting_line,0,0);
	check_sem_init(&cars_loaded_ferry,0,0);
	check_sem_init(&trucks_loaded_ferry,0,0);
	check_sem_init(&cars_sailing,0,0);
	check_sem_init(&trucks_sailing,0,0);
	check_sem_init(&cars_unloaded_ferry,0,0);
	check_sem_init(&trucks_unloaded_ferry,0,0);
	check_sem_init(&cars_exit,0,0);
	check_sem_init(&trucks_exit,0,0);
	
	check_pthread_mutex_init(&protect_cars_queued_counter,NULL);
	check_pthread_mutex_init(&protect_trucks_queued_counter,NULL);
	check_pthread_mutex_init(&protect_cars_loaded_counter,NULL);
	check_pthread_mutex_init(&protect_trucks_loaded_counter,NULL);
	check_pthread_mutex_init(&protect_cars_sailing_counter,NULL);
	check_pthread_mutex_init(&protect_trucks_sailing_counter,NULL);
	check_pthread_mutex_init(&protect_cars_unloaded_counter,NULL);
	check_pthread_mutex_init(&protect_trucks_unloaded_counter,NULL);
	check_pthread_mutex_init(&protect_trucks_exit_counter,NULL);
	check_pthread_mutex_init(&protect_cars_exit_counter,NULL);
	/*part(b) set input part*/
	printf("Please enter integer values for the following variables\n");
	printf("Enter the percent probability that the next vehicle is a truck\n");
	scanf("%d",&truck_probability);
	car_probability=100-truck_probability;
	printf("Enter the maximum length of the interval between vehicles\n");
	printf("time interval should be >1000\n");
	scanf("%d",&max_length);
	printf("Enter the seed for random number generation\n");
	scanf("%d", &seed);
	srand(seed);
	
	/*part(c) create a thread to create vehicles*/
	/*If thread is created successfully, the first argument will hold the thread id
   	  The second argument It could be priority.
          The third argument is a function pointer. This is something to keep in mind that each thread starts with a function and that functions address is passed here as the third argument so that the kernel knows which function to start the thread from.    
	 this pointer could be a pointer to a structure that may contain these arguments.
	*/
	pthread_create(&vehicle_thread, NULL, create_vehicle_thread, NULL);
	pthread_create(&captain_thread, NULL, create_captain, NULL);
	pthread_join(vehicle_thread, NULL);
	pthread_join(captain_thread, NULL);
	
	check_pthread_mutex_destroy(&protect_cars_queued_counter);
	check_pthread_mutex_destroy(&protect_trucks_queued_counter);
	check_pthread_mutex_destroy(&protect_cars_loaded_counter);
	check_pthread_mutex_destroy(&protect_trucks_loaded_counter);
	check_pthread_mutex_destroy(&protect_cars_sailing_counter);
	check_pthread_mutex_destroy(&protect_trucks_sailing_counter);
	check_pthread_mutex_destroy(&protect_cars_unloaded_counter);
	check_pthread_mutex_destroy(&protect_trucks_unloaded_counter);
	check_pthread_mutex_destroy(&protect_trucks_exit_counter);
	check_pthread_mutex_destroy(&protect_cars_exit_counter);

	check_sem_destroy(&cars_waiting_line);
	check_sem_destroy(&trucks_waiting_line);
	check_sem_destroy(&cars_loaded_ferry);
	check_sem_destroy(&trucks_loaded_ferry);
	check_sem_destroy(&cars_sailing);
	check_sem_destroy(&trucks_sailing);
	check_sem_destroy(&cars_unloaded_ferry);
	check_sem_destroy(&trucks_unloaded_ferry);
	check_sem_destroy(&cars_exit);
	check_sem_destroy(&trucks_exit);
	
	
	return 0;
	
}


int check_sem_init(sem_t *sem, int pshared, unsigned int value)
{	
	int ret;
	ret = sem_init(sem,pshared,value);            
	if(ret!=0)
	{
		printf("semaphores initilize is not successful! program will exit\n");
		exit(0);
	}
	return ret; 

}


int check_pthread_mutex_init(pthread_mutex_t *mp,const pthread_mutexattr_t *mattr)
{	
	int ret=pthread_mutex_init(mp,mattr);
	if(ret<0)
	{	
		printf("mutex initilize is not successful! program will exit\n");
		exit(0);
	}
	return ret; 
}

void *create_vehicle_thread(void *arg)
{	
	
	struct timeval startTime;
	printf("CREATEVEHICLE   Vehicle creation thread has been started\n");
	gettimeofday(&startTime, NULL);
	int next_arrijjval_time=0;
	int elapsed_time=timeChange(startTime);
	printf("CREATEVEHICLE:   Elapsed time %d msec\n",elapsed_time);
	while(terminate==0)
	{       
	
		int random_probability;
		random_probability=rand()%100;
		next_arrijjval_time=next_arrijjval_time+rand()%(max_length-1000)+1000;
		printf("CREATEVEHICLE:   Next arrival time %d msec\n",next_arrijjval_time);
		while(elapsed_time<=next_arrijjval_time)
		{
			elapsed_time=timeChange(startTime);
		}
		elapsed_time--;
		printf("CREATEVEHICLE:   Elapsed time %d msec\n",elapsed_time);
		
		if (random_probability<=truck_probability && next_arrijjval_time!=0)
		{       
			
			printf("CREATEVEHICLE   Created a truck thread\n");
			pthread_create(&truck_thread, NULL,create_truck, NULL);
		}
		else
		{
			printf("CREATEVEHICLE   Created a car thread\n");
			pthread_create(&car_thread, NULL,create_car, NULL);
		}

		
	}
	
}

void *create_truck(void *arg)
{	
	/*a*/
	long int get_id=(long int)pthread_self();
	printf("CAR:   Truck with threadID %li queued\n",get_id);
	pthread_mutex_lock(&protect_trucks_queued_counter);
	trucks_queued_counter++;
	pthread_mutex_unlock(&protect_trucks_queued_counter);
	check_sem_wait(&trucks_waiting_line);
	
	/*b*/
	printf("CAR:   Truck with threadID %li leaving the queue to load\n", get_id);
	
	/*c figure out later*/
	check_sem_post(&trucks_loaded_ferry);
	pthread_mutex_lock(&protect_trucks_loaded_counter);
	trucks_loaded_counter++;
	pthread_mutex_unlock(&protect_trucks_loaded_counter);
	
	/*d*/
	printf("CAR:   Truck with threadID %li is onboard the ferry\n",get_id);
	check_sem_wait(&trucks_sailing);
	pthread_mutex_lock(&protect_trucks_sailing_counter);
	trucks_sailing_counter++;
	pthread_mutex_unlock(&protect_trucks_sailing_counter);
	
	/*e*/
	printf("CAR:   Truck with threadID %li is now unloading\n",get_id);
	
	/*f figure out later*/
	check_sem_post(&trucks_unloaded_ferry);
	pthread_mutex_lock(&protect_trucks_unloaded_counter);
	trucks_unloaded_counter++;
	pthread_mutex_unlock(&protect_trucks_unloaded_counter);
	
	/*g*/
	printf("CAR:   Truck with threadID %li has unloaded\n",get_id);

	/*h figure out later*/
	check_sem_wait(&trucks_exit);
	pthread_mutex_lock(&protect_trucks_queued_counter);
	trucks_exit_counter++;
	pthread_mutex_unlock(&protect_trucks_queued_counter);
	printf("CAR:   Truck with threadID %li is about to exit\n",get_id);
	
	/*i*/
	pthread_exit(0);
	
	
	
}

void *create_car(void *arg)
{	
	/*a*/
	long int get_id=(long int)pthread_self();
	printf("CAR:   Car with threadID %li queued\n",get_id);
	pthread_mutex_lock(&protect_cars_queued_counter);
	cars_queued_counter++;
	pthread_mutex_unlock(&protect_cars_queued_counter);
	check_sem_wait(&cars_waiting_line);
	
	/*b*/
	printf("CAR:   Car with threadID %li leaving the queue to load\n", get_id);
	
	/*c figure out later*/
	check_sem_post(&cars_loaded_ferry);
	pthread_mutex_lock(&protect_cars_loaded_counter);
	cars_loaded_counter++;
	pthread_mutex_unlock(&protect_cars_loaded_counter);
	
	/*d*/
	printf("CAR:   Car with threadID %li is onboard the ferry\n",get_id);
	pthread_mutex_lock(&protect_cars_sailing_counter);
	cars_sailing_counter++;
	pthread_mutex_unlock(&protect_cars_sailing_counter);
	check_sem_wait(&cars_sailing);
	
	/*e*/
	printf("CAR:   Car with threadID %li is now unloading\n",get_id);
	
	/*f figure out later*/
	check_sem_post(&cars_unloaded_ferry);
	pthread_mutex_lock(&protect_cars_unloaded_counter);
	cars_unloaded_counter++;
	pthread_mutex_unlock(&protect_cars_unloaded_counter);
	
	/*g*/
	printf("CAR:   Car with threadID %li has unloaded\n",get_id);

	/*h figure out later*/
	check_sem_wait(&cars_exit);
	pthread_mutex_lock(&protect_cars_queued_counter);
	cars_exit_counter++;
	pthread_mutex_unlock(&protect_cars_queued_counter);
	printf("CAR:   Car with threadID %li is about to exit\n",get_id);
	
	/*i*/
	pthread_exit(0);
}

void *create_captain(void *arg)
{	
        
	printf("CAPTAIN:   captain thread started\n");
	int vehicle_number=0;
	int check_car_number=0;
	int check_truck_number=0;
	int late_car_number=0;
	int late_truck_number=0;
	int travel_time=0;

	while(travel_time<5)
	{	
	
		while(vehicle_number<8)
		{	
			pthread_mutex_lock(&protect_trucks_queued_counter);
			check_truck_number=trucks_queued_counter;
			pthread_mutex_unlock(&protect_trucks_queued_counter);
			pthread_mutex_lock(&protect_cars_queued_counter);
			check_car_number=cars_queued_counter;
			vehicle_number=trucks_queued_counter+cars_queued_counter;
			pthread_mutex_unlock(&protect_cars_queued_counter);
			
		}
		
		if(vehicle_number>=8)
		{
			printf("loop start\n");
			if(check_truck_number>=2 && check_car_number>=2)
			{
				for(int i=0;i<2;i++)
				{		
					
					printf("CAPTAIN:   Truck selected for loading\n");				
					check_sem_post(&trucks_waiting_line);
					pthread_mutex_lock(&protect_trucks_queued_counter);
					trucks_queued_counter--;
					pthread_mutex_unlock(&protect_trucks_queued_counter);
					check_sem_wait(&trucks_loaded_ferry);
					printf("CAPTAIN: Captain knows truck is loaded\n");

				}
				for(int i=0;i<2;i++)
				{	
					printf("CAPTAIN: Car selected for loading\n");
					check_sem_post(&cars_waiting_line);
					pthread_mutex_lock(&protect_cars_queued_counter);
					cars_queued_counter--;
					pthread_mutex_unlock(&protect_cars_queued_counter);
					check_sem_wait(&cars_loaded_ferry);
					printf("CAPTAIN:   Captain knows car is loaded\n");

				}
			}
			else if(check_truck_number==0 && check_car_number>=6)
			{
				for(int i=0;i<6;i++)
				{	
					printf("CAPTAIN:   Car selected for loading\n");
					check_sem_post(&cars_waiting_line);
					pthread_mutex_lock(&protect_cars_queued_counter);
					cars_queued_counter--;
					pthread_mutex_unlock(&protect_cars_queued_counter);
					check_sem_wait(&cars_loaded_ferry);
					printf("CAPTAIN:   Captain knows car is loaded\n");
				}
				
			}
			else if(check_truck_number==1 && check_car_number>=4)
			{
				for(int i=0;i<1;i++)
				{		
					printf("CAPTAIN:   Truck selected for loading\n");
					check_sem_post(&trucks_waiting_line);
					pthread_mutex_lock(&protect_trucks_queued_counter);
					trucks_queued_counter--;
					pthread_mutex_unlock(&protect_trucks_queued_counter);
					check_sem_wait(&trucks_loaded_ferry);
					printf("CAPTAIN:   Captain knows truck is loaded\n");
				}
				for(int i=0;i<=3;i++)
				{	
					printf("CAPTAIN:   Car selected for loading\n");
					check_sem_post(&cars_waiting_line);
					pthread_mutex_lock(&protect_cars_queued_counter);
					cars_queued_counter--;
					pthread_mutex_unlock(&protect_cars_queued_counter);
					check_sem_wait(&cars_loaded_ferry);
					printf("CAPTAIN:   Captain knows car is loaded\n");

				}
			}
			printf("\n");
			printf("CAPTAIN:   Ferry is full,staring to sail\n\n");	
		}
		if(travel_time==4)
		{
		   terminate=1;
		}
		
		usleep(2000000);
		printf("CAPTAIN:   Ferry has reached the destination port\n");
		

		while(trucks_loaded_counter>0)
		{	
			pthread_mutex_lock(&protect_trucks_loaded_counter);
			trucks_loaded_counter--;
			pthread_mutex_unlock(&protect_trucks_loaded_counter);
			check_sem_post(&trucks_sailing);
			check_sem_wait(&trucks_unloaded_ferry);
			printf("CAPTAIN:   Captain knows a truck has unloaded from the ferry\n");
			printf("CAPTAIN:   Captain sees a truck leaving the ferry termnial\n");
			check_sem_post(&trucks_exit);
			
		}
		
		while(cars_loaded_counter>0)
		{
			pthread_mutex_lock(&protect_cars_loaded_counter);
			cars_loaded_counter--;
			pthread_mutex_unlock(&protect_cars_loaded_counter);
			check_sem_post(&cars_sailing);
			check_sem_wait(&cars_unloaded_ferry);
			printf("CAPTAIN:   Captain knows a car has unloaded from the ferry\n");
			printf("CAPTAIN:   Captain sees a car leaving the ferry termnial\n");
			check_sem_post(&cars_exit);
	
		}
		

		printf("\n");
		printf("CAPTAIN:   Unloading complete\n\n");
		
		usleep(2000000);
		
		printf("\n\n\n\n\n\n");
		
		printf("CAPTAIN:   arrived at loading dock\n");
	

		pthread_mutex_lock(&protect_trucks_queued_counter);
		travel_time++;
		check_truck_number=trucks_queued_counter;
		pthread_mutex_unlock(&protect_trucks_queued_counter);
		pthread_mutex_lock(&protect_cars_queued_counter);
		check_car_number=cars_queued_counter;
		vehicle_number=trucks_queued_counter+cars_queued_counter;
		pthread_mutex_unlock(&protect_cars_queued_counter);
		
	}
	
}


int timeChange( const struct timeval startTime )
{
  	struct timeval nowTime;
	long int elapsed;
	int elapsedTime;
	gettimeofday(&nowTime, NULL);
	elapsed = (nowTime.tv_sec - startTime.tv_sec) * 1000000
	+ (nowTime.tv_usec - startTime.tv_usec);
	elapsedTime = elapsed / 1000;
	return elapsedTime;
}


int check_sem_post(sem_t *sem)
{
	int ret=sem_post(sem);
	if(ret != 0)
	{
		printf("error happened when call sem_post\n");
		exit(0);
	}
	return ret;
}

int check_sem_wait(sem_t *sem)
{
	int ret=sem_wait(sem);
	if(ret != 0)
	{
		printf("error happened when call sem_post\n");
		exit(0);
	}
	
	return ret;
}


int check_pthread_mutex_destroy(pthread_mutex_t *mutex) {
	int ret = pthread_mutex_destroy(mutex);
	if (ret != 0) {
		printf("error happened when destroy the pthread_mutex\n");
		exit(0);
	}
	return ret;
}

int check_sem_destroy(sem_t *sem) {
	int ret = sem_destroy(sem);
	if (ret != 0) {
		printf("error occurs when destroy sem\n");
		exit(0);
	}
	return ret;
}



