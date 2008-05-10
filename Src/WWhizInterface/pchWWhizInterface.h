///////////////////////////////////////////////////////////////////////////////
// $Workfile: pchWWhizInterface.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/pchWWhizInterface.h $
// $Date: 2003/06/23 $ $Revision: #13 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2008 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS

#pragma warning( disable : 4244 )
#pragma warning( disable : 4311 )
#pragma warning( disable : 4312 )
#include <afx.h>
#include <afxcmn.h>
#include <afxtempl.h>
#include <atlbase.h>
#include <mmsystem.h>
#include "WCollection.h"

using namespace ATL;

#ifdef WWHIZ_VC6

// Developer Studio Object Model
#include <ObjModel\appauto.h>

#endif WWHIZ_VC6

#pragma warning(push, 3)
#pragma warning(disable: 4035)
__forceinline int PASCAL FastStrlen(LPCSTR lpsz)
{
	_asm
	{
		xor eax, eax
		mov ebx, lpsz
LoopTop:
		cmp byte ptr [ebx + eax], 0
		jz Done
		cmp byte ptr [ebx + eax + 1], 0
		jz Done1
		cmp byte ptr [ebx + eax + 2], 0
		jz Done2
		cmp byte ptr [ebx + eax + 3], 0
		jz Done3
		add eax, 4
		jmp LoopTop
Done1:
		inc eax
		jmp Done
Done2:
		add eax, 2
		jmp Done
Done3:
		add eax, 3
Done:
	}
}

__forceinline bool FastCompareExactString(LPCSTR s1, LPCSTR s2, DWORD len)
{
	_asm
	{
		push esi
		mov eax, s2
		mov ebx, s1
		mov esi, len
		xor ecx, ecx
		shr esi, 2
		jz Do1
		shl esi, 2
LoopTop4:
		mov edx, [eax + ecx]
		add ecx, 4
		cmp [ebx + ecx - 4], edx
		jnz Bad
		cmp ecx, esi
		jnz LoopTop4

Do1:
		mov esi, len
		cmp ecx, esi
		jz Good
LoopTop1:
		mov dl, [eax + ecx]
		inc ecx
		cmp [ebx + ecx - 1], dl
		jnz Bad
		cmp ecx, esi
		jnz LoopTop1

Good:
		mov eax, 1
		jmp Done

Bad:
		xor eax, eax

Done:
		pop esi

	}

}

__forceinline bool FastCompareString(LPCSTR s1, LPCSTR s2, DWORD len)
{
	_asm
	{
		push esi
		mov eax, s2
		mov ebx, s1
		mov esi, len
		xor ecx, ecx
		shr esi, 2
		jz Do1
		shl esi, 2
LoopTop4:
		mov edx, [eax + ecx]
		add ecx, 4
		cmp [ebx + ecx - 4], edx
		jl Bad
		cmp ecx, esi
		jnz LoopTop4

Do1:
		mov esi, len
		cmp ecx, esi
		jz Good
LoopTop1:
		mov dl, [eax + ecx]
		inc ecx
		cmp [ebx + ecx - 1], dl
		jl Bad
		cmp ecx, esi
		jnz LoopTop1

Good:
		mov eax, 1
		jmp Done

Bad:
		xor eax, eax

Done:
		pop esi

	}

}

#pragma warning(pop)


// determine number of elements in an array (not bytes)
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif _countof

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WWHIZ_VC6
typedef bool (*pfnGetWorkspaceName)(LPTSTR buffer, int bufSize);
#endif WWHIZ_VC6

#include "ObjModelHelper.h"
#include "RCBase.h"
#include "WWhizInterface3.h"
#include "WWhizReg.h"
#include "Auto.h"

extern WWhizInterface* g_wwhizInterface;
extern WWhizReg* g_wwhizReg;
extern HINSTANCE g_wwhizInterfaceInstance;



