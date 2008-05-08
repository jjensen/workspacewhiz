///////////////////////////////////////////////////////////////////////////////
// $Workfile: Template.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizTemplateManager/Template.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 23   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "pchWWhizTemplateManager.h"
#include "Template.h"
#include <io.h>
#include <ObjModel\addguid.h>
#include <ObjModel\appguid.h>
#include <ObjModel\bldguid.h>
#include <ObjModel\textguid.h>
#include <ObjModel\dbgguid.h>
#include <direct.h>
#include "TemplateGroup.h"
#include "TemplateManager.h"

extern TemplateManager g_templateManager;

typedef WWhiz_TemplateException TException;

static CString s_emptyString;

/**
**/
class TemplateCommandArgs : public WWhizTemplateCommandArgs
{
public:
	virtual int GetCount() const
	{
		return m_argList.GetSize();
	}

	virtual CString Get(int index) const
	{
		if (index >= m_argList.GetSize())
			return s_emptyString;
		else
			return m_argList.GetAt(index);
	}

	virtual void Return(const CString& returnValue)
	{
		m_returnValue = returnValue;
	}

	virtual void Return(int returnValue)
	{
		m_returnValue.Format("%d", returnValue);
	}

	CArrayEx<CString, CString&> m_argList;
	CString m_returnValue;
};


class TemplateHelper
{
public:
	ObjModelHelper m_objModel;

	LPCTSTR m_codeBolPtr;
	int m_outBolPos;

	CharArray m_buffer;

	// Used during the parse.
	LPCTSTR m_codePtr;
	long m_tabSize;

	Template* m_parent;

	CMapEx<WWhizTemplateCommand*, WWhizTemplateCommand*, bool, bool> m_commandsUsed;

	TemplateHelper() : m_parent(NULL) { }
	TemplateHelper(Template* parent);
	~TemplateHelper();
	void Refresh();
	WWhizTemplateCommand* GetCommand(CString command);
};

TemplateHelper::TemplateHelper(Template* parent) :
	m_parent(parent)
{
	Refresh();
}


TemplateHelper::~TemplateHelper()
{
	POSITION pos = m_commandsUsed.GetStartPosition();
	while (pos)
	{
		WWhizTemplateCommand* command;
		bool reserved;
		m_commandsUsed.GetNextAssoc(pos, command, reserved);
		command->Destroy(m_parent);
	}
}


void TemplateHelper::Refresh()
{
	m_objModel.GetActiveDocument();
	m_tabSize = m_objModel.GetTabSize();
}


WWhizTemplateCommand* TemplateHelper::GetCommand(CString command)
{
	WWhizTemplateCommand* commandObject = g_templateManager.GetCommand(command);
	if (!commandObject)
		return NULL;
	bool reserved;
	if (!m_commandsUsed.Lookup(commandObject, reserved))
	{
		// Not there yet.
		m_commandsUsed[commandObject] = true;
		commandObject->Create(m_parent);
	}

	return commandObject;
}


///////////////////////////////////////////////////////////////////////////////
/**
**/
Template::Template(TemplateGroup& templateGroup) :
	m_parent(templateGroup)
{
}


/**
**/
Template::~Template()
{
}


/**
	Parse a number token.
**/
static int ParseNumber(LPCTSTR& codePtr)
{
	TCHAR numberBuffer[80];
	TCHAR* numberPtr = numberBuffer;
	while (*codePtr  &&  isdigit(*codePtr))
	{
		*numberPtr++ = *codePtr++;
	}
	*numberPtr = 0;
	return atoi(numberBuffer);
}


class TokenHelper
{
public:
	long m_tabSize;
	LPCTSTR m_outBolPtr;
	WWhizTemplate* m_file;
	TemplateHelper* m_helper;
};


static bool ParseTag(LPCTSTR& codePtr, CString& out, TokenHelper* helper);

