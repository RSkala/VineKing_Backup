//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Utility.
//-----------------------------------------------------------------------------

#ifndef _T2DUTILITY_H_
#define _T2DUTILITY_H_

#ifndef _MRANDOM_H_
#include "math/mRandom.h"
#endif

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif

#include "platform/platform.h"
#include "platform/platformGL.h"
#include "math/mMath.h"


//-----------------------------------------------------------------------------
// Miscellaneous Defines.
//-----------------------------------------------------------------------------

// Engine Version.
#define T2D_ENGINE_VERSION      		"v1.7.5"           ///< Engine Version String.
#define T2D_IPHONETOOLS_VERSION      	"v1.4.1"          ///< Engine Version String for iPhone tools. Changing this will allow a fresh AppData folder to avoid conflicts with other builds existing on the system.

/// Precision Constants.
#define T2D_CONST_EPSILON               (0.0001f)
#define T2D_CONST_EPSILON_RECIP         (1.0f/T2D_CONST_EPSILON)

#define T2D_CONST_COLLISION_MARGIN      (0.001f)        ///< Arbitrary small spatial/temporal value.

#define T2D_MASK_ALL                    (U32_MAX)
#define T2D_MASK_BITCOUNT               (32)
#define T2D_DEBUGMODE_COUNT             (8)




//-----------------------------------------------------------------------------
// Globals.
//-----------------------------------------------------------------------------
extern MRandomLCG gT2DRandomGenerator;

class t2dSceneGraph; // Yuk!
extern SimObjectPtr<t2dSceneGraph> gDefaultSceneGraph;
extern SimObjectPtr<t2dSceneGraph> gLoadingSceneGraph;

DefineConsoleType( TypePoint2FVector )

//-----------------------------------------------------------------------------
// Constants.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Utility Functions.
//-----------------------------------------------------------------------------

/// Precision Rounding.
inline F32 mRound(F32& value, const F32 epsilon = T2D_CONST_EPSILON) { return value = (mFloor(value * (1.0f/epsilon) + 0.5f) * epsilon); };

/// Is NAN?
inline F32 mIsNAN(const F32& value) { return (value != value); };

/// Tolerate Is Zero?
inline bool mIsZero(const F32& value) { return mFabs(value) < T2D_CONST_EPSILON; };

/// Tolerate Not Zero?
inline bool mNotZero(const F32& value) { return !mIsZero(value); };

/// Tolerate Less-Than?
inline bool mLessThan(const F32& a, const F32& b) { return a < b; };

/// Tolerate Greater-Than?
inline bool mGreaterThan(const F32& a, const F32& b) { return a > b; };

/// Safe Less Than Zero?
inline bool mLessThanZero(const F32& value) { return mLessThan(value, 0.0f); };

/// Safe Greater Than Zero?
inline bool mGreaterThanZero(const F32& value) { return mGreaterThan(value, 0.0f); };

/// Safe Is Equal?
inline bool mIsEqual(const F32& a, const F32& b) { return mIsZero(mFabs(a-b)); };

/// Safe Not Equal?
inline bool mNotEqual(const F32& a, const F32& b) { return !mIsEqual(a,b); };

/// Tolerate Is Equal within Range?
inline bool mIsEqualRange(const F32& a, const F32& b, const F32 epsilon = T2D_CONST_EPSILON) { return mFabs(a-b) <= epsilon; };

/// Tolerate Is One?
inline bool mIsOne(const F32& value) { return mIsEqual(value, 1.0f); };

/// Tolerate Less-Than or Equal?
inline bool mLessThanOrEqual(const F32& a, const F32& b) { return ( (a < b) || (!(a>b) && mIsEqual(a,b)) ); };

/// Tolerate Greater-Than or Equal?
inline bool mGreaterThanOrEqual(const F32&a, const F32& b) { return ( (a > b) || (!(a < b) && mIsEqual(a,b)) ); };

/// Get Min.
inline F32 mGetMin(const F32& a, const F32& b) { if ( mLessThan(a,b) ) return a; else return b; };

/// Get Max.
inline F32 mGetMax(const F32& a, const F32& b) { if ( mGreaterThan(a,b) ) return a; else return b; };

/// Get Min/Max.
inline void mGetMinMax(const F32& a, const F32& b, F32& min, F32& max) { if ( mGreaterThan(a,b) ) { max = a; min = b; } else { max = b; min = a; } };

/// Swap.
inline void mSwap(F32& a, F32& b) { F32 temp = b; b = a; a = temp; };

/// Random Float Range.
inline F32 mGetT2DRandomF( F32 from, F32 to ) { return gT2DRandomGenerator.randF( from, to ); };

/// Random Float.
inline F32 mGetT2DRandomF() { return gT2DRandomGenerator.randF(); };

/// Random Integer Range.
inline S32 mGetT2DRandomI( U32 from, U32 to ) { return gT2DRandomGenerator.randI( from, to ); };

/// Random Integer.
inline S32 mGetT2DRandomI() { return gT2DRandomGenerator.randI(); };

/// Max Texture Size.
inline S32 mGetT2DMaxTextureSize( void )
{
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    mClamp( maxTextureSize, 256, S32_MAX );
    return S32(maxTextureSize);
}

#endif // _T2DUTILITY_H_
