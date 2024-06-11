//
//  iPhoneSocialNetworkingManager.mm
//  Torque2D for iPhone
//
//  Created by Ian Eborn
//  Copyright 2010 Luma Arcade. All rights reserved.
//

#if defined(_USE_SOCIAL_NETWORKING)

#include "platformiPhone/iPhoneSocialNetworkingManager.h"

iPhoneSocialNetworkingManager* socialNetworkingManager = NULL;

void socialNetworkingDeleteManager()
{
    if(socialNetworkingManager != NULL)
    {
        delete socialNetworkingManager;
        socialNetworkingManager = NULL;
    }
}

//////////////////// SocialNetworkingManager functions

iPhoneSocialNetworkingManager::iPhoneSocialNetworkingManager()
{
}

iPhoneSocialNetworkingManager::~iPhoneSocialNetworkingManager()
{
}

bool iPhoneSocialNetworkingManager::socialNetworkingShow(int view, bool allowUserSwitching)
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingHide(bool animatedIfApplicable)
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingShowProfilePicker()
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingShowProfile()
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingShowFriends()
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingShowAwards()
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingShowScores(const char* scoreBoardID)
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingSubmitScore(S32 score, const char* metaData, const char* scoreBoardID)
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingUnlockAward(const char* awardID)
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingToResignActive()
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingBecameActive()
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}

bool iPhoneSocialNetworkingManager::socialNetworkingSetInterfaceOrientation(UIInterfaceOrientation newOrientation)
{
    Con::printf("This is the base social networking manager - a more specific version (AGON, OpenFeint, Scoreloop, etc.) should probably be used instead.");
    return false;
}


//////////////////// Console functions

ConsoleFunction(socialNetworkingShow, bool, 3, 3, "socialNetworkingShow( view, allowUserSwitching )")
{
    if(socialNetworkingManager != NULL)
    {
        return socialNetworkingManager->socialNetworkingShow(dAtoi(argv[1]), dAtob(argv[2]));
    }
    return false;
}

ConsoleFunction(socialNetworkingHide, bool, 1, 1, "socialNetworkingHide( animatedIfApplicable )")
{
    if(socialNetworkingManager != NULL)
    {
        return socialNetworkingManager->socialNetworkingHide(dAtob(argv[1]));
    }
    return false;
}

ConsoleFunction(socialNetworkingShowProfilePicker, bool, 1, 1, "socialNetworkingShowProfilePicker")
{
    if(socialNetworkingManager != NULL)
    {
        return socialNetworkingManager->socialNetworkingShowProfilePicker();
    }
    return false;
}

ConsoleFunction(socialNetworkingShowProfile, bool, 1, 1, "socialNetworkingShowProfile")
{
    if(socialNetworkingManager != NULL)
    {
        return socialNetworkingManager->socialNetworkingShowProfile();
    }
    return false;
}

ConsoleFunction(socialNetworkingShowFriends, bool, 1, 1, "socialNetworkingShowFriends")
{
    if(socialNetworkingManager != NULL)
    {
        return socialNetworkingManager->socialNetworkingShowFriends();
    }
    return false;
}

ConsoleFunction(socialNetworkingShowAwards, bool, 1, 1, "socialNetworkingShowAwards")
{
    if(socialNetworkingManager != NULL)
    {
        return socialNetworkingManager->socialNetworkingShowAwards();
    }
    return false;
}

ConsoleFunction(socialNetworkingShowScores, bool, 2, 2, "socialNetworkingShowScores( scoreBoardID )")
{
    if(socialNetworkingManager != NULL)
    {
        return socialNetworkingManager->socialNetworkingShowScores(argv[1]);
    }
    return false;
}

ConsoleFunction(socialNetworkingSubmitScore, bool, 4, 4, "socialNetworkingSubmitScore( score, metaData, scoreBoardID )")
{
    if(socialNetworkingManager != NULL)
    {
        return socialNetworkingManager->socialNetworkingSubmitScore(dAtoi(argv[1]), argv[2],  argv[3]);
    }
    return false;
}

ConsoleFunction(socialNetworkingUnlockAward, bool, 2, 2, "socialNetworkingUnlockAward( awardID )")
{
    if(socialNetworkingManager != NULL)
    {
        return socialNetworkingManager->socialNetworkingUnlockAward(argv[1]);
    }
    return false;
}

ConsoleFunction(socialNetworkingSetInterfaceOrientation, bool, 2, 2, "socialNetworkingSetInterfaceOrientation( newOrientation )")
{
    if(socialNetworkingManager != NULL)
    {
        return socialNetworkingManager->socialNetworkingSetInterfaceOrientation((UIInterfaceOrientation)dAtoi(argv[1]));
    }
    return false;
}

#endif // _USE_SOCIAL_NETWORKING
