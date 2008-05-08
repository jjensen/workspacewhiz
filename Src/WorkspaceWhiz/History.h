///////////////////////////////////////////////////////////////////////////////
// $Workfile: History.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/History.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 6    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef HISTORY_H
#define HISTORY_H

class CHistoryDialog;

/**
**/
class History
{
	friend class WorkspaceInfo;
	friend class CHistoryDialog;
public:
	class Info
	{
	public:
		long m_x;
		long m_y;
		CString m_filename;
	};

	typedef CList<Info, Info&> Stack;

	static int GetCount();

	static void Reset();
	static bool PreAdd();
	static bool Add();
	static void UndoAdd();
	static bool Back();
	static bool Forward();
	static Info* Get();
	static Info* GetAt(int index);
	static bool Goto(Info* info = NULL);

protected:
	static History::Stack s_stack;
	static POSITION s_curPos;
};


#endif //HISTORY_H
