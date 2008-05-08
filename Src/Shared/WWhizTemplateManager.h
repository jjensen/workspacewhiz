///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizTemplateManager.h $
// $Archive: /WorkspaceWhiz/Src/Shared/WWhizTemplateManager.h $
// $Date: 2003/01/30 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

/**
	In case of a fatal error, this exception is thrown.

	\code
	CString err;
	err.Format("Unable to query the new file.");
	throw WWhiz_TemplateException(TException::UNABLE_TO_CREATE_NEW_FILE, err);
	\endcode
**/
class WWhiz_TemplateException
{
public:
	enum Types
	{
		UNEXPECTED_EOL,
		UNEXPECTED_EOF,
		CONDSTACK_EMPTY,
		IF_VAR_MISSING,
		IF_COND_INVALID,
		IF_COND_CHECKVAR_MISSING,
		MISSING_VARIABLE,
		MISSING_COMMAND,
		FILENAME_REQUIRED,
		PROJECT_NAME_REQUIRED,
		PATH_REQUIRED,
		PROJECT_TYPE_REQUIRED,
		UNABLE_TO_CREATE_NEW_FILE,
		NO_ACTIVE_FILE,
		NO_ACTIVE_PROJECT,
		UNABLE_TO_SAVE_FILE,
		UNABLE_TO_ADD_FILE_TO_PROJECT,
		UNABLE_TO_ADD_NEW_PROJECT,
		UNABLE_TO_EXECUTE_COMMAND,
		MISSING_FIND_STRING,
		INVALID_FIND_FLAG,
		FIND_FAILED,
		PATH_STATUS_INVALID,
		INVALID_TEMPLATE_NAME,
		INVALID_PROJECT_NAME,
		INVALID_PARAMETER,
		INVALID_CONFIG_NAME,
		CONFIG_NAME_REQUIRED,
		NO_ACTIVE_CONFIG,
		OLD_STYLE_EXPRESSION,
		VALUE_OUT_OF_RANGE,
		GENERAL_FAILURE,
		USER_DEFINED = 1000,
	};

	WWhiz_TemplateException(int errorNum)
	{
		m_errorNum = errorNum;
	}

	WWhiz_TemplateException(int errorNum, LPCTSTR msg, ...)
	{
		va_list args;
		char textBuffer[1024];

		va_start(args, msg);
		_vsnprintf(textBuffer, 1023, msg, args);
		va_end(args);

		m_errorNum = errorNum;
		m_errorMsg = textBuffer;
	}

	int GetErrorNum() const
	{
		return m_errorNum;
	}

	const CString& GetErrorMsg() const
	{
		return m_errorMsg;
	}

protected:
	int m_errorNum;
	CString m_errorMsg;
};


class WWhizTemplateGroup;
class WWhizTemplateCommandArgs;
class WWhizTemplateDialog;


/**
	A code template.
**/
class WWhizTemplate
{
public:
	/**
		@return Retrieves the WWhizTemplateGroup this template belongs to.
	**/
	virtual WWhizTemplateGroup& GetParent() const = 0;

	/**
		@return Retrieves the name of the template.
	**/
	virtual const CString& GetName() const = 0;

	/**
		@return Retrieves the code block of the template.
	**/
	virtual const CString& GetCode() const = 0;

	/**
		@return Retrieves the short memo describing the template.
	**/
	virtual const CString& GetMemo() const = 0;

	/**
		@param page The HTML dialog page to retrieve.
		@return Returns the HTML dialog page specified by [page].
	**/
	virtual const CString& GetPage(int page) = 0;

	/**
		@return Retrieves the number of HTML dialog pages associated with this
			template.
	**/
	virtual int GetPageCount() const = 0;


	/**
		Parse the code associated with this template.  This function may only
		be called once for a given template pointer.

		@return Returns true if the code template was fully parsed, false
			otherwise.
	**/
	virtual bool Parse() = 0;

