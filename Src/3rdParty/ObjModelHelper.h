#include <ObjModel\AppAuto.h>
#include <ObjModel\textdefs.h>
#include <ObjModel\TextAuto.h>
#include "ObjModelGUID.h"

struct IGenericDocumentVC : public IGenericDocument { };
struct IDocumentsVC : public IDocuments { };
struct ITextDocumentVC : public ITextDocument { };
struct IGenericDocumentEVC : public IGenericDocument { };
struct IDocumentsEVC : public IDocuments { };
struct ITextDocumentEVC : public ITextDocument { };

class ObjModelHelper
{
protected:
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

	long m_tabSize;

public:
	ObjModelHelper();
	void Reset();
	void Refresh();

	bool OpenDocument(LPCTSTR filename, LPCTSTR type = NULL);
	bool GetActiveDocument();
    CString GetFilename();
	void PutText(CString text);
	int GetTabSize() const				{  return m_tabSize;  }
	void MoveTo(int y, int x, long type);
	void EndOfDocument(long type);
	bool SetReadOnly(bool readOnly);
	void GetPosition(long& x, long& y);
	void PutLanguage(LPCTSTR language);
	CString GetText();
	void SelectLine();
	bool FindText(CString searchString, long flags);
	bool SaveDocument(const CString& filename);
	bool SaveAll();

	bool IsTextDocument();

protected:
	void SetupTextDocument();
	void SetupTextSelection();

};

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


inline void ObjModelHelper::MoveTo(int y, int x, long type)
{
	m_pTextSelection->MoveTo(y, x, CComVariant(type));
}

		
inline void ObjModelHelper::EndOfDocument(long type)
{
	m_pTextSelection->EndOfDocument(CComVariant(type));
}

		
inline ObjModelHelper::ObjModelHelper()
{
//	Refresh();

	Reset();
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
		m_pTextDocumentVC->get_TabSize(&m_tabSize);
	else if (m_pTextDocumentEVC)
		m_pTextDocumentEVC->get_TabSize(&m_tabSize);
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



