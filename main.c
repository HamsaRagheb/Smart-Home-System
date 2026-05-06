/*
 * main.c
 *
 *  Created on: Sep 19, 2023
 *      Author: hamsa
 */

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "MATH_HELPER.h"

#include "DIO_interface.h"
#include "PORT_interface.h"
#include "USART_interface.h"
#include "ADC_interface.h"
#include "TIMER_interface.h"
#include "EXTI_interface.h"
#include "GIE_interface.h"

#include "KPD_interface.h"
#include "LCD_interface.h"
#include "SERVO_interface.h"
#include "LDR_interface.h"
#include "ULTSONIC_interface.h"

#include "APP_interface.h"
#include "APP_config.h"

#include <util/delay.h>

/*************************************************************************************************************************/
static u16 ReceivedData;
static u8 Main_u8MotorSpeed;
static u8 Local_u8Counter;

static u8 Main_u8LDRFlag    = CLOSED;
static u8 Main_u8ControlFan = CLOSED;
static u8 Main_u8AlarmFlag  = CLOSED;
static u8 Main_u8FanFlag    = CLOSED;
static u8 Main_u8FanState   = CLOSED;

/*************************************************************************************************************************/

void NotificationTimer0Func();
void APP_voidMainMenu();
void INT0Emergency();

/*************************************************************************************************************************/

