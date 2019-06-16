/**
 * @file    terminal_packet_builders.cpp
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-06-12
 * @brief   This file contains builder classes for terminal packets
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


#include "terminal_packet_builders.h"

Terminal_Command_Buffer::Terminal_Command_Buffer()
{
	// always set current API version
	mHeader.coreApiVersion = KETCUBE_MODULEID_CORE_API;
}

void Terminal_Command_Buffer::Set_Opcode(ketCube_terminal_command_opcode_t opcode)
{
	mHeader.opcode = opcode;
}

ketCube_terminal_command_opcode_t Terminal_Command_Buffer::Get_Opcode() const
{
	return static_cast<ketCube_terminal_command_opcode_t>(mHeader.opcode);
}

void Terminal_Command_Buffer::Set_Sequence_No(uint8_t seq)
{
	mHeader.seq = seq;
}

uint8_t Terminal_Command_Buffer::Get_Sequence_No() const
{
	return mHeader.seq;
}

void Terminal_Command_Buffer::Set_Flag_16bit_Module_ID(bool set)
{
	mHeader.is_16b_moduleid = set;
}

bool Terminal_Command_Buffer::Has_Flag_16bit_Module_Id() const
{
	return mHeader.is_16b_moduleid;
}

void Terminal_Command_Buffer::Serialize(std::vector<uint8_t>& bytesTarget, const TSerializable_Options& options) const
{
	TSerializable_Options opts;

	opts.IsModuleID16Bit = mHeader.is_16b_moduleid;

	// nasty scope; serialize header
	{
		const uint8_t* hdrPtr = reinterpret_cast<const uint8_t*>(&mHeader);
		for (size_t i = 0; i < sizeof(ketCube_remoteTerminal_packet_header_t); i++)
			bytesTarget.push_back(hdrPtr[i]);
	}

	// depending on opcode, serializer options vary
	switch (Get_Opcode())
	{
		case KETCUBE_TERMINAL_OPCODE_CMD:
			opts.PrependLength = false;
			break;
		case KETCUBE_TERMINAL_OPCODE_BATCH:
			opts.PrependLength = true;
			break;
	}

	// serialize all terminal command blocks
	for (size_t i = 0; i < mBlocks.size(); i++) {
		mBlocks[i].Serialize(bytesTarget, opts);
	}
}

void Terminal_Command_Buffer::Append(const Terminal_Command_Block& block)
{
	mBlocks.push_back(block);
}

void Terminal_Command_Buffer::Reset()
{
	mBlocks.clear();
}

void Terminal_Command_Block::Serialize(std::vector<uint8_t>& bytesTarget, const TSerializable_Options& options) const
{
	// prepend length; the length includes module ID ("subheader")
	if (options.PrependLength) {
		bytesTarget.push_back(static_cast<uint8_t>(size() + (options.IsModuleID16Bit ? sizeof(uint16_t) : sizeof(uint8_t))));
	}

	// append module ID (LSB, and if 16bit flag is set, include MSB)
	bytesTarget.push_back(static_cast<uint8_t>(mModuleID & 0xFF));
	if (options.IsModuleID16Bit) {
		bytesTarget.push_back(static_cast<uint8_t>((mModuleID >> 8) & 0xFF));
	}

	// copy the rest of contents (the actual "path")
	std::copy(begin(), end(), std::back_inserter(bytesTarget));
}

void Terminal_Command_Block::Set_Module_ID(ketCube_moduleID_t id)
{
	mModuleID = id;
}

ketCube_moduleID_t Terminal_Command_Block::Get_Module_ID() const
{
	return mModuleID;
}
