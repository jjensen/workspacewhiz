///////////////////////////////////////////////////////////////////////////////
// $Workfile: TemplateWizardDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/TemplateWizardDialog.cpp $
// $Date: 2003/01/16 $ $Revision: #7 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "TemplateWizardDialog.h"
#include <MsHtml.h>

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateWizardDialog dialog

BEGIN_EVENTSINK_MAP(CTemplateWizardDialog, TEMPLATE_WIZARD_DIALOG)
	ON_EVENT(CTemplateWizardDialog, AFX_IDW_PANE_FIRST, 259 /* DocumentComplete */, DocumentComplete, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()


//
// Function: LoadWebOCFromStream
//
// Description: Takes an IWebBrowser pointer and causes the stream to 
// be loaded into the contained HTML Document Object.
//
HRESULT LoadWebOCFromStream(IWebBrowser *pWebBrowser, IStream *pStream)
{
    HRESULT             hr;
    LPDISPATCH          pHtmlDoc = NULL;
    IPersistStreamInit *pPersistStreamInit = NULL;

    // Test for valid pointers.
    //
    if (!pWebBrowser || !pStream)
        return E_POINTER;

    // Get to the document object's IDispatch pointer.
    //
    hr = pWebBrowser->get_Document(&pHtmlDoc);
    if (hr)
        goto CleanUp;

    // Query for IPersistStreamInit
    //
    hr = pHtmlDoc->QueryInterface(IID_IPersistStreamInit, 
            (void**)&pPersistStreamInit);
    if (hr)
        goto CleanUp;

    // Tell the document we'll be reinitializing it.
    //
    hr = pPersistStreamInit->InitNew();
    if (hr)
        goto CleanUp;

    // Load the contents of the stream.
    //
    hr = pPersistStreamInit->Load(pStream);

CleanUp:
    if (pHtmlDoc)
        pHtmlDoc->Release();
    if (pPersistStreamInit)
        pPersistStreamInit->Release();
    return hr;

}


void CTemplateWizardDialog::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	UNUSED_ALWAYS(pDisp);
//	ASSERT(V_VT(URL) == VT_BSTR);

	CString str(V_BSTR(URL));

	// Load in the new file...
    HRESULT       hr;
    LPUNKNOWN     pUnkContainedBrowser = NULL;
    LPUNKNOWN     pUnkDispParam = NULL;
    IStream       *pStream = NULL;
    HGLOBAL       hHTMLText;
	CComPtr<IDispatch> pDispDocument;
//	HWND shellWnd;
//	HWND ieWnd;

    // Test for valid pointers.
    if (!m_pBrowserApp || !pDisp)
        goto CleanUp;

    // To test object equality, use COM identity rules: query both 
    // pointers for IUnknown and compare them.
    hr = m_pBrowserApp->QueryInterface(IID_IUnknown, 
        (void**)&pUnkContainedBrowser);
    if (hr)
        goto CleanUp;

    // Query the passed-in IDispatch for IUnknown.
    hr = pDisp->QueryInterface(IID_IUnknown, 
        (void**)&pUnkDispParam);
    if (hr)
        goto CleanUp;

    // If they're unequal, the event is for a subframe and we're not
    // interested.
    if (pUnkContainedBrowser != pUnkDispParam)
        goto CleanUp;

    // As a further check, make sure the URL is "about:blank".
    if (str == "about:blank")
	{
		// The string is about:blank.  This means load the correct page.
		LONG len = (LONG)m_htmlFile.GetLength();
		BYTE* data = m_htmlFile.Detach();
		hHTMLText = GlobalAlloc(GPTR, len + 1);
		if (!hHTMLText)
			goto CleanUp;

		memcpy((CHAR *)hHTMLText, (char*)data, len);
		*(char*)((char*)hHTMLText + len) = 0;

		free(data);

		hr = ::CreateStreamOnHGlobal(hHTMLText, TRUE, &pStream);
		if (hr)
			goto CleanUp;

		// Call the helper function to load the WebOC from the stream.
		//
		hr = LoadWebOCFromStream(m_pBrowserApp, pStream);

		goto CleanUp;
	}
	
/*	// Set the focus to the right window.
	shellWnd = ::FindWindowEx(m_wndBrowser.GetSafeHwnd(), NULL, "Shell DocObject View", NULL);
	ieWnd = ::FindWindowEx(shellWnd, NULL, "Internet Explorer_Server", NULL);
	::SetFocus(ieWnd);
*/
	// Set to the first available input field.
	m_pBrowserApp->get_Document(&pDispDocument);

	if (pDispDocument)
	{
		CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> pDocument(pDispDocument);

		// Get all of the HTML elements.
		CComPtr<IHTMLElementCollection> pElements;
		pDocument->get_all(&pElements);

		CComVariant nullVariant;
		CComPtr<IDispatch> pDispFirstElement;

		// Now get the INPUT elements.
		CComPtr<IDispatch> pDispInputElements;
		pElements->tags(CComVariant("INPUT"), &pDispInputElements);
		if (pDispInputElements)
		{
			CComQIPtr<IHTMLElementCollection, &IID_IHTMLElementCollection> pInputElements(pDispInputElements);
			if (pInputElements)
			{
				long count;
				pInputElements->get_length(&count);

				for (int i = 0; i < count; i++)
				{
					// Get the element, if it exists.
					CComPtr<IDispatch> pDispElement;
					pInputElements->item(CComVariant(i), nullVariant, &pDispElement);
					if (!pDispElement)
						continue;

					CComQIPtr<IHTMLElement, &IID_IHTMLElement> pElement(pDispElement);
					if (!pElement)
						continue;		//??

					CComBSTR bstrID;
					pElement->get_id(&bstrID);

					CComQIPtr<IHTMLInputElement, &IID_IHTMLInputElement> pInputElement(pElement);
					if (!pInputElement)
						continue;		//??

					// Get the type.
					CComBSTR bstrType;
					pInputElement->get_type(&bstrType);
					CString strType(bstrType);
					
					CString id(bstrID);
					CString value;
					if (!m_params.Lookup(id, value))
					{
						value = m_code.GetEntry(id);
					}

					if (strType == "text")
					{
						value = g_wwhizTemplateManager->ParseCode(value, NULL, &m_code);
						pInputElement->put_value(CComBSTR(value));

						if (!pDispFirstElement)
							pDispFirstElement = pDispElement;
					}
					else if (strType == "checkbox")
					{
						pInputElement->put_checked(value == "1" ? VARIANT_TRUE : VARIANT_FALSE);
					}
					else if (strType == "radio")
					{
						pInputElement->put_checked(value == "1" ? VARIANT_TRUE : VARIANT_FALSE);
					}
					else if (strType == "file")
					{
						CComQIPtr<IHTMLInputFileElement,
							&IID_IHTMLInputFileElement> pInputFileElement(pElement);
						if (!pInputFileElement)
							continue;		//??

						value = g_wwhizTemplateManager->ParseCode(value, NULL, &m_code);
						pInputFileElement->put_value(CComBSTR(value));

						if (!pDispFirstElement)
							pDispFirstElement = pDispElement;
					}
				}
			}
		}

		// Now get the SELECT elements.
		CComPtr<IDispatch> pDispSelectElements;
		pElements->tags(CComVariant("SELECT"), &pDispSelectElements);
		if (pDispSelectElements)
		{
			CComQIPtr<IHTMLElementCollection, &IID_IHTMLElementCollection> pSelectElements(pDispSelectElements);
			if (pSelectElements)
			{
				long count;
				pSelectElements->get_length(&count);

				for (int i = 0; i < count; i++)
				{
					// Get the element, if it exists.
					CComPtr<IDispatch> pDispElement;
					pSelectElements->item(CComVariant(i), nullVariant, &pDispElement);
					if (!pDispElement)
						continue;

					CComQIPtr<IHTMLElement, &IID_IHTMLElement> pElement(pDispElement);
					if (!pElement)
						continue;		//??

					CComBSTR bstrID;
					pElement->get_id(&bstrID);

					CComQIPtr<IHTMLSelectElement, &IID_IHTMLSelectElement> pSelectElement(pElement);
					if (!pSelectElement)
						continue;		//??

					// Get the type.
					CComBSTR bstrType;
					pSelectElement->get_type(&bstrType);
					CString strType(bstrType);
					
					CString id(bstrID);
					CString value;
					if (!m_params.Lookup(id, value))
					{
						value = m_code.GetEntry(id);
					}

					// This is the only way I can figure out to do this!

					// Match the name.
					long optionCount;
					pSelectElement->get_length(&optionCount);
					int j;
					for (j = 0; j < optionCount; j++)
					{
						// Get the item at the index.
						CComPtr<IDispatch> pDispOptionElement;
						pSelectElement->item(CComVariant(j), nullVariant, &pDispOptionElement);
						if (pDispOptionElement)
						{
							CComQIPtr<IHTMLOptionElement, &IID_IHTMLOptionElement> pOptionElement(pDispOptionElement);

							CComBSTR bstrItem;
							pOptionElement->get_text(&bstrItem);
							CString strItem(bstrItem);
							if (value == strItem)
							{
								pSelectElement->put_selectedIndex(j);
								break;
							}
						}
					}

					if (j == optionCount)
					{
						pSelectElement->put_selectedIndex(0);
					}
				}
			}
		}

		if (pDispFirstElement)
		{
			CComQIPtr<IHTMLControlElement, &IID_IHTMLControlElement> pHtmlElement(pDispFirstElement);
			pHtmlElement->focus();
			CComQIPtr<IHTMLInputTextElement, &IID_IHTMLInputTextElement> pElement(pDispFirstElement);
			if (pElement)
			{
				pElement->select();
			}
		}
	}

CleanUp:
    if (pStream)
        pStream->Release();
    if (pUnkContainedBrowser)
        pUnkContainedBrowser->Release();
    if (pUnkDispParam)
        pUnkDispParam->Release();

	if (!m_asciiFilename.IsEmpty())
	{
		_unlink(m_asciiFilename);
		m_asciiFilename.Empty();
	}
}


