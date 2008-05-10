////////////////////////////////////////////////////////////////
// 1997 Microsoft Systems Journal. 
// If this program works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
//
// CSubclassWnd is a generic class for hooking another window's messages.

#include "pchWWhizReg.h"
#include "Subclass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////
// The message hook map is derived from CMapPtrToPtr, which associates
// a pointer with another pointer. It maps an HWND to a CSubclassWnd, like
// the way MFC's internal maps map HWND's to CWnd's. The first CSubclassWnd
// attached to a window is stored in the map; all other CSubclassWnd's for that
// window are then chained via CSubclassWnd::m_pNext.
//
class CSubclassWndMap : private CMapPtrToPtr {
public:
	CSubclassWndMap();
	~CSubclassWndMap();
	static CSubclassWndMap& GetHookMap();
	void Add(HWND hwnd, CSubclassWnd* pSubclassWnd);
	void Remove(CSubclassWnd* pSubclassWnd);
	void RemoveAll(HWND hwnd);
	CSubclassWnd* Lookup(HWND hwnd);
};

// This trick is used so the hook map isn't
// instantiated until someone actually requests it.
//
#define	theHookMap	(CSubclassWndMap::GetHookMap())

IMPLEMENT_DYNAMIC(CSubclassWnd, CWnd);

CSubclassWnd::CSubclassWnd()
{
	m_pNext = NULL;
	m_pOldWndProc = NULL;	
	m_pWndHooked  = NULL;
}

CSubclassWnd::~CSubclassWnd()
{
	ASSERT(m_pWndHooked==NULL);		// can't destroy while still hooked!
	ASSERT(m_pOldWndProc==NULL);
}

//////////////////
// Hook a window.
// This installs a new window proc that directs messages to the CSubclassWnd.
// pWnd=NULL to remove.
//
BOOL CSubclassWnd::HookWindow(CWnd* pWnd)
{
	if (pWnd) {
		// Hook the window
		ASSERT(m_pWndHooked==NULL);
		HWND hwnd = pWnd->m_hWnd;
		ASSERT(hwnd && ::IsWindow(hwnd));
		theHookMap.Add(hwnd, this);			// Add to map of hooks

	} else {
		// Unhook the window
		ASSERT(m_pWndHooked!=NULL);
		theHookMap.Remove(this);				// Remove from map
		m_pOldWndProc = NULL;
	}
	m_pWndHooked = pWnd;
	return TRUE;
}

//////////////////
// Window proc-like virtual function which specific CSubclassWnds will
// override to do stuff. Default passes the message to the next hook; 
// the last hook passes the message to the original window.
// You MUST call this at the end of your WindowProc if you want the real
// window to get the message. This is just like CWnd::WindowProc, except that
// a CSubclassWnd is not a window.
//
LRESULT CSubclassWnd::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	ASSERT(m_pOldWndProc);
	return m_pNext ? m_pNext->WindowProc(msg, wp, lp) :	
		::CallWindowProc(m_pOldWndProc, m_pWndHooked->m_hWnd, msg, wp, lp);
}

//////////////////
// Like calling base class WindowProc, but with no args, so individual
// message handlers can do the default thing. Like CWnd::Default
//
LRESULT CSubclassWnd::Default()
{
	// MFC stores current MSG in thread state
	MSG& curMsg = AfxGetThreadState()->m_lastSentMsg;
	// Note: must explicitly call CSubclassWnd::WindowProc to avoid infinte
	// recursion on virtual function
	return CSubclassWnd::WindowProc(curMsg.message, curMsg.wParam, curMsg.lParam);
}

