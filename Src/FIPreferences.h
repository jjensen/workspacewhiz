#if !defined(AFX_FIPREFERENCES_H__D131B421_2ECB_11D2_A392_A37A383F5767__INCLUDED_)
#define AFX_FIPREFERENCES_H__D131B421_2ECB_11D2_A392_A37A383F5767__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FIPreferences.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFIPreferences dialog

class CFIPreferences : public CDialog
{
// Construction
public:
	CFIPreferences(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFIPreferences)
	enum { IDD = IDD_FI_PREFERENCES };
	BOOL	m_classes;
	BOOL	m_constants;
	BOOL	m_functions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFIPreferences)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFIPreferences)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIPREFERENCES_H__D131B421_2ECB_11D2_A392_A37A383F5767__INCLUDED_)
