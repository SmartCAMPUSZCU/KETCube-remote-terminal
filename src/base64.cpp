/**
 * @file    base64.cpp
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-05-27
 * @brief   This file contains base64 encoding/decoding class implementation
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

#include "base64.h"

const uint8_t Base64::Decoding_Table[128] = {
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255,  62, 255,  63,
	52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,   0, 255, 255, 255,
   255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255,  63,
   255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
	41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255
};

const char Base64::Encoding_Table[65] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

void Base64::Encode(std::string &ret, const uint8_t* buf, const size_t bufLen)
{
	size_t missing = 0;
	size_t ret_size = bufLen;
	while ((ret_size % 3) != 0)
	{
		++ret_size;
		++missing;
	}

	ret_size = 4 * ret_size / 3;

	ret.clear();
	ret.reserve(ret_size);

	for (size_t i = 0; i < ret_size / 4; ++i)
	{
		const size_t index = i * 3;
		const uint8_t b3_0 = (index + 0 < bufLen) ? buf[index + 0] : 0;
		const uint8_t b3_1 = (index + 1 < bufLen) ? buf[index + 1] : 0;
		const uint8_t b3_2 = (index + 2 < bufLen) ? buf[index + 2] : 0;

		const uint8_t b4_0 = ((b3_0 & 0xfc) >> 2);
		const uint8_t b4_1 = ((b3_0 & 0x03) << 4) + ((b3_1 & 0xf0) >> 4);
		const uint8_t b4_2 = ((b3_1 & 0x0f) << 2) + ((b3_2 & 0xc0) >> 6);
		const uint8_t b4_3 = ((b3_2 & 0x3f) << 0);

		ret.push_back(Encoding_Table[b4_0]);
		ret.push_back(Encoding_Table[b4_1]);
		ret.push_back(Encoding_Table[b4_2]);
		ret.push_back(Encoding_Table[b4_3]);
	}

	for (size_t i = 0; i != missing; ++i)
		ret[ret_size - i - 1] = '=';
}

void Base64::Encode(std::string &out, const std::vector<uint8_t>& buf)
{
	if (buf.empty())
		Encode(out, nullptr, 0);
	else
		Encode(out, &buf[0], buf.size());
}

void Base64::Decode(std::vector<uint8_t> &ret, const std::string& encoded_string)
{
	size_t encoded_size = encoded_string.size();

	while ((encoded_size % 4) != 0)
		++encoded_size;

	const size_t N = encoded_string.size();
	ret.clear();
	ret.reserve(3 * encoded_size / 4);

	for (size_t i = 0; i < encoded_size; i += 4)
	{
		const uint8_t b4_0 = (encoded_string[i + 0] <= 'z') ? Base64::Decoding_Table[static_cast<uint8_t>(encoded_string[i + 0])] : 0xff;
		const uint8_t b4_1 = (i + 1 < N && encoded_string[i + 1] <= 'z') ? Base64::Decoding_Table[static_cast<uint8_t>(encoded_string[i + 1])] : 0xff;
		const uint8_t b4_2 = (i + 2 < N && encoded_string[i + 2] <= 'z') ? Base64::Decoding_Table[static_cast<uint8_t>(encoded_string[i + 2])] : 0xff;
		const uint8_t b4_3 = (i + 3 < N && encoded_string[i + 3] <= 'z') ? Base64::Decoding_Table[static_cast<uint8_t>(encoded_string[i + 3])] : 0xff;

		const uint8_t b3_0 = ((b4_0 & 0x3f) << 2) + ((b4_1 & 0x30) >> 4);
		const uint8_t b3_1 = ((b4_1 & 0x0f) << 4) + ((b4_2 & 0x3c) >> 2);
		const uint8_t b3_2 = ((b4_2 & 0x03) << 6) + ((b4_3 & 0x3f) >> 0);

		if (b4_1 != 0xff) ret.push_back(b3_0);
		if (b4_2 != 0xff) ret.push_back(b3_1);
		if (b4_3 != 0xff) ret.push_back(b3_2);
	}
}
