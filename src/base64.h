/**
 * @file    base64.h
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-05-27
 * @brief   This file contains base64 encoding/decoding class interface
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

#include <cstdint>
#include <string>
#include <vector>

/*
 * Base64 encode/decode static class
 */
class Base64
{
	private:
		// no instance allowed, delete constructor
		Base64() = delete;

		// static encode fnc
		static void Encode(std::string &ret, const uint8_t* buf, const size_t bufLen);

	private:
		// pre-generated decoding table
		static const uint8_t Decoding_Table[128];
		// pre-generated encoding table
		static const char Encoding_Table[65];

	public:
		// encodes "buf" byte buffer to base64 string "out"
		static void Encode(std::string &out, const std::vector<uint8_t>& buf);

		// decodes input "encoded_string" from base64 to byte buffer "out"
		static void Decode(std::vector<uint8_t> &out, const std::string& encoded_string);
};
