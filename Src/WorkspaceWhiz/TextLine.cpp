///////////////////////////////////////////////////////////////////////////////
// $Workfile: TextLine.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/TextLine.cpp $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "TextLine.h"

bool TextLine::GetLine()
{
	// Start with nothing in the initial line.
	m_line = "";

	// Get the active document.
	if (!m_objModel.GetActiveDocument())
	{
		// If there is no active document, then return an empty search string.
		return false;
	}

	// If this isn't a text document, then abort.
	if (!m_objModel.IsTextDocument())
	{
		return false;
	}

	// Get the document's tab size.
	m_tabSize = m_objModel.GetTabSize();

	// Save the last coordinate.
	m_objModel.GetPosition(m_curX, m_curY);

	// Select the current line.
	m_objModel.SelectLine();

	// Copy the current line into a local buffer.
	CString strLine = m_objModel.GetText();

	// Get the untabified line size.
	int newLineWidth = 0;
	for (int i = 0; i < strLine.GetLength(); i++)
	{
		if (strLine[i] == '\t')
			newLineWidth += m_tabSize - (newLineWidth % m_tabSize);
		else
			newLineWidth++;
	}

	// Untabify line.
	LPTSTR buf = m_line.GetBufferSetLength(newLineWidth);
	int curWidth = 0;
	for (i = 0; i < strLine.GetLength(); i++)
	{
		if (strLine[i] == '\t')
		{
			int spaceCount = m_tabSize - (curWidth % m_tabSize);
			for (int j = 0; j < spaceCount; j++)
				*buf++ = ' ';
			curWidth += spaceCount;
		}
		else
		{
			*buf++ = strLine[i];
			curWidth++;
		}
	}
	m_line.ReleaseBuffer(newLineWidth);

	m_objModel.MoveTo(m_curY, m_curX, false);

	return true;
}


CString TextLine::GetWord(void)
{
	if (!GetLine())
		return "";

	CString word;
	CString& line = m_line;
	if (m_curX <= line.GetLength())
	{
		// Okay, we have an untabified line now.
		// Try and find a word.
		int leftX = m_curX - 1;
		while (leftX - 1 >= 0  &&  (isalnum(line[leftX - 1])  ||  line[leftX - 1] == '_'))
			leftX--;

		int rightX = m_curX - 1;
		while (rightX < line.GetLength()  &&  (isalnum(line[rightX])  ||  line[rightX] == '_'))
			rightX++;

		word = line.Mid(leftX, rightX - leftX);
		m_startX = leftX;
		m_endX = rightX;
	}
	else
	{
		m_startX = m_curX;
		m_endX = m_curX;
	}

	return word;
}

CString TextLine::GetIdent(bool getExtraChars, bool lookRight, bool addExtraTags)
{
	if (!GetLine())
		return "";

	// Pick out the word.
	bool isFunction = false;
	CString word;
	CString parent;
	int leftX;
	int rightX;
	CString& line = m_line;
	if (line.IsEmpty())
		return "";

	// If the starting X coordinate is outside of the line range (that is,
	// Virtual Space is turned on), then exit.
	if (m_curX > line.GetLength() + 1)
	{
		m_startX = m_curX;
		m_endX = m_curX;
		goto Done;
	}

	// Try and find a word.
	leftX = m_curX - 1;

	// Check for a space.
//	if (leftX - 1 < line.GetLength()  &&  line[leftX - 1] == ' ')
//		goto Done; 
	
	while (leftX - 1 >= 0  &&  (isalnum(line[leftX - 1])  ||  line[leftX - 1] == '_'  ||
			(getExtraChars  &&  (line[leftX - 1] == '~'  ||  line[leftX - 1] == '/'))))
		leftX--;

	rightX = m_curX - 1;
	if (lookRight)
	{
		while (rightX < line.GetLength()  &&  (isalnum(line[rightX])  ||  line[rightX] == '_'  ||
				(getExtraChars  &&  (line[rightX] == '~'  ||  line[rightX] == '/'))))
			rightX++;
	}

	if (addExtraTags)
	{
		// Try and find a paranthesis...
		int paranRightX = rightX;
		while (paranRightX < line.GetLength())
		{
			if (line[paranRightX] == '(')
			{
				isFunction = true;
				break;
			}

			paranRightX++;
		}
	}

	word = line.Mid(leftX, rightX - leftX);
	m_startX = leftX;
	m_endX = rightX;

	// Continue from the left side and see if there is a parent qualifier.
	//   parent::ident
	if (leftX - 2 >= 0  &&  line[leftX - 2] == ':'  &&  line[leftX - 1] == ':')
	{
		rightX = leftX - 2;
		leftX = rightX;

		while (leftX > 0)
		{
			leftX--;
			TCHAR ch = line[leftX];

			// Is it another ::  ???
			if (ch == ':')
			{
				if (leftX > 1  &&  line[leftX - 1] == ':')
				{
					leftX--;
					continue;
				}
				else
				{
					leftX++;
					break;
				}
			}
			else if (!isalnum(ch)  &&  ch != '_')
			{
				leftX++;
				break;
			}
		}
		parent = line.Mid(leftX, rightX - leftX);
	}
	
	// See if it is a -> or .
/*	else if ((leftX - 2 >= 0  &&  line[leftX - 2] == '-'  &&  line[leftX - 1] == '>')  ||
		(leftX - 1 >= 0  &&  line[leftX - 1] == '.'))
	{
		if (line[leftX - 1] == '>')
			rightX = leftX - 2;
		else
			rightX = leftX - 1;

		leftX = rightX;

		while (leftX > 0)
		{
			leftX--;
			TCHAR ch = line[leftX];

			// Is it another ::  ???
			if (!isalnum(ch)  &&  ch != '_')
			{
				leftX++;
				break;
			}
		}

		parent = line.Mid(leftX, rightX - leftX);

		if (!parent.IsEmpty())
			return word + ";!=" + parent;
		else
			return word;
	}
*/
Done:
	CString extra;
	if (addExtraTags)
	{
//		if (isFunction)
//			extra = "'f";
	}
	if (!parent.IsEmpty())
		return extra + word + ";" + parent;
	else
		return extra + word;
}


