/**
 * @file    mqtt_terminal.h
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-05-27
 * @brief   This file contains MQTT terminal variant
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 University of West Bohemia in Pilsen
 * All rights reserved.</center></h2>
 *
 * Developed by:
 * The SmartCampus Team
 * Department of Technologies and Measurement
 * www.smartcampus.cz | www.zcu.cz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"),
 * to deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimers.
 *
 *    - Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimers in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    - Neither the names of The SmartCampus Team, Department of Technologies and Measurement
 *      and Faculty of Electrical Engineering University of West Bohemia in Pilsen,
 *      nor the names of its contributors may be used to endorse or promote products
 *      derived from this Software without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
 */

#pragma once

#include "terminal.h"

// PAHO client is written in pure C, to avoid linkage errors, let's wrap it in extern "C" block
extern "C"
{
	#include "MQTTClient.h"
}

/*
 * Container of MQTT settings
 */
struct MQTT_Settings
{
	std::string server;					// server hostname/IP
	uint16_t port;						// server MQTT port

	std::string clientIdentifier;		// client application identifier

	std::string username;				// login username
	std::string password;				// login password
	
	uint16_t loraPort;					// which LoRa port to use for remote terminal
	std::string txTopic;				// TX topic (server to node)
	std::string rxTopic;				// RX topic (node to server)

	long connectionTimeout;				// seconds to give up connecting
	long keepaliveInterval;				// interval for MQTT keepalive
	long responseTimeout;				// how many seconds to wait for response from node
	long maxBatchCommands;				// maximum number of commands in batch
};

/*
 * Implementation of MQTT terminal
 */
class MQTT_Terminal : public Terminal_Base
{
	private:
		// settings used for this terminal instance
		MQTT_Settings mSettings;
		// MQTT connection options
		MQTTClient_connectOptions mConnOpts;
		// PAHO MQTT client instance
		MQTTClient mClient;

	protected:
		// (re)connects to the server; returns true on success
		bool Reconnect();

	public:
		MQTT_Terminal(const MQTT_Settings& settings);
		virtual ~MQTT_Terminal() = default;

		// PAHO-called method upon receiving a new message
		bool Incoming_Message(const std::string& topicName, const std::string& message);
		// PAHO-called method when the connection is lost
		void Connection_Lost(const std::string& reason);
		// PAHO-called method upon delivering message with given token
		void Message_Delivered(const MQTTClient_deliveryToken& tok);

		/* Terminal_Base iface */

		virtual bool Init() override;
		virtual bool Send_Command(const std::vector<uint8_t>& parsed_command) override;
};
