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
