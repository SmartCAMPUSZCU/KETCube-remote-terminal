#include <iostream>
#include <vector>
#include "mqtt_terminal.h"

#include "terminal_handler.h"

Terminal_Handler::Terminal_Handler(std::istream& input, std::ostream& output, long responseTimeoutSecs, long maxBatchCommands)
	: mInput(input), mOutput(output), mResponseTimeout(responseTimeoutSecs * 1000), mMaxBatchCommands(static_cast<size_t>(maxBatchCommands))
{
	//
}

void Terminal_Handler::Process_Single(Terminal_Base& terminal, std::vector<uint8_t>& encoded, std::string& inStr, uint8_t seq)
{
	bool result, responseOK, seqOK;
	std::vector<uint8_t> response;
	std::string respStr;

	result = terminal.Send_Command(encoded);
	if (!result)
	{
		mOutput << "Send_Command: failed to send command: " << inStr << std::endl;
		return;
	}

	// when sending "reload", we actually have no chance to send back response
	if (inStr == "reload")
	{
		mOutput << "(node will be reloaded on next period timer tick; no response expected)" << std::endl;
		return;
	}

	do
	{
		response.clear();

		result = terminal.Await_Message(response, mResponseTimeout);
		if (!result)
		{
			mOutput << "Await_Message: no response received" << std::endl;
			break;
		}

		result = terminal.Decode_Single_Response(response, responseOK, respStr, seq, seqOK);
	} while (!seqOK);

	if (!result)
	{
		mOutput << "Decode_Response: failed to decode incoming byte buffer" << std::endl;
		return;
	}

	std::cout << respStr << std::endl;
}

void Terminal_Handler::Process_Batch(Terminal_Base& terminal, std::vector<uint8_t>& encoded, uint8_t seq)
{
	bool result, responseOK, seqOK;
	std::vector<uint8_t> response;
	std::string respStr;

	result = terminal.Send_Command(encoded);
	if (!result)
	{
		mOutput << "Send_Command: failed to send command batch" << std::endl;
		return;
	}

	do
	{
		response.clear();

		result = terminal.Await_Message(response, mResponseTimeout);
		if (!result)
		{
			mOutput << "Await_Message: no response received" << std::endl;
			return;
		}

		result = terminal.Decode_Batch_Response(response, responseOK, respStr, seq, seqOK);
	} while (!seqOK);

	if (!result)
	{
		mOutput << "Decode_Response: failed to decode incoming byte buffer" << std::endl;
		return;
	}

	std::cout << respStr << std::endl;
}

int Terminal_Handler::Run(Terminal_Base& terminal)
{
	std::vector<uint8_t> vec, batchVec, response;
	bool result, batchMode;
	std::string inStr, respStr;
	size_t batchCtr;

	size_t batchHdrStart;

	batchMode = false;
	uint8_t seq = 0;

	batchCtr = 0;

	auto assignSeq = [&seq]() { return ++seq; };

	while (mInput.good() && mOutput.good())
	{
		mOutput << ">> ";

		if (std::getline(mInput, inStr))
		{
			if (inStr.length() == 0)
				continue;

			if (inStr[0] == '!')
			{
				if (inStr == "!batch")
				{
					if (batchMode)
						mOutput << "Batch mode already started!" << std::endl;
					else
					{
						batchCtr = 0;
						batchVec.clear();
						batchMode = true;
						mOutput << "Batch mode begin" << std::endl;

						terminal.Start_Command_Batch(batchVec, assignSeq());
					}
				}
				else if (inStr == "!commit")
				{
					if (!batchMode)
						mOutput << "Not in batch mode!" << std::endl;
					else if (batchCtr == 0)
						mOutput << "No batch commands entered!" << std::endl;
					else
					{
						batchMode = false;
						mOutput << "Batch mode ended; performing commit" << std::endl;

						Process_Batch(terminal, batchVec, seq);
					}
				}
				else
					mOutput << "Unknown control command: " << inStr << std::endl;

				continue;
			}

			vec.clear();

			if (batchMode)
			{
				if (batchCtr >= mMaxBatchCommands)
				{
					mOutput << "Maximum number of batch commands reached: " << batchCtr << "; please, perform !commit" << std::endl;
					continue;
				}

				batchHdrStart = terminal.Prepare_Batch_Command_Header(batchVec);
			}
			else
				terminal.Start_Single_Command(vec, assignSeq());

			result = terminal.Encode_Command(inStr, vec);
			if (!result)
			{
				mOutput << "Encode_Command: unknown command: " << inStr << std::endl;
				continue;
			}

			if (!batchMode)
				Process_Single(terminal, vec, inStr, seq);
			else
			{
				batchCtr++;

				terminal.Append_Batch_Command(batchHdrStart, batchVec, vec);
				mOutput << "Enqueued batch command: " << inStr << std::endl;
			}
		}
		else
			break;
	}

	return 0;
}
