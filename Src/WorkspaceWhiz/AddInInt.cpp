///////////////////////////////////////////////////////////////////////////////
// $Workfile: AddInInt.cpp $
// $Archive: /WorkspaceWhiz/Src/WorkspaceWhiz/AddInInt.cpp $
// $Date:: 1/03/01 12:13a  $ $Revision:: 12   $ $Author: Jjensen $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz! source distribution and
// is Copyright 1997-2001 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WorkspaceCommands.h"
#include "History.h"

#define ERR_SUCCESS             0

extern int HeaderFlip(LPCTSTR fullNameStr);

int OnHeaderFlip(LPCTSTR pParams)
{
	bool didAdd = History::PreAdd();
	int ret = HeaderFlip(pParams);
	if (ret == 0)
		History::Add();
	else if (didAdd)
		History::UndoAdd();
	return ret;
}

/**
**/
int OnGetWWhizInterface2(LPCTSTR pParams)
{
	return (int)g_wwhizInterface;
}

struct command_t
{
    LPCTSTR pName;
    int (*pFn)(LPCTSTR pParams);
};

static command_t commands[] = 
{
    { "HeaderFlip",			OnHeaderFlip },
    { "HF",					OnHeaderFlip },
	{ "GetWWhizInterface2",	OnGetWWhizInterface2 },
};


// callback for AddInComm
int AddinCallback200(LPCTSTR pCmd)
{
    char buf[1024];
    int i, p;

    // isolate command string (command string -> buf)
    p = 0;
    while (pCmd[p]  &&   isspace(pCmd[p])) p++;
    while (pCmd[p]  &&  !isspace(pCmd[p])) p++;
    strncpy(buf, pCmd, p);
    buf[p] = '\0';

    // see if it's a known command
    for (i = 0; i < _countof(commands); i++)
    {
        if (_tcsicmp(buf, commands[i].pName) == 0)
        {
            // trim white space from beginning of params
            while (pCmd[p]  &&  isspace(pCmd[p])) p++;
   
            return (commands[i].pFn(pCmd + p));
        }
    }

    return -1;
}

