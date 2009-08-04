///////////////////////////////////////////////////////////////////////////////
// $Workfile: History.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/History.cpp $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "History.h"

History::Stack History::s_stack;
POSITION History::s_curPos;
static int s_maxStackSize = 100;

int History::GetCount()
{
	return s_stack.GetCount();
}

/**
**/
void History::Reset()
{
	s_stack.RemoveAll();
	s_curPos = NULL;
}


/**
	If there are no items in the history, then add the current document.
**/
bool History::PreAdd()
{
	return Add();
}


/**
**/
void History::UndoAdd()
{
	if (s_stack.GetCount() > 0)
	{
		s_stack.RemoveTail();
		s_curPos = s_stack.GetTailPosition();
	}
}


/**
	Adds the current text document and position to the history.
**/
bool History::Add()
{
	ObjModelHelper objModel;
	if (!objModel.GetActiveDocument())
		return false;
	if (!objModel.IsTextDocument())
		return false;

	Info info;
	info.m_filename = objModel.GetFilename();
	
	objModel.GetPosition(info.m_x, info.m_y);

	// Remove any ending stuff.
	while (s_stack.GetTailPosition() != s_curPos)
	{
		s_stack.RemoveTail();
	}
	
	// See if there are too many.
	if (s_stack.GetCount() == s_maxStackSize)
		s_stack.RemoveHead();

	// See if the new one is a duplicate of the current one.
	if (s_curPos)
	{
		Info& curInfo = s_stack.GetAt(s_curPos);
		if (curInfo.m_filename == info.m_filename  &&  curInfo.m_x == info.m_x  &&
			curInfo.m_y == info.m_y)
		{
			return false;
		}
	}

	// Insert the new one.
	s_curPos = s_stack.AddTail(info);

	return true;
}


/**
**/
bool History::Back()
{
	if (s_curPos != s_stack.GetHeadPosition())
	{
		s_stack.GetPrev(s_curPos);
		return true;
	}

	return false;
}


/**
**/
bool History::Forward()
{
	if (s_curPos != s_stack.GetTailPosition())
	{
		s_stack.GetNext(s_curPos);
		return true;
	}

	return false;
}


/**
**/
History::Info* History::Get()
{
	if (s_curPos)
	{
		return &s_stack.GetAt(s_curPos);
	}
	else
	{
		return NULL;
	}
}


/**
**/
History::Info* History::GetAt(int index)
{
	ASSERT(index < s_stack.GetCount());
	POSITION pos = s_stack.FindIndex(index);
	return &s_stack.GetAt(pos);
}


/**
**/
bool History::Goto(Info* info)
{
	// Get the current stack position.
	if (!info)
		info = Get();
	if (!info)
		return false;

	ObjModelHelper objModel;
	if (!objModel.OpenDocument(info->m_filename, "Auto"))
		return false;

	if (objModel.IsTextDocument())
	{
		objModel.MoveTo(info->m_y, info->m_x, false);
		return true;
	}

	return false;
}
