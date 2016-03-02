#include <stdio.h>
#include "getch_in_unix.h"
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>

#define GPIO1 7
#define GPIO2 0
#define GPIO3 1

// Thread function argument variables

struct thread_data{
   int speed;
   char direction;
   int power;
};

// Thread function - loop to keep motor running
void* run_motor(void* parameters){
        struct thread_data *my_data;
        my_data = (struct thread_data *) parameters;
        
		// Loop to keep motor running
		while(my_data->power == 1){
			softPwmWrite(GPIO3, my_data->speed);
			delay(100);
		}
		
		// Stop motor (i.e: set speed to zero)
        softPwmWrite(GPIO3, 0);
        pthread_exit(NULL);
}

int main(){
        wiringPiSetup();
        char ch;
        int returnFlag = 0;
		// Set up pins to run motor
        pinMode(GPIO1, OUTPUT);
        pinMode(GPIO2, OUTPUT);
        softPwmCreate(GPIO3, 0, 100);
        digitalWrite(GPIO1, LOW);
        digitalWrite(GPIO2, HIGH);
        
   	 	printf("Press SPACE to activate/deactivate motor...\n");
		
        while((ch = getch()) != 'q' || returnFlag != 1){
                switch(ch){
                        case ' ': // Activate/Deactivate Motor
                                td.power = (td.power == 0) ? 1:0;
                                printf("Motor is now %s\n", (td.power == 0) ? "OFF":"ON");
                                if (td.power == 1){
                                        td.speed = 10;
										td.direction = 'L';
                                        ret = pthread_create(&thread_handle, 0, run_motor, (void *)&td);

                                        if (ret != 0){
                                                printf("Creation of thread failed! Error: %d", ret);
												returnFlag = 1;
                                        }
                                }
                                break;
                        case '\033':
                                if (td.power == 1){
                                        getch();  // skip the [
                                        switch(getch()) { // the real value
                                                case 'A':
                                                    // Increase motor speed
                                                    if(td.speed < 100){
                                                            td.speed += 10;
                                                            printf("Speed increased to: %d \n", td.speed);
                                                    }
                                                    else{
                                                            printf("Maximum speed reached...\n");
                                                    }
                                          
                                                    break;
                                                case 'B':
                                                    // Decrease motor speed
                                                    if(td.speed > 10){
                                                            td.speed -= 10;
                                                            printf("Speed decreased to: %d \n", td.speed);
                                                    }
                                                    else{
                                                            printf("Minimum speed reached...\n");
                                                    }
                                                    break;
                                                case 'C':
                                                    // Direction - right
													if(td.direction != 'R'){
                                                            td.speed = 10;
                                                            digitalWrite(GPIO1, HIGH);
                                                            digitalWrite(GPIO2, LOW);
                                                            td.direction = 'R';
                                                    }
                                                    break;
                                                case 'D':
                                                    // Direction - left
                                                    if(td.direction != 'L'){
                                                            td.speed = 10;
                                                            digitalWrite(GPIO1, LOW);
                                                            digitalWrite(GPIO2, HIGH);
                                                            td.direction = 'L';
                                                    }
                                                    break;
                                         }
                                }
                                else printf("Power must be on to operate the motor...\n");
                                break;
                }
        }
		if (returnFlag != 1){
	        if (td.power == 1){
	            td.power = 0;
	            pthread_join(thread_handle, 0); // Wait for thread to terminate
	        }
	
	        digitalWrite(GPIO1, LOW);
	        digitalWrite(GPIO2, LOW);
	        printf("Program terminated. Motor is now OFF...");
		}
		else printf("Error in thread creation...program is terminating...");
       EndOfProgram: return returnFlag;
}

		