/**
**/
CString ParseToken(LPCTSTR& codePtr, TokenHelper* helper)
{
	// Build our master string.
	CharArray buffer;

	// Remove leading whitespace.
	while (*codePtr != 0  &&  *codePtr != '\n'  &&  *codePtr == ' ')
	{
		codePtr++;
	}
		
	// Get the parameter.
	bool inQuote = false;
	if (*codePtr == '"')
	{
		inQuote = true;
		codePtr++;
	}

	while (*codePtr != 0)
	{
		if (*codePtr == '\n')
		{
			if (inQuote)
			{
				CString err;
				err.Format("Unexpected end of line while parsing a quoted string.");
				throw TException(TException::UNEXPECTED_EOL, err);
			}
			break;
		}
		else if (*codePtr == '%')
		{
			switch (*(codePtr + 1))
			{
				case '"':
					buffer.Add('"');
					codePtr += 2;
					break;
				case '@':
					buffer.Add('@');
					codePtr += 2;
					break;
				case '%':
					buffer.Add('%');
					codePtr += 2;
					break;
			}
		}
		else if (*codePtr == '"')
		{
			if (inQuote)
			{
				codePtr++;
				break;
			}
			else
			{
				buffer.Add(*codePtr++);
			}
		}
		else if (*codePtr == ' ')
		{
			if (inQuote)
			{
				buffer.Add(*codePtr++);
			}
			else
			{
				codePtr++;
				break;
			}
		}
		else if (helper  &&  *codePtr == '@'  &&  *(codePtr + 1) == '@')
		{
			CString out;
			if (ParseTag(codePtr, out, helper))
			{
				// Move the output string into the buffer.
				for (int i = 0; i < out.GetLength(); i++)
					buffer.Add(out[i]);
			}
		}
		else
		{
			// Copy it straight.
			buffer.Add(*codePtr++);
		}
	}
	buffer.Add(0);

	return CString(buffer.GetData());
}


