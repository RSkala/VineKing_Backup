//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Vector Utility.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "console/console.h"
#include "./t2dSceneObject.h"
#include "./t2dMatrix.h"
#include "./t2dUtility.h"
#include "./t2dVector.h"

// Vector/Matrix Operators.
//
t2dVector t2dVector::operator * (const t2dMatrix& M) const  { return t2dVector(mX * M.mE11 + mY * M.mE12, mX * M.mE21 + mY * M.mE22); };
t2dVector t2dVector::operator ^ (const t2dMatrix& M) const  { return t2dVector(mX * M.mE11 + mY * M.mE21, mX * M.mE12 + mY * M.mE22); };
t2dVector& t2dVector::operator *= (const t2dMatrix& M)      { t2dVector temp = *this; mX = temp.mX * M.mE11 + temp.mY * M.mE12; mY = temp.mX * M.mE21 + temp.mY * M.mE22; return *this; };
t2dVector& t2dVector::operator ^= (const t2dMatrix& M)      { t2dVector temp = *this; mX = temp.mX * M.mE11 + temp.mY * M.mE21; mY = temp.mX * M.mE12 + temp.mY * M.mE22; return *this; };
F32 t2dVector::len(void) const                              { return mSqrt(mX*mX + mY*mY); };
F32 t2dVector::lenSquared(void) const                       { return mX*mX + mY*mY; };


//-----------------------------------------------------------------------------

ConsoleFunctionGroupBegin( t2dVectorMath, "T2D Vector manipulation functions.");

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Add two 2D Vectors.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dVectorAdd, const char*, 3, 3, "(t2dVector v1$, t2dVector v2$) - Returns v1+v2.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 || t2dSceneObject::getStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dVectorAdd() - Invalid number of parameters!");
        return NULL;
    }

    // Input Vectors.
    t2dVector v1(0,0),v2(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &v1.mX, &v1.mY);
    dSscanf(argv[2],"%f %f", &v2.mX, &v2.mY);
    // Do Vector Operation.
    t2dVector v = v1 + v2;
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", v.mX, v.mY);
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Subtract two 2D Vectors.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dVectorSub, const char*, 3, 3, "(t2dVector v1$, t2dVector v2$) - Returns v1-v2.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 || t2dSceneObject::getStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dVectorSub() - Invalid number of parameters!");
        return NULL;
    }

    // Input Vectors.
    t2dVector v1(0,0),v2(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &v1.mX, &v1.mY);
    dSscanf(argv[2],"%f %f", &v2.mX, &v2.mY);
    // Do Vector Operation.
    t2dVector v = v1 - v2;
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", v.mX, v.mY);
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Multiply two 2D Vectors (Not Dot-Product!)
//-----------------------------------------------------------------------------
ConsoleFunction( t2dVectorMult, const char*, 3, 3, "(t2dVector v1$, t2dVector v2$) - Returns v1 mult v2.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 || t2dSceneObject::getStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dVectorMult() - Invalid number of parameters!");
        return NULL;
    }

    // Input Vectors.
    t2dVector v1(0,0),v2(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &v1.mX, &v1.mY);
    dSscanf(argv[2],"%f %f", &v2.mX, &v2.mY);
    // Do Vector Operation.
    t2dVector v( v1.mX*v2.mX, v1.mY*v2.mY );
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", v1.mX*v2.mX, v1.mY*v2.mY );
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Scale a 2D Vector.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dVectorScale, const char*, 3, 3, "(t2dVector v1$, scale) - Returns v1 scaled by scale.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 )
    {
        Con::warnf("t2dVectorScale() - Invalid number of parameters!");
        return NULL;
    }

    // Input Vectors.
    t2dVector v1(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &v1.mX, &v1.mY);
    // Do Vector Operation.
    v1 *= dAtof(argv[2]);
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", v1.mX, v1.mY);
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Normalise a 2D Vector.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dVectorNormalise, const char*, 2, 2, "(t2dVector v1$) - Returns normalised v1.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 )
    {
        Con::warnf("t2dVectorNormalise() - Invalid number of parameters!");
        return NULL;
    }

    // Input Vectors.
    t2dVector v1(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &v1.mX, &v1.mY);
    // Do Vector Operation.
    v1.normalise();
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%f %f", v1.mX, v1.mY);
    // Return Velocity.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Dot-Product of two 2D Vectors.
//-----------------------------------------------------------------------------
ConsoleFunction(t2dVectorDot, F32, 3, 3, "(t2dVector v1$, t2dVector v2$) - Returns dot-product of v1 and v2.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 || t2dSceneObject::getStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dVectorDot() - Invalid number of parameters!");
        return 0.0f;
    }

    // Input Vectors.
    t2dVector v1(0,0), v2(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &v1.mX, &v1.mY);
    dSscanf(argv[2],"%f %f", &v2.mX, &v2.mY);
    // Do Vector Operation.
    return v1 * v2;
}


