#include "stdafx.h"
#include "WorkspaceUtils.h"
#include "Commands.h"
#include "FindFileDialog.h"
#include "FindTagDialog.h"
#include "WorkspaceInfo.h"
#include "WorkspaceTags.h"
#include "CompilerFiles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**
	Stolen from the MFC 6.0 source.  See CString::Find().
**/
int CStringFind(const CString& str, LPCTSTR lpszSub, int nStart)
{
	ASSERT(AfxIsValidString(lpszSub));

	int nLength = str.GetLength();
	if (nStart > nLength)
		return -1;

	// find first matching substring
	LPTSTR lpsz = _tcsstr((LPCTSTR)str + nStart, lpszSub);

	// return -1 for not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - (LPCTSTR)str);
}


/**
	Stolen from the MFC 6.0 source.  See CString::Find().
**/
int CStringFind(const CString& str, TCHAR ch, int nStart)
{
	int nLength = str.GetLength();
	if (nStart >= nLength)
		return -1;

	// find first single character
	LPTSTR lpsz = _tcschr((LPCTSTR)str + nStart, (_TUCHAR)ch);

	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - (LPCTSTR)str);
}


/////////////////////////////////////////////////////////////////////////////
// CCommands

CFindFileDialog g_wfoDialog;			// Workspace file open dialog.
CFindFileDialog g_cfoDialog;			// Compiler file open dialog.

CCommands::CCommands()
{
	m_pApplication = NULL;
	m_pApplicationEventsObj = NULL;
	m_pDebuggerEventsObj = NULL;
}

CCommands::~CCommands()
{
	WorkspaceInfo::RemoveAll();
	WorkspaceTags::RemoveAll();

	ASSERT (m_pApplication != NULL);
	m_pApplication->Release();
}

void CCommands::SetApplicationObject(IApplication* pApplication)
{
	// This function assumes pApplication has already been AddRef'd
	//  for us, which CDSAddIn did in its QueryInterface call
	//  just before it called us.
	m_pApplication = pApplication;

	// Create Application event handlers
	XApplicationEventsObj::CreateInstance(&m_pApplicationEventsObj);
	m_pApplicationEventsObj->AddRef();
	m_pApplicationEventsObj->Connect(m_pApplication);
	m_pApplicationEventsObj->m_pCommands = this;

	// Create Debugger event handler
	CComPtr<IDispatch> pDebugger;
	if (SUCCEEDED(m_pApplication->get_Debugger(&pDebugger))
		&& pDebugger != NULL)
	{
		XDebuggerEventsObj::CreateInstance(&m_pDebuggerEventsObj);
		m_pDebuggerEventsObj->AddRef();
		m_pDebuggerEventsObj->Connect(pDebugger);
		m_pDebuggerEventsObj->m_pCommands = this;
	}

	CFindFileDialog::m_pApplication = m_pApplication;
	CFindTagDialog::m_pApplication = m_pApplication;
	WorkspaceInfo::s_pApplication = m_pApplication;
	WorkspaceTags::m_pApplication = m_pApplication;
	char *regString = "^(_|~*[a-zA-Z:][a-zA-Z0-9_+*/%&|~!=<>, \t:~]*)\\([.]*";

//	WorkspaceTags::m_regEx.RegComp(regString);
}

