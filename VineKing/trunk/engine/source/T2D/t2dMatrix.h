//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Matrix Utility.
//-----------------------------------------------------------------------------

#ifndef _T2DMATRIX_H_
#define _T2DMATRIX_H_

#ifndef _T2DVECTOR_H_
#include "t2dVector.h"
#endif

#ifndef _T2DUTILITY_H_
#include "./t2dUtility.h"
#endif


///-----------------------------------------------------------------------------
/// Matrix 2D.
///-----------------------------------------------------------------------------
class t2dMatrix
{
public:

    /// Storage.
    union
    {
        /// Seperate Elements.
        struct
        {
            F32 mE11;
            F32 mE12;
            F32 mE21;
            F32 mE22;
        };

        /// Arrayed Elements.
        F32 mE[2][2];
    };

    /// Construction.
    t2dMatrix() {}
    t2dMatrix( const F32 e11, const F32 e12, const F32 e21, const F32 e22 ) : mE11(e11), mE12(e12), mE21(e21), mE22(e22) {};

    /// Specified Angle.
    t2dMatrix( F32 angle )
    {
        /// Convert.
        angle = -mDegToRad(angle);
        /// Calculate Gradients.
        F32 cos = mCos(angle);
        F32 sin = mSin(angle);
        /// Orientate Gradients.
        mE11 = cos;
        mE12 = sin;
        mE21 = -sin;
        mE22 = cos;
    }

    /// Set.
    void set( const F32 e11, const F32 e12, const F32 e21, const F32 e22 ) { mE11 = e11; mE12 = e12; mE21 = e21; mE22 = e22; };
    /// Identity.
    t2dMatrix& identity() { static const t2dMatrix M(1.0f, 0.0f, 0.0f, 1.0f); (*this) = M; return (*this); };
    /// Is NAN?
    bool isNAN(void) { return (mE11!=mE11) || (mE12!=mE12) || (mE21!=mE21) || (mE22!=mE22); };
    /// Null.
    t2dMatrix zero() { static const t2dMatrix M(0.0f, 0.0f, 0.0f, 0.0f); (*this) = M; return (*this); };
    /// Transpose.
    t2dMatrix& transpose() { F32 temp = mE12; mE12 = mE21; mE21 = temp; return *this; };
    /// Orientate.
    void orientate( F32 angle ) { angle = -mDegToRad(angle); F32 cos = mCos(angle); F32 sin = mSin(angle); mE11 = cos; mE12 = sin; mE21 = -sin; mE22 = cos; };
    /// Inverse.
    t2dMatrix& inverse(void)
    {
        // Find Determinant.
        F32 det = mE11 * mE22 - mE12 * mE21;
        // Bail if we can't invert it!
        AssertFatal( mNotZero(det), "t2dMatrix::inverse() - Non-singular matrix; no inverse!");

        // Get rid of divisions to follow.
        det = 1.0f / det;
        // Do the inversion.
        F32 tempE11 = mE11;
        mE11 = mE22 * det;
        mE22 = tempE11 * det;
        mE12 *= -det; 
        mE21 *= -det;
        // Return inverted matrix.
        return *this;
    }

    /// Operators.
    t2dMatrix operator* ( const t2dMatrix& M ) const 
    {
        return t2dMatrix(mE11 * M.mE11 + mE12 * M.mE21,
                        mE11 * M.mE12 + mE12 * M.mE22,
                        mE21 * M.mE11 + mE22 * M.mE21,
                        mE21 * M.mE12 + mE22 * M.mE22 );
    }
    t2dMatrix operator^ ( const t2dMatrix& M ) const 
    {
        return t2dMatrix(mE11 * M.mE11 + mE12 * M.mE12,
                        mE11 * M.mE21 + mE12 * M.mE22,
                        mE21 * M.mE11 + mE22 * M.mE12,
                        mE21 * M.mE21 + mE22 * M.mE22 );
    }
    inline t2dMatrix operator* ( F32 scale ) const
    {
        return t2dMatrix(mE11 * scale,
                        mE12 * scale,
                        mE21 * scale,
                        mE22 * scale );
    }
    inline t2dMatrix operator* ( t2dVector& scaleXY ) const
    {
        return t2dMatrix(mE11 * scaleXY.mX,
                        mE12 * scaleXY.mY,
                        mE21 * scaleXY.mX,
                        mE22 * scaleXY.mY );
    }


    /// Accessors.
    F32  operator()(U32 i, U32 j) const                 { return mE[i][j]; }
    F32& operator()(U32 i, U32 j)                       { return mE[i][j]; }
    const t2dVector& operator[](U32 i) const            { return reinterpret_cast<const t2dVector&>(mE[i][0]); };
    t2dVector& operator[](U32 i)                        { return reinterpret_cast<t2dVector&>(mE[i][0]);    };      
    static const t2dMatrix getIdentity()                { static const t2dMatrix I(1.0f, 0.0f, 0.0f, 1.0f); return I; };
};

#endif // _T2DMATRIX_H_