/**
**/
static bool ParseTag(LPCTSTR& codePtr, CString& out, TokenHelper* helper)
{
	// Increment past the @@.
	codePtr += 2;
	
	// Error detection.
	if (*codePtr == 0)
	{
		throw TException(TException::UNEXPECTED_EOF);
	}

	// Are we in a larger command?
	int numParanLevels = 0;
	if (*codePtr == '(')
		numParanLevels++;
	
	// Retrieve the command in the process.
	char commandString[512];
	char flags[256];
	flags[0] = 0;
	bool doingFlags = false;
	char *comPtr = commandString;
	bool foundDoubleAt = false;
	bool inQuote = false;
	while (*codePtr != 0)
	{
		// Is it the EOL?
		if (*codePtr == '\n')
		{
			CString err;
			err.Format("Unexpected end of line while parsing a @@ expression.");
			throw TException(TException::UNEXPECTED_EOL, err);
		}

		// Is it a quote?
		else if (*codePtr == '"')
		{
			inQuote ^= 1;
			*comPtr++ = *codePtr++;
		}

		// Is it a flag?
		else if (!inQuote  &&  *codePtr == ':')
		{
			if (!doingFlags)
			{
				// Increment past the :
				codePtr++;

				// Finish up the commandString.
				*comPtr = 0;

				// Start the flags.
				comPtr = flags;

				// Set to true so we can copy multiple colons.
				doingFlags = true;
			}
			else
			{
				*comPtr++ = *codePtr++;
			}
		}

		// Is it an opening @@(  .... only do if we opened with one.
		else if (numParanLevels > 0  &&  *codePtr == '@'  &&
				*(codePtr + 1) == '@'  &&  *(codePtr + 2) == '(')
		{
			// Increment past the @@(
			*comPtr++ = *codePtr++;
			*comPtr++ = *codePtr++;
			*comPtr++ = *codePtr++;
			numParanLevels++;
		}

		// Is it the closing )@@.
		else if (numParanLevels > 0  &&  *codePtr == ')'  &&
				*(codePtr + 1) == '@'  &&  *(codePtr + 2) == '@')
		{
			// Increment past the )@@.
			if (numParanLevels > 1)
			{
				*comPtr++ = *codePtr++;
				*comPtr++ = *codePtr++;
				*comPtr++ = *codePtr++;
			}
			else
				codePtr++;		// Just skip the )... let the @@ code below pick it up.
			numParanLevels--;
		}

		// Is it the closing @@.
		else if (numParanLevels == 0  &&  *codePtr == '@'  &&  *(codePtr + 1) == '@')
		{
			// Increment past the @@.
			codePtr += 2;
			foundDoubleAt = true;
			break;
		}
		else
		{
			// Copy the commandString or flag.
			*comPtr++ = *codePtr++;
		}
	}

	if (!foundDoubleAt)
	{
		CString err;
		err.Format("Unexpected end of line or file while parsing a double @@ expression.");
		throw TException(TException::UNEXPECTED_EOL, err);
	}
	
	*comPtr = 0;

	// Is it a parameter?
	if (commandString[0] == '!'  &&  commandString[1] == '!')
	{
		CString err;
		err.Format("Old style variable expression found.  Search for @@!! in your code.  Replace with @@( and )@@ at the end.");
		throw TException(TException::OLD_STYLE_EXPRESSION, err);
	}
	else if (commandString[0] != '(')
	{
		if (helper->m_file)
			out = helper->m_file->GetEntry(commandString);
	}
	else
	{
		comPtr = commandString + 1;
		bool noReturnValue = false;
		if (commandString[1] == '!'  &&  commandString[2] == '!')
		{
			noReturnValue = true;
			comPtr += 2;
		}

		CString command = ParseToken(comPtr);
		command.MakeLower();

		///////////////////////////////////////////////////////////////////
		// Look up the command in the map.
		///////////////////////////////////////////////////////////////////
		WWhizTemplateCommand* commandObject = helper->m_helper->GetCommand(command);
		if (commandObject)
		{
			commandObject->Create(helper->m_file);

			TemplateCommandArgs args;

			// Found a command.  Build the arg list.
			while (true)
			{
				CString arg = ParseToken(comPtr, helper);
				if (arg.IsEmpty())
					break;
				args.m_argList.Add(arg);
			}

			// Run the command.
			commandObject->Run(helper->m_file, args);
			helper->m_helper->Refresh();

//			commandObject->Destroy(helper->m_file);

			if (!noReturnValue)
				out = args.m_returnValue;
		}
		else
		{
			CString err;
			err.Format("Unable to find the command [%s].", command);
			if (AfxMessageBox(err + "\n\nKeep executing?", MB_YESNO) == IDNO)
			{
				throw TException(TException::MISSING_COMMAND, err);
			}
		}
	}

	// Okay, now format the output string.
	char* flagPtr = flags;
	while (*flagPtr)
	{
		// Uppercase?
		if (*flagPtr == 'U')
		{
			out.MakeUpper();
			flagPtr++;
		}

		// Lowercase?
		else if (*flagPtr == 'L')
		{
			out.MakeLower();
			flagPtr++;
		}

		// Left justified?
		else if (*flagPtr == 'l')
		{
			flagPtr++;

			// Get the number.
			int amount = ParseNumber(flagPtr);
			if (amount < out.GetLength())
			{
				out = out.Left(amount);
			}
			else
			{
				int addSpaces = amount - out.GetLength();
				if (addSpaces > 0)
				{
					CString spaceStr = CString(' ', addSpaces);
					out = out + spaceStr;
				}
			}
		}

		// Right justified?
		else if (*flagPtr == 'r')
		{
			flagPtr++;

			// Get the number.
			int amount = ParseNumber(flagPtr);
			if (amount < out.GetLength())
			{
				out = out.Right(amount);
			}
			else
			{
				int addSpaces = amount - out.GetLength();
				if (addSpaces > 0)
				{
					CString spaceStr = CString(' ', addSpaces);
					out = spaceStr + out;
				}
			}
		}

		// Align to specific column?
		else if (*flagPtr == 'c')
		{
			flagPtr++;

			// Get the column number.
			int toCol = ParseNumber(flagPtr);

			// Count the column position we're at right now.
			int curCol = 1;
			if (helper->m_outBolPtr)
			{
				LPCTSTR ptr = helper->m_outBolPtr;
				while (*ptr)
				{
					if (*ptr++ == '\t')
						curCol += helper->m_tabSize - (curCol % helper->m_tabSize);
					else
						curCol++;
				}
			}

			// Okay, now we have the current column.  Figure out how many spaces
			// to insert.
			CString spaces;
			int numSpacesToInsert = toCol - curCol;
			if (numSpacesToInsert <= 0)
				continue;

			// Tack on leading spaces, if any.
			int tabSize = 4;
			if (helper)
				tabSize = helper->m_tabSize;
			int nextTabPos = 1 + (curCol + tabSize - 1) / tabSize * tabSize;
			if (nextTabPos != curCol)
			{
				// Insert spaces.
				int numLeadingSpaces = nextTabPos - curCol;
				spaces += CString(' ', numLeadingSpaces);
				numSpacesToInsert -= numLeadingSpaces;
			}
			
			// Insert tabs.
			int numTabsToInsert = numSpacesToInsert / tabSize;
			if (numTabsToInsert > 0)
			{
				spaces += CString('\t', numTabsToInsert);
				numSpacesToInsert -= numTabsToInsert * tabSize;
			}

			// Insert trailing spaces.
			if (numSpacesToInsert > 0)
			{
				spaces += CString(' ', numSpacesToInsert);
			}

			out = spaces + out;
		}

		else
		{
			// Unknown.  Skip the character.
			flagPtr++;
		}
	}

	return true;
}