void CCommands::UnadviseFromEvents()
{
	ASSERT (m_pApplicationEventsObj != NULL);
	m_pApplicationEventsObj->Disconnect(m_pApplication);
	m_pApplicationEventsObj->Release();
	m_pApplicationEventsObj = NULL;

	if (m_pDebuggerEventsObj != NULL)
	{
		// Since we were able to connect to the Debugger events, we
		//  should be able to access the Debugger object again to
		//  unadvise from its events (thus the VERIFY_OK below--see stdafx.h).
		CComPtr<IDispatch> pDebugger;
		VERIFY_OK(m_pApplication->get_Debugger(&pDebugger));
		ASSERT (pDebugger != NULL);
		m_pDebuggerEventsObj->Disconnect(pDebugger);
		m_pDebuggerEventsObj->Release();
		m_pDebuggerEventsObj = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Event handlers

// Application events

HRESULT CCommands::XApplicationEvents::BeforeBuildStart()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BuildFinish(long nNumErrors, long nNumWarnings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeApplicationShutDown()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	WorkspaceInfo::RemoveAll();
	WorkspaceTags::RemoveAll();
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentOpen(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeDocumentClose(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentSave(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewDocument(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowActivate(IDispatch* theWindow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowDeactivate(IDispatch* theWindow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	WorkspaceInfo::RemoveAll();
	WorkspaceTags::RemoveAll();
	WorkspaceInfo::SetWorkspaceLocation();
	WorkspaceInfo::Refresh();
	g_wfoDialog.m_lastFilename.Empty();
	g_wfoDialog.m_lastPosition = -1;
	CFindTagDialog::m_lastFunction.Empty();
	CFindTagDialog::m_lastPosition = -1;
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceClose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	WorkspaceInfo::RemoveAll();
	WorkspaceTags::RemoveAll();
	g_wfoDialog.m_lastFilename.Empty();
	g_wfoDialog.m_lastPosition = -1;
	CFindTagDialog::m_lastFunction.Empty();
	CFindTagDialog::m_lastPosition = -1;
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewWorkspace()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	WorkspaceInfo::RemoveAll();
	WorkspaceTags::RemoveAll();
	WorkspaceInfo::Refresh();
	g_wfoDialog.m_lastFilename.Empty();
	g_wfoDialog.m_lastPosition = -1;
	CFindTagDialog::m_lastFunction.Empty();
	CFindTagDialog::m_lastPosition = -1;
	return S_OK;
}

// Debugger event

HRESULT CCommands::XDebuggerEvents::BreakpointHit(IDispatch* pBreakpoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CCommands methods

STDMETHODIMP CCommands::WUFileOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	//First, get a pointer to the dispatch for the Projects collection
	CComPtr<IDispatch> pDispProjects;
	VERIFY_OK(m_pApplication->get_Projects(&pDispProjects));
	CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

	//Get the number of projects in the collection
	long lCount;
	VERIFY_OK(pProjects->get_Count(&lCount));
	if (lCount == 0)
	{
		m_pApplication->ExecuteCommand(CComBSTR("FileOpen"));
	}
	else
	{
		if (WorkspaceInfo::Refresh() == false)
			g_wfoDialog.m_lastFilename.Empty();

		g_wfoDialog.m_fileList = &WorkspaceInfo::GetFileList();
		g_wfoDialog.m_workspaceOpen = true;
		g_wfoDialog.DoModal();
	}

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_OK;
}


/**
	Given a file list and a filename, find the closest match.
**/
bool FindFile(const FileList& fileList, const CString& filename, CString& fullName)
{
	// Create the file structure.
	File file;
	File::Create(file, "", filename);

	// Scan the file list.
	for (int i = 0; i < fileList.GetCount(); i++)
	{
		File& fileCmp = fileList[i];

		// Compare the extension.
		if (file.GetExt() != fileCmp.GetExt())
			continue;

		// Compare the short name.
		if (file.GetShortName() == fileCmp.GetShortName())
		{
			fullName = fileCmp.GetPath() + fileCmp.GetTitle() + "." + fileCmp.GetExt();

			return true;
		}
	}

	// Didn't find it.
	fullName.Empty();

	return false;
}


STDMETHODIMP CCommands::WUHeaderFlip()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Get the filename of the active file.
	CString fullName;
	WorkspaceInfo::GetCurrentFilename(fullName);
	if (fullName.IsEmpty())
	{
		return S_OK;
	}

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	// Update the workspace, in case it changed.
	if (!WorkspaceInfo::Refresh())
	{
		// Clear out the last filename if the workspace changed.
		g_wfoDialog.m_lastFilename.Empty();
	}

	//First, get a pointer to the dispatch for the Projects collection
	CComPtr<IDispatch> pDispProjects;
	VERIFY_OK(m_pApplication->get_Projects(&pDispProjects));
	CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

	// Get a pointer to the dispatch for the Documents collection.
	CComPtr<IDispatch> p_Documents;
	m_pApplication->get_Documents(&p_Documents);
	CComQIPtr<IDocuments, &IID_IDocuments> pDocuments(p_Documents);
	CComPtr<IDispatch> p_Document;

	// Split it.
	CString path;
	CString title;
	CString ext;
	WorkspaceInfo::SplitPath(fullName, path, title, ext);
 
	// There must be a file extension to perform the flip.
	if (ext.IsEmpty())
	{
		VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
		return S_OK;
	}
	ext.MakeLower();

	// Set up our known extensions list.
	char* extList[] = { "cpp", "c", "cxx", "h", "hpp", "inl" };
	int numExts = _countof(extList);

	// Figure out which extension we should start on.
	int whichExt = -1;
	for (int i = 0; i < numExts; i++)
	{
		if (ext == extList[i])
		{
			whichExt = i;
			break;
		}
	}

	// Did we match ANY extension at all?  If not, then leave.  We can't properly
	// do a flip.
	if (whichExt == -1)
	{
		VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
		return S_OK;
	}

	int startExt = whichExt + 1;
	if (startExt == numExts)
		startExt = 0;

	bool found = false;		// Found the file.

	// Get the number of projects in the collection
	long lCount;
	VERIFY_OK(pProjects->get_Count(&lCount));
	if (lCount)
	{
		// Refresh the workspace.
		WorkspaceInfo::Refresh();

		// Try within the workspace first.
		int curExt = startExt;
		do
		{
			CString fullNameWithPath;
			if (FindFile(WorkspaceInfo::GetFileList(), title + extList[curExt], fullNameWithPath) == true)
			{
				pDocuments->Open(CComBSTR(fullNameWithPath), CComVariant("Auto"),
								 CComVariant(VARIANT_FALSE), &p_Document);
				if (p_Document != NULL)
				{
					found = true;
					break;
				}
			}

			curExt++;
			if (curExt == numExts)
				curExt = 0;
		} while (curExt != whichExt);
	}

	// Now try on disk.
	if (!found)
	{
		int curExt = startExt;

		do
		{
			CString fullNameWithPath = path + title + extList[curExt];
			pDocuments->Open(CComBSTR(fullNameWithPath), CComVariant("Auto"),
							 CComVariant(VARIANT_FALSE), &p_Document);
			if (p_Document != NULL)
			{
				found = true;
				break;
			}

			curExt++;
			if (curExt == numExts)
				curExt = 0;
		} while (curExt != whichExt);
	}

	// Refresh the compiler files.
	CompilerFiles::Refresh();

	// Try within the workspace first.
	int curExt = startExt;
	do
	{
		CString fullNameWithPath;
		if (FindFile(CompilerFiles::GetFileList(), title + extList[curExt], fullNameWithPath) == true)
		{
			pDocuments->Open(CComBSTR(fullNameWithPath), CComVariant("Auto"),
							 CComVariant(VARIANT_FALSE), &p_Document);
			if (p_Document != NULL)
			{
				found = true;
				break;
			}
		}

		curExt++;
		if (curExt == numExts)
			curExt = 0;
	} while (curExt != whichExt);

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
	return S_OK;
}




STDMETHODIMP CCommands::WUFindTagDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	// Update the workspace.
	WorkspaceInfo::Refresh();

	//First, get a pointer to the dispatch for the Projects collection
	CComPtr<IDispatch> pDispProjects;
	VERIFY_OK(m_pApplication->get_Projects(&pDispProjects));
	CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

	//Get the number of projects in the collection
	long lCount;
	VERIFY_OK(pProjects->get_Count(&lCount));
	if (lCount != 0)
	{
		CFindTagDialog dlg;
		dlg.DoModal();
	}

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

	return S_OK;
}


CString CCommands::GetWord(void)
{
	CComPtr<IDispatch> pActiveDocument;
	m_pApplication->get_ActiveDocument(&pActiveDocument);
	CComQIPtr<ITextDocument, &IID_ITextDocument> pText(pActiveDocument);

	CComPtr<IDispatch> p_TextSelection;
	pText->get_Selection(&p_TextSelection);
	CComQIPtr<ITextSelection, &IID_ITextSelection> pSelection(p_TextSelection);

	CComBSTR startFind = "\\{[^a-zA-Z0-9_]\\}";
	CComBSTR endFind   = "\\{^\\![^a-zA-Z0-9_]\\}";

	short findReturn;
	pSelection->FindText(startFind, CComVariant(dsMatchBackward | dsMatchRegExp), &findReturn);
	if (findReturn != 0)
	{
		// Found it... save the last coordinate.
		LONG startX, startY;
		pSelection->get_CurrentColumn(&startX);
		pSelection->get_CurrentLine(&startY);

		pSelection->FindText(endFind, CComVariant(dsMatchRegExp), &findReturn);
		if (findReturn != 0)
		{
			pSelection->MoveTo(startY, startX, CComVariant(dsExtend));

			CComBSTR foundTextBSTR;
			pSelection->get_Text(&foundTextBSTR);

			return foundTextBSTR;
		}
	}

	return "";
}

STDMETHODIMP CCommands::WUFindTagAtCursorDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	// Update the workspace.
	WorkspaceInfo::Refresh();
	WorkspaceTags::Update();

	//First, get a pointer to the dispatch for the Projects collection
	CComPtr<IDispatch> pDispProjects;
	VERIFY_OK(m_pApplication->get_Projects(&pDispProjects));
	CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

	//Get the number of projects in the collection
	long lCount;
	VERIFY_OK(pProjects->get_Count(&lCount));
	if (lCount != 0)
	{
		CString foundText = GetWord();
		CFindTagDialog::m_lastFunction = foundText;

		CFindTagDialog dlg;
		dlg.DoModal();
	}

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

	return S_OK;
}

STDMETHODIMP CCommands::WUFindTagAtCursor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// Update the workspace.
	WorkspaceInfo::Refresh();
	WorkspaceTags::Update();

	//First, get a pointer to the dispatch for the Projects collection
	CComPtr<IDispatch> pDispProjects;
	VERIFY_OK(m_pApplication->get_Projects(&pDispProjects));
	CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

	//Get the number of projects in the collection
	long lCount;
	VERIFY_OK(pProjects->get_Count(&lCount));
	if (lCount != 0)
	{
		CString foundText = GetWord();
		WorkspaceTags::MatchIdentifier(foundText);
		if (WorkspaceTags::GetMatchHead() != NULL)
			WorkspaceTags::GotoTag(WorkspaceTags::GetMatchHead());
	}

	return S_OK;
}

STDMETHODIMP CCommands::WUFindNextTag()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (WorkspaceTags::GetLastMatchedTag() != NULL)
	{
		const WorkspaceTags::Tag* tag = WorkspaceTags::GetLastMatchedTag();
		if (tag->GetMatchNext() != NULL)
			tag = tag->GetMatchNext();
		else
			tag = WorkspaceTags::GetMatchHead();
		WorkspaceTags::GotoTag(tag);
	}

	return S_OK;
}

STDMETHODIMP CCommands::WUFindPrevTag()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (WorkspaceTags::GetLastMatchedTag() != NULL)
	{
		const WorkspaceTags::Tag* tag = WorkspaceTags::GetLastMatchedTag();
		if (tag->GetMatchPrev() != NULL)
			tag = tag->GetMatchPrev();
		else
			tag = WorkspaceTags::GetMatchTail();
		WorkspaceTags::GotoTag(tag);
	}

	return S_OK;
}

STDMETHODIMP CCommands::WUCompilerFileOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	if (CompilerFiles::Refresh() == false)
		g_cfoDialog.m_lastFilename.Empty();

	g_cfoDialog.m_fileList = &CompilerFiles::GetFileList();
	g_cfoDialog.m_workspaceOpen = false;
	g_cfoDialog.DoModal();

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

	return S_OK;
}

bool CCommands::GetActiveFile(CString& activeFilename)
{
	//First, get a pointer to the dispatch for the Projects collection
	CComPtr<IDispatch> pDispProjects;
	VERIFY_OK(m_pApplication->get_Projects(&pDispProjects));
	CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

	// Get the filename of the active file.
	CComPtr<IDispatch> pActiveDocument;
	m_pApplication->get_ActiveDocument(&pActiveDocument);
	if (!pActiveDocument)
	{
		return false;
	}

	CComQIPtr<ITextDocument, &IID_ITextDocument> pText(pActiveDocument);
	CComBSTR bszStr;
	pText->get_FullName(&bszStr);
	activeFilename = bszStr;

	return true;
}


STDMETHODIMP CCommands::WUFindNextFile()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (WorkspaceInfo::Refresh() == false)
		g_wfoDialog.m_lastFilename.Empty();

	CString filename;
	if (!GetActiveFile(filename))
	{
		return S_OK;
	}
		
	CComPtr<IDispatch> p_Documents;
	m_pApplication->get_Documents(&p_Documents);
	CComQIPtr<IDocuments, &IID_IDocuments> pDocuments(p_Documents);
	CComPtr<IDispatch> p_Document;

	// Create the file structure.
	File file;
	File::Create(file, "", filename);

	// Find the exact file.
	const FileList& fileList = WorkspaceInfo::GetFileList();
	int startPos = fileList.FindExact(file);
	if (startPos == -1)
	{
		// Try without the extension.
		file.SetExt("");
		startPos = fileList.FindExact(file);
	}

	if (startPos == -1)
	{
		MessageBeep(0xFFFFFFFF);
		return S_OK;
	}

	// Now, go to the next one.
	int nextPos = fileList.FindNext(startPos, file);
	if (nextPos == -1)
	{
		return S_OK;
	}

	File& fileFound = fileList[nextPos];

	CString fullNameWithPath = fileFound.GetPath() + fileFound.GetTitle() + "." + fileFound.GetExt();
	pDocuments->Open(CComBSTR(fullNameWithPath), CComVariant("Auto"),
					 CComVariant(VARIANT_FALSE), &p_Document);

	return S_OK;
}

