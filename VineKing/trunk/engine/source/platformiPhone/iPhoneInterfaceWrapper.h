//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef IPHONEINTERFACEWRAPPER_H
#define IPHONEINTERFACEWRAPPER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace ActivityIndicatorWrapper
{
	void CreateIndicator();
	void DeleteIndicator();
	void ShowIndicator();
	void HideIndicator();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace DeviceTypeWrapper
{
	enum EIOSDeviceType
	{
		DEVICE_TYPE_NONE = 0,
		
		DEVICE_TYPE_SIMULATOR,
		
		DEVICE_TYPE_IPHONE_ORIGINAL,
		DEVICE_TYPE_IPHONE_3G,
		DEVICE_TYPE_IPHONE_3GS,
		DEVICE_TYPE_IPHONE_4,
		
		DEVICE_TYPE_IPOD_TOUCH_1ST_GEN,
		DEVICE_TYPE_IPOD_TOUCH_2ND_GEN,
		DEVICE_TYPE_IPOD_TOUCH_3RD_GEN,

		DEVICE_TYPE_IPAD,
		DEVICE_TYPE_IPAD_2,
	};
	
	EIOSDeviceType GetIOSDeviceType();
	EIOSDeviceType ConvertStringToIOSDeviceTypeEnum( const char* );
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace AppStoreLinkWrapper
{
	void SendUserToVineKingInAppStore();
	void SendUserToVineKingRatingsPage();
	void SendUserToVineKingWebSite();
	void SendUserToVineKingFacebookPage();
	void SendUserToVineKingTwitterPage();
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace FacebookWrapper
{
	void OpenFeedDialog( const char* );
	bool IsLoggedIn();
	bool LogIn();
	bool LogOut();
	
	void OnLogIn();
	void OnLogOut();
	
	void RequestFacebookUserName();
	
	void OnSuccessfulFacebookPost();
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace TwitterWrapper
{
	void OpenTwitterDialog();
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // #define IPHONEINTERFACEWRAPPER_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------