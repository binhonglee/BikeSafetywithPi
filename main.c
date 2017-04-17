#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <errno.h>

#define MPU3050_PWR	(0x6b)
#define MPU3050_I2C	(0x69)
#define MPU3050_REG	(0x21)
#define	LED	0
#define BUTTON	5
#define DEBOUNCE_MS	1000

volatile int trigger;
volatile uint8_t photoTaken;
volatile unsigned int prevTime = 0;
volatile uint8_t btnClicked = 0;

void myInterrupt(void)
{
	unsigned int currentTime = millis();
	unsigned int deltaTime = currentTime - prevTime;

	printf("%d\n", deltaTime);
	if (deltaTime > DEBOUNCE_MS)
	{
		btnClicked = 1;
		prevTime = currentTime;
		printf("test\n");
		if (trigger == 0)
		{
			photoTaken = 0;
			trigger = 1;
			digitalWrite(LED, LOW);
			printf("T R I G G E R R E D\n");
		}
		else
		{
			photoTaken = 0;
			trigger = 0;
			digitalWrite(LED, HIGH);
			delay(100);
			digitalWrite(LED, LOW);
			delay(100);
			digitalWrite(LED, HIGH);
			delay(100);
			digitalWrite(LED, LOW);
			printf("T R I G G E R R E D  A G A I N\n");
		}
	}
}

int main()
{
	photoTaken = 0;
	trigger = 0;

	wiringPiSetup();
	pinMode(BUTTON, INPUT);
	pinMode(LED, OUTPUT);
	
	int isrSetup = wiringPiISR(BUTTON, INT_EDGE_FALLING, &myInterrupt);
	int i2cSetup = wiringPiI2CSetup(MPU3050_I2C);

	if (i2cSetup < 0||isrSetup < 0)
		return 0;

	wiringPiI2CWriteReg8(i2cSetup, MPU3050_PWR, 0x01);
	int gyroInput;
	int count = 0;
	while(1)
	{
		gyroInput = wiringPiI2CReadReg8(i2cSetup, MPU3050_REG);

		if(gyroInput == 0)
		{
			count++;
			if (count > 10 && trigger == 0 && photoTaken == 0)
			{
				digitalWrite(LED, HIGH);
				delay(2000);

				if (!btnClicked)
				{
					system("raspistill -v -o photo.jpg && ./sendMail.sh");
					photoTaken = 1;
				}

				btnClicked = 0;
			}
		}
		else
		{
			count = 0;
			digitalWrite(LED, LOW);
		}

		delay(100);
	}
	return 0;
}