//-----------------------------------------------------------------------------
// Equality of two 2D Points.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dVectorCompare, bool, 3, 4, "(t2dVector p1$, t2dVector p2$, [epsilon]) - Compares points p1 and p2 with optional difference (epsilon).")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 || t2dSceneObject::getStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dVectorCompare() - Invalid number of parameters!");
        return NULL;
    }

    // Input Vectors.
    t2dVector p1(0,0), p2(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &p1.mX, &p1.mY);
    dSscanf(argv[2],"%f %f", &p2.mX, &p2.mY);
    // Do Vector Operation.
    const F32 delta = (p2 - p1).len();
    // Calculate Epsilon.
    const F32 epsilon = (argc >= 4) ? dAtof(argv[3]) : T2D_CONST_EPSILON;
    // Return  epsilon delta.
    return mIsEqualRange( delta, 0.0f, epsilon );
}


//-----------------------------------------------------------------------------
// Distance between two 2D Points.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dVectorDistance, F32, 3, 3, "(t2dVector p1$, t2dVector p2$) - Returns the distance between points p1 and p2.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 || t2dSceneObject::getStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dVectorDistance() - Invalid number of parameters!");
        return NULL;
    }

    // Input Vectors.
    t2dVector p1(0,0), p2(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &p1.mX, &p1.mY);
    dSscanf(argv[2],"%f %f", &p2.mX, &p2.mY);
    // Do Vector Operation.
    return (p2 - p1).len();
}


//-----------------------------------------------------------------------------
// Angle between two 2D Vectors.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dAngleBetween, F32, 3, 3, "(t2dVector v1$, t2dVector v2$) - Returns the angle between v1 and v2.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 || t2dSceneObject::getStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dAngleBetween() - Invalid number of parameters!");
        return NULL;
    }

    t2dVector v1(0,0), v2(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &v1.mX, &v1.mY);
    dSscanf(argv[2],"%f %f", &v2.mX, &v2.mY);

    v1.normalise();
    v2.normalise();

    // Do Vector Operation.
    return mRadToDeg( mAcos( v1.dot(v2) ) );
}


//-----------------------------------------------------------------------------
// Angle from one point to another.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dAngleToPoint, F32, 3, 3, "(t2dVector p1, t2dVector p1) - Returns the angle from p1 to p2.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 || t2dSceneObject::getStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dAngleToPoint() - Invalid number of parameters!");
        return NULL;
    }

    t2dVector p1(0,0), p2(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &p1.mX, &p1.mY);
    dSscanf(argv[2],"%f %f", &p2.mX, &p2.mY);

    // Do Operation.
	return mRadToDeg( mAtan((p2.mX - p1.mX), (p1.mY - p2.mY)) );
}


//-----------------------------------------------------------------------------
// Length of a 2D Vector.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dVectorLength, F32, 2, 2, "(t2dVector v1$) - Returns the length of v1.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 )
    {
        Con::warnf("t2dVectorLength() - Invalid number of parameters!");
        return 0.0f;
    }

    // Input Vectors.
    t2dVector v1(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &v1.mX, &v1.mY);
    // Do Vector Operation.
    return v1.len();
}


//-----------------------------------------------------------------------------
// Normalise Rectangle (two 2D Vectors) with relation to each other.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dRectNormalise, const char*, 3, 3, "(t2dVector v1$, t2dVector v2$) - Returns normalise rectangle of v1 and v2.")
{
    // Check Parameters.
    if ( t2dSceneObject::getStringElementCount(argv[1]) < 2 || t2dSceneObject::getStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dRectNormalise() - Invalid number of parameters!");
        return NULL;
    }

    // Input Vectors.
    t2dVector v1(0,0), v2(0,0);
    // Scan-in vectors.
    dSscanf(argv[1],"%f %f", &v1.mX, &v1.mY);
    dSscanf(argv[2],"%f %f", &v2.mX, &v2.mY);
    // Do Vector Operation.
    t2dVector topLeft( (v1.mX <= v2.mX) ? v1.mX : v2.mX, (v1.mY <= v2.mY) ? v1.mY : v2.mY );
    t2dVector bottomRight( (v1.mX > v2.mX) ? v1.mX : v2.mX, (v1.mY > v2.mY) ? v1.mY : v2.mY );

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer( 64 );
    // Format Buffer.
    dSprintf(pBuffer, 64, "%f %f %f %f", topLeft.mX, topLeft.mY, bottomRight.mX, bottomRight.mY);
    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleFunctionGroupEnd( t2dVectorMath );

//-----------------------------------------------------------------------------
