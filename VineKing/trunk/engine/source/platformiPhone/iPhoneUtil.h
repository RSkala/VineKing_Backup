//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _iPhoneUTIL_H_
#define _iPhoneUTIL_H_


#include "game/net/tcpobject.h"



//Luma:	Orientation support
enum iPhoneOrientation
{
	iPhoneOrientationUnkown,				//All applications start in this state
	iPhoneOrientationLandscapeLeft,			//The home button is on the RIGHT
	iPhoneOrientationLandscapeRight,		//The home button is on the LEFT
	iPhoneOrientationPortrait,				//The home button is on the bottom
	iPhoneOrientationPortraitUpsideDown		//The home button is on the top
};

int _iPhoneGameGetCurrentOrientation();	
void _iPhoneGameSetCurrentOrientation(int iOrientation);	


//Luma: Ability to get the Local IP (Internal IP) for an iPhone as opposed to it's External one
void _iPhoneGetLocalIP(unsigned char *pcIPString);

//Luma: Make sure that the iPhone Radio is on before connection via TCP... NOTE: sometimes the Radio wont be ready for immediate use after this is processed... need to see why
void OpeniPhoneNetworkingAndConnectToTCPObject(TCPObject *psTCPObject, const char *pcAddress);

//Luma: ability to quickly tell if this is an iPhone or an iTouch
bool IsDeviceiPhone(void);


#endif // _iPhoneUTIL_H_
