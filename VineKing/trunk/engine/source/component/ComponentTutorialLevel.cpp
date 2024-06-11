//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "ComponentTutorialLevel.h"

#include "ComponentEndLevelScreen.h"
#include "ComponentGlobals.h"
#include "ComponentLandBrick.h"
#include "ComponentLineDrawAttack.h"
#include "ComponentLineDrawAttackObject.h"
#include "ComponentManaBar.h"
#include "ComponentManaSeedItemDrop.h"
#include "ComponentPathGridHandler.h"
#include "ComponentPlayerHomeBase.h"
#include "ComponentSeedMagnetButton.h"
#include "ComponentTutorialPathHandler.h"

#include "SoundManager.h"

#include "core/iTickable.h"
#include "T2D/t2dPath.h"
#include "T2D/t2dSceneObject.h"
#include "T2D/t2dStaticSprite.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char g_szGrayBox[32] = "GrayBox";
static t2dStaticSprite* g_pGrayBox = NULL;

static const char g_szLineDrawImage[32] = "TutorialLevel_LineDraw_Image";
static const char g_szLineDrawButton[32] = "TutorialLevel_LineDraw_Button";

static const char g_szSeedSuckImage[32] = "TutorialLevel_SeedSuck_Image";
static const char g_szSeedSuckButton[32] = "TutorialLevel_SeedSuck_Button";

static const char g_szBlavaImage[32] = "TutorialLevel_Blava_Image";
static const char g_szBlavaButton[32] = "TutorialLevel_Blava_Button";

static const char g_szVineAttackImage[32] = "TutorialLevel_VineAttack_Image";
static const char g_szVineAttackButton[32] = "TutorialLevel_VineAttack_Button";

static const char g_szDamageImage[32] = "TutorialLevel_Damage_Image";
static const char g_szDamageButton[32] = "TutorialLevel_Damage_Button";

static t2dStaticSprite* g_pLineDrawImage = NULL;
static t2dStaticSprite* g_pLineDrawButton = NULL;

static t2dStaticSprite* g_pSeedSuckImage = NULL;
static t2dStaticSprite* g_pSeedSuckButton = NULL;

static t2dStaticSprite* g_pBlavaImage = NULL;
static t2dStaticSprite* g_pBlavaButton = NULL;

static t2dStaticSprite* g_pVineAttackImage = NULL;
static t2dStaticSprite* g_pVineAttackButton = NULL;

static t2dStaticSprite* g_pDamageImage = NULL;
static t2dStaticSprite* g_pDamageButton = NULL;


// HUD STUFF
//static t2dSceneObject* g_pSeedButton_Glow = NULL;
static t2dSceneObject* g_pSeedMagnetButton = NULL; // SeedMagnetButton
static t2dSceneObject* g_pInGameHUD = NULL;
static t2dSceneObject* g_pPauseButton = NULL; //"PauseButton"
static t2dSceneObject* g_pManaBarPart01 = NULL; // ManaBarFrame_TOP
static t2dSceneObject* g_pManaBarPart02 = NULL; // ManaBarFrame_CENTER
static t2dSceneObject* g_pManaBarPart03 = NULL; // ManaBarFrame_BOTTOM
static t2dSceneObject* g_pManaBarPart04 = NULL; // ManaBarObject
static t2dSceneObject* g_pHealthBarPart01 = NULL; // HealthBarFrame_LEFT
static t2dSceneObject* g_pHealthBarPart02 = NULL; // HealthBarFrame_CENTER
static t2dSceneObject* g_pHealthBarPart03 = NULL; // HealthBarFrame_RIGHT
static t2dSceneObject* g_pHealthBarPart04 = NULL; // HealthBarObject

static t2dSceneObject* g_pFinger_Off_On_Anim = NULL; // Finger_Off_On_Anim


static const t2dVector g_vSeedButtonPos( -129.024, 208.896 );



static const t2dVector g_vSafeTutorialObjectPosition( 1000.0f, 1000.0f );


// Scaling and Rotation
static F32 g_fCurrentRotation = 0.0f;

static const F32 g_fMinScale = 0.0f;
static const F32 g_fMaxScale = 1.0f;

static F32 g_fCurrentScale = g_fMinScale;

static t2dStaticSprite* g_pCurrentDisplayImage = NULL;
static t2dStaticSprite* g_pCurrentDisplayButton = NULL;

static const F32 g_fDisplayObjectScaleRate		= 6.0;		// Units of scaling per second
static const S32 g_iDisplayObjectNumRotations	= 0;		// The number of rotations

static const F32 g_fScaleTime = (g_fMaxScale - g_fMinScale) / g_fDisplayObjectScaleRate; // TOTAL_SCALE_TIME = SCALE_DISTANCE / SCALE_RATE
static const F32 g_fTotalDegreesToRotate = 360.0f * (F32)g_iDisplayObjectNumRotations;
static const F32 g_fDisplayObjectRotationRate = g_fTotalDegreesToRotate / g_fScaleTime; // ROTATION_RATE = ROTATION_DISTANCE / TIME ("Distance" means # of degrees to rotate)

static t2dVector g_vOriginalDisplayImageSize;
static t2dVector g_vOriginalDisplayButtonSize;
static t2dVector g_vTempSize;

static const F32 g_fSeedTutorialDelayTimeSeconds = 1.5f;


//static F32 g_fSpawnDelayTime = 0.0f;
//static const F32 g_fSpawnDelayTimeSeconds = 5.0f;


// Fade stuff
static const char g_szFadeBox[32] = "BlackBox";
static t2dStaticSprite* g_pFadeBox = NULL;
static const F32 g_fFadeTimeSeconds = 1.0;

//m_pPathObject->addNode( _vWorldMousePoint, 0.0f, 0.0f );

// This array was grabbed directly from Level_01_01.t2d
static t2dVector g_PathNodeVectorArray[] =
{
    t2dVector( 12.267639,    8.433990   ),
    t2dVector( 28.113312,    -13.545517 ),
    t2dVector( 40.380951,    -27.857758 ),
    t2dVector( 54.182037,    -34.502716 ),
    t2dVector( 69.516571,    -38.080780 ),
    t2dVector( 82.295349,    -36.036179 ),
    t2dVector( 91.496063,    -31.435806 ),
    t2dVector( 97.118744,    -22.235092 ),
    t2dVector( 100.696808,   -10.989761 ),
    t2dVector( 101.719116,   2.811325   ),
    t2dVector( 101.207947,   16.612396  ),
    t2dVector( 100.696808,   30.413483  ),
    t2dVector( 98.141052,    46.770325  ),
    t2dVector( 91.282318,    54.845612  ),
    t2dVector( 83.967407,    58.980133  ),
    t2dVector( 76.652496,    61.524445  ),
    t2dVector( 69.655640,    62.796600  ),
    t2dVector( 62.658752,    63.432678  ),
    t2dVector( 55.343842,    64.068756  ),
    t2dVector( 47.710907,    65.022873  ),
    t2dVector( 41.032074,    65.340912  ),
    t2dVector( 34.035187,    65.976990  ),
    t2dVector( 27.674408,    66.931107  ),
    t2dVector( 20.677551,    66.613068  ),
    t2dVector( 12.726562,    67.885239  ),
    t2dVector( 6.683807,     68.203278  ),
    t2dVector( -1.267181,    67.885239  ),
    t2dVector( -9.218170,    68.203278  ),
    t2dVector( -18.441315,   68.839355  ),
    t2dVector( -27.664459,   70.111511  ),
    t2dVector( -37.841736,   70.429550  ),
    t2dVector( -47.382919,   70.429550  ),
    t2dVector( -55.969986,   70.429550  ),
    t2dVector( -65.829208,   70.111511  ),
    t2dVector( -76.006470,   67.885239  ),
    t2dVector( -84.275497,   61.842484  ),
    t2dVector( -89.046082,   54.209534  ),
    t2dVector( -90.000214,   43.396194  ),
    t2dVector( -90.000214,   31.310684  ),
    t2dVector( -90.000214,   21.133423  ),
    t2dVector( -90.636292,   11.592239  ),
    t2dVector( -93.816681,   1.733017   ),
    t2dVector( -96.997070,   -6.536011  ),
    t2dVector( -100.177475,  -15.441116 ),
    t2dVector( -102.721786,  -26.254463 ),
    t2dVector( -104.948059,  -33.251328 ),
    t2dVector( -106.177719,  -38.782639 ),
    t2dVector( -108.128464,  -45.654869 ),
    t2dVector( -110.036697,  -54.559975 ),
    t2dVector( -109.400620,  -62.829002 ),
    t2dVector( -107.492386,  -69.189796 ),
    t2dVector( -105.584137,  -76.822746 ),
    t2dVector( -104.630020,  -84.455688 ),
    t2dVector( -101.131592,  -91.452560 ),
    t2dVector( -95.088837,   -94.314911 ),
    t2dVector( -88.728043,   -93.042755 ),
    t2dVector( -84.275497,   -87.318047 ),
    t2dVector( -79.822937,   -81.275299 ),
    t2dVector( -72.508041,   -70.779991 ),
    t2dVector( 5.088684,     13.883469  ),
};

