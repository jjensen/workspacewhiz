///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizReg.h $
// $Archive: /WorkspaceWhiz/Src/Shared/WWhizReg.h $
// $Date: 2003/01/05 $ $Revision: #6 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

typedef WArray<const WWhizTag*> TagArray;

#define WM_USER_REFRESH (WM_USER + 1)

struct WWhizReg
{
public:
	struct ExtraFilesInfo
	{
		CString m_name;
		bool m_active:1;
		bool m_noRefresh:1;
	};
	typedef WList<ExtraFilesInfo> ExtraFilesList;

	void (*RegistrationDialog)(void);
	
	bool (*CreateWFOControls)(CDialog* dlgParent, UINT dlgID,
			CComboBoxEx*& editCtrl, CComboBoxEx*& prefixEditCtrl, CListCtrl*& listCtrl,
			WWhizFileList** fileList, WArray<int>* foundFiles );

	bool (*DestroyWFOControls)();

	void (*WFOAddEditString)(const CString& string);
	void (*WFOAddPrefixEditString)(const CString& string);
	
	bool (*CreateTagControls)(CDialog* dlgParent, UINT dlgID,
			CComboBoxEx*& editParentCtrl, CComboBoxEx*& editTagCtrl, CListCtrl*& listCtrl,
			TagArray* tagArray );

	void (*TagAddEditTagString)(const CString& string);
	void (*TagAddEditParentString)(const CString& string);

	bool (*DestroyTagControls)();

	bool (*ReadExtraFiles)(bool addToProjectList);
	bool (*WriteExtraFiles)();
	void (*GetExtraFiles)(ExtraFilesList& extraFiles);
	void (*SetExtraFiles)(const ExtraFilesList& extraFiles);

	const WWhizTag* (*GetLastMatchedTag)(void);
	bool (*GotoTag)(const WWhizTag* tag);
};

