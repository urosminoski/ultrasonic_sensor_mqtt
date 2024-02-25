/*
 * processMessages.c
 *
 *  Created on: Feb 13, 2024
 *      Author: Uros
 */

#include "mqtt.h"

static void wifi_conn(void)
{
	char txData[256];
	uint16_t len = sprintf(txData, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PSSWD);
	HAL_UART_Transmit(&huart3, (uint8_t *)txData, len, 300);
}

static void wifi_disconn(void)
{
	char txWifiCmdD[] = WIFI_DISCONN;
	HAL_UART_Transmit(&huart3, (uint8_t *)txWifiCmdD, strlen(txWifiCmdD), 300);
}

static void mqtt_conn(void)
{
	char txMqttCmd1[] = MQTT_CONFIG;
	char txMqttCmd2[] = MQTT_CONN;
	HAL_UART_Transmit(&huart3, (uint8_t *)txMqttCmd1, strlen(txMqttCmd1), 300);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart3, (uint8_t *)txMqttCmd2, strlen(txMqttCmd2), 300);
}

static void mqtt_disconn(void)
{
	char txMqttCmdD[] = MQTT_DISCONN;
	HAL_UART_Transmit(&huart3, (uint8_t *)txMqttCmdD, strlen(txMqttCmdD), 300);
}


static userCmd_t check_userCmd(const char* string)
{
	if(strncmp(string, C_USER_PWM_ON, strlen(C_USER_PWM_ON)) == 0)
	{
		return USER_PWM_ON;
	}
	else if(strncmp(string, C_USER_PWM_OFF, strlen(C_USER_PWM_OFF)) == 0)
	{
		return USER_PWM_OFF;
	}
	else if(strncmp(string, C_USER_WIFI_CONN, strlen(C_USER_WIFI_CONN)) == 0)
	{
		return USER_WIFI_CONN;
	}
	else if(strncmp(string, C_USER_WIFI_DISCONN, strlen(C_USER_WIFI_DISCONN)) == 0)
	{
		return USER_WIFI_DISCONN;
	}
	else if(strncmp(string, C_USER_MQTT_CONN, strlen(C_USER_MQTT_CONN)) == 0)
	{
		return USER_MQTT_CONN;
	}
	else if(strncmp(string, C_USER_MQTT_DISCONN, strlen(C_USER_MQTT_DISCONN)) == 0)
	{
		return USER_MQTT_DISCONN;
	}
	else
	{
		return USER_ERROR;
	}
}

static bool find_string(const char* string1, const char* string2)
{
	char* pCh = strstr(string1, string2);
	if(pCh != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

static atCmd_t check_atCmd(const char* string)
{
	if(find_string(string, CMP_WIFI_CONN))
	{
		return AT_WIFI_CONN;
	}
	else if(find_string(string, CMP_WIFI_DISCONN))
	{
		return AT_WIFI_DISCONN;
	}
	else if(find_string(string, CMP_MQTT_CONN))
	{
		return AT_MQTT_CONN;
	}
	else if(find_string(string, CMP_MQTT_DISCONN))
	{
		return AT_MQTT_DISCONN;
	}
	else if(find_string(string, CMP_ERROR))
	{
		return AT_ERROR;
	}
	else
	{
		return AT_INIT;
	}
}

void process_user_cmd(const char* string, bool* pwmFlag)
{
	switch(check_userCmd(string)){

	case USER_PWM_ON:
		/* Start PWM timer */
		LL_TIM_EnableCounter(TIM2);
		printf("\n\rPWM is powered ON!\r\n");
		*pwmFlag = true;
		break;

	case USER_PWM_OFF:
		/* Stop PWM timer */
		LL_TIM_DisableCounter(TIM2);
		/* Stop CNT timer */
		LL_TIM_DisableCounter(TIM16);
		printf("\n\rPWM is powered OFF!\r\n");
		*pwmFlag = true;
		break;

	case USER_WIFI_CONN:
		wifi_conn();
		break;

	case USER_WIFI_DISCONN:
		wifi_disconn();
		break;

	case USER_MQTT_CONN:
		mqtt_conn();
		break;

	case USER_MQTT_DISCONN:
		mqtt_disconn();
		break;

	case USER_ERROR:
		printf("\n\rInvalid user command!\r\n");
		break;
	}
}

void process_at_cmd(const char* string, bool* wifiFlag, bool* mqttiFlag)
{
	switch(check_atCmd(string)){

	case AT_WIFI_CONN:
		*wifiFlag = true;
		break;

	case AT_WIFI_DISCONN:
		*wifiFlag = false;
		mqtt_disconn();
		break;

	case AT_MQTT_CONN:
		*mqttiFlag = true;
		break;

	case AT_MQTT_DISCONN:
		*mqttiFlag = false;
		break;

	case AT_ERROR:
		printf("\n\rInvalid AT command!\r\n");
		break;

	case AT_INIT:
		break;
	}
}

void publish_data(const uint32_t* data)
{
	char pubData[256];
	uint16_t len = sprintf(pubData, "AT+MQTTPUB=0,\"urosminoski/ultrasonic_sensor\",\"%lu\",1,0\r\n", *data);
	HAL_UART_Transmit_IT(&huart3, (uint8_t *)pubData, len);
}