/**
**/
CString Template::ParseCode(LPCTSTR codePtr, LPCTSTR bolPtr, WWhizTemplate* file)
{
	// Build our master string.
	CharArray buffer;

	// Remove leading whitespace.
	while (*codePtr != 0  &&  *codePtr != '\n'  &&  *codePtr == ' ')
	{
		codePtr++;
	}
		
	TokenHelper helper;
	helper.m_tabSize = 4;
	helper.m_outBolPtr = bolPtr;
	helper.m_file = file;
	helper.m_helper = new TemplateHelper(NULL);
	
	// Get the parameter.
	while (*codePtr != 0)
	{
		if (*codePtr == '@'  &&  *(codePtr + 1) == '@')
		{
			CString out;
			if (ParseTag(codePtr, out, &helper))
			{
				// Move the output string into the buffer.
				for (int i = 0; i < out.GetLength(); i++)
					buffer.Add(out[i]);
			}
		}
		else
		{
			// Copy it straight.
			buffer.Add(*codePtr++);
		}
	}
	buffer.Add(0);

	delete helper.m_helper;
	
	return CString(buffer.GetData());
}


/**
**/
void Template::SkipToEol(LPCTSTR& codePtr, CharArray* buffer)
{
	// Remove leading whitespace.
	while (*codePtr != 0)
	{
		if (*codePtr == '\n')
		{
			codePtr++;
			if (buffer)
				m_helper->m_outBolPos = buffer->GetSize() + 1;
			m_helper->m_codeBolPtr = codePtr;
			break;
		}
		codePtr++;
	}
}


void Template::SetCodePtr(LPCTSTR codePtr)
{
	m_helper->m_codePtr = codePtr;
	m_helper->m_codeBolPtr = codePtr;
}


class QuickAuto
{
public:
	QuickAuto() : m_type(NULL) { }
	
	~QuickAuto()
	{
		delete m_type;
	}

	TemplateHelper* m_type;
};


/**
**/
bool Template::Parse()
{
	Auto<TemplateHelper> autoTemplateHelper;
	autoTemplateHelper = new TemplateHelper(this);
	m_helper = autoTemplateHelper;
	
	try
	{
		return ParseHelper();
	}
	catch(TException e)
	{
		int lineNumber = -1;
		DWORD offset = m_helper->m_codePtr - (LPCTSTR)m_code;

		if (m_codeOffsets.GetSize() > 1)
		{
			OffsetInfo* info = NULL;
			for (int i = 0; i < m_codeOffsets.GetSize() - 1; i++)
			{
				info = &m_codeOffsets[i];
				OffsetInfo& info2 = m_codeOffsets[i + 1];
				if (info->m_offset <= offset  &&  info2.m_offset >= offset)
					break;
			}
			if (info)
				lineNumber = m_codeOffsets[i].m_line;
		}
		else if (m_codeOffsets.GetSize() == 1)
		{
			lineNumber = m_codeOffsets[0].m_line;
		}

		CString msg;
		msg.Format("Error %d occurred while processing the template on line %d.\n\n%s\n\n"
			"Open %s to offending line number?",
			e.GetErrorNum(), lineNumber, e.GetErrorMsg(), m_parent.GetFilename());
		if (AfxMessageBox(msg, MB_YESNO) == IDYES)
		{
			ObjModelHelper objModel;
			if (objModel.OpenDocument(m_parent.GetFilename(), "Auto"))
			{
				objModel.PutLanguage(DS_CPP);

				objModel.MoveTo(lineNumber, 1, dsMove);
			}
		}

		return false;
	}
}


