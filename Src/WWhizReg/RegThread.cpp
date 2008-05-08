///////////////////////////////////////////////////////////////////////////////
// $Workfile: RegThread.cpp $
// $Archive: /WorkspaceWhiz/Src/WWhizReg/RegThread.cpp $
// $Date:: 7/14/00 2:23a   $ $Revision:: 4    $ $Author: Jjensen $
//
// This source file is Copyright 1997-2000 by Joshua C. Jensen.
//
// Unauthorized use of this source file, by modification, copying, or
// distribution is strictly prohibited unless prior consent has
// been given by Joshua C. Jensen.
///////////////////////////////////////////////////////////////////////////////
#include "wwhizreg.h"
#include "RegThread.h"

#ifdef _DEBUG
#define WNEW DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_INITCHECK (WM_USER + 1)

static CWinThread* s_regThread;

static CEvent           s_EvJobs(TRUE);
static long             s_lJobs = 0;
static CSemaphore       s_SemIn;

#define PostToRegThread(msg, wParam, lParam) \
    s_regThread->PostThreadMessage((UINT)msg, (WPARAM)wParam, (LPARAM)lParam)

#define WaitForJobs() s_EvJobs.Lock()

inline void EndJob()
{
    if (InterlockedDecrement(&s_lJobs) == 0)
    {
        s_EvJobs.SetEvent();
    }
}

void StartJob2(UINT msg, WPARAM wParam, LPARAM lParam)
{
    InterlockedIncrement(&s_lJobs);
    s_EvJobs.ResetEvent();
    PostToRegThread(msg, wParam, lParam);
}

#define StartJob(msg, wParam, lParam) \
    StartJob2((UINT)(msg), (WPARAM)(wParam), (LPARAM)(lParam))



void InitReg()
{
    static volatile bool inited;

    inited = false;

    s_SemIn.Lock();

    if (!inited)
	{
		s_regThread = AfxBeginThread(RUNTIME_CLASS(CRegThread));

		while (!inited)
		{
			PostToRegThread(WM_INITCHECK, 0, &inited);
			Sleep(1);
		}
	}

    s_SemIn.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
// CRegThread

IMPLEMENT_DYNCREATE(CRegThread, CWinThread)

CRegThread::CRegThread()
{
}

CRegThread::~CRegThread()
{
}

BOOL CRegThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CRegThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CRegThread, CWinThread)
	//{{AFX_MSG_MAP(CRegThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
    ON_THREAD_MESSAGE(WM_INITCHECK, OnInitCheck)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegThread message handlers

#ifdef WWHIZ_VC6
LRESULT CRegThread::OnInitCheck(WPARAM wParam, LPARAM lParam)
{
    *((bool*)lParam) = true;
    return TRUE;
}
#endif WWHIZ_VC6
#ifdef WWHIZ_VSNET
void CRegThread::OnInitCheck(WPARAM wParam, LPARAM lParam)
{
    *((bool*)lParam) = true;
}
#endif WWHIZ_VSNET