STDMETHODIMP CCommands::WUFindPrevFile()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (WorkspaceInfo::Refresh() == false)
		g_wfoDialog.m_lastFilename.Empty();

	CString filename;
	if (!GetActiveFile(filename))
	{
		return S_OK;
	}
		
	CComPtr<IDispatch> p_Documents;
	m_pApplication->get_Documents(&p_Documents);
	CComQIPtr<IDocuments, &IID_IDocuments> pDocuments(p_Documents);
	CComPtr<IDispatch> p_Document;

	// Create the file structure.
	File file;
	File::Create(file, "", filename);

	// Find the exact file.
	const FileList& fileList = WorkspaceInfo::GetFileList();
	int startPos = fileList.FindExact(file);
	if (startPos == -1)
	{
		// Try without the extension.
		file.SetExt("");
		startPos = fileList.FindExact(file);
	}

	if (startPos == -1)
	{
		MessageBeep(0xFFFFFFFF);
		return S_OK;
	}

	// Now, go to the next one.
	int prevPos = fileList.FindPrevious(startPos, file);
	if (prevPos == -1)
	{
		return S_OK;
	}

	File& fileFound = fileList[prevPos];

	CString fullNameWithPath = fileFound.GetPath() + fileFound.GetTitle() + "." + fileFound.GetExt();
	pDocuments->Open(CComBSTR(fullNameWithPath), CComVariant("Auto"),
					 CComVariant(VARIANT_FALSE), &p_Document);

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////

