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
