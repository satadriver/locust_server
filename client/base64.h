#pragma once


#include <windows.h>

using namespace std;

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);


int base64_decode(unsigned char* encoded_string, int in_len, unsigned char* ret);