/**
 * @file    terminal.cpp
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-05-27
 * @brief   This file contains terminal general logic implementation
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

#include "terminal.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <numeric>

/*
 * Enumerator of lookup phases;
 */
enum class LookupPhase
{
	Root,		// module or subtree follows
	Module,		// subtree follows
	Subtree		// module or subtree follows (module NYI)
};

extern ketCube_terminal_paramSet_t commandIOParams;

ketCube_moduleID_t lookup_module_id(const std::string& moduleName)
{
	const size_t moduleCnt = get_module_count();
	const ketCube_cfg_Module_t* modlist = get_module_list();

	for (size_t i = 0; i < moduleCnt; i++) {
		if (moduleName == modlist[i].name) {
			return modlist[i].id;
		}
	}

	return KETCUBE_MODULEID_INVALID;
}

static uint8_t ketCube_terminal_getNextParam(const char* commandBuffer, uint8_t ptr)
{
	// find next param
	while (commandBuffer[ptr] != ' ') {
		if (ptr < KETCUBE_TERMINAL_CMD_MAX_LEN) {
			ptr++;
		}
		else {
			return 0;
		}
	}
	while (commandBuffer[ptr] == ' ') {
		if (ptr < KETCUBE_TERMINAL_CMD_MAX_LEN) {
			ptr++;
		}
		else {
			return 0;
		}
	}

	return ptr;
}

static bool ketCube_terminal_parseParams(ketCube_terminal_cmd_t* command,
	ketCube_terminal_command_flags_t *contextFlags, const char* commandBuffer)
{
	uint8_t ptr = 0;
	uint8_t len = 0;
	int i, tmpCmdLen, tmpSeverity;
	char *endptr;

	if ((contextFlags->isGeneric == TRUE)
		&& (contextFlags->isShowCmd == TRUE)) {
		return TRUE;
	}

	size_t commandParamsPos = 0;

	switch (command->paramSetType)
	{
		default:
		case KETCUBE_TERMINAL_PARAMS_NONE:
			return TRUE;
		case KETCUBE_TERMINAL_PARAMS_STRING:
		{
			strncpy(commandIOParams.as_string, &(commandBuffer[commandParamsPos]), KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH);
			return TRUE;
		}
		case KETCUBE_TERMINAL_PARAMS_BYTE:
		{
			commandIOParams.as_byte = (uint8_t)strtol(&(commandBuffer[commandParamsPos]), &endptr, 10);

			if (endptr == &(commandBuffer[commandParamsPos])) {
				return FALSE;
			}
			return TRUE;
		}
		case KETCUBE_TERMINAL_PARAMS_BOOLEAN:
		{
			commandIOParams.as_uint32 = strtoul(&(commandBuffer[commandParamsPos]), &endptr, 10);

			if (endptr == &(commandBuffer[commandParamsPos])) {
				return FALSE;
			}

			if (commandIOParams.as_uint32 != 0) {
				commandIOParams.as_bool = TRUE;
			}
			else {
				commandIOParams.as_bool = FALSE;
			}
			return TRUE;
		}
		case KETCUBE_TERMINAL_PARAMS_MODULEID:
		{
			commandIOParams.as_module_id.module_id = (uint16_t)-1;
			commandIOParams.as_module_id.severity = KETCUBE_CORECFG_DEFAULT_SEVERITY;

			ketCube_cfg_Module_t *modlist = get_module_list();
			size_t modcnt = get_module_count();

			for (i = KETCUBE_LISTS_MODULEID_FIRST; i < modcnt; i++) {

				tmpCmdLen = (int)strlen(&(modlist[i].name[0]));

				if (strncmp(&(modlist[i].name[0]), &(commandBuffer[commandParamsPos]), tmpCmdLen) == 0) {

					commandIOParams.as_module_id.module_id = modlist[i].id;

					if (commandBuffer[commandParamsPos + tmpCmdLen] == 0x00) {
						break;
					}

					if (commandBuffer[commandParamsPos + tmpCmdLen] == ' ') {

						sscanf(&(commandBuffer[commandParamsPos + tmpCmdLen + 1]), "%d", (int*)&tmpSeverity);
						commandIOParams.as_module_id.severity = (ketCube_severity_t)tmpSeverity;
						if (commandIOParams.as_module_id.severity > KETCUBE_CFG_SEVERITY_DEBUG) {
							commandIOParams.as_module_id.severity = KETCUBE_CORECFG_DEFAULT_SEVERITY;
						}
						break;
					}
				}
			}

			if (i == modcnt) {
				return FALSE;
			}

			return TRUE;
		}
		case KETCUBE_TERMINAL_PARAMS_INT32:
		{
			commandIOParams.as_int32 = strtol(&(commandBuffer[commandParamsPos]), &endptr, 10);

			if (endptr == &(commandBuffer[commandParamsPos])) {
				return FALSE;
			}
			return TRUE;
		}
		case KETCUBE_TERMINAL_PARAMS_UINT32:
		{
			commandIOParams.as_uint32 = strtoul(&(commandBuffer[commandParamsPos]), &endptr, 10);

			if (endptr == &(commandBuffer[commandParamsPos])) {
				return FALSE;
			}
			return TRUE;
		}
		case KETCUBE_TERMINAL_PARAMS_INT32_PAIR:
		{
			commandIOParams.as_int32_pair.first = strtol(&(commandBuffer[commandParamsPos]), &endptr, 10);

			if (endptr == &(commandBuffer[commandParamsPos])) {
				return FALSE;
			}

			ptr = ketCube_terminal_getNextParam(commandBuffer, (uint8_t)commandParamsPos);
			if (ptr == 0) {
				return FALSE;
			}

			commandIOParams.as_int32_pair.second = strtol(&(commandBuffer[ptr]), &endptr, 10);

			if (endptr == &(commandBuffer[ptr])) {
				return FALSE;
			}
			return TRUE;
		}
		case KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY:
		{
			len = ketCube_common_Min((uint8_t)strlen(&(commandBuffer[commandParamsPos])), KETCUBE_TERMINAL_PARAM_STR_MAX_LENGTH);

			if (ketCube_common_IsHexString(&(commandBuffer[commandParamsPos]), len) == FALSE) {
				return FALSE;
			}

			ketCube_common_Hex2Bytes((uint8_t *) &(commandIOParams.as_byte_array.data[0]), &(commandBuffer[commandParamsPos]), len);

			commandIOParams.as_byte_array.length = len / 2;

			return TRUE;
		}
	}

	return FALSE;
}

