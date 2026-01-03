#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int main(){
    /* Opening IPC FIFO to write data into */
    int fd = open("IPC", O_WRONLY);
    if(fd == -1){
        printf("Couldn't open IPC\n");
        return 1;
    } 

    uint16_t wheel_speed = 1000;

    while(1){
        /* Continuous writes to FIFO at 100 Hz(10ms) */
        if(write(fd, &wheel_speed,sizeof(uint16_t)) == -1){
            printf("Write failed!\n");
        }
        printf("Data sent: %d\n",wheel_speed);
        usleep(10000);
    }
}