static bool AsciiToInteger(LPCTSTR nptr, int& total)
{
	int c;              // current char
	int sign;           // if '-', then negative, otherwise positive
	
	c = (int)(BYTE)*nptr++;
	sign = c;           /* save sign indication */
	if (c == '-' || c == '+')
		c = (int)(BYTE)*nptr++;    /* skip sign */
	
	total = 0;
	
	while (isdigit(c))
	{
		total = 10 * total + (c - '0');     /* accumulate digit */
		c = (int)(BYTE)*nptr++;    /* get next char */
	}

	if (c == 0)
	{
		if (sign == '-')
			total = -total;
		return true;
	}

	return false;
}


static bool CheckCondition(const CString& value1, const CString& condSymbol, const CString& value2)
{
	// Try converting to integers.
	int value1Int;
	int value2Int;
	if (AsciiToInteger(value1, value1Int)  &&  AsciiToInteger(value2, value2Int))
	{
		if (condSymbol == "==")
			return value1Int == value2Int;
		else if (condSymbol == "!=")
			return value1Int != value2Int;
		else if (condSymbol == ">=")
			return value1Int >= value2Int;
		else if (condSymbol == "<=")
			return value1Int <= value2Int;
		else if (condSymbol == ">")
			return value1Int > value2Int;
		else if (condSymbol == "<")
			return value1Int < value2Int;
	}
	else
	{
		if (condSymbol == "==")
			return _tcscmp(value1, value2) == 0;
		else if (condSymbol == "!=")
			return _tcscmp(value1, value2) != 0;
		else if (condSymbol == ">=")
			return _tcscmp(value1, value2) >= 0;
		else if (condSymbol == "<=")
			return _tcscmp(value1, value2) <= 0;
		else if (condSymbol == ">")
			return _tcscmp(value1, value2) > 0;
		else if (condSymbol == "<")
			return _tcscmp(value1, value2) < 0;
	}

	CString err;
	err.Format("The conditional check type is invalid.");
	throw TException(TException::IF_COND_INVALID, err);
}


struct CondInfo
{
	bool m_curState:1;
	bool m_thisLevelTrue:1;			// True if this level has been true already.

	CondInfo() : m_curState(false), m_thisLevelTrue(false) { }
};


