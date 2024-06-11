//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// T2D Serialisation Macros.
//-----------------------------------------------------------------------------

#ifndef _T2DSERIALISATION_H_
#define _T2DSERIALISATION_H_

///-----------------------------------------------------------------------------
/// Load/Save Arguments.
///-----------------------------------------------------------------------------
#define T2D_SERIALISE_LOAD_ARGS_DEF     Stream& stream, t2dSceneGraph* pT2DSceneGraph, Vector<t2dSceneObject*>& ObjReferenceList, bool ignoreLayerOrder
#define T2D_SERIALISE_LOAD_ARGS_PASS    stream, pT2DSceneGraph, ObjReferenceList, ignoreLayerOrder
#define T2D_SERIALISE_SAVE_ARGS_DEF     Stream& stream, U32 serialiseID, U32 serialiseKey
#define T2D_SERIALISE_SAVE_ARGS_PASS    stream, serialiseID, serialiseKey


///-----------------------------------------------------------------------------
/// Serialise Entry Registration.
///-----------------------------------------------------------------------------
#define REGISTER_SERIALISE_START( className )                                                                   \
    static className::tSerialiseEntry serialEntryTable_##className[] = {

#define REGISTER_SERIALISE_VERSION( className, version, deprecated )                                            \
    { version, deprecated, className::loadStream_##version, className::saveStream_##version },

#define REGISTER_SERIALISE_END() };


///-----------------------------------------------------------------------------
/// Load/Save Method Declaration/Implementation.
///-----------------------------------------------------------------------------
#define DECLARE_T2D_LOAD_METHOD( className, version )                                                           \
    static bool loadStream_##version( className*, T2D_SERIALISE_LOAD_ARGS_DEF );

#define DECLARE_T2D_SAVE_METHOD( className, version )                                                           \
    static bool saveStream_##version( className*, T2D_SERIALISE_SAVE_ARGS_DEF );

#define DECLARE_T2D_LOADSAVE_METHOD( className, version )                                                       \
    DECLARE_T2D_LOAD_METHOD( className, version );                                                              \
    DECLARE_T2D_SAVE_METHOD( className, version );

#define IMPLEMENT_T2D_LOAD_METHOD( className, version )                                                         \
    bool className::loadStream_##version( className* object, T2D_SERIALISE_LOAD_ARGS_DEF )

#define IMPLEMENT_T2D_SAVE_METHOD( className, version )                                                         \
    bool className::saveStream_##version( className* object, T2D_SERIALISE_SAVE_ARGS_DEF )


///-----------------------------------------------------------------------------
/// Header Load.
///-----------------------------------------------------------------------------
#define T2D_SERIALISE_HEADER_LOAD( className )                                                                  \
    U32 streamHeaderID;                                                                                         \
    U32 streamVersion;                                                                                          \
    if ( !stream.read( &streamHeaderID ) ||                                                                     \
         !stream.read( &streamVersion ) )                                                                       \
        return false;                                                                                           \
    if ( streamHeaderID != T2D_Stream_HeaderID ) {                                                              \
        Con::warnf(#className "::loadStream() - Invalid Stream Header ID!");                                    \
        return false; }


///-----------------------------------------------------------------------------
/// Header Save.
///-----------------------------------------------------------------------------
#define T2D_SERIALISE_HEADER_SAVE( className, version )                                                         \
    if (    !stream.write( T2D_Stream_HeaderID ) ||                                                             \
            !stream.write( version ) ) {                                                                        \
        Con::warnf(#className "::saveStream() - Couldn't Save Stream Header/Version!");                            \
        return false; }


///-----------------------------------------------------------------------------
/// Route Version Load.
///-----------------------------------------------------------------------------
#define T2D_SERIALISE_ROUTE_LOAD( className, version )                                                          \
    U32 entries = sizeof(serialEntryTable_##className) / sizeof(className::tSerialiseEntry);                    \
    for ( U32 n = 0; n < entries; n++ ) {                                                                       \
        if ( serialEntryTable_##className[n].mStreamVersion == version ) {                                      \
            if ( serialEntryTable_##className[n].mDeprecated && Con::getBoolVariable( "$pref::T2D::warnFileDeprecated" ) )      \
                Con::warnf(#className "::loadStream() - Loading Deprecated Version %d! (Save as higher version!)", version );   \
            return serialEntryTable_##className[n].mpFnLoadStream( this, T2D_SERIALISE_LOAD_ARGS_PASS );    \
        }                                                                                                       \
    }                                                                                                           \
    Con::warnf(#className "::loadStream() - Could not find handler for Version %d!", version );                 \
    return false;


///-----------------------------------------------------------------------------
/// Route Version Save.
///-----------------------------------------------------------------------------
#define T2D_SERIALISE_ROUTE_SAVE( className, version )                                                          \
    U32 entries = sizeof(serialEntryTable_##className) / sizeof(className::tSerialiseEntry);                    \
    for ( U32 n = 0; n < entries; n++ ) {                                                                       \
        if ( serialEntryTable_##className[n].mStreamVersion == version ) {                                      \
            if ( serialEntryTable_##className[n].mDeprecated && Con::getBoolVariable( "$pref::T2D::warnFileDeprecated" ) )      \
                Con::warnf(#className "::saveStream() - Saving Deprecated Version %d!", version );              \
            return serialEntryTable_##className[n].mpFnSaveStream( this, T2D_SERIALISE_SAVE_ARGS_PASS );        \
        }                                                                                                       \
    }                                                                                                           \
    Con::warnf(#className "::saveStream() - Could not find handler for Version %d!", version );                 \
    return false;

    
///-----------------------------------------------------------------------------
/// Declare Serialisation.
///-----------------------------------------------------------------------------
#define DECLARE_T2D_SERIALISE( className )                                                                      \
    typedef bool (*typeFnLoadStream)( className*, T2D_SERIALISE_LOAD_ARGS_DEF );                                \
    typedef bool (*typeFnSaveStream)( className*, T2D_SERIALISE_SAVE_ARGS_DEF );                                \
    U32 mLocalSerialiseID;                                                                                      \
    struct tSerialiseEntry { U32 mStreamVersion; bool mDeprecated; typeFnLoadStream mpFnLoadStream; typeFnSaveStream mpFnSaveStream; };   \
    const U32 T2D_Stream_HeaderID;                                                                              \
    virtual bool loadStream( T2D_SERIALISE_LOAD_ARGS_DEF );                                                     \
    virtual bool saveStream( T2D_SERIALISE_SAVE_ARGS_DEF );


///-----------------------------------------------------------------------------
/// Implement Serialise Base (t2dSceneObject).
///-----------------------------------------------------------------------------
#define IMPLEMENT_T2D_SERIALISE_BASE( className, saveVersion )                                                  \
    bool className::loadStream( T2D_SERIALISE_LOAD_ARGS_DEF ) {                                                 \
        if ( !stream.read( &mSerialiseKey ) ) return false;                                                     \
        T2D_SERIALISE_HEADER_LOAD( className );                                                                 \
        T2D_SERIALISE_ROUTE_LOAD( className, streamVersion );                                                   \
    };                                                                                                          \
    bool className::saveStream( T2D_SERIALISE_SAVE_ARGS_DEF ) {                                                 \
        if ( serialiseID == mLocalSerialiseID ) return true;                                                    \
        setSerialiseState( serialiseID, serialiseKey );                                                         \
        if ( !stream.write( mSerialiseKey ) ) return false;                                                     \
        T2D_SERIALISE_HEADER_SAVE( className, saveVersion );                                                    \
        T2D_SERIALISE_ROUTE_SAVE( className, saveVersion );                                                     \
    };

///-----------------------------------------------------------------------------
/// Implement Serialise Parent (based upon t2dSceneObject).
///-----------------------------------------------------------------------------
#define IMPLEMENT_T2D_SERIALISE_PARENT( className, saveVersion )                                                \
    bool className::loadStream( T2D_SERIALISE_LOAD_ARGS_DEF ) {                                                 \
        if ( !Parent::loadStream( T2D_SERIALISE_LOAD_ARGS_PASS ) ) return false;                                \
        T2D_SERIALISE_HEADER_LOAD( className );                                                                 \
        T2D_SERIALISE_ROUTE_LOAD( className, streamVersion );                                                   \
    };                                                                                                          \
    bool className::saveStream( T2D_SERIALISE_SAVE_ARGS_DEF ) {                                                 \
        if ( serialiseID == mLocalSerialiseID ) return true;                                                    \
        if ( !Parent::saveStream( T2D_SERIALISE_SAVE_ARGS_PASS ) )												\
			{																									\
				Con::printf("Luma :: Cannot save parents stream. Child stream will fail");										\
				return false;																					\
			}																									\
        T2D_SERIALISE_HEADER_SAVE( className, saveVersion );                                                    \
        T2D_SERIALISE_ROUTE_SAVE( className, saveVersion );                                                     \
    };

///-----------------------------------------------------------------------------
/// Implement Serialise Leaf (NOT based upon t2dSceneObject).
///-----------------------------------------------------------------------------
#define IMPLEMENT_T2D_SERIALISE_LEAF( className, saveVersion )                                                  \
    bool className::loadStream( T2D_SERIALISE_LOAD_ARGS_DEF ) {                                                 \
        T2D_SERIALISE_HEADER_LOAD( className );                                                                 \
        T2D_SERIALISE_ROUTE_LOAD( className, streamVersion );                                                   \
    };                                                                                                          \
    bool className::saveStream( T2D_SERIALISE_SAVE_ARGS_DEF ) {                                                 \
        if ( serialiseID == mLocalSerialiseID ) return true;                                                    \
        mLocalSerialiseID = serialiseID;                                                                        \
        T2D_SERIALISE_HEADER_SAVE( className, saveVersion );                                                    \
        T2D_SERIALISE_ROUTE_SAVE( className, saveVersion );                                                     \
    };


#endif // _T2DSERIALISATION_H_
