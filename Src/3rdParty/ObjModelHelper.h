#pragma once

#include <afxdisp.h>

#ifdef WWHIZ_VC6

#include <ObjModel\AppAuto.h>
#include <ObjModel\BldAuto.h>
#include <ObjModel\textdefs.h>
#include <ObjModel\TextAuto.h>
#include "ObjModelGUID.h"

__declspec(selectany) IApplication* g_pApplication = NULL;

struct IGenericDocumentVC : public IGenericDocument { };
struct IDocumentsVC : public IDocuments { };
struct ITextDocumentVC : public ITextDocument { };
struct IConfigurationVC : public IConfiguration { };
struct IGenericDocumentEVC : public IGenericDocument { };
struct IDocumentsEVC : public IDocuments { };
struct ITextDocumentEVC : public ITextDocument { };
struct IConfigurationEVC : public IConfiguration { };

#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
#include "dte.tlh"

__declspec(selectany) CComPtr<EnvDTE::_DTE> g_pDTE;
#endif WWHIZ_VSNET

#undef FindText

class ObjModelHelper
{
public:
	ObjModelHelper();

	static bool VStudioExists();
	
#ifdef WWHIZ_VC6
	static IApplication* GetInterface();
	static void SetInterface(IApplication* pApplication);
#endif WWHIZ_VC6

#ifdef WWHIZ_VSNET
	static CComPtr<EnvDTE::_DTE>& GetInterface();
	static void SetInterface(CComPtr<EnvDTE::_DTE>& pDTE);
#endif WWHIZ_VSNET
	
	static CString GetVersion();

	void Reset();
	void Refresh();

	bool OpenDocument(LPCTSTR filename, LPCTSTR type = NULL);
	bool GetActiveDocument();
    CString GetFilename();
	void PutText(CString text);
	void Delete();
	int GetTabSize() const				{  return m_tabSize;  }
	void MoveTo(int y, int x, bool extend);
	void EndOfDocument(bool extend);
	bool SetReadOnly(bool readOnly);
	void GetPosition(long& x, long& y);
	void PutLanguage(LPCTSTR language);
	CString GetText();
	void SelectLine();
	bool FindText(CString searchString, CStringArray& strArray);
	bool FindText(CString searchString, CString flagStr);
	bool FindText(CString searchString, long flags);
	bool SaveDocument(const CString& filename);
	bool SaveAll();
	static bool ExecuteCommand(CString command, CString args);
	CString GetCurrentConfigName();
		
	bool IsTextDocument();
	void ScrollToCenter();

	CString GetCurrentProjectName();

	static bool SetKeyBinding(bool justText, const CString& command, const CString& binding);

#ifdef WWHIZ_VSNET
	void GetActiveProject(CComPtr<EnvDTE::Project>& pProject);

	// Various COM pointers we will need later, during the parse.
	CComPtr<EnvDTE::Document> m_pDocument;
	CComQIPtr<EnvDTE::TextDocument> m_pTextDocument;

	CComPtr<EnvDTE::TextSelection> m_pTextSelection;
#endif WWHIZ_VSNET

protected:
	void SetupTextDocument();
	void SetupTextSelection();

protected:
#ifdef WWHIZ_VC6
	// Various COM pointers we will need later, during the parse.
	CComPtr<IDispatch> m_pDispDocument;

    CComQIPtr<IGenericDocumentVC, &IID_IGenericDocumentVC> m_pGenericDocumentVC;
    CComQIPtr<IGenericDocumentEVC, &IID_IGenericDocumentEVC> m_pGenericDocumentEVC;
	CComQIPtr<ITextDocumentVC, &IID_ITextDocumentVC> m_pTextDocumentVC;
	CComQIPtr<ITextDocumentEVC, &IID_ITextDocumentEVC> m_pTextDocumentEVC;

	CComPtr<IDispatch> m_pDispTextSelection;
	CComQIPtr<ITextSelection, &IID_ITextSelectionBoth> m_pTextSelection;

