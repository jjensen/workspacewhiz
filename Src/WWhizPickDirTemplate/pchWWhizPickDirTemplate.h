///////////////////////////////////////////////////////////////////////////////
// $Workfile: pchWWhizPickDirTemplate.h $
// $Archive: /WorkspaceWhiz/Src/WWhizPickDirTemplate/pchWWhizPickDirTemplate.h $
// $Date: 2003/06/23 $ $Revision: #8 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef WINVER
#define WINVER 0x0400
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

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

#ifdef WWHIZ_VSNET

#pragma warning( disable : 4278 )
#pragma warning( disable : 4146 )
	//The following #import imports the IDTExtensibility2 interface based on it's LIBID
	#import "libid:AC0714F2-3D04-11D1-AE7D-00A0C90F26F4" version("1.0") lcid("0")  raw_interfaces_only named_guids

	//The following #import imports VS Command Bars based on it's LIBID
	#import "libid:1CBA492E-7263-47BB-87FE-639000619B15" version("8.0") lcid("0") raw_interfaces_only named_guids

	//The following #import imports DTE based on it's LIBID
	#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("7.0") lcid("0") raw_interfaces_only named_guids

	//The following #import imports DTE80 based on it's LIBID
	#import "libid:1A31287A-4D7D-413e-8E32-3B374931BD89" version("8.0") lcid("0") raw_interfaces_only named_guids
#pragma warning( default : 4146 )
#pragma warning( default : 4278 )

#endif WWHIZ_VSNET

// determine number of elements in an array (not bytes)
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif _countof

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ObjModelHelper.h"
#include "Auto.h"
#include "MemFile.h"
#include "WWhizTemplateManager.h"
#include "WWhizInterface3.h"
#include "WWhizReg.h"

extern WWhizTemplateManager* g_templateManager;
extern WWhizInterface* g_wwhizInterface;
extern WWhizReg* g_wwhizReg;




