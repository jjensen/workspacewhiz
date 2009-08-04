#include "Base32.h"

extern "C"
{
	#include "md5global.h"
	#include "md5.h"
};

#include <time.h>

bool ValidateRegistrationCode(const char* registrationCode, const char* registrationName, time_t& registrationTime)
{
	registrationTime = -1;

	// Convert it back.
	unsigned char digest[16];
	char data32[27];

	int encodeLength = Base32::GetEncode32Length(16);

	size_t len = strlen(registrationCode);
	if (len != 29)
		return false;

	if (registrationCode[5] != '-'  ||  registrationCode[11] != '-'  ||  registrationCode[17] != '-'  ||
		registrationCode[23] != '-')
		return false;

	int index = 0;
	int i;
	for (i = 0; i < (int)strlen(registrationCode); ++i)
	{
		if (registrationCode[i] != '-')
			data32[index++] = registrationCode[i];
	}
	data32[25] = '1';
	data32[26] = 0;

	const char alphabet[] = "123456789ABCDEFGHJKMNPQRSTUVWXYZ";
	if (!Base32::Unmap32((unsigned char*)data32, encodeLength, (unsigned char*)alphabet))
		return false;

	int decodeLength = Base32::GetDecode32Length(26);
	Base32::Decode32((unsigned char*)data32, encodeLength, (unsigned char*)digest);

	// Fudge it.
	int outBit = ((digest[0] >> 5) + 2) % 8;
	int inBit = outBit;

	unsigned int registrationBits = 0;
	for (i = 0; i < 32; ++i)
	{
		registrationBits |= (((digest[1 + (i % 14)] >> outBit) & 1) << inBit);
		outBit = (outBit + 7) % 8;
		inBit = (inBit + 31) % 32;
	}

	registrationTime = registrationBits;

	MD5_CTX c;
	MD5Init(&c);
	const char* productName = "Workspace Whiz 4.0";
	MD5Update(&c, (unsigned char*)(const char*)productName, (unsigned int)strlen(productName));
	MD5Update(&c, (unsigned char*)(const char*)registrationName, (unsigned int)strlen(registrationName));
	MD5Update(&c, (unsigned char*)&registrationTime, sizeof(int));
	unsigned char newDigest[16];
	MD5Final(newDigest, &c);
	newDigest[15] &= 0xf8;

	// Fudge it.
	outBit = ((digest[0] >> 5) + 2) % 8;
	inBit = outBit;

	for (i = 0; i < 32; ++i)
	{
		digest[1 + (i % 14)] &= ~(1 << outBit);
		digest[1 + (i % 14)] ^= (newDigest[1 + (i % 14)] & (1 << outBit));
		outBit = (outBit + 7) % 8;
		inBit = (inBit + 31) % 32;
	}

	return memcmp(digest, newDigest, 16) == 0;
}
