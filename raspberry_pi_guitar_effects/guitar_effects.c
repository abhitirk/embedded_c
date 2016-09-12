
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wiringPi.h> // Include WiringPi library!
#include <mcp3004.h>
#include "time_elapsed.c"


#define MAX_CMN_LEN 100
#define BASE 100
#define SPI_CHAN 0

const int but1Pin = 29; 
const int but2Pin = 28;
const int but3Pin = 27;
const int but4Pin = 26;
char cmd[MAX_CMN_LEN] = "";

void init_pd_settings(){

	sprintf(cmd, "echo \'0 1;\' | pdsend 3000");
	system(cmd);
	sprintf(cmd, "echo \'1 1;\' | pdsend 3000");
	system(cmd);

	sprintf(cmd, "echo \'depth 2.4;\' | pdsend 6000");
	system(cmd);

	sprintf(cmd, "echo \'feedback -0.30;\' | pdsend 6000");
	system(cmd);

	sprintf(cmd, "echo \'lvl 10.65;\' | pdsend 5000");
	system(cmd);

	return;
}

void init_pi(){	
	mcp3004Setup(BASE, SPI_CHAN);

	wiringPiSetup(); 

	pinMode(but1Pin, INPUT);      // Set button as INPUT
	pullUpDnControl(but1Pin, PUD_UP); // Enable pull-up resistor on button

	pinMode(but2Pin, INPUT);      // Set button as INPUT
	pullUpDnControl(but2Pin, PUD_UP); // Enable pull-up resistor on button
    
	pinMode(but3Pin, INPUT);      // Set button as INPUT
	pullUpDnControl(but3Pin, PUD_UP); // Enable pull-up resistor on button
	
	pinMode(but4Pin, INPUT);      // Set button as INPUT
	pullUpDnControl(but4Pin, PUD_UP); // Enable pull-up resistor on button

	return;
}
int main (int argc)
{
	init_pd_settings();
	char time_string[10] = "";
	float time_dbl;
	int time_ms;
	
	int exit_pgm = 0;

   	int pot1, pot2, pot3, temp, temp2, temp3;
        pot1 = 0;
	pot2 = 0;
	pot3 = 0;
	int newpot1, newpot2, newpot3;
	int patch_lvl_1 = 0, patch_lvl_2 = 0, patch_lvl_3 = 0;
	if(argc!=2){
		printf("usage : msp3008 [single_reading 1/0]\n");
		exit_pgm = 1;
	}
	
	if(!exit_pgm)
	{
		init_pi();

	    	int previousReading = LOW;
 		int reading;
    		int button1mode = 0;
    		int button2state = 0;
    		int button2_mode = 0;
    		int button3state = 1;
    		int button4state = 0;

    		struct timeval tvBegin, tvEnd, tvDiff;
   		 // Loop (while(1)):
    		while(1)
    		{
			printf("");

			// Read and check if distortion button is pressed
			reading = digitalRead(but1Pin);
			if(reading == LOW){
				printf("pushhhhed...\n");
				sprintf(cmd, "echo \'1 %d;\' | pdsend 3000", button1mode);
				system(cmd);
				button1mode = (button1mode == 1) ? 0: 1;
				while(reading == LOW){
					printf("");
					reading = digitalRead(but1Pin);
				}
			}

			// Read and check if effects button is pressed
			reading = digitalRead(but2Pin);
			if(reading == LOW){
				if(button2_mode<2){
					button2_mode++;
				}
				else button2_mode = 0;

				printf("pushhhhed 2...\n");
			
				if(button2_mode == 0){
					sprintf(cmd, "echo \'4 0;\' | pdsend 3000");
				}
				else if(button2_mode == 1){
					sprintf(cmd, "echo \'3 1;\' | pdsend 3000");
				}
				else{
					system("echo \'3 0;\' | pdsend 3000");
					sprintf(cmd, "echo \'4 1;\' | pdsend 3000");
				}

				system(cmd);
				while(reading == LOW){
					printf("");
					reading = digitalRead(but2Pin);
				}
			}

			// Read and check if delay button is pressed
			reading = digitalRead(but3Pin);
			if(reading == LOW){
				printf("pushhhhed 3...\n");
				sprintf(cmd, "echo \'5 %d;\' | pdsend 3000", button3state);
				system(cmd);
				button3state = (button3state == 1) ? 0: 1;
				while(reading == LOW){
					printf("");
					reading = digitalRead(but3Pin);
				}
			}

			// Read and check if delay tap button is pressed
			reading = digitalRead(but4Pin);
			if(reading == LOW){
				printf("pushhhhed 4...\n");
				if(button4state==0){
				    gettimeofday(&tvBegin, NULL);
				    button4state = 1;
				}
				else{
					gettimeofday(&tvEnd, NULL);
					timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
					sprintf(time_string, "%ld.%06ld", tvDiff.tv_sec, tvDiff.tv_usec);
					time_dbl = atof(time_string);
					time_ms = (int)(time_dbl * 1000);

					if(time_ms > 2000) time_ms = 2000;
					printf("%s\n",time_string);
					printf("%d\n",time_ms);	
					sprintf(cmd, "echo \'delay %d;\' | pdsend 4000", time_ms);
					system(cmd);	
					button4state = 0;	
				}

				while(reading == LOW){
					printf("");
					reading = digitalRead(but4Pin);
				}
			}
			// Reading potentiometers

			newpot1 = analogRead(BASE+0);
			newpot2 = analogRead(BASE+1);
			newpot3 = analogRead(BASE+2);

			// Distortion potentiometer value
			if(newpot1 > pot1+10 || newpot1 < pot1-10){
					pot1 = newpot1;
					printf("Pot1 = %d \n", pot1);
					temp = pot1/10 + 20;
					if (patch_lvl_1 != temp){
						patch_lvl_1 = temp;
						sprintf(cmd, "echo \'2 %d;\' | pdsend 3000", patch_lvl_1);
						system(cmd);
					}		
			}

			// Delay potentiometer value
			if(newpot2 > pot2+10 || newpot2 < pot2-10){
					pot2 = newpot2;
					printf("Pot2 = %d \n", pot2);
					temp2 = (pot2/10) + 20;
					if (temp2 > 127) temp2 = 127;
					if (patch_lvl_2 != temp2){
						patch_lvl_2 = temp2;
						sprintf(cmd, "echo \'feedback %d;\' | pdsend 4000", patch_lvl_2);
						system(cmd);
					}
				}

			// Effects potentiometer value
			if(newpot3 > pot3+10 || newpot3 < pot3-10){
					pot3 = newpot3;
					printf("Pot3 = %d \n", pot3);
					if(button2_mode == 1){
						temp3 = (pot3/10);
						if (temp3 > 127) temp3 = 127;
						if (patch_lvl_3 != temp3){
							patch_lvl_3 = temp3;
							sprintf(cmd, "echo \'wah_speed %d;\' | pdsend 5000", patch_lvl_3);
							system(cmd);
						}
					}
					else if(button2_mode == 2){
						temp3 = (pot3/100);
						if (temp3 > 127) temp3 = 127;
						if (patch_lvl_3 != temp3){
							patch_lvl_3 = temp3;
							sprintf(cmd, "echo \'level %d;\' | pdsend 6000", patch_lvl_3);
							system(cmd);
						}
					}
			}     
    		}
   	}	
    return 0;
}