	// Get the documents list.
	CComPtr<IDispatch> m_pDispDocuments;
	CComQIPtr<IDocumentsVC, &IID_IDocumentsVC> m_pDocumentsVC;
	CComQIPtr<IDocumentsEVC, &IID_IDocumentsEVC> m_pDocumentsEVC;
#endif WWHIZ_VC6
	long m_tabSize;
};


inline ObjModelHelper::ObjModelHelper()
{
	Reset();
}

#ifdef WWHIZ_VC6

inline bool ObjModelHelper::VStudioExists()
{
	return g_pApplication != NULL;
}


inline IApplication* ObjModelHelper::GetInterface()
{
	return g_pApplication;
}


inline void ObjModelHelper::SetInterface(IApplication* pApplication)
{
	g_pApplication = pApplication;
}


inline CString ObjModelHelper::GetVersion()
{
	HMODULE hModule = GetModuleHandle("MSDEV.EXE");
	if (hModule)
	{
		CComBSTR bstrVersion;
		ObjModelHelper::GetInterface()->get_Version(&bstrVersion);
		CString strVersion = bstrVersion;
		if (strVersion != "5.0")
		{
			return "6";
		}
		else
		{
			return "5";
		}
	}
	else
	{
		hModule = GetModuleHandle("EVC.EXE");
		if (hModule)
		{
			return "EVC";
		}
	}
	return "6";
}


#pragma optimize("", off)
inline bool ObjModelHelper::SaveDocument(const CString& filename)
{
	DsSaveStatus saveStatus;
	if (m_pTextDocumentVC)
	{
		if (FAILED(m_pTextDocumentVC->Save(CComVariant(filename), CComVariant(VARIANT_FALSE), &saveStatus)))
			return false;
	}
	else if (m_pTextDocumentEVC)
	{
		if (FAILED(m_pTextDocumentEVC->Save(CComVariant(filename), CComVariant(VARIANT_FALSE), &saveStatus)))
			return false;
	}

	return true;
}
#pragma optimize("", on)


inline bool ObjModelHelper::FindText(CString searchString, CStringArray& flagsArray)
{
	long flags = 0;
	for (int i = 0; i < flagsArray.GetSize(); ++i)
	{
		CString flagStr = flagsArray[i];
		flagStr.MakeLower();
		if (flagStr == "matchforward")
			flags |= dsMatchForward;
		else if (flagStr == "matchbackward")
			flags |= dsMatchBackward;
		else if (flagStr == "matchfromstart")
			flags |= dsMatchFromStart;
		else if (flagStr == "matchword")
			flags |= dsMatchWord;
		else if (flagStr == "matchcase")
			flags |= dsMatchCase;
		else if (flagStr == "matchregexp")
			flags |= dsMatchRegExp;
	}

	short findReturn;
	m_pTextSelection->FindText(CComBSTR(searchString), CComVariant(flags), &findReturn);
	return (findReturn != 0);
}

		
inline bool ObjModelHelper::FindText(CString searchString, CString flagStr)
{
	CStringArray strArray;
	strArray.Add(flagStr);
	return FindText(searchString, strArray);
}

		
inline bool ObjModelHelper::FindText(CString searchString, long flags)
{
	short findReturn;
	m_pTextSelection->FindText(CComBSTR(searchString), CComVariant(flags), &findReturn);
	return (findReturn != 0);
}

		
inline void ObjModelHelper::SelectLine()
{
	m_pTextSelection->SelectLine();
}


inline CString ObjModelHelper::GetText()
{
	CComBSTR bstrLine;
	m_pTextSelection->get_Text(&bstrLine);
	return CString(bstrLine);
}

	
inline void ObjModelHelper::PutLanguage(LPCTSTR language)
{
	if (strcmp(language, "cpp") == 0)
	{
		language = DS_CPP;
	}
	if (m_pTextDocumentVC)
		m_pTextDocumentVC->put_Language(CComBSTR(language));
	else if (m_pTextDocumentEVC)
		m_pTextDocumentEVC->put_Language(CComBSTR(language));
}


