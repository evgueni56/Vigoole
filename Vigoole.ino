/*************************************
* This code is written by Laurens Wuyts
* For questions: laurens.wuyts@gmail.com
*
*
* Microcontroller: ATtiny85
* Dev board:       Digispark
*
*************************************/
//#include <DigiUSB.h>

uint8_t situation = 0;
uint8_t START = 0;
uint8_t x = 0;
uint8_t BIT = 0;
uint8_t Data = 0;
uint8_t irCode[4];
unsigned long  codeP;
uint8_t Repeat = -1;
uint8_t sended = 0;
int poweron = 0;

uint16_t Time_old = 0;
uint16_t Time = 0;
uint16_t TimeDelta = 0;

void setup(void)
{
	attachInterrupt(0, IR_Read, FALLING);
	pinMode(0, OUTPUT);
	pinMode(1, OUTPUT);
	digitalWrite(0, 1);
	digitalWrite(1, 1);
	poweron = 0;
}

void loop(void)
{
	if (sended == 1)
	{
//		codeP = 0x1000000 * irCode[0] + 0x10000 * irCode[1] + 0x100 * irCode[2] + irCode[3];
//		DigiUSB.println(irCode[2], HEX);
		if (irCode[2] == 0x92)
		{
			detachInterrupt(0); // interrupt messes the loop here
			if (poweron) // the power is on - turn it off
			{
				digitalWrite(0, 1);
				delay(50);
				digitalWrite(1, 1);
				poweron = 0;
			}
			else // was off - turn it on
			{
				digitalWrite(1, 0);
				delay(50);
				digitalWrite(0, 0); //reversed order to avoid spikes. Not implemented!
				poweron = 1;
			}
		}
		sended = 0;
		Repeat = -1; // Ignore redundand repeat after each command
		Time_old = 0;
		attachInterrupt(0, IR_Read, FALLING);
	}
	//else
	//	DigiUSB.delay(10);
	if (Repeat == 1)
	{
		Repeat = 0;
		Time_old = 0;

	}
}

void IR_Read(void)
{
	Time = micros();
	if (Time_old != 0)
	{
		TimeDelta = Time - Time_old;
		if ((TimeDelta > 12000) && (TimeDelta < 16000))
		{
			START = 1;
			x = 0;
			situation = 1;
			irCode[0] = 0;
			irCode[1] = 0;
			Data = 0;
			irCode[3] = 0;
		}

		else if ((TimeDelta > 11000) && (TimeDelta < 11500))
		{
			Repeat++;
		}
		else if ((TimeDelta > 1500) && (TimeDelta < 2500))
		{
			situation = 3; //"1"
			BIT = 1;
		}
		else if ((TimeDelta > 1000) && (TimeDelta < 1500))
		{
			situation = 3; //"0"
			BIT = 0;
		}
		else situation = 5;
		if (situation == 3)
		{
			if (x < 8)
			{
				irCode[0] |= BIT;
				if (x < 7) irCode[0] <<= 1;
				x++;
			}
			else if (x < 16)
			{
				irCode[1] |= BIT;
				if (x < 15) irCode[1] <<= 1;
				x++;
			}
			else if (x < 24)
			{
				Data |= BIT;
				if (x < 23) Data <<= 1;
				x++;
			}
			else if (x < 32)
			{
				irCode[3] |= BIT;
				if (x < 31)
				{
					irCode[3] <<= 1;
				}
				else
				{

					/* DO SOMETHING HERE */
					sended = 1;

					irCode[2] = Data;
					Repeat = -1;
				}
				x++;
			}
		}

	}
	Time_old = Time;
}