//////////////////
// Subclassed window proc for message hooks. Replaces AfxWndProc (or whatever
// else was there before.)
//
LRESULT CALLBACK
HookWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
#ifdef _USRDLL
	// If this is a DLL, need to set up MFC state
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	// Set up MFC message state just in case anyone wants it
	// This is just like AfxCallWindowProc, but we can't use that because
	// a CSubclassWnd is not a CWnd.
	//
	MSG& curMsg = AfxGetThreadState()->m_lastSentMsg;
	MSG  oldMsg = curMsg;   // save for nesting
	curMsg.hwnd		= hwnd;
	curMsg.message = msg;
	curMsg.wParam  = wp;
	curMsg.lParam  = lp;

	// Get hook object for this window. Get from hook map
	CSubclassWnd* pSubclassWnd = theHookMap.Lookup(hwnd);
	ASSERT(pSubclassWnd);

	LRESULT lr;
	if (msg==WM_NCDESTROY) {
		// Window is being destroyed: unhook all hooks (for this window)
		// and pass msg to orginal window proc
		//
		WNDPROC wndproc = pSubclassWnd->m_pOldWndProc;
		theHookMap.RemoveAll(hwnd);
		lr = ::CallWindowProc(wndproc, hwnd, msg, wp, lp);

	} else {
		// pass to msg hook
		lr = pSubclassWnd->WindowProc(msg, wp, lp);
	}
	curMsg = oldMsg;			// pop state
	return lr;
}

////////////////////////////////////////////////////////////////
// CSubclassWndMap implementation

CSubclassWndMap::CSubclassWndMap()
{
}

CSubclassWndMap::~CSubclassWndMap()
{
	ASSERT(IsEmpty());	// all hooks should be removed!	
}

//////////////////
// Get the one and only global hook map
// 
CSubclassWndMap& CSubclassWndMap::GetHookMap()
{
	// By creating theMap here, C++ doesn't instantiate it until/unless
	// it's ever used! This is a good trick to use in C++, to
	// instantiate/initialize a static object the first time it's used.
	//
	static CSubclassWndMap theMap;
	return theMap;
}

/////////////////
// Add hook to map; i.e., associate hook with window
//
void CSubclassWndMap::Add(HWND hwnd, CSubclassWnd* pSubclassWnd)
{
	ASSERT(hwnd && ::IsWindow(hwnd));

	// Add to front of list
	pSubclassWnd->m_pNext = Lookup(hwnd);
	SetAt(hwnd, pSubclassWnd);
	
	if (pSubclassWnd->m_pNext==NULL) {
		// If this is the first hook added, subclass the window
		pSubclassWnd->m_pOldWndProc = 
			(WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)HookWndProc);

	} else {
		// just copy wndproc from next hook
		pSubclassWnd->m_pOldWndProc = pSubclassWnd->m_pNext->m_pOldWndProc;
	}
	ASSERT(pSubclassWnd->m_pOldWndProc);
}

//////////////////
// Remove hook from map
//
void CSubclassWndMap::Remove(CSubclassWnd* pUnHook)
{
	HWND hwnd = pUnHook->m_pWndHooked->GetSafeHwnd();
	ASSERT(hwnd && ::IsWindow(hwnd));

	CSubclassWnd* pHook = Lookup(hwnd);
	ASSERT(pHook);
	if (pHook==pUnHook) {
		// hook to remove is the one in the hash table: replace w/next
		if (pHook->m_pNext)
			SetAt(hwnd, pHook->m_pNext);
		else {
			// This is the last hook for this window: restore wnd proc
			RemoveKey(hwnd);
			SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)pHook->m_pOldWndProc);
		}
	} else {
		// Hook to remove is in the middle: just remove from linked list
		while (pHook->m_pNext!=pUnHook)
			pHook = pHook->m_pNext;
		ASSERT(pHook && pHook->m_pNext==pUnHook);
		pHook->m_pNext = pUnHook->m_pNext;
	}
}

//////////////////
// Remove all the hooks for a window
//
void CSubclassWndMap::RemoveAll(HWND hwnd)
{
	CSubclassWnd* pSubclassWnd;
	while ((pSubclassWnd = Lookup(hwnd))!=NULL)
		pSubclassWnd->HookWindow(NULL);	// (unhook)
}

/////////////////
// Find first hook associate with window
//
CSubclassWnd* CSubclassWndMap::Lookup(HWND hwnd)
{
	CSubclassWnd* pFound = NULL;
	if (!CMapPtrToPtr::Lookup(hwnd, (void*&)pFound))
		return NULL;
	ASSERT_KINDOF(CSubclassWnd, pFound);
	return pFound;
}