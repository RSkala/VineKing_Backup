//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Vector Utility.
//-----------------------------------------------------------------------------

#ifndef _T2DVECTOR_H_
#define _T2DVECTOR_H_

#ifndef _MMATHFN_H_
#include "math/mMathFn.h"
#endif

#ifndef _T2DUTILITY_H_
#include "./t2dUtility.h"
#endif

#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif

///-----------------------------------------------------------------------------
/// Forward Declarations.
///-----------------------------------------------------------------------------
class t2dMatrix;


///-----------------------------------------------------------------------------
/// Vector 2D.
///
/// @note   This class doesn't utilise the default constructor and is
///         therefore compatible with the TGE 'Vector' (list) class which
///         doesn't explicitly call constructors.
///-----------------------------------------------------------------------------
class t2dVector
{
public:
    F32 mX;
    F32 mY;

    /// Constructors.
    inline t2dVector( void ) {};
    inline t2dVector( const t2dVector& copy ) : mX(copy.mX), mY(copy.mY) {};
    inline t2dVector( const F32 x, const F32 y ) : mX(x), mY(y) {};
    inline t2dVector( const F32 angle ) : mX(mSin(mDegToRad(angle))), mY(-mCos(mDegToRad(angle))) {};
    inline t2dVector( const Point2I& point ) : mX(F32(point.x)), mY(F32(point.y)) {};
    inline t2dVector( const Point2F& point ) : mX(point.x), mY(point.y) {};
    inline t2dVector( const Point2D& point ) : mX(F32(point.x)), mY(F32(point.y)) {};

    // Vector/Matrix Operators.
    t2dVector operator * (const t2dMatrix& M) const;
    t2dVector operator ^ (const t2dMatrix& M) const;
    t2dVector& operator *= (const t2dMatrix& M);
    t2dVector& operator ^= (const t2dMatrix& M);

    /// Operators.
    inline t2dVector& operator /= (const F32 s)                         { mX /= s; mY /= s; return *this; };
    inline t2dVector& operator *= (const F32 s)                         { mX *= s; mY *= s; return *this; };
    inline t2dVector& operator += (const t2dVector& v)                  { mX += v.mX; mY += v.mY;   return *this; };
    inline t2dVector& operator -= (const t2dVector& v)                  { mX -= v.mX; mY -= v.mY;   return *this; };
    inline F32 operator ^ (const t2dVector &v) const                    { return (mX * v.mY) - (mY * v.mX); };
    inline F32 operator * (const t2dVector &v) const                    { return (mX*v.mX) + (mY*v.mY); };
    inline t2dVector operator * (F32 s) const                           { return t2dVector(mX*s, mY*s); };
    inline t2dVector operator / (F32 s) const                           { return t2dVector(mX/s, mY/s); };
    inline t2dVector operator + (const t2dVector &v) const              { return t2dVector(mX+v.mX, mY+v.mY); };
    inline t2dVector operator - (const t2dVector &v) const              { return t2dVector(mX-v.mX, mY-v.mY); };
    friend t2dVector operator * (F32 s, const t2dVector& v)             { return t2dVector(v.mX*s, v.mY*s); };
    inline t2dVector operator - (void) const                            { return t2dVector(-mX, -mY); };
    inline bool operator == (const t2dVector &v) const                  { return (v.mX == mX && v.mY == mY); };
    inline bool operator != (const t2dVector &v) const                  { return (v.mX != mX || v.mY != mY); };

    /// Operator 'Point2F' Support (Assignment/Conversion).
    inline t2dVector operator = (const Point2F &p)                      { mX = p.x; mY = p.y; return *this; };
    inline t2dVector operator = (const Point2I &p)                      { mX = F32(p.x); mY = F32(p.y); return *this; };
    inline operator Point2F ()                                          { return Point2F(mX, mY); };

    /// Alias.
    inline F32 dot(const t2dVector& v) const                            { return *this * v; };

