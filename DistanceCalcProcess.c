#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <pthread.h>

#define SAMPLES_PER_CHUNK 100           // 100 samples are generated per second
#define CHUNK_QUEUE_SIZE 20           // For 10 seconds upload delay, there would be 10 chunks of data in the queue. Adding headroom for any other delays.

/* Creating a simple circular buffer for data transfer between main thread and upload thread */
typedef struct{
    uint16_t samples[SAMPLES_PER_CHUNK];
    int count;
}data_chunk_t;

data_chunk_t chunk_queue[CHUNK_QUEUE_SIZE];

int q_head = 0;
int q_tail = 0;
int q_count = 0;

void push_chunk(data_chunk_t* chunk){
    if(q_count == CHUNK_QUEUE_SIZE){
        printf("Queue is full!");
    }
    chunk_queue[q_tail] = *chunk;
    q_tail = (q_tail + 1) % CHUNK_QUEUE_SIZE;
    q_count++;
}

void pop_chunk(data_chunk_t* out){
    *out = chunk_queue[q_head];
    q_head = (q_head + 1) % CHUNK_QUEUE_SIZE;
    q_count--;
}

/* Upload thread */
void* upload_func(){
    data_chunk_t chunk;
    while(1){
        pop_chunk(&chunk);
        printf(chunk.samples[chunk.count++]);
    }
}

int main(){
    uint16_t wheel_speed;
    pthread_t upload_thread;
    data_chunk_t currentChunk;
    currentChunk.count = 0;

    pthread_create(&upload_thread,NULL,upload_func,NULL);

    /* Opening the IPC FIFO to read data from */
    int fd = open("IPC", O_RDONLY);
    if(fd == -1){
        printf("Couldn't open IPC\n");
        return 1;
    }

    while(1){
        /* Continuously reads the data sent by the sensor process */
        if(read(fd, &wheel_speed,sizeof(uint16_t)) == -1){
            printf("Read failed!\n");
        }
        printf("Data received: %d\n",wheel_speed);
        currentChunk.samples[currentChunk.count++] = wheel_speed;

        if(currentChunk.count == SAMPLES_PER_CHUNK){
            push_chunk(&currentChunk);
            currentChunk.count = 0;
        }
        
    }
}