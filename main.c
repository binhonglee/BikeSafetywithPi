#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

//Preset information about MPU3050 (gyro sensor)
#define GYRO_I2C	(0x69)
#define GYRO_REG	(0x21)

//Preset buzzer WiringPi port
#define	BUZZER	0

//Preset button interrupt WiringPi port
#define BUTTON	5
//Preset button debounce time
#define DEBOUNCE_MS	1000

//Declaring all global variables
volatile uint8_t trigger;
volatile uint8_t photoTaken;
volatile unsigned int prevTime = 0;
volatile uint8_t btnClicked = 0;

void buttonInterrupt()
{
	//Get the current time and its difference from last interrupt
	unsigned int currentTime = millis();
	unsigned int deltaTime = currentTime - prevTime;

	//Check if the deltaTime from the last click is longer than the preset debounce time
	if (deltaTime > DEBOUNCE_MS)
	{
		//Set new values
		btnClicked = 1;
		prevTime = currentTime;

		if (trigger == 0)
		{
			printf("Paused\n");
			//Set trigger and turn off buzzer
			trigger = 1;
			digitalWrite(BUZZER, LOW);
		}
		else
		{
			//Reset photoTaken and trigger
			photoTaken = 0;
			trigger = 0;

			printf("Continue\n");
			//Double short beep to inform rider
			digitalWrite(BUZZER, HIGH);
			delay(100);
			digitalWrite(BUZZER, LOW);
			delay(100);
			digitalWrite(BUZZER, HIGH);
			delay(100);
			digitalWrite(BUZZER, LOW);
		}
	}
}

int main()
{
	//Initialize photoTaken and trigger
	photoTaken = 0;
	trigger = 0;

	wiringPiSetup();
	pinMode(BUTTON, INPUT);
	pinMode(BUZZER, OUTPUT);

	//Setup ISR (button) and I2C (gyro sensor)
	int isrSetup = wiringPiISR(BUTTON, INT_EDGE_FALLING, &buttonInterrupt);
	int i2cSetup = wiringPiI2CSetup(GYRO_I2C);

	//Check if setups are successful
	if (i2cSetup < 0||isrSetup < 0)
	{
		return 0;
	}

	//Declare 'gyroInput' to be used to contain the input from gyro sensor later
	int gyroInput;
	//Initialize 'count' to be used to keep track how many consecutive 0.1 secs had the gyro sensor stayed at 0
	int count = 0;

	//Double short beep to inform rider
	digitalWrite(BUZZER, HIGH);
	delay(100);
	digitalWrite(BUZZER, LOW);
	delay(100);
	digitalWrite(BUZZER, HIGH);
	delay(100);
	digitalWrite(BUZZER, LOW);

	printf("While loop begins~\n");

	while (1)
	{
		//Get gyro sensor input
		gyroInput = wiringPiI2CReadReg8(i2cSetup, GYRO_REG);

		if (gyroInput == 0)
		{
			//Increase count if gyro sensor returns 0
			count++;

			//If the gyro sensor reading stayed at '0' consecutively for over a full second
			if (count > 10 && trigger == 0 && photoTaken == 0)
			{
				//Set off the buzzer
				digitalWrite(BUZZER, HIGH);
				//Allow rider 2 seconds to cancel taking photo and sending email
				delay(2000);

				if (!btnClicked)
				{
					/* If rider did not press the button after 2 seconds, send an email asking
					 * for help from emergency contact attaching a photo taken on the spot
					 */
					system("raspistill -v -o photo.jpg && ./sendMail.sh");
					photoTaken = 1;
				}

				//Reset button clicks
				btnClicked = 0;
			}
		}
		else
		{
			//Reset counter to 0
			count = 0;
		}

		//Only read value from gyro sensor every 0.1 sec
		delay(100);
	}

	return 0;
}
