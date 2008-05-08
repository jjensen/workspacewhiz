///////////////////////////////////////////////////////////////////////////////
// $Workfile: Node.cpp $
// $Archive: /WorkspaceWhiz/Src/Shared/Node.cpp $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#include "Node.h"

Node::Node() :
	m_parentNode(NULL),
	m_nextNode(NULL),
	m_prevNode(NULL),
	m_firstChildNode(NULL),
	m_lastChildNode(NULL)
{
}


Node::~Node()
{
	DetachNode();

	DestroyChildNodes();
}


void Node::DestroyChildNodes()
{					  
	Node* childNode = m_firstChildNode;
	while (childNode)
	{
		Node* nextNode = childNode->m_nextNode;
		childNode->DestroyChildNodes();
		delete childNode;
		childNode = nextNode;
	}
}


void Node::AttachNode(Node* parentNode, Node* insertAfterNode)
{
	// OK, insert the node at the end of the list.  (Draw it on paper...
	//   it's easier to see.
	if (!parentNode->m_firstChildNode)
	{
		// Link it at the start.
		m_nextNode = m_prevNode = NULL;
		parentNode->m_firstChildNode = parentNode->m_lastChildNode = this;
	}
	else
	{
		// Link it elsewhere.
		if (!insertAfterNode)
			insertAfterNode = parentNode->m_lastChildNode;

		m_nextNode = insertAfterNode->m_nextNode;
		m_prevNode = insertAfterNode;

		if (parentNode->m_lastChildNode == insertAfterNode)
		{
			parentNode->m_lastChildNode->m_nextNode = this;
			parentNode->m_lastChildNode = this;
		}
		else
		{
			if (insertAfterNode->m_nextNode)
				insertAfterNode->m_nextNode->m_prevNode = this;
			insertAfterNode->m_nextNode = this;
		}
	}

	m_parentNode = parentNode;
}


void Node::DetachNode()
{
	if (m_parentNode)
	{
		if (m_parentNode->m_lastChildNode == this)
			m_parentNode->m_lastChildNode = m_prevNode;
		if (m_parentNode->m_firstChildNode == this)
			m_parentNode->m_firstChildNode = m_nextNode;
		if (m_prevNode)
			m_prevNode->m_nextNode = m_nextNode;
		if (m_nextNode)
			m_nextNode->m_prevNode = m_prevNode;
		m_parentNode = NULL;
	}
}