	/**
		Text is queued up for output to DevStudio.  If a command is to be
		executed that requires the text to be flushed (such as a file save),
		use FlushText() to output all the text to the current document in
		DevStudio.
	**/
	virtual void FlushText() = 0;

	/**
		Call RefreshFileInfo() after manually loading a new source file, so
		the internal template pointers can be updated.
	**/
	virtual void RefreshFileInfo() = 0;

	/**
		Sets a new code pointer position.  Use with great caution.

		@param codePtr The new code location to parse from.  Should be set to
			an end of line in order to properly update the internal template
			pointers.
	**/
	virtual void SetCodePtr(LPCTSTR codePtr) = 0;


	/**
		If a WWhizTemplateCommand needs to call another WWhizTemplateCommand,
		it is necessary to call CreateArgs() to create a
		WWhizTemplateCommandArgs object for storing the argument list.

		@return Returns a WWhizTemplateCommandArgs object.  The caller is
			responsible for its destruction through the DestroyArgs()
			function.
	**/
	virtual WWhizTemplateCommandArgs* CreateArgs() = 0;

	/**
		Destroys an argument list created by CreateArgs().

		@param args The argument list to destroy.
	**/
	virtual void DestroyArgs(WWhizTemplateCommandArgs* args) = 0;


	/**
		Sets a dictionary entry (a variable) to a value.

		@param dicEntry The entry to set.
		@param value The value to assign to the entry.
	**/
	virtual void SetEntry(const CString& entry, CString value) = 0;

	/**
		Retrieves a dictionary entry (a variable).
		
		@param entry The entry to retrieve.
		@return Returns the value at [entry].
	**/
	virtual CString GetEntry(const CString& entry) const = 0;


	/**
		Clears the dictionary and resets the default parameters.
	**/
	virtual void ResetDictionary() = 0;

	/**
		Resets the default parameters.
	**/
	virtual void ResetDictionaryDefaults() = 0;

	/**
		@return Returns the autocompletion phrase.
	**/
	virtual const CString& GetKey() const = 0;

	/**
		@return Returns the start line number of the template.
	**/
	virtual int GetLineNumber() const = 0;

	/**
		@return Returns the current template wizard dialog.
	**/
	virtual WWhizTemplateDialog* GetTemplateDialog() const = 0;
};


/**
**/
class WWhizTemplateDialog
{
public:
	virtual CString Run(WWhizTemplate& code, const CString& page, bool nextOn,
			bool previousOn, bool finishOn) = 0;
};


/**
	The argument list passed to a template command.
**/
class WWhizTemplateCommandArgs
{
public:
	/**
		@return Returns the count of arguments.
	**/
	virtual int GetCount() const = 0;

	/**
		@param index The index of the argument to retrieve.
		@return Returns the argument at [index].
	**/
	virtual CString Get(int index) const = 0;

	/**
		Sets the return value for the template command.  Only useful in a
		template variable command expression.

		@param returnValue The return value string.
	**/
	virtual void Return(const CString& returnValue) = 0;

	/**
		Sets the return value for the template command.  Only useful in a
		template variable command expression.

		@param returnValue The return value integer.
	**/
	virtual void Return(int returnValue) = 0;
};


/**
	The base class of a template command.
**/
class WWhizTemplateCommand
{
public:
	/**
		Called whenever a template command is first executed within a given
		template.

		@param curTemplate The currently active template.
	**/
	virtual void Create(WWhizTemplate* /*curTemplate*/) { };

	/**
		Called whenever the template is finished executing.

		@param curTemplate The currently active template.
	**/
	virtual void Destroy(WWhizTemplate* /*curTemplate*/) { };

	/**
		Called whenever the command is seen within the script.  All processing
		should occur in this function.

		If the command is a variable command expression, then Run() should fill
		in a return value (args.Return()) if necessary.

		@param curTemplate The currently active template.
		@param args The arguments from the template command's argument list.
	**/
	virtual void Run(WWhizTemplate* curTemplate, WWhizTemplateCommandArgs& args) = 0;
};


