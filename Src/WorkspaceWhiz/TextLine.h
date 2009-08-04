///////////////////////////////////////////////////////////////////////////////
// $Workfile: TextLine.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/TextLine.h $
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

class TextLine
{
public:
	bool GetLine();
	CString GetWord(void);
	CString GetIdent(bool getExtraChars = false, bool lookRight = true, bool addExtraTags = true);

	CString m_line;
	long m_tabSize;
	long m_curX;						// The current document X
	long m_curY;						// The current document X
	long m_startX;						// The current identifier start X
	long m_endX;						// The current identifier end X

	ObjModelHelper m_objModel;
};
