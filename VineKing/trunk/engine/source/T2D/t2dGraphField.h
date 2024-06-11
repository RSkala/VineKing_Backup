//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Graph Field Object.
//-----------------------------------------------------------------------------

#ifndef _T2DGRAPHFIELD_H_
#define _T2DGRAPHFIELD_H_

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _T2DSERIALISATION_H_
#include "./t2dSerialise.h"
#endif


///-----------------------------------------------------------------------------
/// T2d Graph Field.
///-----------------------------------------------------------------------------
class t2dGraphField
{
public:
    /// Data Key Node.
    struct tDataKeyNode
    {
        F32     mTime;
        F32     mValue;
    };

private:
    Vector<tDataKeyNode>    mDataKeys;

    F32                     mValueScale;
    F32                     mTimeScale;
    F32                     mMaxTime;
    F32                     mMinValue;
    F32                     mMaxValue;
    F32                     mDefaultValue;

public:
    t2dGraphField();
    virtual ~t2dGraphField();

    virtual void copyTo(t2dGraphField& graph);

    const tDataKeyNode getDataKeyNode( const U32 index ) const;
    void resetDataKeys( void );
    bool setTimeRepeat( const F32 timeRepeat );
    bool setValueScale( const F32 valueScale );
    void setValueBounds( F32 maxTime, F32 minValue, F32 maxValue, F32 defaultValue );
    S32 addDataKey( const F32 time, const F32 value );
    bool removeDataKey( const U32 index );
    void clearDataKeys( void );
    bool setDataKeyValue( const U32 index, const F32 value );
    F32 getDataKeyValue( const U32 index ) const;
    F32 getDataKeyTime( const U32 index ) const;
    U32 getDataKeyCount( void ) const;
    F32 getGraphValue( F32 time ) const;
    F32 getMinValue( void ) const { return mMinValue; };
    F32 getMaxValue( void ) const { return mMaxValue; };
    F32 getMinTime( void ) const { return 0.0f; }
    F32 getMaxTime( void ) const { return mMaxTime; };
    F32 getTimeRepeat( void ) const { return mTimeScale - 1.0f; };
    F32 getValueScale( void ) const { return mValueScale; };

    // Calculate Graph Variants.
    static F32 calcGraphBV( const t2dGraphField& base, const t2dGraphField& variation, const F32 effectAge, const bool modulate = false, const F32 modulo = 0.0f );
    static F32 calcGraphBVE( const t2dGraphField& base, const t2dGraphField& variation, const t2dGraphField& effect, const F32 effectAge, const bool modulate = false, const F32 modulo = 0.0f );
    static F32 calcGraphBVLE( const t2dGraphField& base, const t2dGraphField& variation, const t2dGraphField& overlife, const t2dGraphField& effect, const F32 effectTime, const F32 particleAge, const bool modulate = false, const F32 modulo = 0.0f );

    /// Declare Serialise Object.
    DECLARE_T2D_SERIALISE( t2dGraphField );
    /// Declare Serialise Objects.
    DECLARE_T2D_LOADSAVE_METHOD( t2dGraphField, 1 );
    DECLARE_T2D_LOADSAVE_METHOD( t2dGraphField, 2 );
};


///-----------------------------------------------------------------------------
/// Base-Only Graph.
///-----------------------------------------------------------------------------
struct t2dGraphField_B
{
   t2dGraphField       t2dGraphField_Base;

   virtual void copyTo(t2dGraphField_B& graph)
   {
      t2dGraphField_Base.copyTo(graph.t2dGraphField_Base);
   };

};

///-----------------------------------------------------------------------------
/// Life-Only Graph.
///-----------------------------------------------------------------------------
struct t2dGraphField_L
{
   t2dGraphField       t2dGraphField_OverLife;

   void copyTo(t2dGraphField_L& graph)
   {
      t2dGraphField_OverLife.copyTo(graph.t2dGraphField_OverLife);
   };

};

///-----------------------------------------------------------------------------
/// Base & Variation Graphs.
///-----------------------------------------------------------------------------
struct t2dGraphField_BV
{
    t2dGraphField       t2dGraphField_Base;
    t2dGraphField       t2dGraphField_Variation;

    void copyTo(t2dGraphField_BV& graph)
    {
       t2dGraphField_Base.copyTo(graph.t2dGraphField_Base);
       t2dGraphField_Variation.copyTo(graph.t2dGraphField_Variation);
    };

};

///-----------------------------------------------------------------------------
/// Base, Variation and Over-Time Graphs.
///-----------------------------------------------------------------------------
struct t2dGraphField_BVL
{
    t2dGraphField       t2dGraphField_Base;
    t2dGraphField       t2dGraphField_Variation;
    t2dGraphField       t2dGraphField_OverLife;

    void copyTo(t2dGraphField_BVL& graph)
    {
       t2dGraphField_Base.copyTo(graph.t2dGraphField_Base);
       t2dGraphField_Variation.copyTo(graph.t2dGraphField_Variation);
       t2dGraphField_OverLife.copyTo(graph.t2dGraphField_OverLife);
    };

};


#endif // _T2DGRAPHFIELD_H_