//node0     = "12.267639    8.433990        0.000000 10.000000";
//node1     = "28.113312    -13.545517      0.000000 10.000000";
//node2     = "40.380951    -27.857758      0.000000 10.000000";
//node3     = "54.182037    -34.502716      0.000000 10.000000";
//node4     = "69.516571    -38.080780      0.000000 10.000000";
//node5     = "82.295349    -36.036179      0.000000 10.000000";
//node6     = "91.496063    -31.435806      0.000000 10.000000";
//node7     = "97.118744    -22.235092      0.000000 10.000000";
//node8     = "100.696808   -10.989761      0.000000 10.000000";
//node9     = "101.719116   2.811325        0.000000 10.000000";
//Node10    = "101.207947   16.612396       0.000000 10.000000";
//Node11    = "100.696808   30.413483       0.000000 10.000000";
//Node12    = "98.141052    46.770325       0.000000 10.000000";
//Node13    = "91.282318    54.845612       0.000000 10.000000";
//Node14    = "83.967407    58.980133       0.000000 10.000000";
//Node15    = "76.652496    61.524445       0.000000 10.000000";
//node16    = "69.655640    62.796600       0.000000 10.000000";
//node17    = "62.658752    63.432678       0.000000 10.000000";
//node18    = "55.343842    64.068756       0.000000 10.000000";
//node19    = "47.710907    65.022873       0.000000 10.000000";
//node20    = "41.032074    65.340912       0.000000 10.000000";
//node21    = "34.035187    65.976990       0.000000 10.000000";
//node22    = "27.674408    66.931107       0.000000 10.000000";
//node23    = "20.677551    66.613068       0.000000 10.000000";
//node24    = "12.726562    67.885239       0.000000 10.000000";
//node25    = "6.683807     68.203278       0.000000 10.000000";
//node26    = "-1.267181    67.885239       0.000000 10.000000";
//node27    = "-9.218170    68.203278       0.000000 10.000000";
//node28    = "-18.441315   68.839355       0.000000 10.000000";
//node29    = "-27.664459   70.111511       0.000000 10.000000";
//node30    = "-37.841736   70.429550       0.000000 10.000000";
//node31    = "-47.382919   70.429550       0.000000 10.000000";
//node32    = "-55.969986   70.429550       0.000000 10.000000";
//node33    = "-65.829208   70.111511       0.000000 10.000000";
//node34    = "-76.006470   67.885239       0.000000 10.000000";
//node35    = "-84.275497   61.842484       0.000000 10.000000";
//node36    = "-89.046082   54.209534       0.000000 10.000000";
//node37    = "-90.000214   43.396194       0.000000 10.000000";
//node38    = "-90.000214   31.310684       0.000000 10.000000";
//node39    = "-90.000214   21.133423       0.000000 10.000000";
//node40    = "-90.636292   11.592239       0.000000 10.000000";
//node41    = "-93.816681   1.733017        0.000000 10.000000";
//node42    = "-96.997070   -6.536011       0.000000 10.000000";
//node43    = "-100.177475  -15.441116      0.000000 10.000000";
//node44    = "-102.721786  -26.254463      0.000000 10.000000";
//node45    = "-104.948059  -33.251328      0.000000 10.000000";
//node46    = "-106.177719  -38.782639      0.000000 10.000000";
//node47    = "-108.128464  -45.654869      0.000000 10.000000";
//node48    = "-110.036697  -54.559975      0.000000 10.000000";
//node49    = "-109.400620  -62.829002      0.000000 10.000000";
//node50    = "-107.492386  -69.189796      0.000000 10.000000";
//node51    = "-105.584137  -76.822746      0.000000 10.000000";
//node52    = "-104.630020  -84.455688      0.000000 10.000000";
//node53    = "-101.131592  -91.452560      0.000000 10.000000";
//node54    = "-95.088837   -94.314911      0.000000 10.000000";
//node55    = "-88.728043   -93.042755      0.000000 10.000000";
//node56    = "-84.275497   -87.318047      0.000000 10.000000";
//node57    = "-79.822937   -81.275299      0.000000 10.000000";
//node58    = "-72.508041   -70.779991      0.000000 10.000000";
//node59    = "5.088684     13.883469       0.000000 10.000000";


//t2dVector( 12.267639, 8.433990 ), 

//t2dVector( 12.267639,    8.433990   ),
//t2dVector( 28.113312,    -13.545517 ),
//t2dVector( 101.207947,   16.612396  ),
//t2dVector( 100.696808,   30.413483  ),
//t2dVector( 98.141052,    46.770325  ),
//t2dVector( 91.282318,    54.845612  ),
//t2dVector( 83.967407,    58.980133  ),
//t2dVector( 76.652496,    61.524445  ),
//t2dVector( 69.655640,    62.796600  ),
//t2dVector( 62.658752,    63.432678  ),
//t2dVector( 55.343842,    64.068756  ),
//t2dVector( 47.710907,    65.022873  ),
//t2dVector( 40.380951,    -27.857758 ),
//t2dVector( 41.032074,    65.340912  ),
//t2dVector( 34.035187,    65.976990  ),
//t2dVector( 27.674408,    66.931107  ),
//t2dVector( 20.677551,    66.613068  ),
//t2dVector( 12.726562,    67.885239  ),
//t2dVector( 6.683807,     68.203278  ),
//t2dVector( -1.267181,    67.885239  ),
//t2dVector( -9.218170,    68.203278  ),
//t2dVector( -18.441315,   68.839355  ),
//t2dVector( -27.664459,   70.111511  ),
//t2dVector( 54.182037,    -34.502716 ),
//t2dVector( -37.841736,   70.429550  ),
//t2dVector( -47.382919,   70.429550  ),
//t2dVector( -55.969986,   70.429550  ),
//t2dVector( -65.829208,   70.111511  ),
//t2dVector( -76.006470,   67.885239  ),
//t2dVector( -84.275497,   61.842484  ),
//t2dVector( -89.046082,   54.209534  ),
//t2dVector( -90.000214,   43.396194  ),
//t2dVector( -90.000214,   31.310684  ),
//t2dVector( -90.000214,   21.133423  ),
//t2dVector( 69.516571,    -38.080780 ),
//t2dVector( -90.636292,   11.592239  ),
//t2dVector( -93.816681,   1.733017   ),
//t2dVector( -96.997070,   -6.536011  ),
//t2dVector( -100.177475,  -15.441116 ),
//t2dVector( -102.721786,  -26.254463 ),
//t2dVector( -104.948059,  -33.251328 ),
//t2dVector( -106.177719,  -38.782639 ),
//t2dVector( -108.128464,  -45.654869 ),
//t2dVector( -110.036697,  -54.559975 ),
//t2dVector( -109.400620,  -62.829002 ),
//t2dVector( 82.295349,    -36.036179 ),
//t2dVector( -107.492386,  -69.189796 ),
//t2dVector( -105.584137,  -76.822746 ),
//t2dVector( -104.630020,  -84.455688 ),
//t2dVector( -101.131592,  -91.452560 ),
//t2dVector( -95.088837,   -94.314911 ),
//t2dVector( -88.728043,   -93.042755 ),
//t2dVector( -84.275497,   -87.318047 ),
//t2dVector( -79.822937,   -81.275299 ),
//t2dVector( -72.508041,   -70.779991 ),
//t2dVector( 5.088684,     13.883469  ),
//t2dVector( 91.496063,    -31.435806 ),
//t2dVector( 97.118744,    -22.235092 ),
//t2dVector( 100.696808,   -10.989761 ),
//t2dVector( 101.719116,   2.811325   ),



