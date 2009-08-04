///////////////////////////////////////////////////////////////////////////////
// $Workfile: pchWWhizReg.h $
// $Archive: /WWhizReg/pchWWhizReg.h $
// $Date:: 11/13/00 7:19p  $ $Revision:: 19   $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
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
#include <afxcmn.h>
#include <afxtempl.h>
#include <atlbase.h>
#include <afxmt.h>
#include <mmsystem.h>
#include "WCollection.h"

using namespace ATL;

#ifdef WWHIZ_VC6

// Developer Studio Object Model
#include <ObjModel\appauto.h>

#endif WWHIZ_VC6

// determine number of elements in an array (not bytes)
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif _countof

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ObjModelHelper.h"
#include "WWhizInterface3.h"
#include "WWhizReg.h"
#include "resource.h"

extern WWhizInterface* g_wwhizInterface;
extern HINSTANCE g_instance;

//#define USE_EXPIRATION

__forceinline bool HasExpiredHelper()
{
#ifdef USE_EXPIRATION
	CTime timeStart(2007, 5, 19, 00, 00, 00);
	CTime currentTime = CTime::GetCurrentTime();
	CTimeSpan time30Days(30, 0, 0, 0);

	if (timeStart + time30Days < currentTime)
		return true;
	return false;
#else
	return false;
#endif USE_EXPIRATION
}

__forceinline bool HasExpired()
{
#ifdef USE_EXPIRATION
	if (HasExpiredHelper())
	{
		AfxMessageBox("Workspace Whiz 4.0 Build 1115 has expired.  Please go to "
			"http://workspacewhiz.com/ to download the latest version.\n");

		return true;
	}

	return false;
#else
	return false;
#endif
}

#define WM_DOREG		(WM_APP + 1)
#define WM_FILLREG		(WM_APP + 2)

