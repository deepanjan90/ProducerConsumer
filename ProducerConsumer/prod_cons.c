#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>   // POSIX library
#include<sys/time.h>  // Getting Current Time


#define BUF_SIZE 2	// Shared Buffer Size
#define MAX_PRODUCE_PER_PRODUCER 1000	//Produce count for each producer

#define CONSUMERFILENAME     ((const unsigned char *)"Consumer.txt")
#define REDPRODUCERFILENAME     ((const unsigned char *)"Producer_RED.txt")
#define BLACKPRODUCERFILENAME     ((const unsigned char *)"Producer_BLACK.txt")
#define WHITEPRODUCERFILENAME     ((const unsigned char *)"Producer_WHITE.txt")


char* printStringToFile(char printString[],const char printFileName[]);
void *producer_red( void *ptr );
void *producer_black( void *ptr );
void *producer_white( void *ptr );
void *consumer_main( void *ptr );


//The Buffer Stucture imokementing buffer queue and Semaphore variables
typedef struct {
    char *buf[BUF_SIZE]; // the buffer
    int len; // number of items in the buffer
	int top; // index for inserting
	int old; // index for oldest value
    pthread_mutex_t mutex; // to lock buffer when in use
    pthread_cond_t can_produce; // signaled when items are removed
    pthread_cond_t can_consume; // signaled when items are added
} buffer_t;

buffer_t buffer;

// for inserting values into buffer
void inserIntoBuffer(char data[])
{	
	buffer.buf[buffer.top] = (char *) realloc(buffer.buf[buffer.top], strlen(data) + 1);
	
	strcpy(buffer.buf[buffer.top],data);
	++buffer.len;
	
	if(buffer.top+1==BUF_SIZE) //If the top element pointer is at the last postion of th array
	{
		buffer.top=0;
	}
	else
	{
		++buffer.top;
	}	
}

//For getting values from the buffer and removing that
char* getDataFromBuffe()
{
	char *data = " ";
	
	//If Buffer has Items
	if(buffer.len>0)
	{
		data = buffer.buf[buffer.old];
		--buffer.len;
		
		if(buffer.old+1==BUF_SIZE) //If the old element pointer is at the last postion of th array
		{
			buffer.old=0;
		}
		else					
		{
			++buffer.old;
		}				
	}
	else
	{
		//Empty Buffer
	}
	return data;
}


//Creating the threads for producers and consumers
void createThread()
{
	pthread_t thread_producer_red, thread_producer_black, thread_producer_white, thread_consumer;
	int err1, err2, err3, err4;
	err1 = pthread_create( &thread_producer_red, NULL, &producer_red, NULL);
	err2 = pthread_create( &thread_producer_black, NULL, &producer_black, NULL);
	err3 = pthread_create( &thread_producer_white, NULL, &producer_white, NULL);
	err4 = pthread_create( &thread_consumer, NULL, &consumer_main, NULL);
	
	//Waiting for all the threads to finish before leaving
	pthread_join( thread_producer_red, NULL);
	pthread_join( thread_producer_black, NULL);
	pthread_join( thread_producer_white, NULL);
	pthread_join( thread_consumer, NULL);
}

//Producer funcion, that adds item to buffer and adds a log entry
void producer(char producerData[],const char producerFileName[])
{
	//lock critical region
	pthread_mutex_lock(&buffer.mutex);

	while(buffer.len == BUF_SIZE) // Buffer full so need to wait to produce
	{ 
		pthread_cond_wait(&buffer.can_produce, &buffer.mutex);
	}

	//Inserting produce into buffer
	inserIntoBuffer(producerData);
	
	//adding log entry for the above
	printStringToFile(producerData,producerFileName);
	
	
	// if buffer was empty, then signal the consumer, as might have been sleeping
	if(buffer.len==1)
	{
		pthread_cond_signal(&buffer.can_consume);
	}
	
	//unlock critical region
	pthread_mutex_unlock(&buffer.mutex);
	
}


//Consumer reads data from the buffer and writes in a log file
void consumer(const char consumerFileName[])
{

	char* data = "";
	//lock critical region
	pthread_mutex_lock(&buffer.mutex);

	if(buffer.len == 0)     // Buffer empty so need to wait for produce
	{ 
		pthread_cond_wait(&buffer.can_consume, &buffer.mutex);
	}

	//fetch data from buffer
	data = getDataFromBuffe();
	//log the fetched data
	printStringToFile(data,consumerFileName);

	
	if(buffer.len==BUF_SIZE -1)  // if buffer was full, then broadcast signal to the producers, as might have been sleeping
	{
		pthread_cond_broadcast(&buffer.can_produce);		
	}
	

	//unlock critical region
	pthread_mutex_unlock(&buffer.mutex);
}

