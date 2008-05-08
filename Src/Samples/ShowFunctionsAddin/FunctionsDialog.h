///////////////////////////////////////////////////////////////////////////////
// $Workfile: FunctionsDialog.h $
// $Archive: /WorkspaceWhiz/Src/Samples/ShowFunctionsAddin/FunctionsDialog.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_FUNCTIONSDIALOG_H__1762B741_7F5C_411C_ADD6_9744CBE21A1B__INCLUDED_)
#define AFX_FUNCTIONSDIALOG_H__1762B741_7F5C_411C_ADD6_9744CBE21A1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FunctionsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFunctionsDialog dialog

class CFunctionsDialog : public CDialog
{
// Construction
public:
	CFunctionsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFunctionsDialog)
	enum { IDD = IDD_FUNCTIONS };
	CRichEditCtrl	m_functionCode;
	CListBox	m_functionList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFunctionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	WWhizTagList* m_orderedTagList;

	// Generated message map functions
	//{{AFX_MSG(CFunctionsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFunctionlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FUNCTIONSDIALOG_H__1762B741_7F5C_411C_ADD6_9744CBE21A1B__INCLUDED_)
