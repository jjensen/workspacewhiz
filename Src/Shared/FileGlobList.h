#pragma once

#include "FileGlobBase.h"

/**
	Provides a storage list for all found files.
**/
class FileGlobList : public FileGlobBase, public std::list< std::string >
{
public:
	typedef std::list< std::string >::iterator Iterator;

	virtual void FoundMatch( const char* fileName )
	{
		// Only add the found match if it doesn't already exist in the list.
		// This could be slow for large file lists.
		for ( Iterator it = begin(); it != end(); ++it )
			if ( stricmp( (*it).c_str(), fileName ) == 0 )
				return;
		push_back( fileName );
	}

protected:

private:
};