static bool ketCube_terminal_parseCommandOutput(ketCube_terminal_cmd_t* command, std::string& out)
{
	uint16_t i;

	if (command->outputSetType == KETCUBE_TERMINAL_PARAMS_NONE) {
		return false;
	}

	switch (command->outputSetType)
	{
		case KETCUBE_TERMINAL_PARAMS_BOOLEAN:
		{
			if (commandIOParams.as_bool == TRUE) {
				out = "TRUE";
			} else {
				out = "FALSE";
			}
			break;
		}
		case KETCUBE_TERMINAL_PARAMS_STRING:
		{
			out = commandIOParams.as_string;
			break;
		}
		case KETCUBE_TERMINAL_PARAMS_INT32:
		{
			out = std::to_string(commandIOParams.as_int32);
			break;
		}
		case KETCUBE_TERMINAL_PARAMS_UINT32:
		{
			out = std::to_string(commandIOParams.as_uint32);
			break;
		}
		case KETCUBE_TERMINAL_PARAMS_BYTE:
		{
			out = std::to_string(static_cast<int>(commandIOParams.as_byte));
			break;
		}
		case KETCUBE_TERMINAL_PARAMS_INT32_PAIR:
		{
			out = std::to_string(commandIOParams.as_int32_pair.first) + ", " + std::to_string(commandIOParams.as_int32_pair.second);
			break;
		}
		case KETCUBE_TERMINAL_PARAMS_BYTE_ARRAY:
		{
			std::ostringstream ostr;
			for (i = 0; i < commandIOParams.as_byte_array.length; i++) {

				ostr << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << commandIOParams.as_byte_array.data[i];
				if (i != commandIOParams.as_byte_array.length) {
					ostr << "-";
				}
			}

			out = ostr.str();
			break;
		}
		case KETCUBE_TERMINAL_PARAMS_MODULEID:
		{
			ketCube_cfg_Module_t *modlist = get_module_list();
			size_t modcnt = get_module_count();

			for (i = KETCUBE_LISTS_MODULEID_FIRST; i < modcnt; i++) {
				if (modlist[i].id == commandIOParams.as_module_id.module_id) {
					out = modlist[i].name;
					break;
				}
			}

			if (i == modcnt) {
				out = "invalid module";
			}
		}
		default:
		{
			out = "<unknown return type>";
			break;
		}
	}

	return true;
}

