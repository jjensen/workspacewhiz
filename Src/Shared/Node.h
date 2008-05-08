///////////////////////////////////////////////////////////////////////////////
// $Workfile: Node.h $
// $Archive: /WorkspaceWhiz/Src/Shared/Node.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <stdio.h>

/**
	Represents a node in a tree hierarchy.
**/
class Node
{
public:
	Node();
	virtual ~Node();

	/**
		@return Returns the parent of the current node, or NULL if there is no
			parent.
	**/
	Node* GetParent(void)					{  return m_parentNode;  }

	/**
		@return Returns the next sibling node or NULL if there are no more
			siblings.
	**/
	Node* GetNextSiblingNode(void)			{  return m_nextNode;  }

	/**
		@return Returns the previous sibling node or NULL if there are no more
			siblings.
	**/
	Node* GetPrevSiblingNode(void)			{  return m_prevNode;  }

	/**
		@return Returns the first child node of the current node or NULL 
			if there are no children.
	**/
	Node* GetFirstChildNode(void)			{  return m_firstChildNode;  }

	/**
		@return Returns the last child node of the current node or NULL 
			if there are no children.
	**/
	Node* GetLastChildNode(void)			{  return m_lastChildNode;  }

	/**
		Recursively destroys all children nodes.
	**/
	void DestroyChildNodes(void);

	/**
		Attachs the current node instance to the [parentNode] children. Inserted
		after [insertAfterNode].

		@param parentNode The node to attach this node to as a child.
		@param insertAfterNode The node to insert this child after (must be
			one of the parent's children) or NULL to insert at the end of the
			child list.
	**/
	void AttachNode(Node* parentNode, Node* insertAfterNode = NULL);

	/**
		Detaches the current node from the parent.
	**/
	void DetachNode();

private:
	Node* m_parentNode;
	Node* m_nextNode;
	Node* m_prevNode;
	Node* m_firstChildNode;
	Node* m_lastChildNode;
};