/**
	A single template group, which corresponds to a single .WWTpl file on the
	disk.
**/
class WWhizTemplateGroup
{
public:
	/**
		@return Returns the number of templates contained in the group.
	**/
	virtual int GetCount() = 0;

	/**
		@param index The index of the template to return.
		@return Returns the WWhizTemplate at [index].
	**/
	virtual WWhizTemplate* Get(int index) = 0;

	/**
		Finds a template by name.
	
		@param name The full name as specified in the .WWTpl file between the
			[full name].
		@return Returns the index of the found template.  Returns -1 if the
			template was not found.
	**/
	virtual int Find(LPCTSTR name) = 0;

	/**
		\internal Used by WWhizTemplateManager::Refresh().
	**/
	virtual bool Refresh() = 0;


	/**
		@return Returns the name of the template group.
	**/
	virtual const CString& GetName() const = 0;

	/**
		@return Returns the full path name to the template group.
	**/
	virtual const CString& GetFilename() const = 0;

	/**
		@return Returns the time stamp of the template group, as it existed
			when WWhizTemplateManager::Refresh() was called.
	**/
	virtual const CTime& GetTimeStamp() const = 0;

	/**
	**/
	virtual bool IsActive() const = 0;

	/**
	**/
	virtual void SetActive(bool active) = 0;
};


class CTreeCtrlEx;


/**
	The WWhizTemplateManager is the primary interface to Workspace Whiz's
	template system.  Workspace Whiz offers a dynamic, powerful code template
	system.
**/
class WWhizTemplateManager
{
public:
	/**
		Add a new code template file.  Most code template files come with the
		file extension of .WWTpl.  A template file is internally known as
		a WWhizTemplateGroup.

		@param filename The full path to the template file.
		@return Returns the index where the file was inserted.
	**/
	virtual int Add(CString filename) = 0;

	/**
		Remove a code template group.

		@param index The index of the template group to remove.
		@return Returns true if the template group was removed, false otherwise.
	**/
	virtual bool Remove(int index) = 0;

	/**
		Reloads any changed template files.

		@return Returns true if a template file was reloaded, false otherwise.
	**/
	virtual bool Refresh() = 0;


	/**
		@return Retrieves the number of template groups.
	**/
	virtual int GetCount() = 0;

	/**
		@param index The index of the template group to retrieve.
		@return Returns the WWhizTemplateGroup existing at [index].
	**/
	virtual WWhizTemplateGroup* GetTemplateGroup(int index) = 0;

	/**
		The Workspace Whiz template system allows additions of user created
		commands.

		@param commandName The name of the command being added.
		@param templateCommand A reference to the instance of the
			WWhizTemplateCommand-derived class being added.
	**/
	virtual void RegisterCommand(CString commandName, WWhizTemplateCommand& templateCommand) = 0;

	/**
		@param The name of the command to retrieve (not case-sensitive).
		@return Returns a pointer to the command specified by [name].  If there
			isn't a command called [name], NULL is returned.
	**/
	virtual WWhizTemplateCommand* GetCommand(CString name) = 0;
	
	/**
		\internal
	**/
	virtual CFile& GetTreeStateFile() = 0;


	/**
		\internal
	**/
	virtual void LoadRegistry() = 0;

	/**
		\internal
	**/
	virtual void SaveRegistry() = 0;


	/**
		\internal
	**/
	virtual bool RestoreTreeState(CTreeCtrlEx& tree) = 0;

	/**
		\internal
	**/
	virtual void SaveTreeState(CTreeCtrlEx& tree) = 0;


	/**
		\internal
	**/
	virtual CString ParseCode(LPCTSTR codePtr, LPCTSTR bolPtr, WWhizTemplate* file = NULL) = 0;


	/**
		\internal
	**/
	virtual void SetTemplateDialog(WWhizTemplateDialog* templateDialog) = 0;
};