bool ExtractOption(CString& srcStr, LPCTSTR findStr, CString* extractedStr = NULL)
{
	// Find the findStr.
	int findStartPos = CStringFind(srcStr, findStr, 0);
	if (findStartPos == -1)
	{
		// Not found.
		return false;
	}

	// See if there is a quote.
	int findStrLen = strlen(findStr);

	int argStartPos = findStartPos + findStrLen;
	int argEndPos;
	int findEndPos;
	if (srcStr[argStartPos] == '"')
	{
		// Yes, find the matching quote.
		argStartPos++;
		argEndPos = argStartPos;
		while (srcStr[argEndPos] != '"')
		{
			argEndPos++;
		}

		findEndPos = argEndPos + 1;
		while (findEndPos < srcStr.GetLength()  &&  srcStr[findEndPos] == ' ')
		{
			findEndPos++;
		}
	}
	else
	{
		// Otherwise, find the first space or end of line.
		argEndPos = argStartPos;
		while (argEndPos < srcStr.GetLength()  &&  srcStr[argEndPos] != ' ')
		{
			argEndPos++;
		}

		// Find the last space.
		findEndPos = argEndPos;
		while (findEndPos < srcStr.GetLength()  &&  srcStr[findEndPos] == ' ')
		{
			findEndPos++;
		}
	}

	// Extract the parameter.
	if (extractedStr)
		*extractedStr = srcStr.Mid(argStartPos, argEndPos - argStartPos);

	// Got it.  Extract the new string.
	srcStr = srcStr.Left(findStartPos) + srcStr.Mid(findEndPos);

	return true;
}