static bool ketCube_terminal_processCommandErrors(ketCube_terminal_command_errorCode_t retCode, std::string& out)
{
	if (retCode == KETCUBE_TERMINAL_CMD_ERR_OK) {
		out = "Command execution OK";
		return true;
	}

	out = "Command returned error: ";

	switch (retCode)
	{
		case KETCUBE_TERMINAL_CMD_ERR_INVALID_PARAMS:
			out += "invalid parameters";
			break;
		case KETCUBE_TERMINAL_CMD_ERR_MEMORY_IO_FAIL:
			out += "could not read/write memory";
			break;
		case KETCUBE_TERMINAL_CMD_ERR_COMMAND_NOT_FOUND:
			out += "requested command not found";
			break;
		case KETCUBE_TERMINAL_CMD_ERR_MODULE_NOT_FOUND:
			out += "requested module not found";
			break;
		case KETCUBE_TERMINAL_CMD_ERR_FAILED_CONTEXT:
			out += "invalid command context";
			break;
		case KETCUBE_TERMINAL_CMD_ERR_UNSPECIFIED_ERROR:
			out += "unspecified error";
			break;
		case KETCUBE_TERMINAL_CMD_ERR_NOT_SUPPORTED:
			out += "command or parameter not supported";
			break;
		case KETCUBE_TERMINAL_CMD_ERR_CORE_API_MISMATCH:
			out += "mismatch between local and remote API version (local: " + std::to_string(KETCUBE_MODULEID_CORE_API) + ")";
			break;
		default:
			out += "unknown error (" + std::to_string(retCode) + ")";
			break;
	}

	return false;
}

void Terminal_Base::Start_Single_Command(Terminal_Command_Buffer& target, uint8_t seq)
{
	target.Set_Opcode(KETCUBE_TERMINAL_OPCODE_CMD);
	target.Set_Flag_16bit_Module_ID(false);
	target.Set_Sequence_No(seq);

	mPendingCommandRef.clear();
}

void Terminal_Base::Start_Command_Batch(Terminal_Command_Buffer& target, uint8_t seq)
{
	target.Set_Opcode(KETCUBE_TERMINAL_OPCODE_BATCH);
	target.Set_Flag_16bit_Module_ID(false);
	target.Set_Sequence_No(seq);

	mPendingCommandRef.clear();
}

