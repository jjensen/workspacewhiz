///////////////////////////////////////////////////////////////////////////////
// $Workfile: RCBase.h $
// $Archive: /WorkspaceWhiz/Src/WWhizInterface/RCBase.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "pchWWhizInterface.h"

class WWhizRefVar
{
public:
	WWhizRefVar() : m_var(0)		{ }
	operator UINT&()				{  return m_var;  }

protected:
	UINT m_var;
};

#define WWHIZ_DECLARE_REFCOUNT() \
	public:\
		virtual UINT GetRefCount();\
		virtual UINT AddRef();\
		virtual UINT Release();\
	protected:\
		WWhizRefVar m_refCount;

#define WWHIZ_DEFINE_REFCOUNT(name) \
UINT name::GetRefCount() { return m_refCount; }\
UINT name::AddRef() { return ++m_refCount; }\
UINT name::Release() {\
	ASSERT(m_refCount != 0xdddddddd);\
	if (--m_refCount == 0)\
	{\
		delete this;\
		return 0;\
	}\
	return m_refCount;\
}

