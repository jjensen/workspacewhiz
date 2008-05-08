#pragma once

extern WWhizInterface* g_wwhizInterface;
extern WWhizReg* g_wwhizReg;
extern WWhizTemplateManager* g_wwhizTemplateManager;

extern Config g_config;

class WWhizCommands
{
public:
	struct CommandInfo
	{
		LPCWSTR m_name;
		LPCTSTR m_keyStr;
		bool m_hasIcon;
		bool m_justText;
		DWORD m_bitmapID;
	};

	enum CommandEnums
	{
		Command_WWOptions,
		Command_WWFileOpen,
		Command_WWFileGlobalOpen,
		Command_WWHeaderFlip,
		Command_WWFileFindPrev,
		Command_WWFileFindNext,
		Command_WWHistoryBack,
		Command_WWHistoryForward,
		Command_WWHistoryDlg,
		Command_WWTagFindDlg,
		Command_WWTagFindSpecialDlg,
		Command_WWTagFlip,
		Command_WWTagFindAtCursor,
		Command_WWTagFindAtCursorDlg,
		Command_WWTagFindPrev,
		Command_WWTagFindNext,
		Command_WWTagCompletePrev,
		Command_WWTagCompleteNext,
		Command_WWTagCompleteRestore,
		Command_WWTagCompleteDlg,
		Command_WWTemplateSelect,
		Command_WWTemplateComplete,
	};

	static int GetCommandCount();
	static const CommandInfo* GetCommandList();
	static const CommandInfo* GetCommand(int index);

	static STDMETHODIMP WWOptions();
	static STDMETHODIMP WWFileOpen();
	static STDMETHODIMP WWFileGlobalOpen();
	static STDMETHODIMP WWHeaderFlip();
	static STDMETHODIMP WWFileFindPrev();
	static STDMETHODIMP WWFileFindNext();
	static STDMETHODIMP WWHistoryBack();
	static STDMETHODIMP WWHistoryForward();
	static STDMETHODIMP WWHistoryDlg();
	static STDMETHODIMP WWTagFindDlg();
	static STDMETHODIMP WWTagFindSpecialDlg();
	static STDMETHODIMP WWTagFindAtCursorDlg();
	static STDMETHODIMP WWTagFindAtCursor();
	static STDMETHODIMP WWTagFlip();
	static STDMETHODIMP WWTagFindNext();
	static STDMETHODIMP WWTagFindPrev();
	static STDMETHODIMP WWTagCompleteNext();
	static STDMETHODIMP WWTagCompletePrev();
	static STDMETHODIMP WWTagCompleteRestore();
	static STDMETHODIMP WWTagCompleteDlg();
	static STDMETHODIMP WWTemplateSelect();
	static STDMETHODIMP WWTemplateComplete();

	static void LoadTags();
	static void SaveTags();
	static void OnWorkspaceOpen();
	static void OnWorkspaceClose();
	static void OnWorkspaceNew();
	static void OnDocumentSave();
	static void OnShutdown();

	static void SetDefaultWWhizDirectory();
	static void EnableMainWindow(BOOL enable);
	static int OnHeaderFlip(LPCTSTR pParams);

	static int PutTemplate(WWhizTemplate& code);
};