inline void ObjModelHelper::GetPosition(long& x, long& y)
{
	m_pTextSelection->get_CurrentColumn(&x);
	m_pTextSelection->get_CurrentLine(&y);
}

	
inline bool ObjModelHelper::SetReadOnly(bool readOnly)
{
	bool succeeded = false;
	if (m_pGenericDocumentVC)
	{
		if (SUCCEEDED(m_pGenericDocumentVC->put_ReadOnly((readOnly) ? VARIANT_FALSE : VARIANT_TRUE)))
			succeeded = true;
	}
	else if (m_pGenericDocumentEVC)
	{
		if (SUCCEEDED(m_pGenericDocumentEVC->put_ReadOnly((readOnly) ? VARIANT_FALSE : VARIANT_TRUE)))
			succeeded = true;
	}

	return succeeded;
}


inline void ObjModelHelper::MoveTo(int y, int x, bool extend)
{
	long type = extend ? dsExtend : dsMove;
	m_pTextSelection->MoveTo(y, x, CComVariant(type));
}

		
inline void ObjModelHelper::EndOfDocument(bool extend)
{
	long type = extend ? dsExtend : dsMove;
	m_pTextSelection->EndOfDocument(CComVariant(type));
}

		
inline void ObjModelHelper::Reset()
{
	m_pDispTextSelection = NULL;
	m_pTextSelection = m_pDispTextSelection;
	m_pDispDocument = NULL;
	m_pTextDocumentVC = m_pDispDocument;
	m_pTextDocumentEVC = m_pDispDocument;
	m_pDispDocuments = NULL;
	m_pDocumentsVC = m_pDispDocuments;
	m_pDocumentsEVC = m_pDispDocuments;

	g_pApplication->get_Documents(&m_pDispDocuments);
	m_pDocumentsVC = m_pDispDocuments;
	m_pDocumentsEVC = m_pDispDocuments;
}


inline void ObjModelHelper::SetupTextDocument()
{
	m_pGenericDocumentVC = m_pDispDocument;
	m_pGenericDocumentEVC = m_pDispDocument;
	m_pTextDocumentVC = m_pDispDocument;
	m_pTextDocumentEVC = m_pDispDocument;

	// Get the tab size here, after the file has been saved, so the
	// extension is known.
	m_tabSize = 0;
	if (m_pTextDocumentVC)
	{
		m_pTextDocumentVC->get_TabSize(&m_tabSize);
		m_pTextDocumentVC->put_Active(VARIANT_TRUE);
	}
	else if (m_pTextDocumentEVC)
	{
		m_pTextDocumentEVC->get_TabSize(&m_tabSize);
		m_pTextDocumentEVC->put_Active(VARIANT_TRUE);
	}
}


inline void ObjModelHelper::SetupTextSelection()
{
	m_pDispTextSelection = NULL;

	if (m_pTextDocumentVC)
		m_pTextDocumentVC->get_Selection(&m_pDispTextSelection);
	else if (m_pTextDocumentEVC)
		m_pTextDocumentEVC->get_Selection(&m_pDispTextSelection);

	m_pTextSelection = m_pDispTextSelection;
}


inline void ObjModelHelper::PutText(CString text)
{
	SetupTextSelection();
	m_pTextSelection->put_Text(CComBSTR(text));
}


inline void ObjModelHelper::Delete()
{
	ObjModelHelper::GetInterface()->ExecuteCommand(CComBSTR("Delete"));
}


inline bool ObjModelHelper::IsTextDocument()
{
	if (!m_pTextDocumentVC  &&  !m_pTextDocumentEVC)
		return false;
	return true;
}


