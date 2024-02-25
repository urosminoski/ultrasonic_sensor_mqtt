/*
 * checkString.h
 *
 *  Created on: Feb 10, 2024
 *      Author: Uros
 */

#ifndef INC_CHECKSTRING_H_
#define INC_CHECKSTRING_H_

#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* For checking received messages */
#define WIFI_CONN_ 		( "WIFI CONNECTED" )		// WIFI connected
#define WIFI_DISCONN_	( "WIFI DISCONNECT" )		// WIFI disconnected
#define MQTT_CONN_		( "MQTTCONN" )				// First message to check if MQTT broker is connected
#define MQTT_OK			( "OK" )					// After first message, MQTT broker connected
#define MQTT_ERROR		( "ERROR" )					// After first message, MQTT broker disconnected
#define PWM_ON			( "power on" )				// PWM on
#define PWM_OFF			( "power off" )				// PWM off
#define USER_WRITES		( "user," )					// User writing
#define MQTT_PUBRAW_Y	( "+MQTTPUB:OK" )			// Successful mqttpubraw
#define MQTT_PUBRAW_N	( "+MQTTPUB:FAIL" )			// Unsuccessful mqttpubraw

bool check_pwm_status(const char* buffer, bool* flag);
bool check_wifi_status(const char* buffer, bool* flag);
bool check_mqtt_status(const char* buffer, bool* flag);
//bool check_mqttPub_status(const char* buffer, pub_status_t* flag);

#endif /* INC_CHECKSTRING_H_ */
