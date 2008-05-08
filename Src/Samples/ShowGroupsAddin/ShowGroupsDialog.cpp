///////////////////////////////////////////////////////////////////////////////
// $Workfile: ShowGroupsDialog.cpp $
// $Archive: /WorkspaceWhiz/Src/Samples/ShowGroupsAddin/ShowGroupsDialog.cpp $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ShowGroupsAddin.h"
#include "ShowGroupsDialog.h"
#include "XmlData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowGroupsDialog dialog


CShowGroupsDialog::CShowGroupsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CShowGroupsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowGroupsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CShowGroupsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowGroupsDialog)
	DDX_Control(pDX, IDC_SG_TREE, m_tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowGroupsDialog, CDialog)
	//{{AFX_MSG_MAP(CShowGroupsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowGroupsDialog message handlers

/**
**/
static void RecurseFileNodes(XmlNode* parentNode, CTreeCursor cursor)
{
	// If the parent is NULL, then abort.
	if (!parentNode)
		return;

	// Start with the first child.
    XmlNode* node = (XmlNode*)parentNode->GetFirstChildNode();

    // Iterate the children until there are no more.
	while (node)
    {
		// Is it a <File> node?
		if (node->GetName() == "File")
		{
			// Get the relative path of the filename.
			XmlNode::Attribute* attr = node->FindAttribute("RelativePath");
			if (attr)
			{
				// Add it to the appropriate place in the tree control.
				cursor.AddTail(attr->GetValue());
			}
		}

		// Else is it a <Filter> node?
		else if (node->GetName() == "Filter")
		{
			// Get the name of the filter (the group name).
			XmlNode::Attribute* attr = node->FindAttribute("Name");

			// Give it a default, in case something is wrong.
			CString name = "Unknown";
			if (attr)
			{
				name = attr->GetValue();
			}

			// Add it to the appropriate place in the tree control.
			CTreeCursor childCursor = cursor.AddTail(name);

			// Recurse deeper into the XML.
			RecurseFileNodes(node, childCursor);
		}

		// Go to the next XML node.
		node = (XmlNode*)node->GetNextSiblingNode();
	}
}


BOOL CShowGroupsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Refresh the file list.
	g_wwhizInterface->RefreshFileList();

	// Get the WWhizProjectList.
	WWhizProjectList& projectList = g_wwhizInterface->GetProjectList();

	// Get the root tree item.
	CTreeCursor rootCursor = m_tree.GetRootItem();

	// Go through each project in the project list.
	for (int i = 0; i < projectList.GetProjectCount(); ++i)
	{
		// Retrieve the WWhizProject.
		WWhizProject* project = projectList.GetProjectByIndex(i);

		// Only show it if it is a workspace project.
		if (!project->IsWorkspaceProject())
			continue;

		// Add the project name to the root of the tree.
		CTreeCursor childCursor = rootCursor.AddTail(project->GetName());

		// Get the project's XML information.
		XmlData& xmlFile = project->GetXmlData();

		// Find the project's <Files> section.
		XmlNode* xmlNode = xmlFile.Find("Files");

		// Recurse the <Files> section.
		RecurseFileNodes(xmlNode, childCursor);
	}

	// Sort the root.
	rootCursor.SortChildren();

	// Set the input focus to the tree.
	m_tree.SetFocus();
	m_tree.GetRootItem().Select();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