inline bool ObjModelHelper::OpenDocument(LPCTSTR filename, LPCTSTR type)
{
	Reset();

	if (type == NULL)
		type = "Text";

	if (m_pDocumentsVC)
	{
		m_pDocumentsVC->Open(CComBSTR(filename), CComVariant(type),
						 CComVariant(VARIANT_FALSE), &m_pDispDocument);
	}
	else if (m_pDocumentsEVC)
	{
		m_pDocumentsEVC->Open(CComBSTR(filename), CComVariant(type),
						 CComVariant(VARIANT_FALSE), &m_pDispDocument);
	}

	SetupTextDocument();
	SetupTextSelection();

	return (m_pDispDocument != NULL);
}


inline bool ObjModelHelper::GetActiveDocument()
{
	Reset();

	g_pApplication->get_ActiveDocument(&m_pDispDocument);
	
	if (m_pDispDocument)
	{
        SetupTextDocument();
        SetupTextSelection();

		return true;
	}

	return false;
}


inline CString ObjModelHelper::GetFilename()
{
	CComBSTR bstrFilename;
	if (m_pTextDocumentVC)
		m_pTextDocumentVC->get_FullName(&bstrFilename);
	else if (m_pTextDocumentEVC)
		m_pTextDocumentEVC->get_FullName(&bstrFilename);
	return CString(bstrFilename);
}


inline void ObjModelHelper::Refresh()
{
	// In case of a second call to Refresh().
	m_pDispTextSelection = NULL;
	m_pTextSelection = m_pDispTextSelection;
	m_pDispDocument = NULL;
	m_pTextDocumentVC = m_pDispDocument;
	m_pTextDocumentEVC = m_pDispDocument;
	m_pDispDocuments = NULL;
	m_pDocumentsVC = m_pDispDocuments;
	m_pDocumentsEVC = m_pDispDocuments;

/*	// Get the active document.  If there is one, set up the m_helper->m_pTextSelection
	// pointer.
	g_pApplication->get_ActiveDocument(&m_pDispDocument);
	if (m_pDispDocument)
	{
		m_pTextDocument = m_pDispDocument;
		if (m_pTextDocument)
		{
			m_pTextDocument->get_Selection(&m_pDispTextSelection);
			m_pTextSelection = m_pDispTextSelection;

			// Get the tab size here, after the file has been saved, so the
			// extension is known.
			if (FAILED(m_pTextDocument->get_TabSize(&m_tabSize)))
			{
				throw TException(TException::UNABLE_TO_CREATE_NEW_FILE, "Unable to query the new file.");
			}
		}
	}
	m_pDispDocument = NULL;
	g_pApplication->get_Documents(&m_pDispDocuments);
	m_pDocuments = m_pDispDocuments;
	m_pDispDocuments = NULL;
*/
}


inline void ObjModelHelper::ScrollToCenter()
{
	g_pApplication->ExecuteCommand(CComBSTR("WindowScrollToCenter"));
}


inline CString ObjModelHelper::GetCurrentProjectName()
{
	if (!g_pApplication)
	{
		return "";
	}

	// Get the current project
	CComPtr<IDispatch> pActiveProject;
	g_pApplication->get_ActiveProject(&pActiveProject);
	if (pActiveProject)
	{
		CComQIPtr<IGenericProject, &IID_IGenericProjectVC> pProject(pActiveProject);
		if (pProject)
		{
			CComBSTR bstrProjectName;
			pProject->get_FullName(&bstrProjectName);
			CString projectName = bstrProjectName;
			return projectName;
		}
	}

	return "";
}


inline bool ObjModelHelper::ExecuteCommand(CString command, CString args)
{
	if (SUCCEEDED(ObjModelHelper::GetInterface()->ExecuteCommand(
			CComBSTR(command))))
		return true;
	return false;
}


#endif WWHIZ_VC6




//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
#ifdef WWHIZ_VSNET

inline bool ObjModelHelper::VStudioExists()
{
	return g_pDTE != NULL;
}


inline CComPtr<EnvDTE::_DTE>& ObjModelHelper::GetInterface()
{
	return g_pDTE;
}


inline void ObjModelHelper::SetInterface(CComPtr<EnvDTE::_DTE>& pDTE)
{
	g_pDTE = pDTE;
}


