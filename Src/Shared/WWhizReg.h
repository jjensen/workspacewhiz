///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizReg.h $
// $Archive: /WorkspaceWhiz/Src/Shared/WWhizReg.h $
// $Date:: 1/03/01 12:13a  $ $Revision:: 17   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

typedef CArrayEx<const WWhizTag*, const WWhizTag*> TagArray;

#define WM_USER_REFRESH (WM_USER + 1)

struct WWhizReg
{
public:
	struct ExtraFilesInfo
	{
		bool m_active;
		CString m_name;
	};
	typedef CList<ExtraFilesInfo, ExtraFilesInfo&> ExtraFilesList;

	void (*RegistrationDialog)(void);
	
	bool (*CreateWFOControls)(CDialog* dlgParent, UINT dlgID,
			CEdit*& editCtrl, CEdit*& prefixEditCtrl, CListCtrl*& listCtrl,
			WWhizFileList** fileList, CArrayEx<int, int>* foundFiles );

	bool (*DestroyWFOControls)();
	
	bool (*CreateTagControls)(CDialog* dlgParent, UINT dlgID,
			CEdit*& editParentCtrl, CEdit*& editTagCtrl, CListCtrl*& listCtrl,
			TagArray* tagArray );

	bool (*DestroyTagControls)();

	bool (*ReadExtraFiles)(bool addToProjectList);
	bool (*WriteExtraFiles)();
	void (*GetExtraFiles)(ExtraFilesList& extraFiles);
	void (*SetExtraFiles)(const ExtraFilesList& extraFiles);

	const WWhizTag* (*GetLastMatchedTag)(void);
	bool (*GotoTag)(const WWhizTag* tag);
};

