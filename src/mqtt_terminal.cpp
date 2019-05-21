#include <iostream>

#include "base64.h"
#include "mqtt_terminal.h"
#include "json11.hpp"

#include <string>

// bridge function callback, calls the method od MQTT_Terminal context
static int MQTT_Terminal_Bridge_Incoming_Message(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	MQTT_Terminal* terminal = static_cast<MQTT_Terminal*>(context);
	char* msgPtr = static_cast<char*>(message->payload);
	std::string inMsg(msgPtr, msgPtr + message->payloadlen);

	bool result = terminal->Incoming_Message(topicName, inMsg);

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);

	return result ? 1 : 0;
}

// bridge function callback, calls the method od MQTT_Terminal context
static void MQTT_Terminal_Bridge_Connection_Lost(void *context, char *cause)
{
	MQTT_Terminal* terminal = static_cast<MQTT_Terminal*>(context);

	terminal->Connection_Lost(cause ? cause : "");
}

// bridge function callback, calls the method od MQTT_Terminal context
void MQTT_Terminal_Bridge_Delivery_Complete(void* context, MQTTClient_deliveryToken dt)
{
	MQTT_Terminal* terminal = static_cast<MQTT_Terminal*>(context);

	terminal->Message_Delivered(dt);
}

MQTT_Terminal::MQTT_Terminal(const MQTT_Settings& settings)
	: mSettings(settings)
{
	//
}

bool MQTT_Terminal::Init()
{
	std::string connStr = "" + mSettings.server + ":" + std::to_string(mSettings.port);

	std::cout << "Connecting to MQTT server " << mSettings.server << ":" << mSettings.port << " ... " << std::endl;

	mConnOpts = MQTTClient_connectOptions_initializer;

	MQTTClient_create(&mClient, connStr.c_str(), "RemoteTerminal", MQTTCLIENT_PERSISTENCE_NONE, NULL);
	mConnOpts.connectTimeout = mSettings.connectionTimeout;
	mConnOpts.keepAliveInterval = mSettings.keepaliveInterval;
	mConnOpts.cleansession = 1;

	mConnOpts.password = mSettings.password.c_str();
	mConnOpts.username = mSettings.username.c_str();
	mConnOpts.MQTTVersion = MQTTVERSION_DEFAULT;

	MQTTClient_setCallbacks(mClient, this, MQTT_Terminal_Bridge_Connection_Lost, MQTT_Terminal_Bridge_Incoming_Message, nullptr);

	return Reconnect();
}

bool MQTT_Terminal::Reconnect()
{
	int rc;

	if ((rc = MQTTClient_connect(mClient, &mConnOpts)) != MQTTCLIENT_SUCCESS)
	{
		std::cerr << "Unable to connect to MQTT server" << std::endl;
		return false;
	}

	std::cout << "Connected!" << std::endl;

	return (MQTTClient_subscribe(mClient, mSettings.rxTopic.c_str(), 0) == MQTTCLIENT_SUCCESS);
}

bool MQTT_Terminal::Send_Command(const std::vector<uint8_t>& parsed_command)
{
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

	std::string encodedMsg;
	Base64::Encode(encodedMsg, parsed_command);

	std::string payload = "{\"reference\": \"" + std::to_string(time(nullptr)) + "\", \"confirmed\": false, \"fPort\": " + std::to_string(mSettings.loraPort) + ", \"data\": \"" + encodedMsg + "\"}";

	pubmsg.payload = (void*)payload.c_str();
	pubmsg.payloadlen = (int)payload.length();
	pubmsg.qos = 0;
	pubmsg.retained = 0;
	MQTTClient_publishMessage(mClient, mSettings.txTopic.c_str(), &pubmsg, &token);

	const unsigned long timeout = mSettings.connectionTimeout;

	int rc = MQTTClient_waitForCompletion(mClient, token, timeout);

	return (rc == MQTTCLIENT_SUCCESS);
}

bool MQTT_Terminal::Incoming_Message(const std::string& topicName, const std::string& message)
{
	std::string err;
	json11::Json parsedMsg = json11::Json::parse(message, err);

	if (!err.empty())
	{
		// TODO: proper message
		std::cerr << "json parse error: " << err << std::endl;
		return false;
	}

	if (parsedMsg["fPort"].int_value() == mSettings.loraPort && parsedMsg["data"].is_string())
	{
		std::vector<uint8_t> out;
		Base64::Decode(out, parsedMsg["data"].string_value());

		std::unique_lock<std::mutex> lck(mQueue_Mtx);

		mIncoming_Queue.push(std::move(out));

		mQueue_Cv.notify_one();
	}

	return true;
}

void MQTT_Terminal::Connection_Lost(const std::string& reason)
{
	// TODO: some message

	Reconnect();
}

void MQTT_Terminal::Message_Delivered(const MQTTClient_deliveryToken& tok)
{
	// not used atm
}