//Producer Red
//Randomizes production by generating random number
//and checking if is even or not
//tries to produce when even
void *producer_red( void *ptr )
{
	int maxProduce = MAX_PRODUCE_PER_PRODUCER;
	int currentProduce = 0;
	int randomNumber = 0;
	struct timeval tv;
	char produceData[]="";
	const char producerFileName[] = "Producer_BLACK.txt";
	while(currentProduce < maxProduce)
	{
		randomNumber = rand();
		if(randomNumber%2==0)
		{
			gettimeofday(&tv, NULL);
			sprintf(produceData,"RED_%li.%li",tv.tv_sec,tv.tv_usec);
			producer(produceData,REDPRODUCERFILENAME);
			currentProduce++;

		}
		else
		{
			//miss turn
		}
	}
}

//Producer Black
//Randomizes production by generating random number
//and checking if is even or not
//tries to produce when even
void *producer_black( void *ptr )
{
	int maxProduce = MAX_PRODUCE_PER_PRODUCER;
	int currentProduce = 0;
	int randomNumber = 0;
	struct timeval tv;
	char produceData[]="";
	const char producerFileName[] = "Producer_BLACK.txt";
	while(currentProduce < maxProduce)
	{
		randomNumber = rand();
		if(randomNumber%2==0)
		{
			gettimeofday(&tv, NULL);
			sprintf(produceData,"BLACK_%li.%li",tv.tv_sec,tv.tv_usec);
			producer(produceData,BLACKPRODUCERFILENAME);
			currentProduce++;

		}
		else
		{
			//miss turn
		}
	}
}

//Producer White
//Randomizes production by generating random number
//and checking if is even or not
//tries to produce when even
void *producer_white( void *ptr )
{
	int maxProduce = MAX_PRODUCE_PER_PRODUCER;
	int currentProduce = 0;
	int randomNumber = 0;
	struct timeval tv;
	char produceData[]="";
	const char producerFileName[] = "Producer_BLACK.txt";
	while(currentProduce < maxProduce)
	{
		randomNumber = rand();
		if(randomNumber%2==0)
		{
			gettimeofday(&tv, NULL);
			sprintf(produceData,"WHITE_%li.%li",tv.tv_sec,tv.tv_usec);
			producer(produceData,WHITEPRODUCERFILENAME);
			currentProduce++;

		}
		else
		{
			//miss turn
		}
	}
}

//The consumer method which tracks of the amount of consumption
void *consumer_main( void *ptr )
{
	int maxConsume = MAX_PRODUCE_PER_PRODUCER*3;
	int currentConsume = 0;
	char producerFileName[] = "Consumer.txt";
	
	while(currentConsume < maxConsume)
	{
		consumer(producerFileName);
		currentConsume++;
	}
}

//Function through which logs are printed
char* printStringToFile(char printString[], const char printFileName[])
{
	FILE *fptr;
	char *errorString = " ";
	fptr=fopen(printFileName,"a");
	
	if(fptr==NULL)
	{
		errorString = ("Could not Open File - %s", (char *)printFileName);
	}
	else
	{
		fprintf(fptr,"%s\n",printString);
	}
	fclose(fptr);
	
	return errorString;
}

//To remove all the log files
void cleanUp()
{
	remove(BLACKPRODUCERFILENAME);
	remove(WHITEPRODUCERFILENAME);
	remove(REDPRODUCERFILENAME);
	remove(CONSUMERFILENAME);
}

//Innitialzing the buffer
//Set up queue variables
//set up conditonal and mutex variables
void setUpBuffer()
{
	int i=0;
	
	buffer_t buffer = {
        .len = 0,
		.old = 0,
		.top = 0
    };
	
	pthread_mutex_init(&buffer.mutex, NULL);	
	pthread_cond_init(&buffer.can_produce, NULL);		
	pthread_cond_init(&buffer.can_consume, NULL);


	for(i=0;i<BUF_SIZE;i++)
	{
		buffer.buf[i]=malloc(1);
	}
	

}

//Initialize function to set up the structure
void innitialize()
{	
	srand ( time(NULL) );
	
	cleanUp();
			
	setUpBuffer();
		
	createThread();
}

//main
void main()
{
	innitialize();
	
	exit(0);
}


