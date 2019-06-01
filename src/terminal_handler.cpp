/**
 * @file    terminal_handler.cpp
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-05-27
 * @brief   This file contains implementation of terminal command sending manager
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

#include <iostream>
#include <vector>
#include <random>
#include "mqtt_terminal.h"

#include "terminal_handler.h"

// random device instance for generating initial SEQ's
static std::random_device Random_Device;

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
	uint8_t seq = static_cast<uint8_t>(Random_Device());

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
				else if (inStr == "!abort")
				{
					if (!batchMode)
						mOutput << "Not in batch mode!" << std::endl;
					else
					{
						batchMode = false;
						mOutput << "Batch mode aborted" << std::endl;
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