void main (void)
{
	APP_voidInit();
	u8 Local_u8SystemKey;
	do
	{
		Local_u8SystemKey = KPD_u8GetPressedKey();

	} while (Local_u8SystemKey == 0xff);

	if(Local_u8SystemKey=='A')
	{
		LCD_voidSendString2("**** HELLO ****");
		LCD_voidPositionSet(1,0);
		LCD_voidSendString2("Let's Start ^_^");
		_delay_ms(2000);

		APP_voidLoginSystem();
		/*************************************************************************************************************************/
		/*timer2 for deleay*/
		Timer_t Timer2={TIMER_CTC,PRSCLR_DIV_BY_8,OCM_DISCONNECT,INT_DISABLE,INT_DISABLE};
		TIMER2_u8Init(&Timer2);
		/*if true ID&PASS Open the door for 3sec only*/
		APP_voidDoorOpen();
		TIMER0_delay_ms(&Timer2,2000);
		APP_voidDoorClose();

		/*************************************************************************************************************************/

		u8 Local_u8LEDsMappingNumber,Local_u8ONLEDsNumber;
		u16 Local_u16Distance,Local_u16LDRReading;

		ULTSONIC_t Local_stUltsonic;
		Local_stUltsonic.TrigPort = BUZZER_PORT;
		Local_stUltsonic.TrigPin = DIO_u8PIN7;

		/*PWM OF FAN*/
		Timer_t Timer0=
		{
				TIMER_CTC,
				PRSCLR_DIV_BY_8,
				OCM_DISCONNECT,
				INT_ENABLE,
				INT_DISABLE
		};
		TIMER_u8SetCompMatch(TIMER0,200);
		TIMER_u8SetCallBack(INT_TIMER0_COMP,&NotificationTimer0Func);

		EXTI_voidINT0Init();
		EXTI_u8Int0SetCallBack(&INT0Emergency);

		GIE_voidEnable();

		while(1)
		{

			USART_u8ReceiveCharAsynch(&ReceivedData,&APP_voidMainMenu);
			Local_u8SystemKey = KPD_u8GetPressedKey();

			/*************************************************************************************************************************/
			if(Local_u8SystemKey == 'B')
			{
				LCD_voidClearDisplay();
				while(1)
				{
					LCD_voidSendString2("**** Thanks ****");
					LCD_voidPositionSet(1,0);
					LCD_voidSendString2("See You Soon ^_^");
				}
			}
			if(Main_u8LDRFlag==OPENED)
			{
				LCD_voidClearDisplay();
				LDR_u8GetDigitalReading(LDR_u8ADC_CHANNEL4,&Local_u16LDRReading);
				Local_u8LEDsMappingNumber=MATH_s32Map(LDR_MIN_READING,LDR_MAX_READING,LDR_MIN_LEDsNumber,LDR_MAX_LEDsNumber,Local_u16LDRReading);
				Local_u8ONLEDsNumber=LDR_MAX_LEDsNumber-Local_u8LEDsMappingNumber;
				DIO_u8SetPortValue(LDR_LEDs_PORT,DIO_u8PORT_LOW);
				for(Local_u8Counter=0; Local_u8Counter<Local_u8ONLEDsNumber; Local_u8Counter++)
				{
					DIO_u8SetPinValue(LDR_LEDs_PORT,Local_u8Counter,DIO_u8PIN_HIGH);
				}
				/*Display the Digital value of ADC reading on LCD*/
				LCD_voidSendString2("ADC Reading:");
				LCD_voidPositionSet(0,12);
				LCD_voidSendNumber(Local_u16LDRReading);
				LCD_voidPositionSet(1,0);
				LCD_voidSendString2("ON LEDs:");
				LCD_voidPositionSet(1,12);
				LCD_voidSendNumber(Local_u8ONLEDsNumber);
				_delay_ms(500);
			}
			/*************************************************************************************************************************/
			if(Main_u8ControlFan==OPENED)
			{
				TIMER0_u8Init(&Timer0);
				ULTSONIC_u8MeasureDistance(&Local_stUltsonic, &Local_u16Distance);
				Main_u8MotorSpeed=MATH_s32Map(0,160,3,100,Local_u16Distance);

				LCD_voidClearDisplay();
				LCD_voidSendString2("Distance: ");
				LCD_voidSendNumber(Local_u16Distance);
				LCD_voidSendString2("cm");
				LCD_voidPositionSet(1,0);
				LCD_voidSendString2("Motor Speed: ");
				LCD_voidSendNumber(Main_u8MotorSpeed);
				LCD_voidSendData('%');
				_delay_ms(300);
			}
			/*************************************************************************************************************************/
			if(Main_u8FanFlag==OPENED && Main_u8FanState==ON)
			{
				DIO_u8SetPinValue(FAN_PORT,FAN_PIN,DIO_u8PIN_HIGH);
			}
			/*************************************************************************************************************************/

			if(Main_u8FanFlag==OPENED  && Main_u8FanState==OFF)
			{
				DIO_u8SetPinValue(FAN_PORT,FAN_PIN,DIO_u8PIN_LOW);
			}

		}
	}
	else
	{

	}
}

