//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentTutorialPathHandler.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentGlobals.h"
#include "core/iTickable.h"
//#include "ComponentTutorialLevelElement.h"
//#include "ComponentTutorialLevel.h"

#include "SoundManager.h"

//#include "T2D/t2dSceneObject.h"
#include "T2D/t2dPath.h"
#include "T2D/t2dStaticSprite.h"

static const char g_szTheFinger[] = "TheFinger";
static t2dStaticSprite* g_pTheFinger = NULL;
static t2dPath* g_pPathObject = NULL; // Will be m_pOwner


static const F32 g_fFingerSpeed = 150.0f; //180.0f; //128.0f;


//static const ColorF g_DefaultLineDrawColor( 0.0f, 1.0f, 0.0f );
//static const ColorF g_MaxLineDrawColor( 1.0f, 0.0f, 0.0f );

static const t2dVector g_vWorldUp( 0.0f, -1.0f );

static const char g_szTutorialLineDraw_Circle[] = "TutorialLineDraw_Circle";
static const char g_szTutorialLineDraw_Square[] = "TutorialLineDraw_Square";

static t2dStaticSprite* g_pTutorialLineDraw_Circle = NULL;
static t2dStaticSprite* g_pTutorialLineDraw_Square = NULL;

const char g_szFingerUpImageMapName[] = "finger_off_onImageMap";    // Frame 0
const char g_szFingerDownImageMapName[] = "finger_off_onImageMap";  // Frame 1

const S32 g_iFingerUpImageMapFrame = 0;
const S32 g_iFingerDownImageMapFrame = 1;

static const S32 g_iMaxNumLineObjects = 70;

static const t2dVector g_vLineSize( 4.0f, 4.0f );


static const t2dVector g_vDefaultPathPos( -4.159, -11.943 ); // Copied from the Editor
static const t2dVector g_vSafePathPos( 1000.0, 1000.0 );

