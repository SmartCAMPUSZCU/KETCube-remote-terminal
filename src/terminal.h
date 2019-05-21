#pragma once

#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <condition_variable>
#include <chrono>

struct ketCube_terminal_cmd_t;

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
		void Start_Single_Command(std::vector<uint8_t>& target, uint8_t seq);
		// starts batch command routine
		void Start_Command_Batch(std::vector<uint8_t>& target, uint8_t seq);

		// prepares header for batch command
		size_t Prepare_Batch_Command_Header(std::vector<uint8_t>& target);
		// appends encoded command to batch buffer
		void Append_Batch_Command(size_t beginHeaderByte, std::vector<uint8_t>& target, const std::vector<uint8_t>& cmdBuffer);

		// encodes command using command tree
		bool Encode_Command(const std::string& cmd, std::vector<uint8_t>& target);

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
