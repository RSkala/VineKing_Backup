//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// T2D Shape Vector.
//-----------------------------------------------------------------------------

#ifndef _T2DSHAPEVECTOR_H_
#define _T2DSHAPEVECTOR_H_

#ifndef _T2DSCENEOBJECT_H_
#include "./t2dSceneObject.h"
#endif


//-----------------------------------------------------------------------------
// T2D Shape Vector.
//-----------------------------------------------------------------------------
class t2dShapeVector : public t2dSceneObject
{
	typedef t2dSceneObject			Parent;

protected:
    ColorF                  mLineColour;
    ColorF                  mFillColour;
    bool                    mFillMode;
    t2dVector               mPolygonScale;          ///< Polygon Scale.
    Vector<t2dVector>       mPolygonBasisList;      ///< Polygon Basis List.
    Vector<t2dVector>       mPolygonLocalList;      ///< Polygon Local List.


public:
	t2dShapeVector();
	~t2dShapeVector();

	static void initPersistFields();

    /// Polygon Configuration.
    void setPolyScale( const t2dVector& scale );
    void setPolyPrimitive( const U32 polyVertexCount );
    void setPolyCustom( const U32 polyVertexCount, const char* pCustomPolygon );

    /// Rendering Attributes.
    void setLineColourString( const char* lineColour );
    void setLineColour( const ColorF& lineColour );
    void setLineAlpha( const F32 alpha );
    void setFillColourString( const char* fillColour );
    void setFillColour( const ColorF& fillColour );
    void setFillAlpha( const F32 alpha );
    void setFillMode( const bool fillMode );

    /// Retrieval.
    U32 getPolyVertexCount( void ) { return U32(mPolygonBasisList.size()); };
	const char* getPoly( void );
	inline const t2dVector* getPolyBasis( void ) const         { return &(mPolygonBasisList[0]); };
	bool getFillMode( void );
	const char* getLineColor( void );
	const char* getFillColor( void );

	// Random
	static bool setPolyCustom(void* obj, const char* data)
    {
       //t2dVector poly[t2dPhysics::MAX_COLLISION_POLY_VERTEX];
       U32 count = getStringElementCount(data) >> 1;
       //for (U32 i = 0; i < count; i++)
       //   poly[i] = getStringElementVector(data, i * 2);

       static_cast<t2dShapeVector*>(obj)->setPolyCustom(count, data);
       return false;
    };
	static bool setLineColour(void* obj, const char* data) { static_cast<t2dShapeVector*>(obj)->setLineColourString(data); return false; };
	static bool setFillColour(void* obj, const char* data) { static_cast<t2dShapeVector*>(obj)->setFillColourString(data); return false; };
	static bool setFillMode(void* obj, const char* data) { static_cast<t2dShapeVector*>(obj)->setFillMode(dAtob(data)); return false; };


    /// Internal Crunchers.
    void generateLocalPoly( void );

    /// T2D.
	virtual void setFlip( const bool flipX, bool flipY );
    virtual void setSize( const t2dVector& size );

    /// Core.
	virtual bool onAdd();
	virtual void onRemove();
	virtual void renderObject( const RectF& viewPort, const RectF& viewIntersection );

	/// Clone support
	void copyTo(SimObject* obj);

	/// Declare Serialise Object.
	DECLARE_T2D_SERIALISE( t2dShapeVector );
	/// Declare Serialise Objects.
	DECLARE_T2D_LOADSAVE_METHOD( t2dShapeVector, 1 );

    /// Declare Console Object.
	DECLARE_CONOBJECT(t2dShapeVector);
};

#endif // _T2DSHAPEVECTOR_H_
