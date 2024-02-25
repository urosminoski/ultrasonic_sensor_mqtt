/*
 * checkString.c
 *
 *  Created on: Feb 10, 2024
 *      Author: Uros
 */

#include "checkString.h"

userCmd_t check_userCmd(const char* buffer)
{
	if(strncmp(buffer, C_USER_PWM_ON, strlen(C_USER_PWM_ON)) == 0)
	{
		return USER_PWM_ON;
	}
	else if(strncmp(buffer, C_USER_PWM_OFF, strlen(C_USER_PWM_OFF)) == 0)
	{
		return USER_PWM_OFF;
	}
	else if(strncmp(buffer, C_USER_WIFI_CONN, strlen(C_USER_WIFI_CONN)) == 0)
	{
		return USER_WIFI_CONN;
	}
	else if(strncmp(buffer, C_USER_WIFI_DISCONN, strlen(C_USER_WIFI_DISCONN)) == 0)
	{
		return USER_WIFI_DISCONN;
	}
	else if(strncmp(buffer, C_USER_MQTT_CONN, strlen(C_USER_MQTT_CONN)) == 0)
	{
		return USER_MQTT_CONN;
	}
	else if(strncmp(buffer, C_USER_MQTT_DISCONN, strlen(C_USER_MQTT_DISCONN)) == 0)
	{
		return USER_MQTT_DISCONN;
	}
	else
	{
		return USER_ERROR;
	}
}

/* Checks if received message confirms PWM on
 * Returns true if received message had something to do with PWM */
bool check_pwm_status(const char* buffer, bool* flag)
{
	char* status = NULL;

	status = strstr(buffer, PWM_ON);
	if(status != NULL)
	{
		*flag = true;
		/* Start PWM timer */
		LL_TIM_EnableCounter(TIM2);
		printf("\n\rPWM is powered ON!\r\n");
		return true;
	}

	status = strstr(buffer, PWM_OFF);
	if(status != NULL)
	{
		*flag = false;
		/* Stop PWM timer */
		LL_TIM_DisableCounter(TIM2);
		/* Stop CNT timer */
		LL_TIM_DisableCounter(TIM16);
		printf("\n\rPWM is powered OFF!\r\n");
		return true;
	}
	return false;
}

/* Checks if received message confirms good WIFI connection
 * Returns true if received message had something to do with WIFI connection */
bool check_wifi_status(const char* buffer, bool* flag)
{
	char* status = NULL;

	status = strstr(buffer, WIFI_CONN_);
	if(status != NULL)
	{
		*flag = true;
		return true;
	}

	status = strstr(buffer, WIFI_DISCONN_);
	if(status != NULL)
	{
		*flag = false;
		return true;
	}
	return false;
}

/* Checks if received message confirms good MQTT connection
* Returns true if received message had something to do with MQTT connection */
bool check_mqtt_status(const char* buffer, bool* flag)
{
	char* status = NULL;

	status = strstr(buffer, MQTT_CONN_);
	if(status != NULL)
	{
		status = strstr(buffer, MQTT_OK);
		if(status != NULL)
		{
			*flag = true;
			return true;
		}

		status = strstr(buffer, MQTT_ERROR);
		if(status != NULL)
		{
			*flag = false;
			return true;
		}
	}
	return false;
}