/**
**/
bool Template::ParseHelper()
{
	// Build our master string.
	m_helper->m_buffer.SetSize(0, 1024);
	m_helper->m_outBolPos = 0;

	// Start copying the string in.
	m_helper->m_codePtr = (LPCTSTR)m_code;
	m_helper->m_codeBolPtr = m_helper->m_codePtr;
	LPCTSTR& codePtr = m_helper->m_codePtr;
	CharArray& buffer = m_helper->m_buffer;

	// Conditional stack.
	CList<CondInfo, CondInfo&> conditionalStack;
	int nestedFalseConditionals = 0;

	// Go 'til the end.
	while (*codePtr != 0)
	{
		LPCTSTR outBolPtr = buffer.GetData() + m_helper->m_outBolPos;

		TokenHelper helper;
		helper.m_tabSize = m_helper->m_tabSize;
		helper.m_outBolPtr = outBolPtr;
		helper.m_file = this;
		helper.m_helper = m_helper;

		// See if it is a command.  Commands can only occur at the beginning
		// of a line.
		if (*codePtr == '!'  &&  *(codePtr + 1) == '!'  &&  codePtr == m_helper->m_codeBolPtr)
		{
			// Move past the exclamation points.
			codePtr += 2;

			// Is it a command comment?
			// !!// This is a comment.
			if (*codePtr == '/'  &&  *(codePtr + 1) == '/')
			{
				// Move past the double slash.
				codePtr += 2;
				
				SkipToEol(codePtr);
				
				continue;
			}

			///////////////////////////////////////////////////////////////////
			// Lex the command token.
			char command[256];
			char *comPtr = command;
			while (*codePtr != 0)
			{
				// Is it a space?
				if (*codePtr == ' ')
				{
					// Break at a space.
					codePtr++;
					break;
				}
				else if (*codePtr == '\n'  ||  *codePtr == '\r')
				{
					break;
				}
				else
				{
					// Copy the command or flag.
					*comPtr++ = *codePtr++;
				}
			}
			*comPtr = 0;

			///////////////////////////////////////////////////////////////////
			// "Conditional" commands
			///////////////////////////////////////////////////////////////////
			// If the last conditional was false, then we ignore until the next
			// endif.
			if (conditionalStack.GetCount() > 0  &&
				conditionalStack.GetTail().m_curState == false)
			{
				CondInfo condInfo = conditionalStack.GetTail();
				if (stricmp(command, "if") == 0)
				{
					SkipToEol(codePtr);
					nestedFalseConditionals++;
					continue;
				}
				else if (stricmp(command, "elif") == 0  ||  stricmp(command, "else") == 0)
				{
					// If we're in nested false conditionals, then ignore.
					if (nestedFalseConditionals > 0)
					{
						SkipToEol(codePtr);
						continue;
					}
				}
				else if (stricmp(command, "endif") == 0)
				{
					// If we're in nested false conditionals, then ignore.
					if (nestedFalseConditionals > 0)
					{
						SkipToEol(codePtr);
						nestedFalseConditionals--;
						continue;
					}
				}
			}

			if (stricmp(command, "if") == 0  ||
				stricmp(command, "elif") == 0)
			{
				// Our CondInfo structure.
				CondInfo condInfo;
				
				// If the command is an elif, then pop the old state.
				if (tolower(command[0]) == 'e')
				{
					// The conditional stack can't be empty on an elif.
					if (conditionalStack.IsEmpty())
					{
						CString err;
						err.Format("!!elif used with no preceding !!if.");
						throw TException(TException::CONDSTACK_EMPTY, err);
					}

					// Get the current conditionalStack state.  We are most 
					// interested in retaining the m_thisLevelTrue variable.
					condInfo = conditionalStack.GetTail();
					
					// Remove the old state, because it was false.
					conditionalStack.RemoveTail();
				}
				
				// if and elif are setup in this format:
				// !!if var [cond] [condCheck]
				// condCheck is only optional if cond doesn't exist.
				// var is required.
				// Get the requested dictionary entry name.
				CString var = ParseToken(codePtr, &helper);
/*				if (var.IsEmpty())
				{
					CString err;
					err.Format("The variable is missing.");
					throw TException(TException::IF_VAR_MISSING, err);
				}
*/
				CString cond = ParseToken(codePtr, &helper);
				CString condCheck = ParseToken(codePtr, &helper);
/*				if (!cond.IsEmpty()  &&  condCheck.IsEmpty())
				{
					CString err;
					err.Format("The conditional is missing.");
					throw TException(TException::IF_COND_CHECKVAR_MISSING, err);
				}
*/
				// Check for a ! symbol.
				bool isNot = false;
				if (!var.IsEmpty()  &&  var[0] == '!')
				{
					isNot = true;
					var = var.Mid(1);
				}
				
				// Lookup the value.
				CString value = var;
				SkipToEol(codePtr);

				bool result;
				if (cond.IsEmpty())
				{
					if (value != "0")
						result = true;
					else
						result = false;
				}
				else
				{
					// There is a conditional expression.  Do it.
					result = CheckCondition(value, cond, condCheck);
				}
				if (isNot)
					result ^= 1;
				if (tolower(command[0]) == 'e'  &&  result)
					result ^= condInfo.m_thisLevelTrue;
				condInfo.m_curState = result;
				condInfo.m_thisLevelTrue |= result;		// Retain the old state.
				conditionalStack.AddTail(condInfo);
				continue;
			}

			///////////////////////////////////////////////////////////////////
			else if (stricmp(command, "else") == 0)
			{
				SkipToEol(codePtr);

				if (conditionalStack.IsEmpty())
				{
					CString err;
					err.Format("!!else used with no preceding !!if.");
					throw TException(TException::CONDSTACK_EMPTY, err);
				}

				// Get the current conditionalStack state.  We are most 
				// interested in retaining the m_thisLevelTrue variable.
				CondInfo condInfo = conditionalStack.GetTail();
				
				conditionalStack.RemoveTail();
				condInfo.m_curState = condInfo.m_thisLevelTrue ^ 1;
				conditionalStack.AddTail(condInfo);
				continue;
			}
			else if (stricmp(command, "endif") == 0)
			{
				SkipToEol(codePtr);

				if (conditionalStack.IsEmpty())
				{
					CString err;
					err.Format("!!endif used with no preceding !!if.");
					throw TException(TException::CONDSTACK_EMPTY, err);
				}

				conditionalStack.RemoveTail();

				continue;
			}
			
			if (conditionalStack.GetCount() > 0  &&
				conditionalStack.GetTail().m_curState == false)
			{
				continue;
			}

			///////////////////////////////////////////////////////////////////
			// Look up the command in the map.
			///////////////////////////////////////////////////////////////////
			WWhizTemplateCommand* commandObject = m_helper->GetCommand(command);
			if (commandObject)
			{
				TemplateCommandArgs args;

				// Found a command.  Build the arg list.
				while (true)
				{
					CString arg = ParseToken(codePtr, &helper);
					if (arg.IsEmpty())
						break;
					args.m_argList.Add(arg);
				}

				// Skip anything else.
				SkipToEol(codePtr);

				// Run the command.
				commandObject->Run(this, args);
				RefreshFileInfo();

				// Parse the next part.
				continue;
			}
			else
			{
				CString err;
				err.Format("Unable to find the command [%s].", command);
				if (AfxMessageBox(err + "\n\nKeep executing?", MB_YESNO) == IDNO)
				{
					throw TException(TException::MISSING_COMMAND, err);
				}
			}
		}

		///////////////////////////////////////////////////////////////////////
		else if (*codePtr == '@'  &&  *(codePtr + 1) == '@')
		{
			// If we're ignoring text, then skip.
			if (conditionalStack.GetCount() > 0  &&  conditionalStack.GetTail().m_curState == false)
			{
				codePtr += 2;
				continue;
			}
			
			// Is it a line continuation?
			// @@backslash
			if (*(codePtr + 2) == '\\')
			{
				SkipToEol(codePtr);
				continue;
			}

			CString out;

			// Ugly, but we need that \0 in there.
			buffer.Add(0);
			buffer.RemoveAt(buffer.GetSize() - 1);

			if (ParseTag(codePtr, out, &helper))
			{
				// Move the output string into the buffer.
				for (int i = 0; i < out.GetLength(); i++)
					buffer.Add(out[i]);
			}
		}
		else
		{
			// See if it is an EOL.
			if (*codePtr == '\n')
			{
				m_helper->m_outBolPos = buffer.GetSize() + 1;
				m_helper->m_codeBolPtr = codePtr + 1;
			}

			// If we're ignoring text, then skip.
			if (conditionalStack.GetCount() > 0  &&  conditionalStack.GetTail().m_curState == false)
			{
				codePtr++;
				continue;
			}
			
			// Copy it straight.
			buffer.Add(*codePtr++);
		}
	}

	// Flush what's left.
	FlushText();

	buffer.RemoveAll();

	return true;
}