/**
	Since Visual Studio provides no COM method of returning the names of
	files in a project, we have to cheat, and read them from the .DSP
	files on disk.  This has at least one primary disadvantage.  If a
	change is made to the project in the Developer Studio, the project
	MUST be saved before we can process the change here.
**/
static void FixPrecompiledHeaders(LPCTSTR name, CComPtr<IConfiguration>& pConfig)
{
#if _MSC_VER < 1200
	AfxMessageBox("Not implemented under Visual C++ 5.0!");
	return;
#else
	// Get config name.
	CComBSTR bszStr;
	VERIFY_OK(pConfig->get_Name(&bszStr));
	CString configName = bszStr;
	Status::Print("  Config: %s", configName);

	// Resolve the filename.
	CString inFilename = name;
	WorkspaceInfo::ResolveFilename(WorkspaceInfo::GetWorkspaceLocation(), inFilename);
	
	// Open the .dsp file.
	CStdioFile inFile;
	if (inFile.Open(inFilename, CFile::modeRead) == FALSE)
	{
		// Huh?
		AfxMessageBox("Couldn't open " + inFilename + "!");
		return;
	}

	// Build the root path to resolve filenames from.
	CString rootPath = inFilename.Left(inFilename.ReverseFind('\\') + 1);

	// Begin reading the file.
	enum State
	{
		ScanSource,
		ProcessProject,
		ProcessSource,
	};
	bool skipLines = false;
	State state = ScanSource;
	State lastState = ScanSource;
	bool noPrecompiledHeader = false;
	bool isCFileExt = false;

	CString currentSourceFilename;
	CString globalPrecompFilename;
	CString localPrecompFilename;

	CString line;
	while (1)
	{
		// Read in a line.
		if (!inFile.ReadString(line))
			break;

		// Skip empty lines.
		if (line.IsEmpty())
			continue;

		// Check for preprocessor conditions.
		if (line.GetLength() >= 6  &&  strncmp(line, "!ENDIF", 6) == 0)
		{
			skipLines = false;
		}

		if ((line.GetLength() > 3  &&  strncmp(line, "!IF", 3) == 0) ||
			(line.GetLength() > 7  &&  strncmp(line, "!ELSEIF", 7) == 0))
		{
			// Check to see if this is a line resembling:
			//   !IF  "$(CFG)" == "testcomp - Win32 Release"
			//   !ELSEIF  "$(CFG)" == "testcomp - Win32 Debug"
			int cfgPos = CStringFind(line, "\"$(CFG)\"", 0);
			if (cfgPos != -1)
			{
				// Must find a ==, too.
				int equalsPos = CStringFind(line, "==", cfgPos);
				if (equalsPos == -1)
					continue;

				// Find the third and fourth quotes.
				int quote3 = CStringFind(line, '"', equalsPos);
				if (quote3 == -1)
					continue;
				int quote4 = CStringFind(line, '"', quote3 + 1);
				if (quote4 == -1)
					continue;

				// In between the third and fourth quotes is the current project config.
				CString preprocConfig;		// The config requested by the preprocessor.
				preprocConfig = line.Mid(quote3 + 1, quote4 - (quote3 + 1));

				if (preprocConfig == configName)
				{
					skipLines = false;
				}
				else
				{
					skipLines = true;
				}
				continue;
			}
			else
			{
				// Unrecognized preprocessor condition.  Skip this line.
				continue;
			}
		}

		// If we're skipping lines, then skip everything else.
		if (skipLines)
		{
			continue;
		}
		
		if (state == ScanSource)
		{
			// Check for the beginning of a source file.
			if (line == "# Begin Project")
			{
				state = ProcessProject;
			}
		}

		// In a project...
		else if (state == ProcessProject)
		{
			if (line == "# Begin Source File")
			{
				lastState = state;
				state = ProcessSource;
				noPrecompiledHeader = false;
			}
			else if (line.GetLength() > 15  &&  strncmp(line, "# ADD BASE CPP ", 15) == 0)
			{
				// Get rid of any /Yu or /Fp options on this line.
//				AfxMessageBox("BASE CPP BEGIN: " + line);
				ExtractOption(line, "/Yu", &globalPrecompFilename);
				ExtractOption(line, "/Fp");
//				AfxMessageBox("CPP END: " + line);
			}
			else if (line.GetLength() > 10  &&  strncmp(line, "# ADD CPP ", 10) == 0)
			{
				// Get rid of any /Yu or /Fp options on this line.
//				AfxMessageBox("CPP BEGIN: " + line);
				ExtractOption(line, "/Yu", &globalPrecompFilename);
				ExtractOption(line, "/Fp");
//				AfxMessageBox("CPP END: " + line);
			}
		}

		// In a source file...
		else if (state == ProcessSource)
		{
			// Check for end of source file block.
			if (line == "# End Source File")
			{
				if (!isCFileExt)
					continue;

				if (!noPrecompiledHeader  &&  (!localPrecompFilename.IsEmpty()  ||
					!globalPrecompFilename.IsEmpty()))
				{
					// Look for the extension..
					CString precompFilename;
					if (localPrecompFilename.IsEmpty())
						precompFilename = globalPrecompFilename;
					else
						precompFilename = localPrecompFilename;

					int preDotPos = precompFilename.ReverseFind('.');
					if (preDotPos != -1)
						precompFilename.GetBufferSetLength(preDotPos);

					// Got the filename.
					VARIANT reserved;
					pConfig->RemoveFileSettings(CComBSTR(currentSourceFilename), CComBSTR("/Fp"), reserved);

					CString setting;
					setting.Format("/Fp\"$(OutDir)\\%s.pch\"", precompFilename);
					Status::Print("    %s -- Using %s.pch", currentSourceFilename, precompFilename);
//				AfxMessageBox("CPP: " + setting);
					pConfig->AddFileSettings(CComBSTR(currentSourceFilename), CComBSTR(setting), reserved);
				}
				else
				{
					VARIANT reserved;
					pConfig->RemoveFileSettings(CComBSTR(currentSourceFilename), CComBSTR("/Yc"), reserved);
					pConfig->RemoveFileSettings(CComBSTR(currentSourceFilename), CComBSTR("/Yu"), reserved);
					Status::Print("    %s -- No precompiled header.", currentSourceFilename);
				}
				
				state = lastState;
				localPrecompFilename.Empty();
				currentSourceFilename.Empty();
			}

			// Check for SOURCE= lines.  (Do strncmp() for speed)
			else if (line.GetLength() > 7  &&  strncmp(line, "SOURCE=", 7) == 0)
			{
				ExtractOption(line, "SOURCE=", &currentSourceFilename);
				isCFileExt = false;
				int pos = currentSourceFilename.ReverseFind('.');
				if (pos != -1)
				{
					CString ext = currentSourceFilename.Mid(pos + 1);
					ext.MakeLower();
					if (ext == "c"  ||  ext == "cpp"  ||  ext == "cxx"  ||  ext == "cc")
					{
						isCFileExt = true;
					}
				}
			}

			// Check for # ADD CPP lines.
			else if (line.GetLength() > 10  &&  strncmp(line, "# ADD CPP ", 10) == 0)
			{
				// Get rid of the filename option.  We'll replace it with our own.
				ExtractOption(line, "/Fp");

				// Test for the /Yc or /Yu.
				if (!ExtractOption(line, "/Yc", &localPrecompFilename))
				{
					ExtractOption(line, "/Yu", &localPrecompFilename);
				}
			}

			else if (line.GetLength() > 15  &&  strncmp(line, "# SUBTRACT CPP ", 15) == 0)
			{
				// If the precompiled header was subtracted from this source file,
				// then be sure to handle our version also.
				if (line.Find("/Yc") != -1  ||  line.Find("/Yu") != -1)
				{
					noPrecompiledHeader = true;
				}
			}

		}
	}

	inFile.Close();
#endif
}