//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTutorialLevel* CComponentTutorialLevel::sm_pInstance = NULL;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1( CComponentTutorialLevel );

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTutorialLevel::CComponentTutorialLevel()
	: m_pOwner( NULL )
	, m_eTutorialState( TUTORIAL_STATE_NONE )
	, m_pCurrentDisplayObject( NULL )
    , m_pTextImageTop( NULL )
    , m_pTextImageBottom( NULL )
    , m_bTutorialTextDisplayed( false )
    , m_pTheFinger( NULL )
    , m_pFingerPath( NULL )
    , m_eBillboardState( BILLBOARD_STATE_NONE )
    , m_bUpdateFingerRepeating( true )
    , m_bFingerPausedToLift( false )
    , m_bFingerLifted( false )
    , m_bFingerVanished( false )
    , m_bNodesReadded( false )
    , m_fFingerPausedTimer( 0.0f ) 
    , m_fFingerLiftedTimer( 0.0f )
	, m_bTutorialShown_LineDraw( false )
	, m_bTutorialShown_SeedSuck( false )
	, m_bTutorialShown_Blava( false )
	, m_bTutorialShown_VineAttack( false )
	, m_bTutorialShown_Damage( false )
	, m_bSpawningEnabled( false )
	, m_bTutorialSeedSuckEnabled( false )
	, m_bTutorialBlavaEnabled( false )
	, m_bTutorialSeedSuckInProgress( false )
	, m_bTutorialBlavaInProgress( false )
	, m_bSeedTutorialTimerStarted( false )
	, m_bSeedTutorialTimerActive( false )
	, m_fSeedTutorialDisplayDelayTimer( 0.0f )
	, m_bTimeScaleStopped( false )

    , m_eFadeState( FADE_STATE_NONE )
    , m_fFadeTimer( 0.0 )
    , m_fFadeAmount( 0.0 )
{
	CComponentTutorialLevel::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CComponentTutorialLevel::~CComponentTutorialLevel()
{
	CComponentTutorialLevel::sm_pInstance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentTutorialLevel::IsPauseAllowed()
{
	if( sm_pInstance == NULL )
		return true;
	
	//if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_GAMEPLAY || sm_pInstance->m_eTutorialState == TUTORIAL_STATE_FINISHED )
		return true;
	
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentTutorialLevel::IsLineDrawAllowed()
{
	if( sm_pInstance == NULL )
		return true;
	
	//if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_GAMEPLAY || sm_pInstance->m_eTutorialState == TUTORIAL_STATE_FINISHED )
		return true;
	
	//return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentTutorialLevel::IsSpawningAllowed()
{
	if( sm_pInstance == NULL )
		return true;
	
//	if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_FINISHED )
//		return true;
	
	if( sm_pInstance->m_bSpawningEnabled )
		return true;
	
//	if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_GAMEPLAY )
//	{
//		if( sm_pInstance->m_bTutorialShown_LineDraw == true && sm_pInstance->m_bTutorialShown_SeedSuck == true )
//		{
//			return true;
//		}
//	}
	
	return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentTutorialLevel::IsManaSeedButtonAllowed()
{
	if( sm_pInstance == NULL )
		return true;
	
	if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_GAMEPLAY )
		return true;
	
	//if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_FINISHED )
		return true;
	
	//return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::StartTutorial()
{
	//printf( "Starting tutorial...\n" );
	
	//ShowTutorialLineDraw();
    if( sm_pInstance == NULL )
        return;
    
    sm_pInstance->m_eTutorialState = TUTORIAL_STATE_THE_FINGER_DRAWING;
    
	sm_pInstance->m_bTimeScaleStopped = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::StartTutorialState( int _iTutorialState ) {
    
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnTheFingerPathEndReached() 
{
    //printf( "OnTheFingerPathEndReached\n" );
    if( sm_pInstance == NULL )
        return;
    
    sm_pInstance->m_bFingerPausedToLift = true;
    
//    if( sm_pInstance->m_pTheFinger )
//        sm_pInstance->m_pTheFinger->setPosition( t2dVector::getZero() );
//    
//    if( sm_pInstance->m_pFingerPath )
//        sm_pInstance->m_pFingerPath->clear();
    
    //sm_pInstance->AddAllPathNodes();

    // THIS ALSO CRASHES
    // This clears the path it is attached to so it can be reattached later
//    if( sm_pInstance->m_pTheFinger && sm_pInstance->m_pFingerPath )
//    {
//        printf( "DETACHING THE FINGER FROM THE PATH!\n" );
//        //sm_pInstance->m_pTheFinger->setAttachedToPath( NULL );
//        sm_pInstance->m_pFingerPath->detachObject( sm_pInstance->m_pTheFinger );
//    }
    
    // Clear out the Path's list of objects -- THIS CRASHES
//    if( sm_pInstance->m_pFingerPath )
//        sm_pInstance->m_pFingerPath->ClearObjectList();
    
    
    
//    CComponentTutorialPathHandler::HideAllLineObjects();
//    
//    if( sm_pInstance )
//    {
//        if( sm_pInstance->m_pTheFinger )
//        {
//            sm_pInstance->m_pTheFinger->setVisible( false );
//        }
//    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnPlayerDrewLine() 
{
    //printf( "OnPlayerDrewLine\n" );
    
    if( sm_pInstance == NULL )
        return;
    
//    if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_THE_FINGER_DRAWING )
//    {
//        // Hide the text objects
//        if( sm_pInstance->m_pTextImageTop )
//            sm_pInstance->m_pTextImageTop->setVisible(false);
//        
//        if( sm_pInstance->m_pTextImageBottom )
//            sm_pInstance->m_pTextImageBottom->setVisible(false);
//    }
    
    // Hide the Finger
    if( sm_pInstance->m_pTheFinger )
        sm_pInstance->m_pTheFinger->setVisible(false);
    
    // Hide all the line objects
    CComponentTutorialPathHandler::HideAllLineObjects();
    
    sm_pInstance->m_bUpdateFingerRepeating = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnBrickBroken()
{
    //printf( "CComponentTutorialLevel::OnBrickBroken\n" );

    if( sm_pInstance == NULL )
        return;
    
    if( sm_pInstance->m_eTutorialState > TUTORIAL_STATE_THE_FINGER_DRAWING )
        return;
    
    sm_pInstance->m_eTutorialState = TUTORIAL_STATE_BREAKING_ROCKS;
    
    if( sm_pInstance->m_pTextImageTop )
    {
        sm_pInstance->m_pTextImageTop->setImageMap( "TT03ImageMap", 0 );
        sm_pInstance->m_pTextImageTop->setVisible( true );
    }
    
    if( sm_pInstance->m_pTextImageBottom )
    {
        sm_pInstance->m_pTextImageBottom->setImageMap( "TT04ImageMap", 0 );
        sm_pInstance->m_pTextImageBottom->setVisible( true );
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::FadeToNextTutorialState()
{
    //printf( "CComponentTutorialLevel::FadeToNextTutorialState()\n" );
    
    if( sm_pInstance == NULL )
        return;
    
    sm_pInstance->m_eFadeState = FADE_STATE_FADING_OUT;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentTutorialLevel::IsBrickBreakingAllowed() // NOT USED
{
    if( sm_pInstance == NULL )
        return true;
    
    if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_THE_FINGER_DRAWING ||
        sm_pInstance->m_eTutorialState == TUTORIAL_STATE_BREAKING_ROCKS )
    {
        return false;
    }
    
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentTutorialLevel::IsManaDrainingAllowed()
{
    if( sm_pInstance == NULL )
        return true;
    
    if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_THE_FINGER_DRAWING ||
        sm_pInstance->m_eTutorialState == TUTORIAL_STATE_BREAKING_ROCKS )
    {
        return false;
    }
    
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentTutorialLevel::IsSeedSpawningAllowed()
{
    if( sm_pInstance == NULL )
        return true;
    
    if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_THE_FINGER_DRAWING ||
        sm_pInstance->m_eTutorialState == TUTORIAL_STATE_BREAKING_ROCKS || 
        sm_pInstance->m_eTutorialState == TUTORIAL_STATE_GREEN_METER )
    {
        return false;
    }
    
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnPlayerOutOfMana()
{
    if( sm_pInstance == NULL )
        return;
    
    if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_GREEN_METER )
    {
        sm_pInstance->m_eTutorialState = TUTORIAL_STATE_GATHERING_SEEDS;
        
        // Show the new Tutorial Messages
        if( sm_pInstance->m_pTextImageTop )
        {
            sm_pInstance->m_pTextImageTop->setImageMap( "TT07ImageMap", 0 );
            sm_pInstance->m_pTextImageTop->setVisible( true );
        }
        
        if( sm_pInstance->m_pTextImageBottom )
        {
            sm_pInstance->m_pTextImageBottom->setImageMap( "TT07ImageMap", 0 );
            sm_pInstance->m_pTextImageBottom->setVisible( true );
        }
        
        //CComponentSeedMagnetButton::ShowSeedMagnetButton();
        //if( g_pSeedButton_Glow )
        //    g_pSeedButton_Glow->setVisible(true);
        
        if( g_pSeedMagnetButton )
        {
            //g_pSeedMagnetButton->setPosition( g_vSeedButtonPos );
            g_pSeedMagnetButton->setVisible( true );
        }
        
        // Place the Finger Press animation on the seed button
        if( g_pFinger_Off_On_Anim )
        {
            t2dVector vPos;
            CComponentSeedMagnetButton::GetSeedMagnetButtonPosition( vPos ); // (-129.024, 208.896)
            vPos.mY -= 10.0; //
            g_pFinger_Off_On_Anim->setPosition( vPos );
            g_pFinger_Off_On_Anim->setVisible( true );
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnPlayerEatSeed()
{
    if( sm_pInstance == NULL )
        return;
    
    if( sm_pInstance->m_eTutorialState != TUTORIAL_STATE_GATHERING_SEEDS )
        return;
    
    if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_GATHERING_SEEDS )
    {
        // Show the new Tutorial Messages
        if( sm_pInstance->m_pTextImageTop )
        {
            sm_pInstance->m_pTextImageTop->setImageMap( "TT08ImageMap", 0 );
            sm_pInstance->m_pTextImageTop->setVisible( true );
        }
        
        if( sm_pInstance->m_pTextImageBottom )
        {
            sm_pInstance->m_pTextImageBottom->setImageMap( "TT09ImageMap", 0 );
            sm_pInstance->m_pTextImageBottom->setVisible( true );
        }
    }
    sm_pInstance->m_eTutorialState = TUTORIAL_STATE_EATING_SEEDS;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnPlayerPressedSeedButton()
{
    if( g_pFinger_Off_On_Anim && g_pFinger_Off_On_Anim->getVisible() == true )
        g_pFinger_Off_On_Anim->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnMonsterSpawned()
{ 
    
    if( sm_pInstance == NULL )
        return;
    
    if( sm_pInstance->m_bTutorialShown_Blava == true )
        return;
    
    ShowTutorialBlava();
    sm_pInstance->m_bTutorialShown_Blava = true;
    
//		// If spawning is enabled, then see if the Blava tutorial can be shown
//		if( m_bTutorialBlavaEnabled &&
//		    m_bTutorialBlavaInProgress == false &&
//		    m_bTutorialSeedSuckInProgress == false && 
//		    m_eTutorialState == TUTORIAL_STATE_GAMEPLAY && 
//		    m_bTutorialShown_Blava == false )
//		{
//			ShowTutorialBlava();
//		}		
//	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnMonsterTapped()
{
    if( sm_pInstance == NULL )
        return;
    
    if( sm_pInstance->m_eTutorialState != TUTORIAL_STATE_FINISHING_LEVEL )
    {
        sm_pInstance->m_eTutorialState = TUTORIAL_STATE_FINISHING_LEVEL;
        
        if( sm_pInstance->m_pTextImageBottom )
            sm_pInstance->m_pTextImageBottom->setImageMap( "TT06ImageMap", 0 );
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentTutorialLevel::CanLevelEnd()
{
    if( sm_pInstance == NULL )
        return true;
    
    if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_SPAWNING_MONSTERS ||
        sm_pInstance->m_eTutorialState == TUTORIAL_STATE_FINISHING_LEVEL )
    {
        return true;
    }
    
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnLevelEnded()
{
    if( sm_pInstance == NULL )
        return;
    
    if( sm_pInstance->m_pTextImageTop )
        sm_pInstance->m_pTextImageTop->setVisible( false );
    
    if( sm_pInstance->m_pTextImageBottom )
        sm_pInstance->m_pTextImageBottom->setVisible( false );
    
    if( sm_pInstance->m_pTheFinger )
        sm_pInstance->m_pTheFinger->setVisible( false );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnSufficientNumBricksBroken()
{
	if( sm_pInstance == NULL )
		return;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::ShowTutorialLineDraw()
{
//	if( sm_pInstance == NULL )
//		return;
//	
//	if( g_pGrayBox )
//		g_pGrayBox->setVisible( true );
//	
//	g_fCurrentRotation = 0.0f;
//	g_fCurrentScale = g_fMinScale;
//	
//	sm_pInstance->m_eTutorialState = TUTORIAL_STATE_DISPLAY_LINEDRAW_SCALING;
//	
//	sm_pInstance->InitializeCurrentDisplayObject( g_pLineDrawImage, g_pLineDrawButton );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::ShowTutorialSeedSuck()
{
	if( sm_pInstance == NULL )
		return;
	
//	if( sm_pInstance->m_eTutorialState == TUTORIAL_STATE_DISPLAY_SEEDSUCK_SCALING ||
//	    sm_pInstance->m_eTutorialState == TUTORIAL_STATE_DISPLAY_SEEDSUCK_FULL_SIZE )
//	{
//		return;
//	}
//	
//	if( g_pGrayBox )
//		g_pGrayBox->setVisible( true );
//	
//	g_fCurrentRotation = 0.0f;
//	g_fCurrentScale = g_fMinScale;
//	
//	sm_pInstance->m_eTutorialState = TUTORIAL_STATE_DISPLAY_SEEDSUCK_SCALING;
//	
//	sm_pInstance->InitializeCurrentDisplayObject( g_pSeedSuckImage, g_pSeedSuckButton );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::ShowTutorialBlava()
{
	if( sm_pInstance == NULL )
		return;

	if( g_pGrayBox )
		g_pGrayBox->setVisible( true );
	
	g_fCurrentRotation = 0.0f;
	g_fCurrentScale = g_fMinScale;
	
	//sm_pInstance->m_eTutorialState = TUTORIAL_STATE_DISPLAY_BLAVA_SCALING;
    sm_pInstance->m_eBillboardState = BILLBOARD_STATE_SCALING;
	
	sm_pInstance->InitializeCurrentDisplayObject( g_pBlavaImage, g_pBlavaButton );
	
	sm_pInstance->m_bTutorialBlavaInProgress = true;
    
    CSoundManager::GetInstance().PlayTutorialStingerSound();
    
    if( sm_pInstance->m_pTextImageTop )
        sm_pInstance->m_pTextImageTop->setVisible( false );
    
    if( sm_pInstance->m_pTextImageBottom )
        sm_pInstance->m_pTextImageBottom->setVisible( false );
    
    CComponentSeedMagnetButton::DisableInput();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::ShowTutorialVineAttack()
{
	if( sm_pInstance == NULL )
		return;
	
//	if( g_pGrayBox )
//		g_pGrayBox->setVisible( true );
//	
//	g_fCurrentRotation = 0.0f;
//	g_fCurrentScale = g_fMinScale;
//	
//	sm_pInstance->m_eTutorialState = TUTORIAL_STATE_DISPLAY_VINEATTACK_SCALING;
//	
//	sm_pInstance->InitializeCurrentDisplayObject( g_pVineAttackImage, g_pVineAttackButton );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::ShowTutorialDamage()
{
	if( sm_pInstance == NULL )
		return;
	
//	if( g_pGrayBox )
//		g_pGrayBox->setVisible( true );
//	
//	g_fCurrentRotation = 0.0f;
//	g_fCurrentScale = g_fMinScale;
//	
//	sm_pInstance->m_eTutorialState = TUTORIAL_STATE_DISPLAY_DAMAGE_SCALING;
//	
//	sm_pInstance->InitializeCurrentDisplayObject( g_pDamageImage, g_pDamageButton );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnTutorialButtonPressedLineDraw()
{
	//printf( "OnTutorialButtonPressedLineDraw\n" );
	
	if( sm_pInstance == NULL )
		return;
	
	if( g_pGrayBox )
		g_pGrayBox->setVisible( false );
	
	if( g_pCurrentDisplayImage )
		g_pCurrentDisplayImage->setVisible( false );
	
	sm_pInstance->m_eTutorialState = TUTORIAL_STATE_GAMEPLAY;
	sm_pInstance->m_bTutorialShown_LineDraw = true;
	
	Con::setVariable( "$timeScale", "1" );
	sm_pInstance->m_bTimeScaleStopped = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnTutorialButtonPressedSeedSuck()
{
	//printf( "OnTutorialButtonPressedSeedSuck\n" );
	
	if( sm_pInstance == NULL )
		return;
	
	if( g_pGrayBox )
		g_pGrayBox->setVisible( false );
	
	if( g_pCurrentDisplayImage )
		g_pCurrentDisplayImage->setVisible( false );
	
	sm_pInstance->m_eTutorialState = TUTORIAL_STATE_GAMEPLAY;
	sm_pInstance->m_bTutorialShown_SeedSuck = true;
	
	Con::setVariable( "$timeScale", "1" );
	sm_pInstance->m_bTimeScaleStopped = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnTutorialButtonPressedBlava()
{
	//printf( "OnTutorialButtonPressedBlava\n" );
	
	if( sm_pInstance == NULL )
		return;
	
	if( g_pGrayBox )
		g_pGrayBox->setVisible( false );
	
	if( g_pCurrentDisplayImage )
		g_pCurrentDisplayImage->setVisible( false );
	
	//sm_pInstance->m_eTutorialState = TUTORIAL_STATE_DISPLAY_VINEATTACK_SCALING;
	sm_pInstance->m_bTutorialShown_Blava = true;
	
	Con::setVariable( "$timeScale", "1" ); // This needs to be here because the timeScale must be enabled in order for the display objects to update
	sm_pInstance->m_bTimeScaleStopped = false;
	
    sm_pInstance->m_eBillboardState = BILLBOARD_STATE_FINISHED;
	//ShowTutorialVineAttack();
    
    if( sm_pInstance->m_pTextImageTop )
        sm_pInstance->m_pTextImageTop->setVisible( true );
    
    if( sm_pInstance->m_pTextImageBottom )
        sm_pInstance->m_pTextImageBottom->setVisible( true );
    
    CComponentSeedMagnetButton::EnableInput();
    
    if( sm_pInstance->m_pTextImageTop )
        sm_pInstance->m_pTextImageTop->setImageMap( "TT10ImageMap", 0 );
    
    if( sm_pInstance->m_pTextImageBottom )
        sm_pInstance->m_pTextImageBottom->setImageMap( "TT11ImageMap", 0 );
    
    if( g_pHealthBarPart01 )
        g_pHealthBarPart01->setVisible( true );
    
    if( g_pHealthBarPart02 )
        g_pHealthBarPart02->setVisible( true );
    
    if( g_pHealthBarPart03 )
        g_pHealthBarPart03->setVisible( true );
    
    if( g_pHealthBarPart04 )
        g_pHealthBarPart04->setVisible( true );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnTutorialButtonPressedVineAttack()
{
	//printf( "OnTutorialButtonPressedVineAttack\n" );
	
//	if( sm_pInstance == NULL )
//		return;
//	
//	if( g_pGrayBox )
//		g_pGrayBox->setVisible( false );
//	
//	if( g_pCurrentDisplayImage )
//		g_pCurrentDisplayImage->setVisible( false );
//	
//	sm_pInstance->m_eTutorialState = TUTORIAL_STATE_DISPLAY_DAMAGE_SCALING;
//	sm_pInstance->m_bTutorialShown_VineAttack = true;
//	
//	Con::setVariable( "$timeScale", "1" ); // This needs to be here because the timeScale must be enabled in order for the display objects to update
//	sm_pInstance->m_bTimeScaleStopped = false;
//	
//	ShowTutorialDamage();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::OnTutorialButtonPressedDamage()
{
//	//printf( "OnTutorialButtonPressedDamage\n" );
//	
//	if( sm_pInstance == NULL )
//		return;
//	
//	if( g_pGrayBox )
//		g_pGrayBox->setVisible( false );
//	
//	if( g_pCurrentDisplayImage )
//		g_pCurrentDisplayImage->setVisible( false );
//	
//	sm_pInstance->m_eTutorialState = TUTORIAL_STATE_GAMEPLAY;
//	sm_pInstance->m_bTutorialShown_Damage = true;
//	
//	Con::setVariable( "$timeScale", "1" ); // This needs to be here because the timeScale must be enabled in order for the display objects to update
//	sm_pInstance->m_bTimeScaleStopped = false;
//	
//	sm_pInstance->m_bTutorialBlavaInProgress = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ bool CComponentTutorialLevel::IsTutorialActive()
{
	if( sm_pInstance == NULL )
		return false;
    
    if( sm_pInstance->m_eBillboardState == BILLBOARD_STATE_SCALING ||
        sm_pInstance->m_eBillboardState == BILLBOARD_STATE_FULL_SIZE )
    {
        return true;
    }
    
    return false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::AddAllPathNodes()
{
    if( m_pFingerPath )
    { 
        //printf( "iNumNodes BEFORE ADDING: %d\n", m_pFingerPath->getNodeCount() );
        
        // Re-Add ALL the nodes MANUALLY
        S32 iNumNodes = sizeof( g_PathNodeVectorArray ) / sizeof( t2dVector );
        for( S32 i = 0; i < iNumNodes; ++i )
        {
            m_pFingerPath->addNode( g_PathNodeVectorArray[i], 0.0f, 0.0f );
        }
        
        //printf( "iNumNodes AFTER ADDING: %d\n", m_pFingerPath->getNodeCount() );
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void CComponentTutorialLevel::initPersistFields()
{
	DynamicConsoleMethodComponent::initPersistFields();
	
	addProtectedField( "Owner", TypeSimObjectPtr, Offset( m_pOwner, CComponentTutorialLevel ), &defaultProtectedSetFn, &defaultProtectedGetFn, "" );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CComponentTutorialLevel::onComponentAdd( SimComponent* _pTarget )
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
	
	CComponentTutorialLevel::sm_pInstance = this;
	
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::onUpdate()
{
	if( m_pOwner == NULL /*|| m_eTutorialState == TUTORIAL_STATE_NONE*/ )
		return;
    
    if( m_eTutorialState < TUTORIAL_STATE_GATHERING_SEEDS )
    {
        if( CComponentSeedMagnetButton::IsSeedMagnetButtonVisible() )
        {
            CComponentSeedMagnetButton::HideSeedMagnetButton();
        } 
    }
    
    if( CComponentGlobals::GetInstance().IsTimerActive() == false )
    {
        if( m_eFadeState != FADE_STATE_NONE )
            UpdateFade();
        else 
        {
            //printf( "CComponentTutorialLevel -- timer is NOT active!\n" );
            return;   
        }
    }
    
    
    if( m_bTutorialTextDisplayed == false )
    {
        if( m_pTextImageTop )
        {
            m_pTextImageTop->setVisible(true);
        }

        if( m_pTextImageBottom )
        {
            m_pTextImageBottom->setVisible(true);
        }
        
        m_bTutorialTextDisplayed = true;
    }
    
    // Update the image zoom
    if( m_eBillboardState == BILLBOARD_STATE_SCALING )
    {
        UpdateCurrentDisplayObjectEntry();
    }
    
    // Stop time when fully updated
    if( m_eBillboardState == BILLBOARD_STATE_FULL_SIZE )
    {
        if( m_bTimeScaleStopped == false )
        {
            Con::setVariable( "$timeScale", "0" );
            m_bTimeScaleStopped = true;
        }
    }
    
    if( m_bUpdateFingerRepeating )
    {
        if( m_bFingerPausedToLift )
        {
            m_fFingerPausedTimer += ITickable::smTickSec;
            if( m_fFingerPausedTimer >= 0.5f )
            {
                CComponentTutorialPathHandler::HideAllLineObjects();
                if( m_pTheFinger )
                    m_pTheFinger->setImageMap( "finger_off_onImageMap", 0 ); // Set the image to "Up"
                m_fFingerPausedTimer = 0.0f;
                m_bFingerPausedToLift = false;
                m_bFingerLifted = true;
            }
        }
        else if( m_bFingerLifted )
        {
            m_fFingerLiftedTimer += ITickable::smTickSec;
            if( m_fFingerLiftedTimer >= 1.0 )
            {
                if( m_pTheFinger )
                {
                    m_pTheFinger->setVisible( false );
                    m_pTheFinger->setPosition( g_vSafeTutorialObjectPosition );
                }
                
                m_fFingerLiftedTimer = 0.0f;
                m_bFingerLifted = false;
                m_bFingerVanished = true;
            }
        }
        else if( m_bFingerVanished )
        {
            if( m_bNodesReadded == false )
            {
                if( CComponentTutorialPathHandler::CanPathSafelyAddNodes() == true )
                {
                    AddAllPathNodes();
                    m_bNodesReadded = true;
                }
            }
            
            m_fFingerVanishedTimer += ITickable::smTickSec;
            if( m_fFingerVanishedTimer >= 1.0f )
            {
                if( m_bNodesReadded == true )
                {
                    CComponentTutorialPathHandler::RestartFingerPath();
                    m_fFingerVanishedTimer = 0.0;
                    m_bFingerVanished = false;
                    m_bNodesReadded = false;
                }
            }
        }
    }
    
    

//	// Failsafe in the very unlikely case a tutorial is displaying while the game has finished
//	if( CComponentEndLevelScreen::HasLevelEnded() )
//	{
//		// Check if a Tutorial is active and disable them
//		if( m_eTutorialState != TUTORIAL_STATE_GAMEPLAY && m_eTutorialState != TUTORIAL_STATE_FINISHED )
//		{
//			//printf( "Forcing Tutorial to finish\n" );
//
//			const char* pszTimeScale = Con::getVariable( "$timeScale" );
//			if( pszTimeScale && strcmp( pszTimeScale, "0" ) == 0 )
//			{
//				Con::setVariable( "$timeScale", "1" );
//				m_bTimeScaleStopped = false;
//			}
//			   
//			if( g_pLineDrawImage )
//				g_pLineDrawImage->setVisible( false );
//
//			if( g_pLineDrawButton )
//				g_pLineDrawButton->setVisible( false );
//
//			if( g_pSeedSuckImage )
//				g_pSeedSuckImage->setVisible( false );
//
//			if( g_pSeedSuckButton )
//				g_pSeedSuckButton->setVisible( false );
//
//			if( g_pBlavaImage )
//				g_pBlavaImage->setVisible( false );
//
//			if( g_pBlavaButton )
//				g_pBlavaButton->setVisible( false );
//
//			if( g_pVineAttackImage )
//				g_pVineAttackImage->setVisible( false );
//
//			if( g_pVineAttackButton )
//				g_pVineAttackButton->setVisible( false );
//
//			if( g_pDamageImage )
//				g_pDamageImage->setVisible( false );
//
//			if( g_pDamageButton )
//				g_pDamageButton->setVisible( false );
//
//			m_eTutorialState = TUTORIAL_STATE_FINISHED;
//		}
//	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::OnPostInit()
{
	if( m_pOwner == NULL )
		return;
	
	g_pGrayBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szGrayBox ) );
	if( g_pGrayBox )
	{
		g_pGrayBox->setVisible( false );
		g_pGrayBox->setPosition( t2dVector::getZero() );
		g_pGrayBox->setLayer( LAYER_TUTORIAL_IMAGE + 1 ); // Force the Gray Box to be behind the Tutorial Images
	}
	
	g_fCurrentRotation = 0.0f;
	g_pCurrentDisplayImage = NULL;
	g_fCurrentScale = g_fMinScale;
	
	// Set up tutorial objects
	g_pLineDrawImage = static_cast<t2dStaticSprite*>( Sim::findObject( g_szLineDrawImage ) );
	g_pLineDrawButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szLineDrawButton ) );
	SetDefaultTutorialImageProperties( g_pLineDrawImage );
	SetDefaultTutorialButtonProperties( g_pLineDrawButton );
	
	g_pSeedSuckImage = static_cast<t2dStaticSprite*>( Sim::findObject( g_szSeedSuckImage ) );
	g_pSeedSuckButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szSeedSuckButton ) );
	SetDefaultTutorialImageProperties( g_pSeedSuckImage );
	SetDefaultTutorialButtonProperties( g_pSeedSuckButton );
	
	g_pBlavaImage = static_cast<t2dStaticSprite*>( Sim::findObject( g_szBlavaImage ) );
	g_pBlavaButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szBlavaButton ) );
	SetDefaultTutorialImageProperties( g_pBlavaImage );
	SetDefaultTutorialButtonProperties( g_pBlavaButton );
	
	g_pVineAttackImage = static_cast<t2dStaticSprite*>( Sim::findObject( g_szVineAttackImage ) );
	g_pVineAttackButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szVineAttackButton ) );
	SetDefaultTutorialImageProperties( g_pVineAttackImage );
	SetDefaultTutorialButtonProperties( g_pVineAttackButton );
	
	g_pDamageImage = static_cast<t2dStaticSprite*>( Sim::findObject( g_szDamageImage ) );
	g_pDamageButton = static_cast<t2dStaticSprite*>( Sim::findObject( g_szDamageButton ) );
	SetDefaultTutorialImageProperties( g_pDamageImage );
	SetDefaultTutorialButtonProperties( g_pDamageButton );
    
    
    m_pTextImageTop     = static_cast<t2dStaticSprite*>( Sim::findObject( "TextImage_Top" ) );
    if( m_pTextImageTop )
    {
        m_pTextImageTop->setVisible(false);
    }
    m_pTextImageBottom  = static_cast<t2dStaticSprite*>( Sim::findObject( "TextImage_Bottom" ) );
    if( m_pTextImageBottom )
    {
        m_pTextImageBottom->setVisible(false);
    }
    
    m_pTheFinger = static_cast<t2dStaticSprite*>( Sim::findObject( "TheFinger" ) );
    // TODO: Attach the finger here and not on the path
    
    g_pFadeBox = static_cast<t2dStaticSprite*>( Sim::findObject( g_szFadeBox ) );
    
    CComponentSeedMagnetButton::HideSeedMagnetButton();
    
    
//    g_pSeedButton_Glow = static_cast<t2dSceneObject*>( Sim::findObject( "SeedButton_Glow" ) );
//    if( g_pSeedButton_Glow )
//        g_pSeedButton_Glow->setVisible( false );
    
    g_pInGameHUD = static_cast<t2dSceneObject*>( Sim::findObject( "InGameHUD" ) );
    if( g_pInGameHUD )
        g_pInGameHUD->setVisible( false );
    
    g_pPauseButton      = static_cast<t2dSceneObject*>( Sim::findObject( "PauseButton" ) );             //"PauseButton"
    if( g_pPauseButton )
        g_pPauseButton->setVisible( false );
    
    g_pManaBarPart01    = static_cast<t2dSceneObject*>( Sim::findObject( "ManaBarFrame_TOP" ) );        // ManaBarFrame_TOP
    if( g_pManaBarPart01 )
        g_pManaBarPart01->setVisible( false );
    
    g_pManaBarPart02    = static_cast<t2dSceneObject*>( Sim::findObject( "ManaBarFrame_CENTER" ) );     // ManaBarFrame_CENTER
    if( g_pManaBarPart02 )
        g_pManaBarPart02->setVisible( false );
    
    g_pManaBarPart03    = static_cast<t2dSceneObject*>( Sim::findObject( "ManaBarFrame_BOTTOM" ) );     // ManaBarFrame_BOTTOM
    if( g_pManaBarPart03 )
        g_pManaBarPart03->setVisible( false );
    
    g_pManaBarPart04    = static_cast<t2dSceneObject*>( Sim::findObject( "ManaBarObject" ) );           // ManaBarObject
    if( g_pManaBarPart04 )
        g_pManaBarPart04->setVisible( false );
    
    g_pHealthBarPart01  = static_cast<t2dSceneObject*>( Sim::findObject( "HealthBarFrame_LEFT" ) );     // HealthBarFrame_LEFT
    if( g_pHealthBarPart01 )
        g_pHealthBarPart01->setVisible( false );
    
    g_pHealthBarPart02  = static_cast<t2dSceneObject*>( Sim::findObject( "HealthBarFrame_CENTER" ) );   // HealthBarFrame_CENTER
    if( g_pHealthBarPart02 )
        g_pHealthBarPart02->setVisible( false );
    
    g_pHealthBarPart03  = static_cast<t2dSceneObject*>( Sim::findObject( "HealthBarFrame_RIGHT" ) );    // HealthBarFrame_RIGHT
    if( g_pHealthBarPart03 )
        g_pHealthBarPart03->setVisible( false );
    
    g_pHealthBarPart04  = static_cast<t2dSceneObject*>( Sim::findObject( "HealthBarObject" ) );         // HealthBarObject
    if( g_pHealthBarPart04 )
        g_pHealthBarPart04->setVisible( false );
    
    g_pFinger_Off_On_Anim  = static_cast<t2dSceneObject*>( Sim::findObject( "Finger_Off_On_Anim" ) );         // HealthBarObject
    if( g_pFinger_Off_On_Anim )
        g_pFinger_Off_On_Anim->setVisible( false );
    
    g_pSeedMagnetButton  = static_cast<t2dSceneObject*>( Sim::findObject( "SeedMagnetButton" ) );         // SeedMagnetButton
    if( g_pSeedMagnetButton )
    {
        g_pSeedMagnetButton->setPosition( g_vSeedButtonPos );
        g_pSeedMagnetButton->setVisible( false );
    }
    
    m_pFingerPath = static_cast<t2dPath*>( Sim::findObject( "TutorialLineDraw_Path" ) );
    //AddAllPathNodes();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Private Member Functions

void CComponentTutorialLevel::SetDefaultTutorialImageProperties( t2dStaticSprite* const _pObject )
{
	if( _pObject == NULL )
		return;
	
	_pObject->setVisible( false );
	_pObject->setPosition( g_vSafeTutorialObjectPosition );
	_pObject->setLayer( LAYER_TUTORIAL_IMAGE );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::SetDefaultTutorialButtonProperties( t2dStaticSprite* const _pObject )
{
	if( _pObject == NULL )
		return;
	
	_pObject->setMountTrackRotation( true );
	_pObject->setMountOwned( true );
	_pObject->setMountInheritAttributes( true );
	_pObject->setUseMouseEvents( true );
	_pObject->setLayer( LAYER_TUTORIAL_BUTTON );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::InitializeCurrentDisplayObject( t2dStaticSprite* const _pImage, t2dStaticSprite* const _pButton )
{
	if( _pImage == NULL || _pButton == NULL )
		return;
	
	g_pCurrentDisplayImage = _pImage;
	g_pCurrentDisplayButton = _pButton;
	
	g_pCurrentDisplayButton->setUseMouseEvents( false );
	
	g_vOriginalDisplayImageSize = g_pCurrentDisplayImage->getSize();
	g_vOriginalDisplayButtonSize = g_pCurrentDisplayButton->getSize();
	
	g_pCurrentDisplayImage->setPosition( t2dVector::getZero() );
	
	t2dVector vNewSize = g_pCurrentDisplayImage->getSize();
	vNewSize.mX *= g_fCurrentScale;
	vNewSize.mY *= g_fCurrentScale;
	g_pCurrentDisplayImage->setSize( vNewSize );
	g_pCurrentDisplayImage->setVisible( true );
	
	vNewSize = g_pCurrentDisplayButton->getSize();
	vNewSize.mX *= g_fCurrentScale;
	vNewSize.mY *= g_fCurrentScale;
	g_pCurrentDisplayButton->setSize( vNewSize );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::UpdateCurrentDisplayObjectEntry()
{
	if( g_pCurrentDisplayImage && g_pCurrentDisplayButton )
	{
		// Handle Rotation
		g_fCurrentRotation += g_fDisplayObjectRotationRate * ITickable::smTickSec;
		if( g_fCurrentRotation >= 360.0f )
			g_fCurrentRotation = 0.0f;
		
		g_pCurrentDisplayImage->setRotation( g_fCurrentRotation );
		
		// Handle Scaling
		g_fCurrentScale += g_fDisplayObjectScaleRate * ITickable::smTickSec;
		if( g_fCurrentScale >= g_fMaxScale )
		{
			g_fCurrentScale = g_fMaxScale;
			g_pCurrentDisplayButton->setUseMouseEvents( true );
			OnTutorialImageAtMaxSize();
		}
		
		g_vTempSize = g_vOriginalDisplayImageSize;
		g_vTempSize.mX *= g_fCurrentScale;
		g_vTempSize.mY *= g_fCurrentScale;
		g_pCurrentDisplayImage->setSize( g_vTempSize );
		
		g_vTempSize = g_vOriginalDisplayButtonSize;
		g_vTempSize.mX *= g_fCurrentScale;
		g_vTempSize.mY *= g_fCurrentScale;
		g_pCurrentDisplayButton->setSize( g_vTempSize );
		
		//printf( "Current Rotation: %f\n", g_fCurrentRotation );
		//printf( "Current Scale:    %f\n", g_fCurrentScale );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::OnTutorialImageAtMaxSize()
{
    m_eBillboardState = BILLBOARD_STATE_FULL_SIZE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::UpdateFade()
{
    //printf( "CComponentTutorialLevel::UpdateFade()\n" );
    
	if( g_pFadeBox == NULL )
		return;
	
	switch( m_eFadeState )
	{
		case FADE_STATE_FADING_IN: // Fading In (Turning to transparent)
			
			m_fFadeTimer -= ITickable::smTickSec;
			if( m_fFadeTimer <= 0.0f )
				m_fFadeTimer = 0.0f;
			
			m_fFadeAmount = m_fFadeTimer / g_fFadeTimeSeconds;
			g_pFadeBox->setBlendAlpha( m_fFadeAmount );
			
			
			if( m_fFadeAmount <= 0.0f )
				OnFadeInFinished();
			
			break;
			
		case FADE_STATE_FADING_OUT: // Fading Out (Turning to black)
			
			m_fFadeTimer += ITickable::smTickSec;
			if( m_fFadeTimer >= g_fFadeTimeSeconds )
				m_fFadeTimer = g_fFadeTimeSeconds;
			
			m_fFadeAmount = m_fFadeTimer / g_fFadeTimeSeconds;
			g_pFadeBox->setBlendAlpha( m_fFadeAmount );
			
			if( m_fFadeAmount >= 1.0f )
				OnFadeOutFinished();
			
			break;
			
		default:
			break;	
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::OnFadeOutFinished()
{
	//printf( "CComponentTutorialLevel::OnFadeOutFinished()\n" );
    
    //CComponentGlobals::GetInstance().OnLineAttackObjectPathFinished();
    CComponentLineDrawAttack::ForceLineDrawAttackPathFinished();
    
    
    // CHANGE THE TUTORIAL STATE TO THE NEXT ONE
	m_eFadeState = FADE_STATE_FADING_IN;
	m_fFadeTimer = g_fFadeTimeSeconds; // Set to g_fFadeTimeSeconds because the Fade Box alpha will go from 1.0 -> 0.0
	
	if( g_pFadeBox) 
		g_pFadeBox->setBlendAlpha( 1.0f );
    
    if( m_eTutorialState < TUTORIAL_STATE_FINISHING_LEVEL )
    {
        CSoundManager::GetInstance().PlayInGameBGM();
    }
    
    CComponentEndLevelScreen::GetInstance().SetFinalGrowSoundNotPlayed();
    
    CComponentManaBar::ForceFullMana();
    CComponentManaSeedItemDrop::HideAllManaSeeds();
    
    switch( m_eTutorialState )
    {
        case TUTORIAL_STATE_THE_FINGER_DRAWING:
        case TUTORIAL_STATE_BREAKING_ROCKS:
            
            // Unbreak all the land bricks
            CComponentPathGridHandler::ResetNumFullyGrownLandBricks();
            CComponentLandBrick::UnbreakAllLandBricks();
            
            // Show the new Tutorial Messages
            if( m_pTextImageTop )
            {
                m_pTextImageTop->setImageMap( "TT05ImageMap", 0 );
                m_pTextImageTop->setVisible( true );
            }
            
            if( m_pTextImageBottom )
            {
                m_pTextImageBottom->setImageMap( "TT06ImageMap", 0 );
                m_pTextImageBottom->setVisible( true );
            }
            
            m_eTutorialState = TUTORIAL_STATE_GREEN_METER;
            
            //if( g_pInGameHUD )
            //    g_pInGameHUD->setVisible( true );
            
            if( g_pManaBarPart01 )
                g_pManaBarPart01->setVisible( true );
            
            if( g_pManaBarPart02 )
                g_pManaBarPart02->setVisible( true );
            
            if( g_pManaBarPart03 )
                g_pManaBarPart03->setVisible( true );
            
            if( g_pManaBarPart04 )
                g_pManaBarPart04->setVisible( true );

            break;
            
        case TUTORIAL_STATE_GREEN_METER:
        case TUTORIAL_STATE_GATHERING_SEEDS:
        case TUTORIAL_STATE_EATING_SEEDS:
            if( m_pTextImageTop )
                m_pTextImageTop->setImageMap( "TT06ImageMap", 0 );
            
            if( m_pTextImageBottom )
                m_pTextImageBottom->setImageMap( "TT06ImageMap", 0 );
            
            m_eTutorialState = TUTORIAL_STATE_SPAWNING_MONSTERS;
            
            
//            if( g_pHealthBarPart01 )
//                g_pHealthBarPart01->setVisible( true );
//            
//            if( g_pHealthBarPart02 )
//                g_pHealthBarPart02->setVisible( true );
//            
//            if( g_pHealthBarPart03 )
//                g_pHealthBarPart03->setVisible( true );
//            
//            if( g_pHealthBarPart04 )
//                g_pHealthBarPart04->setVisible( true );
            
            // Unbreak all the land bricks
            CComponentPathGridHandler::ResetNumFullyGrownLandBricks();
            CComponentLandBrick::UnbreakAllLandBricks();
            
            // Make sure the Seed Button is visible
            if( g_pSeedMagnetButton )
            {
                g_pSeedMagnetButton->setPosition( g_vSeedButtonPos );
                g_pSeedMagnetButton->setVisible( true );
            }
            
            break;
            
        default:
            printf( "CComponentTutorialLevel::OnFadeOutFinished - Unhandled Tutorial State: %d\n", m_eTutorialState );
            break;
    }
    
    
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void CComponentTutorialLevel::OnFadeInFinished()
{
	//printf( "CComponentTutorialOpeningScreen::OnFadeInFinished()\n" );
	
	m_eFadeState = FADE_STATE_NONE;
	m_fFadeTimer = 0.0f;  // Set to 0.0f because the Fade Box alpha will go from 0.0f -> 1.0 on Fade Out
	
	if( g_pFadeBox )
		g_pFadeBox->setBlendAlpha( 0.0f );
	
    CComponentGlobals::GetInstance().StartTimer();
    CComponentPlayerHomeBase::ForceHomeBaseIntoIdle();
    
    if( m_eTutorialState == TUTORIAL_STATE_SPAWNING_MONSTERS )
    {
        m_bSpawningEnabled = true;
    }
    
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------