inline CString ObjModelHelper::GetVersion()
{
	CComBSTR version;
	g_pDTE->get_Version(&version);
	return CString(version);
}


inline bool ObjModelHelper::SaveDocument(const CString& filename)
{
	if (m_pDocument)
	{
		EnvDTE::vsSaveStatus saveStatus;
		if (FAILED(m_pDocument->Save(CComBSTR(filename), &saveStatus)))
			return false;
	}

	return true;
}


inline bool ObjModelHelper::FindText(CString searchString, CStringArray& flagsArray)
{
	long flags = 0;
	for (int i = 0; i < flagsArray.GetCount(); ++i)
	{
		CString flagStr = flagsArray[i];
		flagStr.MakeLower();
		if (flagStr == "matchforward")
			flags |= 0;
		else if (flagStr == "matchbackward")
			flags |= EnvDTE::vsFindOptionsBackwards;
		else if (flagStr == "matchfromstart")
			flags |= EnvDTE::vsFindOptionsFromStart;
		else if (flagStr == "matchword")
			flags |= EnvDTE::vsFindOptionsMatchWholeWord;
		else if (flagStr == "matchcase")
			flags |= EnvDTE::vsFindOptionsMatchCase;
		else if (flagStr == "matchregexp")
			flags |= EnvDTE::vsFindOptionsRegularExpression;
	}

	short findReturn;
	m_pTextSelection->FindTextA(CComBSTR(searchString), flags, &findReturn);
	return (findReturn == VARIANT_TRUE);
}

		
inline bool ObjModelHelper::FindText(CString searchString, CString flagStr)
{
	CStringArray strArray;
	strArray.Add(flagStr);
	return FindText(searchString, strArray);
}

		
inline bool ObjModelHelper::FindText(CString searchString, long flags)
{
	short findReturn;
	m_pTextSelection->FindTextA(CComBSTR(searchString), flags, &findReturn);
	return (findReturn == VARIANT_TRUE);
}


inline void ObjModelHelper::SelectLine()
{
	m_pTextSelection->EndOfLine( VARIANT_FALSE );
	m_pTextSelection->StartOfLine( EnvDTE::vsStartOfLineOptionsFirstColumn, VARIANT_TRUE );
}


inline CString ObjModelHelper::GetText()
{
	CComBSTR bstrLine;
	m_pTextSelection->get_Text(&bstrLine);
	return CString(bstrLine);
}

	
inline void ObjModelHelper::PutLanguage(LPCTSTR language)
{
	if (m_pTextDocument)
		m_pTextDocument->put_Language(CComBSTR(language));
}


inline void ObjModelHelper::GetPosition(long& x, long& y)
{
	m_pTextSelection->get_CurrentColumn(&x);
	m_pTextSelection->get_CurrentLine(&y);
}

	
inline bool ObjModelHelper::SetReadOnly(bool readOnly)
{
	bool succeeded = false;
	if (m_pDocument)
	{
		if (SUCCEEDED(m_pDocument->put_ReadOnly((readOnly) ? VARIANT_FALSE : VARIANT_TRUE)))
			succeeded = true;
	}

	return succeeded;
}


inline void ObjModelHelper::MoveTo(int line, int column, bool extend)
{
	m_pTextSelection->MoveTo(line, column, extend ? VARIANT_TRUE : VARIANT_FALSE);
}

		
inline void ObjModelHelper::EndOfDocument(bool extend)
{
	m_pTextSelection->EndOfDocument(extend ? VARIANT_TRUE : VARIANT_FALSE);
}

		
inline void ObjModelHelper::Reset()
{
	m_pTextSelection = NULL;
	m_pDocument = NULL;
	m_pTextDocument = m_pDocument;
}