CComponentTutorialPathHandler* CComponentTutorialPathHandler::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentTutorialPathHandler );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTutorialPathHandler::CComponentTutorialPathHandler()
    : m_pOwner( NULL )
    , m_bFingerAttachedToPath( false )
    , m_iCurrentCircleObjectIndex( 0 )
    , m_iCurrentSquareObjectIndex( 0 )
    , m_bObjectsHidden( false )
    , m_bPathReached( false )
    , m_bClearedAfterPathReached( false )
{
	CComponentTutorialPathHandler::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTutorialPathHandler::~CComponentTutorialPathHandler()
{
	CComponentTutorialPathHandler::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialPathHandler::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentTutorialPathHandler ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentTutorialPathHandler::onComponentAdd( SimComponent* _pTarget )
{
	if( DynamicConsoleMethodComponent::onComponentAdd( _pTarget ) == false )
		return false;
	
	// Make sure the owner is a t2dSceneObject
	t2dSceneObject* pOwnerObject = dynamic_cast<t2dSceneObject*>( _pTarget );
	if( pOwnerObject == NULL )
	{
		char szString[512];
		sprintf( szString, "%s - Must be added to a t2dSceneObject.", __FUNCTION__ );
		Con::warnf( szString );
		return false;
	}
	
	m_pOwner = pOwnerObject;
    
    CComponentTutorialPathHandler::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialPathHandler::onUpdate()
{
    if( CComponentGlobals::GetInstance().IsTimerActive() == false )
		return;
    
    if( m_bFingerAttachedToPath == false )
    {
        if( g_pTheFinger != NULL && g_pPathObject != NULL ) 
        {
            g_pPathObject->setPosition( g_vDefaultPathPos );
            
            if( g_pTheFinger->getVisible() == false )
            {
                g_pTheFinger->setVisible( true );
                t2dVector fingerPos;
                g_pPathObject->GetNodePosition( 0, fingerPos );
                g_pTheFinger->setPosition( fingerPos );
                //printf( "Setting position to: (%f, %f)\n", fingerPos.mX, fingerPos.mY );
            }
            
            S32 iNumNodes = g_pPathObject->getNodeCount();
            //printf( "iNumNodes: %d\n", iNumNodes );
            g_pPathObject->attachObject( g_pTheFinger,           // Object
                                         g_fFingerSpeed,         // Speed
                                         1,                      // Direction (+ or -)
                                         false,                  // OrientToPath?
                                         0,                      // StartNode
                                         iNumNodes - 3,          //iNumNodes - 1,          // EndNode
                                         T2D_PATH_WRAP,          // Path Mode
                                         1,                      // Loops
                                         true );                 // Send to Start?
            
            
            // Set The Finer image as "Pressed"
            g_pTheFinger->setImageMap( g_szFingerDownImageMapName, g_iFingerDownImageMapFrame );
            
            m_bFingerAttachedToPath = true;
            m_bPathReached = false;
            m_bClearedAfterPathReached = false;
        }
    }
    
    if( m_bPathReached && m_bClearedAfterPathReached == false )
    {
        if( g_pPathObject )
        {
            g_pPathObject->clear();
            m_bClearedAfterPathReached = true;
        }
        
//        if( g_pTheFinger )
//        {
//            g_pTheFinger->setPosition( g_vSafePathPos );
//        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialPathHandler::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
    
    g_pTheFinger = static_cast<t2dStaticSprite*>( Sim::findObject(g_szTheFinger) );
    if( g_pTheFinger != NULL ) 
        g_pTheFinger->setImageMap(g_szFingerUpImageMapName, g_iFingerUpImageMapFrame);
    
    g_pPathObject = dynamic_cast<t2dPath*>(m_pOwner);

    // Get the Line Objects
    
    
    
    m_CircleObjectList.clear();
    m_SquareObjectList.clear();
    
    
    // Create the circle objects (the circles that is placed in between each line)
	g_pTutorialLineDraw_Circle = static_cast<t2dStaticSprite*>( Sim::findObject(g_szTutorialLineDraw_Circle) );
	if( g_pTutorialLineDraw_Circle )
	{
		g_pTutorialLineDraw_Circle->setLayer( LAYER_LINE_DRAW );
		
		const char* pszName = g_pTutorialLineDraw_Circle->getName();
		char szNewName[32];
		
		g_pTutorialLineDraw_Circle->setSize( g_vLineSize );
		m_CircleObjectList.push_back( g_pTutorialLineDraw_Circle );
		
		// Create the circle objects
		t2dSceneObject* pNewCircleObject = NULL;
		for( S32 i = 1; i < g_iMaxNumLineObjects; ++i )
		{
			pNewCircleObject = g_pTutorialLineDraw_Circle->clone();
			if( pNewCircleObject )
			{
				sprintf( szNewName, "%s_%d", pszName, i );
				pNewCircleObject->assignName( szNewName );
				m_CircleObjectList.push_back( pNewCircleObject );
			}
		}
	}
    
    // Create the line objects (the rectangles that are stretched from point to point)
	g_pTutorialLineDraw_Square = static_cast<t2dStaticSprite*>( Sim::findObject(g_szTutorialLineDraw_Square) );
	if( g_pTutorialLineDraw_Square )
	{
		g_pTutorialLineDraw_Square->setLayer( LAYER_LINE_DRAW );
		
		const char* pszName = g_pTutorialLineDraw_Square->getName();
		char szNewName[32];
		
		g_pTutorialLineDraw_Square->setSize( g_vLineSize );
		m_SquareObjectList.push_back( g_pTutorialLineDraw_Square );
		
		// Create the square objects
		t2dSceneObject* pNewSquareObject = NULL;
		for( S32 i = 1; i < g_iMaxNumLineObjects; ++i )
		{
			pNewSquareObject = g_pTutorialLineDraw_Square->clone();
			if( pNewSquareObject )
			{
				sprintf( szNewName, "%s_%d", pszName, i );
				pNewSquareObject->assignName( szNewName );
				m_SquareObjectList.push_back( pNewSquareObject );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// This method handles the automatic "line draw"

/*static*/ void CComponentTutorialPathHandler::OnNodeReached( const t2dVector& _startPos, const t2dVector& _endPos ) 
{
    if( sm_pInstance == NULL )
        return;
    
    if( _startPos == _endPos ) 
    {
        //printf( "Start Pos and End Pos are equal. Ignoring.\n" );
        return;
    }
    
    if( sm_pInstance->m_bObjectsHidden == true )
        return;
    
    //if( g_pTutorialLineDraw_Circle == NULL || g_pTutorialLineDraw_Square == NULL )
    //    return;
    
    t2dStaticSprite* pCircleObject = static_cast<t2dStaticSprite*>( sm_pInstance->m_CircleObjectList[sm_pInstance->m_iCurrentCircleObjectIndex] );
    if( pCircleObject != NULL )
    {
        pCircleObject->setPosition(_endPos);
        pCircleObject->setVisible(true);
        
        sm_pInstance->m_iCurrentCircleObjectIndex++;
        if( sm_pInstance->m_iCurrentCircleObjectIndex >= sm_pInstance->m_CircleObjectList.size() )
            sm_pInstance->m_iCurrentCircleObjectIndex = sm_pInstance->m_CircleObjectList.size() - 1;
    }
    
    t2dStaticSprite* pSquareObject = static_cast<t2dStaticSprite*>( sm_pInstance->m_SquareObjectList[sm_pInstance->m_iCurrentSquareObjectIndex] );
    if( pSquareObject != NULL )
    {
        pSquareObject->setPosition(_endPos);
        pSquareObject->setVisible(true);
        
        // Get the vector from the previous point to the current point
        t2dVector vLastPosToCurrentPos = _endPos - _startPos;
        
        // Get the distance between the previous point to the current point
        F32 fDistance = vLastPosToCurrentPos.len();
        t2dVector vSize = g_pTutorialLineDraw_Square->getSize();
        vSize.mY = fDistance;
        pSquareObject->setSize( vSize );
        
        pSquareObject->setPosition( _endPos );
        
        // Get the angle
        t2dVector vLastPosToCurrentPosNormalized = vLastPosToCurrentPos;
        vLastPosToCurrentPosNormalized.normalise();
        
        F32 fDot = g_vWorldUp.dot( vLastPosToCurrentPosNormalized );
        F32 fAngleRadians = mAcos( fDot );
        
        F32 fAngleDegrees = mRadToDeg( fAngleRadians );
        
        // If the vector is on the left side of the screen, then reverse the angle (since the angle retrieved from the dot product can never be greater than 180 degrees (PI radians)
        if( vLastPosToCurrentPos.mX < 0.0f )
            fAngleDegrees *= -1.0f;
            
        // Set the rotation of the connection object
        pSquareObject->setRotation( fAngleDegrees );
        
        // Set the position of the connection object on the midpoint between the last position and the current position (since an object's position is its center in Torque)
        t2dVector vNewObjectPosition;
        t2dVector vHalfLastToCurrent( 0.5f * vLastPosToCurrentPos.mX, 0.5f * vLastPosToCurrentPos.mY );
        vNewObjectPosition = (vHalfLastToCurrent + _startPos);
        pSquareObject->setPosition(vNewObjectPosition);
        
        sm_pInstance->m_iCurrentSquareObjectIndex++;
        if( sm_pInstance->m_iCurrentSquareObjectIndex >= sm_pInstance->m_SquareObjectList.size() )
            sm_pInstance->m_iCurrentSquareObjectIndex = sm_pInstance->m_SquareObjectList.size() - 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialPathHandler::OnPathEndReached()
{
    if( sm_pInstance == NULL )
        return;
    
    sm_pInstance->m_bPathReached = true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialPathHandler::HideAllLineObjects()
{
    if( sm_pInstance == NULL )
        return;

    sm_pInstance->m_iCurrentCircleObjectIndex = 0;
    sm_pInstance->m_iCurrentSquareObjectIndex = 0;
    
    t2dStaticSprite* pObject = NULL;
    S32 iSize = sm_pInstance->m_CircleObjectList.size();
    for( S32 i = 0; i < iSize; ++i ) 
    {
        pObject = static_cast<t2dStaticSprite*>( sm_pInstance->m_CircleObjectList[i] );
        if( pObject )
            pObject->setVisible( false );
    }
    
    iSize = sm_pInstance->m_SquareObjectList.size();
    for( S32 i = 0; i < iSize; ++i ) 
    {
        pObject = static_cast<t2dStaticSprite*>( sm_pInstance->m_SquareObjectList[i] );
        if( pObject )
            pObject->setVisible( false );
    }
    
    sm_pInstance->m_bObjectsHidden = true;

    //if( g_pPathObject )
    //    g_pPathObject->clear();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialPathHandler::RestartFingerPath()
{
    if( sm_pInstance == NULL )
        return;
    
    // Setting this to false will automatically attach the finger to the path on the next update and will restart
    sm_pInstance->m_bFingerAttachedToPath = false;
    sm_pInstance->m_bObjectsHidden = false;
    
    if( g_pPathObject )
    {
        //g_pPathObject->clear(); // CLEAR RESETS THE NUMBER OF NODES! DONT DO THIS!
        g_pPathObject->setPosition( g_vSafePathPos );
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------


/*static*/ bool CComponentTutorialPathHandler::CanPathSafelyAddNodes()
{
    if( g_pPathObject == NULL )
        return false;
    
    if( g_pPathObject->getNodeCount() > 0 )
        return false;
    
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

