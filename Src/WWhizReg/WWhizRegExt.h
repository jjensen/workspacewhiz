///////////////////////////////////////////////////////////////////////////////
// $Workfile: WWhizRegExt.h $
// $Archive: /WWhizReg/WWhizRegExt.h $
// $Date:: 4/12/01 8:19p   $ $Revision:: 7    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////

void RegistrationDialog();

bool CreateWFOControls(CDialog* dlgParent, UINT dlgID,
		CComboBoxEx*& editCtrl, CComboBoxEx*& prefixEditCtrl, CListCtrl*& listCtrl,
		WWhizFileList** fileList, WArray<int>* foundFiles );

bool DestroyWFOControls();

void WFOAddEditString(const CString& string);
void WFOAddPrefixEditString(const CString& string);

bool CreateTagControls(CDialog* dlgParent, UINT dlgID,
		CComboBoxEx*& editParentCtrl, CComboBoxEx*& editTagCtrl, CListCtrl*& listCtrl,
		TagArray* tagArray );

bool DestroyTagControls();

void TagAddEditTagString(const CString& string);
void TagAddEditParentString(const CString& string);

bool ReadExtraFiles(bool addToProjectList);
bool WriteExtraFiles();
void GetExtraFiles(WWhizReg::ExtraFilesList& extraFiles);
void SetExtraFiles(const WWhizReg::ExtraFilesList& extraFiles);

int HeaderFlip(LPCTSTR fullName);

const WWhizTag* GetLastMatchedTag(void);
bool GotoTag(const WWhizTag* tag);
