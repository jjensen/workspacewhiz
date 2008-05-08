///////////////////////////////////////////////////////////////////////////////
// $Workfile: DecodeUtils.cpp $
// $Archive: /WWhizReg/DecodeUtils.cpp $
// $Date:: 9/26/00 1:24a   $ $Revision:: 7    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#define NUM_CHARS 64
#define CHAR_SHIFT 6
static char* s_bin2Print = "1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM[]";

void StringToBinary(LPCTSTR str, BYTE* data, UINT& size)
{
	size = 0;
	LPCTSTR ptr = str;

	BYTE ch = 0;
	int whichBit = 0;
	while (*ptr)
	{
		BYTE value = *ptr++;

		// Make a reverse lookup table...
		for (int j = 0; j < NUM_CHARS; j++)
		{
			if (value == s_bin2Print[j])
			{
				value = (BYTE)j;
				break;
			}
		}
		
		// Build value.
		for (int i = CHAR_SHIFT - 1; i >= 0; --i)
		{
			BYTE bit = (BYTE)((value >> i) & 1);
			ch |= (bit << whichBit);

			whichBit++;
			if (whichBit == 8)
			{
				whichBit  = 0;
				*data++ = ch;
				ch = 0;
				size++;
			}
		}
	}
/*	if (whichBit > 0)
	{
		*data++ = ch;
		size++;
	}
*/
}


void ROT13(BYTE* p)
{
    char c;

    while (*p)
    {
        c = (BYTE)toupper(*p);
        if (c >= 'A'  &&  c <= 'Z')
        {
            (*p) += 13;
            c += 13;
            if ((c) > 'Z') (*p) -= ('Z' - 'A' + 1);
        }
        p++;
    }
}


UINT HashName(LPCSTR key)
{
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}


#ifdef KEYGEN
inline bool HasExpired()
{
	return false;
}
#endif KEYGEN

int ValidateRegistrationCode(CString regName, CString regCode, CString checkCode, int& license, time_t& regDate)
{
	if (HasExpired())
		return 0;

	int validRegistration = 0;

	// Now, undo the check code.
	BYTE checkCodeBin[40];
	UINT checkCodeBinSize;
	StringToBinary(checkCode, (BYTE*)checkCodeBin, checkCodeBinSize);
	if (checkCodeBinSize < 8)
		return 0;
	license = 1;
	if (checkCodeBinSize == 9)
	{
		license = checkCodeBin[8] ^ 0x63;
		for (int i = 0; i < 8; i++)
			checkCodeBin[i] -= (BYTE)(256 - (license ^ 0x61));
	}

	// Undo the registration code.
	BYTE regCodeBin[256];
	UINT regCodeBinSize;
	StringToBinary(regCode, (BYTE*)regCodeBin, regCodeBinSize);
	regCodeBin[regCodeBinSize] = 0;			// Just in case it's invalid.

	// Decode the registration key.
	if (checkCodeBinSize == 9)
	{
		for (UINT i = 0; i < regCodeBinSize; i++)
			regCodeBin[i] += (BYTE)(((BYTE)license ^ 0x85));
	}

	// Figure out the string length.
	UINT nameLen = strlen((LPCTSTR)regCodeBin);
	if (nameLen == regCodeBinSize)
		return 0;

	if (checkCodeBinSize == 9)
	{
		for (UINT i = 0; i < nameLen; i++)
			regCodeBin[i] -= (license ^ 0x20);
	}
	ROT13((BYTE*)regCodeBin);

	if (license > 1  &&  (regCodeBin[nameLen + 1] & 0x60))
		validRegistration++;

	// Check the check code hash.
	UINT checkCodeHash = HashName(checkCode);
	if (*(UINT*)&regCodeBin[nameLen + 2] == checkCodeHash)
		validRegistration++;

	// Now, verify the name.
	if (strcmp((LPCTSTR)regCodeBin, regName) == 0)
		validRegistration++;

	// Check the name hash code.
	UINT regNameHash = HashName(regName);
	if (*(UINT*)&checkCodeBin[4] == regNameHash)
		validRegistration++;

	// Valid registration code.  Get the time.
	regDate = *(time_t*)&checkCodeBin[0];
//	CTime time = timeReg;
//	CString timeStr = time.Format("%A, %B %d, %Y");

	return validRegistration;
}