void APP_voidMainMenu()
{
	switch(ReceivedData)
	{
	case TURN_ON_LIGHT:
		LCD_voidClearDisplay();
		Main_u8LDRFlag=CLOSED;
		APP_voidLight_ON();
		break;

	case TURN_OFF_LIGHT:
		LCD_voidClearDisplay();
		Main_u8LDRFlag=CLOSED;
		APP_voidLight_OFF();
		break;

	case CONTROL_LIGHT_LEDs:
		LCD_voidClearDisplay();
		Main_u8ControlFan=CLOSED;
		Main_u8LDRFlag=OPENED;
		break;

	case OPEN_THE_DOOR :
		LCD_voidClearDisplay();
		APP_voidDoorOpen();
		break;

	case CLOSE_THE_DOOR :
		LCD_voidClearDisplay();
		APP_voidDoorClose();
		break;

	case OPEN_THE_FAN :
		LCD_voidClearDisplay();
		Main_u8LDRFlag=CLOSED;
		Main_u8ControlFan=CLOSED;
		Main_u8FanFlag=OPENED;
		LCD_voidSendString2("___Fan is ON___  ");
		LCD_voidPositionSet(1,0);
		LCD_voidSendString2("Temperature: 35 ");
		DIO_u8SetPinValue(FAN_PORT,FAN_PIN,DIO_u8PIN_HIGH);
		break;

	case CLOSE_THE_FAN :
		LCD_voidClearDisplay();
		Main_u8ControlFan=CLOSED;
		Main_u8LDRFlag=CLOSED;
		Main_u8FanFlag=OPENED;
		LCD_voidSendString2("___Fan is OFF___");
		DIO_u8SetPinValue(FAN_PORT,FAN_PIN,DIO_u8PIN_LOW);
		break;

	case CONTROL_THE_FAN_SPEED :
		LCD_voidClearDisplay();
		Main_u8LDRFlag=CLOSED;
		LCD_voidClearDisplay();
		Main_u8ControlFan=OPENED;
		break;

	case ALARM :
		LCD_voidClearDisplay();
		Main_u8LDRFlag=CLOSED;
		Main_u8ControlFan=CLOSED;
		Main_u8AlarmFlag=OPENED;
		LCD_voidSendString2("ALARM !!!!");
		if(Main_u8AlarmFlag==OPENED)
		{
			for(Local_u8Counter = 0; Local_u8Counter < 25; Local_u8Counter++)
			{
				DIO_u8SetPinValue(BUZZER_PORT,  BUZZER_PIN, DIO_u8PIN_HIGH);
				_delay_ms(100 / 2);
				DIO_u8SetPinValue(BUZZER_PORT,  BUZZER_PIN, DIO_u8PIN_LOW);
				_delay_ms(100 / 2);
			}

			for(Local_u8Counter = 0; Local_u8Counter < 25; Local_u8Counter++)
			{
				DIO_u8SetPinValue(BUZZER_PORT,  BUZZER_PIN, DIO_u8PIN_HIGH);
				_delay_ms(2 * 68);
				DIO_u8SetPinValue(BUZZER_PORT,  BUZZER_PIN, DIO_u8PIN_LOW);
				_delay_ms(2 * 68);
			}

			for(Local_u8Counter = 0; Local_u8Counter < 25; Local_u8Counter++)
			{
				DIO_u8SetPinValue(BUZZER_PORT, BUZZER_PIN, DIO_u8PIN_HIGH);
				_delay_ms(68);
				DIO_u8SetPinValue(BUZZER_PORT,  BUZZER_PIN, DIO_u8PIN_LOW);
				_delay_ms(100);
			}
		}
		Main_u8AlarmFlag=CLOSED;
		break;

	default:
		break;
	}

}

/*Fan PWM*/
void NotificationTimer0Func()
{

	static u8 Local_u8Counter=0;
	Local_u8Counter++;

	if(Local_u8Counter==Main_u8MotorSpeed && Main_u8ControlFan==OPENED)
	{
		DIO_u8SetPinValue(FAN_PORT,FAN_PIN,DIO_u8PIN_LOW);

	}
	else if(Local_u8Counter==100 && Main_u8ControlFan==OPENED)
	{
		DIO_u8SetPinValue(FAN_PORT,FAN_PIN,DIO_u8PIN_HIGH);
		Local_u8Counter=0;

	}


}



void INT0Emergency()
{
	APP_voidLight_OFF();
	LCD_voidClearDisplay();
	LCD_voidSendString2("___EMERGENCY___");
	GIE_voidDisable();
	while(1)
	{
		DIO_u8SetPinValue(RED_LIGHT_PORT,RED_LIGHT_PIN,DIO_u8PIN_HIGH);
		DIO_u8SetPinValue(BUZZER_PORT,BUZZER_PIN,DIO_u8PIN_HIGH);
		_delay_ms(1000);
		DIO_u8SetPinValue(RED_LIGHT_PORT,RED_LIGHT_PIN,DIO_u8PIN_LOW);
		DIO_u8SetPinValue(BUZZER_PORT,BUZZER_PIN,DIO_u8PIN_LOW);
		_delay_ms(1000);

	}

}




