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