bool Terminal_Base::Encode_Command(const std::string& cmd, Terminal_Command_Block& target)
{
	size_t i;

	std::vector<size_t> path;

	std::vector<std::string> tokens;
	std::istringstream istr(cmd);
	std::string word;
	while (std::getline(istr, word, ' ')) {
		tokens.push_back(word);
	}

	if (tokens.size() == 0) {
		return false;
	}

	ketCube_terminal_cmd_t* subtree = get_cmd_tree();
	LookupPhase lupphase = LookupPhase::Root;

	ketCube_moduleID_t moduleId;
	size_t tokSize = 0;

	ketCube_terminal_command_flags_t activeFlags;

	for (size_t tok = 0; tok < tokens.size(); tok++) {

		tokSize += tokens[tok].length() + 1; // +1 for space

		if (lupphase == LookupPhase::Module) {

			moduleId = lookup_module_id(tokens[tok]);
			if (moduleId == KETCUBE_MODULEID_INVALID) {
				//std::cerr << "Module " << tokens[tok] << " not found" << std::endl;
				return false;
			}
		}

		for (i = 0; subtree[i].cmd != nullptr; i++) {

			if (lupphase == LookupPhase::Root || lupphase == LookupPhase::Subtree) {

				if (tokens[tok] == subtree[i].cmd) {
					break;
				}
			} else if (lupphase == LookupPhase::Module) {

				if (moduleId == subtree[i].moduleId) {
					break;
				}
			}
		}

		if (subtree[i].cmd == nullptr) {
			//std::cerr << "Reached end of command without finding requested command" << std::endl;
			return false;
		}

		if (lupphase == LookupPhase::Root) {
			activeFlags = subtree[i].flags;
		} else {
			ketCube_terminal_andCmdFlags(&activeFlags, &activeFlags, &(subtree[i].flags));
		}

		if (lupphase == LookupPhase::Module) {
			target.Set_Module_ID(moduleId);
		} else {
			target.push_back(static_cast<uint8_t>(i));
		}

		if (!subtree[i].flags.isGroup) {
			break;
		}

		if (lupphase == LookupPhase::Root) {

			if (activeFlags.isGeneric && activeFlags.isGroup && (activeFlags.isSetCmd || activeFlags.isShowCmd)) {
				lupphase = LookupPhase::Module;
			}  else {
				lupphase = LookupPhase::Subtree;
			}
		} else if (lupphase == LookupPhase::Module) {
			lupphase = LookupPhase::Subtree;
		}

		subtree = subtree[i].settingsPtr.subCmdList;
	}

	if (!subtree[i].flags.isRemote)  {
		//std::cerr << "Attempt to execute local-only command" << std::endl;
		return false;
	}

	uint8_t result = ketCube_terminal_parseParams(&subtree[i], &activeFlags, cmd.c_str() + tokSize);

	if (result == FALSE) {
		//std::cerr << "Unable to parse command parameters" << std::endl;
		return false;
	}

	size_t paramRawLen = ketCube_terminal_GetIOParamsLength(subtree[i].paramSetType);
	if (paramRawLen > 0) {
		size_t origSize = target.size();
		target.resize(origSize + paramRawLen);

		memcpy(target.data() + origSize, &commandIOParams, paramRawLen);
	}

	mPendingCommandRef.push_back(&subtree[i]);

	return true;
}

bool Terminal_Base::Decode_Response_Contents(const std::vector<uint8_t>& response, size_t startPos, size_t length, bool& responseOK, std::string& target, ketCube_terminal_cmd_t* command) const
{
	std::ostringstream resultBuilder;
	std::string resultStr;
	bool result, success;

	success = true;

	ketCube_terminal_command_errorCode_t errCode = static_cast<ketCube_terminal_command_errorCode_t>(response[startPos]);

	result = ketCube_terminal_processCommandErrors(errCode, resultStr);
	if (result) {

		responseOK = true;

		resultBuilder << resultStr;

		if (length - 1 >= sizeof(commandIOParams)) {
			resultBuilder << "Invalid value set retrieved (size = " << (length - 1) << ", expected max. size = " << sizeof(commandIOParams);

			success = false;
		} else {
			memcpy(&commandIOParams, response.data() + startPos + 1, length - 1);

			result = ketCube_terminal_parseCommandOutput(command, resultStr);
			if (result) {
				resultBuilder << std::endl << command->cmd << " returned: " << resultStr;
			}
		}
	} else {
		// error - no more outputs
		resultBuilder << resultStr;
	}

	target = resultBuilder.str();

	return success;
}

