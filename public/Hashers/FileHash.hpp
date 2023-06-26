#pragma once

#include "PicoSHA2/picosha2.h"

#include <string>

namespace cd
{

std::string FileHash(const char* pFileName)
{
	std::ifstream fin(pFileName, std::ios::binary);
	std::vector<unsigned char> data(picosha2::k_digest_size);
	picosha2::hash256(fin, data.begin(), data.end());

	std::string result;
	picosha2::hash256_hex_string(data, result);
	return result;
}

}