/**
 * @file    terminal.h
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-05-27
 * @brief   This file contains abstract terminal parent
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

#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <condition_variable>
#include <chrono>

#include "impl_bridge.h"
#include "terminal_packet_builders.h"

/*
 * Base class for all terminal implementations
 */
class Terminal_Base
{
	protected:
		// queue of incoming messages
		std::queue<std::vector<uint8_t>> mIncoming_Queue;
		// mutex for queue locking
		std::mutex mQueue_Mtx;
		// condition variable for producer/consument-style message passing
		std::condition_variable mQueue_Cv;

		// vector of pending commands; one record when using single-cmd mode, multiple records in batch mode
		std::vector<ketCube_terminal_cmd_t*> mPendingCommandRef;

	protected:
		// decodes contents of response regardless the type
		bool Decode_Response_Contents(const std::vector<uint8_t>& response, size_t startPos, size_t length, bool& responseOK, std::string& target, ketCube_terminal_cmd_t* command) const;

	public:
		Terminal_Base() = default;
		virtual ~Terminal_Base() = default;

		/* base implementation */

		// starts single command routine
		void Start_Single_Command(Terminal_Command_Buffer& target, uint8_t seq);
		// starts batch command routine
		void Start_Command_Batch(Terminal_Command_Buffer& target, uint8_t seq);

		// encodes command using command tree
		bool Encode_Command(const std::string& cmd, Terminal_Command_Block& target);

		// decodes response of single command requst
		bool Decode_Single_Response(const std::vector<uint8_t>& response, bool& responseOK, std::string& target, uint8_t seq, bool& seqOK) const;
		// decodes response of batch command request
		bool Decode_Batch_Response(const std::vector<uint8_t>& response, bool& responseOK, std::string& target, uint8_t seq, bool& seqOK) const;

		// awaits message for given period of time; returns true on success, false on timeout
		bool Await_Message(std::vector<uint8_t>& target, const size_t timeoutMs);

		/* interface */

		// initializes terminal instance
		virtual bool Init() { return true; };
		// sends command to remote endpoint using given settings
		virtual bool Send_Command(const std::vector<uint8_t>& parsed_command) = 0;
};