STDMETHODIMP CCommands::WUFixMultiplePrecompiledHeaders()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_FALSE));

	if (AfxMessageBox("Fixing the projects for multiple precompiled headers requires "
			"a Save All command be issued.  Continue?", MB_OKCANCEL) == IDCANCEL)
	{
		VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));
		return S_OK;
	}
	
	// Save all files.
	m_pApplication->ExecuteCommand(CComBSTR("FileSaveAll"));

	//First, get a pointer to the dispatch for the Projects collection
	CComPtr<IDispatch> pDispProjects;
	VERIFY_OK(m_pApplication->get_Projects(&pDispProjects));
	CComQIPtr<IProjects, &IID_IProjects> pProjects(pDispProjects);

	Status::Create();

	//Get the number of projects in the collection
	long lCount;
	VERIFY_OK(pProjects->get_Count(&lCount));
	for (long i = 1 ; i < lCount+1 ; i++)
	{
		long lNumConfigs;
		CComVariant Vari = i;

		//Get the next project
		CComPtr<IGenericProject> pGenProject;
		VERIFY_OK(pProjects->Item(Vari, &pGenProject));
		CComQIPtr<IBuildProject, &IID_IBuildProject> pProject(pGenProject);

		// Get the project name.
		CComBSTR bszStr;
		VERIFY_OK(pProject->get_FullName(&bszStr));
		CString projectName = bszStr;

		Status::Print("Project: %s", projectName);

		//Each project has a configuration (Win32 Release, Win32 Debug,
		//  etc.), get the collection of these configurations
		CComPtr<IConfigurations> pConfigs;
		VERIFY_OK(pProject->get_Configurations(&pConfigs));
		VERIFY_OK(pConfigs->get_Count(&lNumConfigs));
		for (long j = 1 ; j < lNumConfigs+1 ; j++)
		{
			CComVariant Varj = j;
			//Get each individual configuration
			CComPtr<IConfiguration> pConfig;
			VERIFY_OK(pConfigs->Item(Varj, &pConfig));

			CComVariant VarDisp = pConfig;
			CComBSTR bszStr;
			VERIFY_OK(pConfig->get_Name(&bszStr));
			CString configName = bszStr;

			FixPrecompiledHeaders(projectName, pConfig);
		}
	}

	Status::Print("");
	Status::Print("Hit Escape to continue...");
	Status::Destroy(true);

	VERIFY_OK(m_pApplication->EnableModeless(VARIANT_TRUE));

	return S_OK;
}


