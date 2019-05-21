#include <iostream>
#include <vector>
#include <fstream>

#include "mqtt_terminal.h"
#include "terminal_handler.h"

#include "../dep/simpleini/SimpleIni.h"

// global MQTT setting container
static MQTT_Settings mqttSettings;

/*
 * CLI parameters simple parser
 */
class CLIParams
{
	private:
		// parsed tokens
		std::vector<std::string> tokens;

	public:
		// constructor passes arguments to internal representation
		CLIParams(const int argc, const char* const* argv)
		{
			for (int i = 1; i < argc; ++i)
				tokens.push_back(std::string(argv[i]));
		}

		// retrieves CLI option, returns default value if not found
		const std::string& getOpt(const std::string &option, const std::string& defaultValue) const
		{
			std::vector<std::string>::const_iterator itr;
			itr = std::find(tokens.begin(), tokens.end(), option);

			if (itr != tokens.end() && ++itr != tokens.end())
				return *itr;

			return defaultValue;
		}

		// determines the presence of CLI option
		bool hasOpt(const std::string &option) const
		{
			return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
		}
};

// loads config file from given path
bool Load_Config(const std::string& path)
{
	CSimpleIni cfg;
	if (cfg.LoadFile(path.c_str()) != SI_OK)
		return false;

	mqttSettings.server = cfg.GetValue("mqtt", "server", nullptr);
	mqttSettings.port = static_cast<uint16_t>(cfg.GetLongValue("mqtt", "port", 1883));
	mqttSettings.username = cfg.GetValue("mqtt", "username", nullptr);
	mqttSettings.password = cfg.GetValue("mqtt", "password", nullptr);
	mqttSettings.rxTopic = cfg.GetValue("mqtt", "rx-topic", nullptr);
	mqttSettings.txTopic = cfg.GetValue("mqtt", "tx-topic", nullptr);
	mqttSettings.clientIdentifier = cfg.GetValue("mqtt", "client-identifier", "RemoteTerminal");
	mqttSettings.connectionTimeout = cfg.GetLongValue("mqtt", "connection-timeout", 30);
	mqttSettings.keepaliveInterval = cfg.GetLongValue("mqtt", "keepalive-interval", 20);

	mqttSettings.loraPort = static_cast<uint16_t>(cfg.GetLongValue("lora", "port", 13));

	mqttSettings.responseTimeout = cfg.GetLongValue("terminal", "response-timeout", 60);
	mqttSettings.maxBatchCommands = cfg.GetLongValue("terminal", "max-batch-commands", 3);

	return true;
}

int main(int argc, char** argv)
{
	CLIParams params(argc, argv);

	std::string configLoc = params.getOpt("--config", params.getOpt("-c", "config.ini"));

	if (!Load_Config(configLoc))
	{
		std::cerr << "Could not load config file" << std::endl;
		return 1;
	}

	std::string inputFile = params.getOpt("--input", params.getOpt("-i", ""));
	std::string outputFile = params.getOpt("--output", params.getOpt("-o", ""));

	MQTT_Terminal term(mqttSettings);

	if (!term.Init())
		return 2;

	std::ifstream inFs;
	if (!inputFile.empty())
	{
		inFs.open(inputFile);
		if (!inFs.is_open())
		{
			std::cerr << "Could not open input file: " << inputFile << std::endl;
			return 3;
		}
	}

	std::ofstream outFs;
	if (!outputFile.empty())
	{
		outFs.open(outputFile);
		if (!outFs.is_open())
		{
			std::cerr << "Could not open output file: " << outputFile << std::endl;
			return 3;
		}
	}

	// init handler
	Terminal_Handler handler(
		inFs.is_open() ? inFs : std::cin,
		outFs.is_open() ? outFs : std::cout,
		mqttSettings.responseTimeout,
		mqttSettings.maxBatchCommands
	);

	return handler.Run(term);
}
