//
//  iPhoneSocialNetworkingManager.h
//  Torque2D for iPhone
//
//  Created by Ian Eborn
//  Copyright 2010 Luma Arcade. All rights reserved.
//

#include "console/console.h"

#if defined(_USE_SOCIAL_NETWORKING)

#if !defined(IPHONE_SOCIAL_NETWORKING_MANAGER)
#define IPHONE_SOCIAL_NETWORKING_MANAGER

#import "TGBAppDelegate.h"

class iPhoneSocialNetworkingManager
{
    public:
        iPhoneSocialNetworkingManager();
        
        ~iPhoneSocialNetworkingManager();

        virtual bool socialNetworkingShow(int view, bool allowUserSwitching);

        virtual bool socialNetworkingHide(bool animatedIfApplicable);

        virtual bool socialNetworkingShowProfilePicker();

        virtual bool socialNetworkingShowProfile();

        virtual bool socialNetworkingShowFriends();

        virtual bool socialNetworkingShowAwards();

        virtual bool socialNetworkingShowScores(const char* scoreBoardID);

        virtual bool socialNetworkingSubmitScore(S32 score, const char* metaData, const char* scoreBoardID);

        virtual bool socialNetworkingUnlockAward(const char* awardID);
    
        virtual bool socialNetworkingToResignActive();
    
        virtual bool socialNetworkingBecameActive();
    
        virtual bool socialNetworkingSetInterfaceOrientation(UIInterfaceOrientation newOrientation);
};

extern iPhoneSocialNetworkingManager* socialNetworkingManager;

extern bool socialNetworkingInit(TGBAppDelegate* appDelegate);

extern bool socialNetworkingCleanup();

void socialNetworkingDeleteManager();

#endif // IPHONE_SOCIAL_NETWORKING_MANAGER

#endif // _USE_SOCIAL_NETWORKING
