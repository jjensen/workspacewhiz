///////////////////////////////////////////////////////////////////////////////
// $Workfile: TemplateWizardDialog.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/TemplateWizardDialog.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_TEMPLATEWIZARDDIALOG_H__D5BAEAA2_662B_11D3_9B27_A0D222CC8952__INCLUDED_)
#define AFX_TEMPLATEWIZARDDIALOG_H__D5BAEAA2_662B_11D3_9B27_A0D222CC8952__INCLUDED_

#pragma once
// TemplateWizardDialog.h : header file
//

#if _MSC_VER < 1200
#include <ExDisp.h>
#endif //_MSC_VER

/////////////////////////////////////////////////////////////////////////////
// CTemplateWizardDialog dialog

interface IWebBrowser2;

#define TEMPLATE_WIZARD_DIALOG CHtmlHelpDialog

class CTemplateWizardDialog : public TEMPLATE_WIZARD_DIALOG
{
// Construction
	DECLARE_EVENTSINK_MAP()
public:
	CTemplateWizardDialog(WWhizTemplate& code, CWnd* pParent = NULL);   // standard constructor
	~CTemplateWizardDialog();

	bool SetCustomPage(const CString& page, bool nextOn, bool previousOn, bool finishOn);
	CString m_lastButtonHit;

// Dialog Data
	//{{AFX_DATA(CTemplateWizardDialog)
	enum { IDD = IDD_TEMPLATEWIZARD };
	CButton	m_butCancel;
	CButton	m_previousButton;
	CButton	m_nextButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateWizardDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd m_wndBrowser;
	IWebBrowser2* m_pBrowserApp;

	bool m_nextOn;
	bool m_previousOn;
	bool m_finishOn;

	MemFile m_htmlFile;
	WWhizTemplate& m_code;
	CString m_page;
	int m_curPage;

	CMapStringToString m_params;
	CString m_asciiFilename;

	void GreyButtons();
	void Render();
	void UpdateFromHtml();
	void WriteString(LPCTSTR str);

	virtual void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

	// Generated message map functions
	//{{AFX_MSG(CTemplateWizardDialog)
	afx_msg void OnTwNext();
	afx_msg void OnTwPrevious();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/**
**/
class WWhizTemplateDialogImpl : public WWhizTemplateDialog
{
public:
	virtual CString Run(WWhizTemplate& code, const CString& page, bool nextOn,
			bool previousOn, bool finishOn)
	{
		CTemplateWizardDialog dlg(code);
		dlg.SetCustomPage(page, nextOn, previousOn, finishOn);
		dlg.DoModal();

		return dlg.m_lastButtonHit;		
	}
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEWIZARDDIALOG_H__D5BAEAA2_662B_11D3_9B27_A0D222CC8952__INCLUDED_)