CTemplateWizardDialog::CTemplateWizardDialog(WWhizTemplate& code, CWnd* pParent /*=NULL*/)
	: TEMPLATE_WIZARD_DIALOG(CTemplateWizardDialog::IDD, pParent),
	m_code(code),
	m_curPage(0),
	m_pBrowserApp(NULL),
	m_nextOn(false),
	m_previousOn(false),
	m_finishOn(false)
{
	//{{AFX_DATA_INIT(CTemplateWizardDialog)
	//}}AFX_DATA_INIT
}


CTemplateWizardDialog::~CTemplateWizardDialog()
{
	if (m_pBrowserApp)
		m_pBrowserApp->Release();
}


bool CTemplateWizardDialog::SetCustomPage(const CString& page, bool nextOn,
										  bool previousOn, bool finishOn)
{
	if (page.IsEmpty())
		return false;

	m_page = page;
	m_curPage = -1;

	m_nextOn = nextOn;
	m_previousOn = previousOn;
	m_finishOn = finishOn;

	if (!m_nextOn  &&  !m_previousOn  &&  !m_finishOn)
		m_nextOn = true;

	return true;
}

	
void CTemplateWizardDialog::DoDataExchange(CDataExchange* pDX)
{
	TEMPLATE_WIZARD_DIALOG::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateWizardDialog)
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_TW_PREVIOUS, m_previousButton);
	DDX_Control(pDX, IDC_TW_NEXT, m_nextButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateWizardDialog, TEMPLATE_WIZARD_DIALOG)
	//{{AFX_MSG_MAP(CTemplateWizardDialog)
	ON_BN_CLICKED(IDC_TW_NEXT, OnTwNext)
	ON_BN_CLICKED(IDC_TW_PREVIOUS, OnTwPrevious)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateWizardDialog message handlers


