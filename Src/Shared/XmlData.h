///////////////////////////////////////////////////////////////////////////////
// $Workfile: XmlData.h $
// $Archive: /WorkspaceWhiz/Src/Shared/XmlData.h $
// $Date: 2003/01/05 $ $Revision: #5 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Node.h"

#include <afx.h>
#include <afxtempl.h>

/**
	Represents a node within an simple XML hierarchy.

	@see XmlData
	@see Node
**/
class XmlNode : public Node
{
public:
	/**
		A single attribute within the given XML node.
	**/
	class Attribute
	{
	public:
		/**
			@return Returns the name of the attribute.
		**/
		const CString& GetName() const		{  return m_name;  }

		/**
			@return Returns the string value of the attribute.
		**/
		const CString& GetValue() const		{  return m_value;  }

	protected:
		CString m_name;
		CString m_value;

		friend class XmlNode;
		friend class XmlData;
	};

	/**
		@return Returns the name of the node.
	**/
	const CString& GetName() const			{  return m_name;  }

	/**
		@return Returns a POSITION representative of the head of the attribute list.
	**/
	POSITION GetAttributeHead() const		{  return m_attributes.GetHeadPosition();  }

	/**
		@param pos Increments [pos] to the next attribute position after returning
			the object at [pos].
		@return Returns a reference to the Attribute object at [pos].
	**/
	Attribute& GetAttributeNext(POSITION& pos) {  return m_attributes.GetNext(pos);  }
	
	/**
		Recursively finds an XML node, starting from this node.

		@param name The name of the XML node to find.
		@return Returns the found XmlNode object pointer or NULL if [name] wasn't
			found.
	**/
	XmlNode* Find(const CString& name);

	/**
		Within the attribute list for this node, finds an attribute called
		[name].

		@param name The name of the attribute to find.
		@return Returns the found Attribute object pointer or NULL if [name] wasn't
			found.
	**/
	Attribute* FindAttribute(const CString& name);

	/**
		Assigns [value] to the attribute [name].

		@param name The name of the attribute to assign [value] to.
		@param value The value to assign to the attribute called [name].
	**/
	void SetAttribute(const CString& name, const CString& value);

protected:
	CString m_name;
	WList<Attribute> m_attributes;

	friend class XmlData;
};


/**
**/
class XmlData
{
public:
	XmlData();
	~XmlData();

	/**
		Parses a simple XML file.

		@param filename The full path name to the XML-formatted file to be parsed.
		@return Returns true if the file was parsed successfully, false otherwise.
	**/
	bool ParseXmlFile(const CString& filename);

	/**
		Parses a simple XML file.

		@param file An open CFile-derived class containing the XML-formatted
			file to be parsed.
		@return Returns true if the file was parsed successfully, false otherwise.
	**/
	bool ParseXmlFile(CFile& file);

	/**
		@return Returns the XmlNode root pointer.
	**/
	XmlNode* GetRootNode() const				{  return m_xmlRoot;  }

	/**
		Recursively finds an XML node, starting at the root.

		@param name The name of the XML node to find.
		@return Returns the found XmlNode object pointer or NULL if [name] wasn't
			found.
	**/
	XmlNode* Find(const CString& name);

protected:
	XmlNode* m_xmlRoot;
};


inline XmlData::XmlData() :
	m_xmlRoot(NULL)
{
}


