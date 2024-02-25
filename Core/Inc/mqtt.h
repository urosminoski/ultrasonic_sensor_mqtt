/*
 * mqtt.h
 *
 *  Created on: Feb 13, 2024
 *      Author: Uros
 */

#ifndef INC_MQTT_H_
#define INC_MQTT_H_

#include "main.h"

/* WIFI SSID */
#define WIFI_SSID		( "Uros123456" )
/* WIFI password*/
#define WIFI_PSSWD		( "2.718281" )

/* AT command for disconnecting from WIFI */
#define WIFI_DISCONN	( "AT+CWQAP\r\n" )
/* AT command for connecting to MQTT broker */
#define MQTT_CONFIG		( "AT+MQTTUSERCFG=0,1,\"uros\",\"uros\",\"123\",0,0,\"\"\r\n" )
#define MQTT_CONN		( "AT+MQTTCONN=0,\"broker.hivemq.com\",1883,1\r\n" )
/* AT command for disconnecting from MQTT broker */
#define MQTT_DISCONN	( "AT+MQTTCLEAN=0\r\n" )

/* User defined commands */
#define C_USER_PWM_ON			( "power on" )			/* Command for starting PWM signal */
#define C_USER_PWM_OFF			( "power off" )			/* Command for stopping PWM signal */
#define C_USER_WIFI_CONN		( "wifi conn" )			/* Command for connecting to WIFI */
#define C_USER_WIFI_DISCONN		( "wifi disconn" )		/* Command for disconnecting to WIFI */
#define C_USER_MQTT_CONN		( "mqtt conn" )			/* Command for connecting to MQTT borker */
#define C_USER_MQTT_DISCONN		( "mqtt disconn" )		/* Command for disconnecting to MQTT broker */

/* AT defined commands */
#define CMP_ERROR				( "ERROR" )
#define CMP_WIFI_CONN 			( "WIFI CONNECTED" )
#define CMP_WIFI_DISCONN 		( "WIFI DISCONNECT" )
#define CMP_MQTT_CONN 			( "MQTTCONNECTED" )
#define CMP_MQTT_DISCONN 		( "MQTTDISCONNECTED" )

typedef enum{
	USER_ERROR,
	USER_PWM_ON,
	USER_PWM_OFF,
	USER_WIFI_CONN,
	USER_WIFI_DISCONN,
	USER_MQTT_CONN,
	USER_MQTT_DISCONN
} userCmd_t;

typedef enum{
	AT_INIT,
	AT_ERROR,
	AT_WIFI_CONN,
	AT_WIFI_DISCONN,
	AT_MQTT_CONN,
	AT_MQTT_DISCONN
} atCmd_t;

void process_user_cmd(const char* string, bool* pwmFlag);
void process_at_cmd(const char* string, bool* wifiFlag, bool* mqttiFlag);
void publish_data(const uint32_t* data);

#endif /* INC_MQTT_H_ */
