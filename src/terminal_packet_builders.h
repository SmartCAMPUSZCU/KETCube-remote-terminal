/**
 * @file    terminal_packet_builders.h
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

#pragma once

#include <vector>

#include "impl_bridge.h"

struct ketCube_terminal_cmd_t;

/*
 * Defined serializable options
 */
struct TSerializable_Options
{
	bool PrependLength = false;
	bool IsModuleID16Bit = false;
};

/*
 * Interface defining serializability
 */
class ISerializable
{
	public:
		// serializes contents into byte buffer; respects serializable options given
		virtual void Serialize(std::vector<uint8_t>& bytesTarget, const TSerializable_Options& options = {}) const = 0;
};

/*
 * Serializable terminal command block
 */
class Terminal_Command_Block : public std::vector<uint8_t>, public ISerializable
{
	protected:
		// encapsulated module ID; the final physical length may vary according to serializer options
		ketCube_moduleID_t mModuleID;

	public:
		virtual void Serialize(std::vector<uint8_t>& bytesTarget, const TSerializable_Options& options = {}) const override;

		// sets module ID, regardless of final length
		void Set_Module_ID(ketCube_moduleID_t id);
		// retrieves module ID (original)
		ketCube_moduleID_t Get_Module_ID() const;
};

/*
 * Class providing additional packet-related routines
 */
class Terminal_Command_Buffer : public ISerializable
{
	private:
		// packet header
		ketCube_remoteTerminal_packet_header_t mHeader;
		// all stored blocks (for all kinds of commands)
		std::vector<Terminal_Command_Block> mBlocks;

	public:
		Terminal_Command_Buffer();
		virtual ~Terminal_Command_Buffer() = default;

		// sets opcode to header
		void Set_Opcode(ketCube_terminal_command_opcode_t opcode);
		// retrieves header opcode
		ketCube_terminal_command_opcode_t Get_Opcode() const;

		// sets sequence number to header
		void Set_Sequence_No(uint8_t seq);
		// retrieves sequence number from header
		uint8_t Get_Sequence_No() const;

		// sets 16bit moduleID flag
		void Set_Flag_16bit_Module_ID(bool set = true);
		// retrieves 16bit moduleID flag
		bool Has_Flag_16bit_Module_Id() const;

		virtual void Serialize(std::vector<uint8_t>& bytesTarget, const TSerializable_Options& options = {}) const override;

		// appends next terminal command block
		void Append(const Terminal_Command_Block& block);
		// resets the contents; reuses the instance
		void Reset();
};
