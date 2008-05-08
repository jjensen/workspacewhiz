///////////////////////////////////////////////////////////////////////////////
// $Workfile: DecodeUtils.h $
// $Archive: /WorkspaceWhiz/Src/WWhizReg/DecodeUtils.h $
// $Date:: 5/13/00 10:58p  $ $Revision:: 4    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#pragma once

void StringToBinary(LPCTSTR str, BYTE* data, UINT& size);
void ROT13(BYTE* p);
UINT HashName(LPCSTR key);
int ValidateRegistrationCode(CString regName, CString regCode, CString checkCode, int& license, time_t& regDate);