bool Terminal_Base::Decode_Single_Response(const std::vector<uint8_t>& response, bool& responseOK, std::string& target, uint8_t seq, bool& seqOK) const
{
	bool success;

	std::ostringstream resultBuilder;

	seqOK = true;
	success = false;
	responseOK = false;

	const ketCube_remoteTerminal_packet_header_t* inHeader = reinterpret_cast<const ketCube_remoteTerminal_packet_header_t*>(response.data());

	// has to be at least two bytes (opcode and size)
	if (response.size() < sizeof(ketCube_remoteTerminal_packet_header_t)) {
		resultBuilder << "No data received" << std::endl;
		success = false;
	}
	// there always has to be a pending command, so we could decode response
	else if (mPendingCommandRef.empty()) {
		resultBuilder << "No pending command" << std::endl;
		success = false;
	}
	// sequence numbers must match
	else if (inHeader->seq != seq) {
		seqOK = false;
		success = false;
	}
	// decoding single response must begin with single command opcode
	else if (inHeader->opcode != KETCUBE_TERMINAL_OPCODE_CMD) {
		resultBuilder << "Unexpected result opcode " << static_cast<int>(response[0]) << " (expected " << static_cast<int>(KETCUBE_TERMINAL_OPCODE_CMD) << ")" << std::endl;
		success = false;
	}
	// everything OK, decode contents
	else {
		std::string cmdResContents;
		success = Decode_Response_Contents(response, 2, response.size() - 2, responseOK, cmdResContents, mPendingCommandRef[0]);

		resultBuilder << cmdResContents;
	}

	target = resultBuilder.str();

	return success;
}

bool Terminal_Base::Decode_Batch_Response(const std::vector<uint8_t>& response, bool& responseOK, std::string& target, uint8_t seq, bool& seqOK) const
{
	bool success;

	std::ostringstream resultBuilder;

	seqOK = true;
	success = false;
	responseOK = false;

	const ketCube_remoteTerminal_packet_header_t* inHeader = reinterpret_cast<const ketCube_remoteTerminal_packet_header_t*>(response.data());

	// has to be at least two bytes (opcode and size)
	if (response.size() < sizeof(ketCube_remoteTerminal_packet_header_t)) {
		resultBuilder << "No data received" << std::endl;
		success = false;
	}
	// there always has to be a pending command
	else if (mPendingCommandRef.empty()) {
		resultBuilder << "No pending command" << std::endl;
		success = false;
	}
	// sequence numbers must match
	else if (inHeader->seq != seq) {
		seqOK = false;
		success = false;
	}
	// decoding batch response must begin with batch command opcode
	else if (inHeader->opcode != KETCUBE_TERMINAL_OPCODE_BATCH) {
		resultBuilder << "Unexpected result opcode " << static_cast<int>(response[0]) << " (expected " << static_cast<int>(KETCUBE_TERMINAL_OPCODE_BATCH) << ")" << std::endl;
		success = false;
	}
	// everything ok, attempt to decode insides
	else {
		size_t startPos;
		std::string cmdResContents;
		bool respOK = true;
		size_t pendCmdPos = 0;
		size_t len;

		// start at position 2 (opcode + seq), end at last byte
		for (startPos = 2; startPos < response.size(); ) {

			// maximum number of responses is limited to actual commands issued count
			if (pendCmdPos >= mPendingCommandRef.size()) {
				resultBuilder << "Received response for more than the length of pending command queue" << std::endl;
				break;
			}
			cmdResContents.clear();

			// first byte = length of response byte array
			len = response[startPos];

			startPos++;

			// overall success is determined by and-ing all partial successes
			success |= Decode_Response_Contents(response, startPos, len, respOK, cmdResContents, mPendingCommandRef[pendCmdPos]);
			// overall "OK" status as well
			responseOK |= respOK;

			resultBuilder << cmdResContents << std::endl;

			// move to next response byte array and command
			startPos += len;
			pendCmdPos++;
		}
	}

	target = resultBuilder.str();

	return success;
}

bool Terminal_Base::Await_Message(std::vector<uint8_t>& target, const size_t timeoutMs)
{
	std::unique_lock<std::mutex> lck(mQueue_Mtx);

	auto waitPred = [this]() { return !mIncoming_Queue.empty(); };

	// timeoutMs == 0 means no timeout (wait forever)
	if (timeoutMs == 0) {
		mQueue_Cv.wait(lck, waitPred);
	} else {
		mQueue_Cv.wait_for(lck, std::chrono::milliseconds(timeoutMs), waitPred);
	}

	// timeout, stolen wakeup or some sort of app-level interruption (cv was signalized)
	if (mIncoming_Queue.empty()) {
		return false;
	}

	// move queue top to avoid copying
	target = std::move(mIncoming_Queue.front());
	mIncoming_Queue.pop();

	return true;
}