inline void ObjModelHelper::SetupTextDocument()
{
	m_pTextDocument.Release();

	if (m_pDocument)
	{
		CComPtr<IDispatch> pDisp;
		m_pDocument->Object(CComBSTR(L"TextDocument"), &pDisp);
		if (pDisp)
		{
			pDisp->QueryInterface(__uuidof(EnvDTE::TextDocument), (LPVOID*)&m_pTextDocument);
		}
	}

	// Get the tab size here, after the file has been saved, so the
	// extension is known.
	m_tabSize = 0;
	if (m_pTextDocument)
	{
		m_pTextDocument->get_TabSize(&m_tabSize);
//		m_pTextDocument->put_Active(VARIANT_TRUE);
	}
}


inline void ObjModelHelper::SetupTextSelection()
{
	m_pTextSelection = NULL;

	if (m_pTextDocument)
		m_pTextDocument->get_Selection(&m_pTextSelection);
}


inline void ObjModelHelper::PutText(CString text)
{
	SetupTextSelection();

	CComPtr<EnvDTE::VirtualPoint> pTextPoint;
	CComPtr<EnvDTE::EditPoint> pEditPoint;
//	m_pTextDocument->get_StartPoint(&pTextPoint);
	m_pTextSelection->get_TopPoint(&pTextPoint);
	if (pTextPoint)
	{
		pTextPoint->CreateEditPoint(&pEditPoint);
		pEditPoint->Insert(CComBSTR(text));
	}
}


inline void ObjModelHelper::Delete()
{
	m_pTextSelection->Delete(1);
}


inline bool ObjModelHelper::IsTextDocument()
{
	return m_pTextDocument  &&  m_pTextSelection;
}


inline bool ObjModelHelper::OpenDocument(LPCTSTR fileName, LPCTSTR type)
{
	Reset();

	if (type == NULL  ||  strcmp(type, "Auto") == 0)
		type = EnvDTE::vsViewKindAny;

	CComPtr<EnvDTE::ItemOperations> pItemOperations;
	if (FAILED(g_pDTE->get_ItemOperations(&pItemOperations)))
		return false;

	EnvDTE::WindowPtr window;
	pItemOperations->OpenFile(CComBSTR(fileName), CComBSTR(type), &window);

	if (window)
	{
		window->get_Document(&m_pDocument);
	}

	SetupTextDocument();
	SetupTextSelection();

	return (window != NULL);
}


inline bool ObjModelHelper::GetActiveDocument()
{
	Reset();

	if (FAILED(g_pDTE->get_ActiveDocument(&m_pDocument)))
		return false;
	
	if (m_pDocument)
	{
        SetupTextDocument();
        SetupTextSelection();

		return true;
	}

	return false;
}


inline CString ObjModelHelper::GetFilename()
{
	CComBSTR bstrFilename;
	if (m_pDocument)
		m_pDocument->get_FullName(&bstrFilename);
	return CString(bstrFilename);
}


inline void ObjModelHelper::Refresh()
{
	// In case of a second call to Refresh().
	m_pTextSelection.Release();
	m_pTextDocument.Release();

/*	// Get the active document.  If there is one, set up the m_helper->m_pTextSelection
	// pointer.
	g_pApplication->get_ActiveDocument(&m_pDispDocument);
	if (m_pDispDocument)
	{
		m_pTextDocument = m_pDispDocument;
		if (m_pTextDocument)
		{
			m_pTextDocument->get_Selection(&m_pDispTextSelection);
			m_pTextSelection = m_pDispTextSelection;

			// Get the tab size here, after the file has been saved, so the
			// extension is known.
			if (FAILED(m_pTextDocument->get_TabSize(&m_tabSize)))
			{
				throw TException(TException::UNABLE_TO_CREATE_NEW_FILE, "Unable to query the new file.");
			}
		}
	}
	m_pDispDocument = NULL;
	g_pApplication->get_Documents(&m_pDispDocuments);
	m_pDocuments = m_pDispDocuments;
	m_pDispDocuments = NULL;
*/
}


inline void ObjModelHelper::ScrollToCenter()
{
	CComPtr<EnvDTE::VirtualPoint> pTextPoint;
	m_pTextSelection->get_ActivePoint(&pTextPoint);
	if (pTextPoint)
	{
		VARIANT_BOOL result;
		pTextPoint->TryToShow(EnvDTE::vsPaneShowCentered, CComVariant(0), &result);
	}
//	g_pDTE->ExecuteCommand(CComBSTR("Edit.ScrollLineCenter"), CComBSTR());
}


