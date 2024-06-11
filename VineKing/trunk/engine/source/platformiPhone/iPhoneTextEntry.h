//
//  iPhoneTextEntry.h
//  iTorque 2D
//
//  Created by Sven Bergstrom
//  GarageGames, LLC. All rights reserved.
//
 

#ifndef IPHONETEXTENTRY_INCLUDED
#define IPHONETEXTENTRY_INCLUDED

#include "core/stringBuffer.h"

namespace iPhoneTextEntry
{
	// Returns false if the user cancels.
	// text [in/out]: The contents of the text entry field.  The text entry field is initialized
	// with the value of the StringBuffer.  On exit, if the function returns true, the StringBuffer
	// contains the text entered by the user; otherwise, the StringBuffer is unchanged.
	bool getUserText(StringBuffer& text);
	bool onUserTextFinished(bool cancelled, const char* &result);
}


#endif