    /// Utility.
    inline void set(const F32 x, const F32 y)                           { mX = x; mY = y; };
    inline void setAngle(const F32 angle)                               { mX = mSin(mDegToRad(angle)); mY = -mCos(mDegToRad(angle)); };
    inline void setPolar(const F32 angle,F32 length)                    { mX = mSin(mDegToRad(angle)*length); mY = -mCos(mDegToRad(angle)*length); };
    inline bool isNAN(void) const                                       { return (mX!=mX) || (mY!=mY); };
    inline bool isEqualRange(const t2dVector& v, const F32 epsilon) const { return mIsEqualRange(mX,v.mX,epsilon) && mIsEqualRange(mY,v.mY,epsilon); };
    inline bool isEqual(const t2dVector& v) const                       { return mIsEqual(mX,v.mX) && mIsEqual(mY,v.mY); };
    inline bool notEqual(const t2dVector& v) const                      { return !isEqual(v); };
    inline const t2dVector& zero()                                      { static const t2dVector M(0.0f, 0.0f); (*this) = M; return *this; };
    inline F32 normalise(void)                                          { F32 length = len(); if ( length > 0.0f ) m_point2F_normalize((F32*)this); return length; };
    inline F32 normalise(F32 s)                                         { F32 length = len(); if ( length > 0.0f ) m_point2F_normalize_f((F32*)this, s); return length; };
    inline t2dVector& receiprocate(void)                                { mX = 1.0f/mX; mY = 1.0f/mY; return *this; };
    inline t2dVector& add(const t2dVector& v)                           { mX += v.mX; mY += v.mY; return *this; };
    inline t2dVector& sub(const t2dVector& v)                           { mX -= v.mX; mY -= v.mY; return *this; };
    inline t2dVector& mult(const t2dVector& v)                          { mX *= v.mX; mY *= v.mY; return *this; };
    inline t2dVector& div(const t2dVector& v)                           { mX /= v.mX; mY /= v.mY; return *this; };
    inline t2dVector& scale(F32 scale)                                  { mX *= scale; mY *= scale; return *this; };
    inline t2dVector& scale(const t2dVector& v)                         { mX *= v.mX; mY *= v.mY; return *this; };
    inline t2dVector& rotate(F32 angle)                                 { angle = mDegToRad(angle); F32 tempX = mX; mX = mX * mCos(angle) - mY * mSin(angle); mY = tempX * mSin(angle) + mY * mCos(angle); return *this; };
    inline t2dVector& rotate(const t2dVector& center, F32 angle)        { t2dVector temp = *this - center; temp.rotate(angle); *this = center + temp; return *this; };
    inline t2dVector& perp(void)                                        { const F32 temp = mX; mX = -mY; mY = temp; return *this; };
    inline t2dVector getPerp(void) const                                { return t2dVector(*this).perp(); };
    inline void lerp(const t2dVector& v, const F32 time, t2dVector& out){ out.set( mX + (v.mX-mX)*time, mY + (v.mY-mY)*time); };
    inline void swap( t2dVector& v)                                     { mSwap( v.mX, mX ); mSwap( v.mY, mY ); };
    inline void clamp(const t2dVector& min, const t2dVector& max)       { mX = (mX < min.mX)? min.mX : (mX > max.mX)? max.mX : mX; mY = (mY < min.mY)? min.mY : (mY > max.mY)? max.mY : mY; };
    inline void rand(const t2dVector& min, const t2dVector& max)        { mX = mGetT2DRandomF(min.mX, max.mX), mY = mGetT2DRandomF(min.mY,max.mY); };
    inline void round(const F32 epsilon = T2D_CONST_EPSILON)            { F32 recip = 1.0f/epsilon; mX = mFloor(mX * recip + 0.5f) * epsilon; mY = mFloor(mY * recip + 0.5f) * epsilon; };
    inline bool isXZero(void) const                                     { return mIsZero(mX); };
    inline bool isYZero(void) const                                     { return mIsZero(mY); };
    inline bool isLenZero(void) const                                   { return mIsZero(len()); };
    inline bool isLenNotZero(void) const                                { return !isLenZero(); };
    inline t2dVector& clampZero(void)                                   { if (isXZero()) mX = 0.0f; if (isYZero()) mY = 0.0f; return *this; };
    inline t2dVector getUnitDirection(void) const                       { t2dVector temp(*this); temp.normalise(); return temp; };
    inline F32 getAngleBetween(const t2dVector& v) const                { F32 dot = (*this) * v; F32 cross = (*this) ^ v; return mRadToDeg(mAtan(cross, dot)); };
    inline F32 getAngle(void) const                                     { return mRadToDeg(mAtan(mX, -mY)); };
    inline static const t2dVector& getZero()                            { static const t2dVector v(0.0f, 0.0f); return v; };
    inline F32 getMinorAxis(void) const                                 { return mLessThan(mX,mY) ? mX : mY; };
    inline F32 getMajorAxis(void) const                                 { return mGreaterThan(mX,mY) ? mX : mY; };
    F32 len(void) const;
    F32 lenSquared(void) const;

};

#endif // _T2DVECTOR_H_