void Template::FlushText()
{
	m_helper->m_buffer.Add(0);
	CString str = m_helper->m_buffer.GetData();

	// Only put the text if there is a current document.
	if (m_helper->m_objModel.IsTextDocument()  &&  !str.IsEmpty())
		m_helper->m_objModel.PutText(str);

	m_helper->m_buffer.SetSize(0, 1024);
}

	
/**
**/
void Template::ResetDictionary()
{
	m_dictionary.RemoveAll();

	ResetDictionaryDefaults();
}


/**
**/
void Template::ResetDictionaryDefaults()
{
	POSITION pos = m_defaults.GetHeadPosition();
	while (pos)
	{
		DefaultInfo& info = m_defaults.GetNext(pos);

		SetEntry(info.m_entry, ParseCode(info.m_value, NULL, this));
	}
}


/**
**/
void Template::SetEntry(const CString& entry, CString value)
{
	m_dictionary[entry] = value;
}



WWhizTemplateCommandArgs* Template::CreateArgs()
{
	return new TemplateCommandArgs;
}


void Template::DestroyArgs(WWhizTemplateCommandArgs* args)
{
	delete args;
}

	
void Template::RefreshFileInfo()
{
	m_helper->Refresh();
}

	
/**
**/
WWhizTemplateDialog* Template::GetTemplateDialog() const
{
	return m_parent.m_manager.m_templateDialog;
}

	