inline CString ObjModelHelper::GetCurrentProjectName()
{
	if (!g_pDTE)
	{
		return "";
	}

	// Get the current project
	CComPtr<EnvDTE::_Solution> pSolution;
	CComPtr<EnvDTE::SolutionBuild> pSolutionBuild;
	CComPtr<EnvDTE::SolutionConfiguration> pSolutionConfiguration;
	g_pDTE->get_Solution(&pSolution);
	if (!pSolution)
	{
//		return "Win32 (x86)";
		return "";
	}

	pSolution->get_SolutionBuild(&pSolutionBuild);
	if (!pSolutionBuild)
	{
//		return "Win32 (x86)";
		return "";
	}

	CComBSTR bstrConfig;

	CComPtr<IDispatch> pActiveProjectDisp;
	CComPtr<EnvDTE::Project> pActiveProject;
	CComVariant varActiveSolutionProjects;
	g_pDTE->get_ActiveSolutionProjects(&varActiveSolutionProjects);
	if(varActiveSolutionProjects.vt == (VT_ARRAY|VT_VARIANT))
	{
		CComVariant varProj;

		LONG lLBound, lUBound;
		SafeArrayGetLBound(varActiveSolutionProjects.parray, 1, &lLBound);
		SafeArrayGetUBound(varActiveSolutionProjects.parray, 1, &lUBound);

		if((lUBound - lLBound) != 0)
		{
			return "";
		}

		SafeArrayGetElement(varActiveSolutionProjects.parray, &lLBound, &varProj);
		pActiveProjectDisp = varProj.pdispVal;
//		pActiveProject->get_Object(&pActiveProjectDisp);
		pActiveProject = pActiveProjectDisp;
//		pProj->get_Name(&bstrName);
	}


	if (pActiveProject)
	{
		CComBSTR bstrProjectName;
		pActiveProject->get_FullName(&bstrProjectName);
		CString projectName(bstrProjectName);
		return projectName;
	}

	return "";
}


inline void ObjModelHelper::GetActiveProject(CComPtr<EnvDTE::Project>& pProject)
{
	pProject = NULL;

	if (!g_pDTE)
	{
		return;
	}

	// Get the current project
	CComPtr<EnvDTE::_Solution> pSolution;
	CComPtr<EnvDTE::SolutionBuild> pSolutionBuild;
	CComPtr<EnvDTE::SolutionConfiguration> pSolutionConfiguration;
	g_pDTE->get_Solution(&pSolution);
	if (!pSolution)
	{
		return;
	}

	pSolution->get_SolutionBuild(&pSolutionBuild);
	if (!pSolutionBuild)
	{
		return;
	}

	CComBSTR bstrConfig;

	CComPtr<IDispatch> pActiveProjectDisp;
	CComVariant varActiveSolutionProjects;
	g_pDTE->get_ActiveSolutionProjects(&varActiveSolutionProjects);
	if(varActiveSolutionProjects.vt == (VT_ARRAY|VT_VARIANT))
	{
		CComVariant varProj;

		LONG lLBound, lUBound;
		SafeArrayGetLBound(varActiveSolutionProjects.parray, 1, &lLBound);
		SafeArrayGetUBound(varActiveSolutionProjects.parray, 1, &lUBound);

		if((lUBound - lLBound) != 0)
		{
			return;
		}

		SafeArrayGetElement(varActiveSolutionProjects.parray, &lLBound, &varProj);
		pActiveProjectDisp = varProj.pdispVal;
		pProject = pActiveProjectDisp;
	}
}


inline bool ObjModelHelper::ExecuteCommand(CString command, CString args)
{
	if (SUCCEEDED(g_pDTE->ExecuteCommand(CComBSTR(command), CComBSTR(args))))
		return true;
	return false;
}


#endif WWHIZ_VSNET
