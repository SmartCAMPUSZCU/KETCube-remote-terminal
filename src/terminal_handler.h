/**
 * @file    terminal_handler.h
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-05-27
 * @brief   This file contains manager of terminal command sending
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

#include <iostream>

#include "terminal.h"

/*
 * Terminal handler class - manages the outer logic of reading from file and performing send routines
 */
class Terminal_Handler final
{
	private:
		// input file
		std::istream& mInput;
		// output file
		std::ostream& mOutput;

		// timeout for response to command
		long mResponseTimeout;
		// maximum number of commands in batch
		size_t mMaxBatchCommands;

	protected:
		// processes sending of single command request
		void Process_Single(Terminal_Base& terminal, std::vector<uint8_t>& encoded, std::string& inStr, uint8_t seq);
		// processes sending of batch command request
		void Process_Batch(Terminal_Base& terminal, std::vector<uint8_t>& encoded, uint8_t seq);

	public:
		Terminal_Handler(std::istream& input, std::ostream& output, long responseTimeoutSecs = 60, long maxBatchCmds = 3);

		// runs the terminal routine, ends after the input reports eof/invalid state
		int Run(Terminal_Base& terminal);
};
