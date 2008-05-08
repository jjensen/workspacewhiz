///////////////////////////////////////////////////////////////////////////////
// $Workfile: HtmlHelpDialog.h $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/HtmlHelpDialog.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 8    $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <afxcmn.h>

#define DECLARE_HTMLHELPDIALOG(classname, baseclass)		\
protected:													\
	DECLARE_DYNCREATE(classname)							\
	afx_msg void OnHelp();									\
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);	\
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);			\
    DECLARE_MESSAGE_MAP()									\
	int m_id

class CHtmlHelpDialog : public cdxCDynamicDialog
{
// Construction
public:
	CHtmlHelpDialog() {}
	CHtmlHelpDialog(UINT nIDTemplate, CWnd* pParent = NULL) :
		cdxCDynamicDialog(nIDTemplate, pParent) { m_id = nIDTemplate; };   // standard constructor

	DECLARE_HTMLHELPDIALOG(CHtmlHelpDialog, cdxCDynamicDialog);
};

class CHtmlHelpPropPage : public CPropertyPage
{
// Construction
public:
	CHtmlHelpPropPage() {}
    CHtmlHelpPropPage(UINT nIDTemplate) :
        CPropertyPage(nIDTemplate) {}

    CHtmlHelpPropPage(LPCTSTR lpszTemplateName) :
        CPropertyPage(lpszTemplateName) {}

    DECLARE_HTMLHELPDIALOG(CHtmlHelpPropPage, CPropertyPage);
};


class CHtmlHelpPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CHtmlHelpPropSheet)

// Construction
public:
	CHtmlHelpPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, 
        UINT iSelectPage = 0) : 
        CPropertySheet(nIDCaption, pParentWnd, iSelectPage) {}
	CHtmlHelpPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, 
        UINT iSelectPage = 0) :
        CPropertySheet(pszCaption, pParentWnd, iSelectPage) {}

protected:
    virtual BOOL OnInitDialog();
};