void CTemplateWizardDialog::GreyButtons()
{
	if (m_curPage == -1)
	{
		m_nextButton.SetWindowText("&Next");
		m_nextButton.EnableWindow(m_nextOn);
			
		m_previousButton.EnableWindow(m_previousOn);

		if (m_finishOn)
		{
			m_nextButton.SetWindowText("&Finish");
			m_nextButton.EnableWindow(TRUE);
		}
	}
	else
	{
		// Sanity check.
		if (m_curPage + 1 == m_code.GetPageCount())
		{
			m_nextButton.SetWindowText("&Finish");
		}
		else
		{
			m_nextButton.SetWindowText("&Next");
		}

		// Do previous button.
		if (m_curPage > 0)
		{
			m_previousButton.EnableWindow(TRUE);
			UINT style = m_previousButton.GetButtonStyle();
		}
		else
		{
			m_previousButton.EnableWindow(FALSE);
			UINT style = m_previousButton.GetButtonStyle();
		}
	}
}


void CTemplateWizardDialog::UpdateFromHtml()
{
	CComPtr<IDispatch> pDispDocument;
	m_pBrowserApp->get_Document(&pDispDocument);
	if (!pDispDocument)
		return;

	CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> pDocument(pDispDocument);

	CComPtr<IHTMLElementCollection> pElements;
	pDocument->get_all(&pElements);

	CComVariant nullVariant;

	// Now get the INPUT elements.
	CComPtr<IDispatch> pDispInputElements;
	pElements->tags(CComVariant("INPUT"), &pDispInputElements);
	if (pDispInputElements)
	{
		CComQIPtr<IHTMLElementCollection, &IID_IHTMLElementCollection> pInputElements(pDispInputElements);
		if (pInputElements)
		{
			long count;
			pInputElements->get_length(&count);

			for (int i = 0; i < count; i++)
			{
				// Get the element, if it exists.
				CComPtr<IDispatch> pDispElement;
				pInputElements->item(CComVariant(i), nullVariant, &pDispElement);
				if (!pDispElement)
					continue;

				CComQIPtr<IHTMLElement, &IID_IHTMLElement> pElement(pDispElement);
				if (!pElement)
					continue;		//??

				CComBSTR bstrID;
				pElement->get_id(&bstrID);

				CComQIPtr<IHTMLInputElement, &IID_IHTMLInputElement> pInputElement(pDispElement);
				if (!pInputElement)
					continue;		//??

				// Get the type.
				CComBSTR bstrType;
				pInputElement->get_type(&bstrType);
				CString strType(bstrType);

				CString id(bstrID);
				if (strType == "text"  ||  strType == "file")
				{
					CComBSTR bstrValue;
					pInputElement->get_value(&bstrValue);
					CString value(bstrValue);
					value = g_wwhizTemplateManager->ParseCode(value, NULL, &m_code);

					m_params[id] = value;
				}
				else if (strType == "checkbox")
				{
					short variant;
					pInputElement->get_checked(&variant);
					if (variant == VARIANT_FALSE)
						m_params[id] = "0";
					else
						m_params[id] = "1";
				}
				else if (strType == "radio")
				{
					short variant;
					pInputElement->get_checked(&variant);
					if (variant == VARIANT_FALSE)
						m_params[id] = "0";
					else
						m_params[id] = "1";
				}
			}
		}
	}

	// Now get the SELECT elements.
	CComPtr<IDispatch> pDispSelectElements;
	pElements->tags(CComVariant("SELECT"), &pDispSelectElements);
	if (pDispSelectElements)
	{
		CComQIPtr<IHTMLElementCollection, &IID_IHTMLElementCollection> pSelectElements(pDispSelectElements);
		if (pSelectElements)
		{
			long count;
			pSelectElements->get_length(&count);

			for (int i = 0; i < count; i++)
			{
				// Get the element, if it exists.
				CComPtr<IDispatch> pDispElement;
				pSelectElements->item(CComVariant(i), nullVariant, &pDispElement);
				if (!pDispElement)
					continue;

				CComQIPtr<IHTMLElement, &IID_IHTMLElement> pElement(pDispElement);
				if (!pElement)
					continue;		//??

				CComBSTR bstrID;
				pElement->get_id(&bstrID);
				CString id(bstrID);
				if (id.IsEmpty())
					continue;

				CComQIPtr<IHTMLSelectElement, &IID_IHTMLSelectElement> pSelectElement(pElement);
				if (!pSelectElement)
					continue;		//??

				// Get the type.
				CComBSTR bstrType;
				pSelectElement->get_type(&bstrType);
				CString strType(bstrType);
				
				// Get the current selection index.
				long curSel;
				pSelectElement->get_selectedIndex(&curSel);

				// Get the item at the index.
				CComPtr<IDispatch> pDispOptionElement;
				pSelectElement->item(CComVariant(curSel), nullVariant, &pDispOptionElement);
				if (pDispOptionElement)
				{
					CComQIPtr<IHTMLOptionElement, &IID_IHTMLOptionElement> pOptionElement(pDispOptionElement);

					CComBSTR bstrItem;
					pOptionElement->get_text(&bstrItem);
					m_params[id] = bstrItem;
				}
			}
		}
	}
}


