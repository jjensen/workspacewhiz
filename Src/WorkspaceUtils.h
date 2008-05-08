// WorkspaceUtils.h : main header file for the WORKSPACEUTILS DLL
//

#if !defined(AFX_WORKSPACEUTILS_H__DEACBACC_24DD_11D2_A392_444553540000__INCLUDED_)
#define AFX_WORKSPACEUTILS_H__DEACBACC_24DD_11D2_A392_444553540000__INCLUDED_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include <ObjModel\addguid.h>
#include <ObjModel\appguid.h>
#include <ObjModel\bldguid.h>
#include <ObjModel\textguid.h>
#include <ObjModel\dbgguid.h>

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceUtilsApp

class CWorkspaceUtilsApp : public CWinApp
{
public:
	CWorkspaceUtilsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceUtilsApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CWorkspaceUtilsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACEUTILS_H__DEACBACC_24DD_11D2_A392_444553540000__INCLUDED)