void CTemplateWizardDialog::OnTwNext() 
{
	if (m_finishOn)
		m_lastButtonHit = "finish";
	else
		m_lastButtonHit = "next";

	UpdateFromHtml();

	if (m_curPage == -1)
	{
		OnOK();
		return;
	}

	m_curPage++;
	
	// Sanity check.
	if (m_curPage == m_code.GetPageCount())
	{
		OnOK();
		return;
	}

	Render();
}

void CTemplateWizardDialog::OnTwPrevious() 
{
	m_lastButtonHit = "previous";

	if (m_curPage == -1)
	{
		OnOK();
		return;
	}

	m_curPage--;
	
	// Sanity check.
	if (m_curPage < 0)
		m_curPage = 0;

	UpdateFromHtml();

	Render();
}

void CTemplateWizardDialog::OnOK() 
{
	// Fill in the parameters.
	POSITION pos = m_params.GetStartPosition();
	while (pos)
	{
		CString entry;
		CString value;
		m_params.GetNextAssoc(pos, entry, value);
		m_code.SetEntry(entry, value);
	}
	
	TEMPLATE_WIZARD_DIALOG::OnOK();
}


BOOL CTemplateWizardDialog::OnInitDialog() 
{
	TEMPLATE_WIZARD_DIALOG::OnInitDialog();
	
	// Set the icon for this dialog.
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_WWHIZ);
	SetIcon(hIcon, TRUE);			// Set big icon
	SetIcon(hIcon, FALSE);		// Set small icon

	CRect dlgClient;
	GetClientRect(dlgClient);
	ClientToScreen(dlgClient);

	CRect rectClient;
	GetDlgItem(IDC_TW_HTML)->GetWindowRect(rectClient);
	rectClient.OffsetRect(-dlgClient.left, -dlgClient.top);
	GetDlgItem(IDC_TW_HTML)->DestroyWindow();
	
	m_wndBrowser.SetWindowPos(NULL, rectClient.left, rectClient.top,
		rectClient.Width(), rectClient.Height(), SWP_NOZORDER);

	// Resizing stuff.
	AddSzControl(*GetDlgItem(IDCANCEL),				mdRepos,	mdRepos);
	AddSzControl(*GetDlgItem(IDC_TW_PREVIOUS),		mdRepos,	mdRepos);
	AddSzControl(*GetDlgItem(IDC_TW_NEXT),			mdRepos,	mdRepos);

	AddSzControl(m_wndBrowser,						mdResize,	mdResize);

	Render();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CTemplateWizardDialog::WriteString(LPCTSTR str)
{
	m_htmlFile.Write(str, strlen(str));
}

	
void CTemplateWizardDialog::Render()
{
	// Enable and disable the proper buttons.
	GreyButtons();

	// Set the new title.
	CString title;
	GetWindowText(title);
	int parenPos = title.Find(" (", 0);
	if (parenPos != -1)
		title = title.Left(parenPos);
	CString newTitle;
	if (m_curPage == -1)
	{
		newTitle = title;
	}
	else
	{
		newTitle.Format("%s (Page %d of %d)", title, m_curPage + 1, m_code.GetPageCount());
	}
	SetWindowText(newTitle);

	// Get the module name and strip the module filename from it, leaving the
	// module path.
	TCHAR moduleName[_MAX_PATH];
	moduleName[0] = 0;
	::GetModuleFileName(AfxGetInstanceHandle(), (TCHAR*)&moduleName, _MAX_PATH);
	TCHAR* ptr = _tcsrchr(moduleName, '\\');
	if (ptr)
	{
		ptr++;
		*ptr = 0;
	}

	///////////////////////////////////////////////////////////////////////////
	// Render the page.
	const CString& pageStr = (m_curPage == -1) ? m_page : m_code.GetPage(m_curPage);

	CRect clientRect;
	GetClientRect(clientRect);

	int curPos = 0;
//	while (true)
//	{
		// Grab the text.
		CString staticStr = pageStr;

//	}

	// See if it is a special case of a URL.
	bool isURL = false;
	CString leftStaticStr = staticStr.Left(7);
	if (leftStaticStr == "http://"  ||  leftStaticStr == "file://")
	{
		isURL = true;
	}

	CString strURL;
	if (!isURL)
	{
		m_htmlFile.SetLength(0);
		if (strnicmp(staticStr, "<html>", 6) != 0)
		{
			WriteString("<html><head>");
			WriteString("</head><body>");
			WriteString(staticStr);
			WriteString("</body></html>");
		}
		else
		{
			WriteString(staticStr);
		}

		// Generate a unique temporary name.
		char* asciiTempName = _tempnam(NULL, "WW200_");
		m_asciiFilename = CString(asciiTempName);
		free(asciiTempName);

		DWORD size = (DWORD)m_htmlFile.GetLength();
		BYTE* mem = m_htmlFile.Detach();
		CFile asciiFile;
		asciiFile.Open(m_asciiFilename, CFile::modeCreate | CFile::modeWrite);
		asciiFile.Write(mem, size);
		asciiFile.Close();
		free(mem);

//	CComBSTR bstrURL = "about:blank";
//	m_pBrowserApp->Navigate(bstrURL, NULL, NULL, NULL, NULL);

		strURL = m_asciiFilename;
	}
	else
	{
		m_asciiFilename.Empty();
		strURL = pageStr;
		strURL = "http://workspacewhiz.com";
	}

	m_pBrowserApp->Navigate2(COleVariant(strURL), NULL, NULL, NULL, NULL);
}



int CTemplateWizardDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (TEMPLATE_WIZARD_DIALOG::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	AfxEnableControlContainer();

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, "",
				WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}

	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}

	return 0;
}

void CTemplateWizardDialog::OnDestroy() 
{
	TEMPLATE_WIZARD_DIALOG::OnDestroy();
	
	if (m_pBrowserApp)
	{
		m_pBrowserApp->Release();
		m_pBrowserApp = NULL;
	}
}

void CTemplateWizardDialog::OnCancel() 
{
	m_lastButtonHit = "cancel";
	
	TEMPLATE_WIZARD_DIALOG::OnCancel();
}
