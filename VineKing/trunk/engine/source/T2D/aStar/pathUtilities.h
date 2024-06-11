//-----------------------------------------------------------------------------
// misc utilities and Globals used in pathGrid2d
// Philip Shenk
//-----------------------------------------------------------------------------

#ifndef _PATHUTILITIES_H_
#define _PATHUTILITIES_H_

#ifndef _PATHCLASSES_H_
#include "./pathClasses.h"
#endif

#ifndef _T2DVECTOR_H_
#include "./t2dVector.h"
#endif

#ifndef _VECTOR2D_H
#include "./tVectorExtension.h"
#endif

#ifndef _TVECTOR_H_
#include "./tVector.h"
#endif

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const U32	PASSABLE = 0;		// Defines the passable value
const U32	BLOCKED = 10;		// Defines the blocked value
								// Weighted values will be any int between PASSABLE and BLOCKED
const U32	QUEUESIZE = 1024;	// can this be used as a cap for how many paths to search?
const S32	DIRTABLE[8][2] =	{	{  0, -1 },
									{  1,  0 },
									{  0,  1 },
									{ -1,  0 },
									{  1, -1 },
									{  1,  1 },
									{ -1,  1 },
									{ -1, -1 } };

const S32 CORNEROFFSET[4][4] =	{	{ -1,  0,  0, 1 },
									{  0, -1, -1, 0 },
									{  0, -1,  1, 0 },
									{  1,  0,  0, 1 } };

const F32 DISTTABLE[8] = {	1.0f, 1.0f, 1.0f, 1.0f,
							1.414214f, 1.414214f, 1.414214f, 1.414214f }; 

//-----------------------------------------------------------------------------
// Algorithms
//-----------------------------------------------------------------------------
inline S32 compareNodesDescending( Node left, Node right )
{
	if( left.mF < right.mF )
		return 1;
	if( left.mF > right.mF )
		return -1;
	return 0;
}

inline F32 aStarHeuristic( t2dVector v1, t2dVector v2 )
{
	return (v2 - v1).len();
}

//-----------------------------------------------------------------------------
// Utility functions
//-----------------------------------------------------------------------------
inline bool rangeComp( const F32 f1, const F32 f2, const F32 t )
{
	if ( f1 < ( f2 + t ) && f1 > ( f2 - t ) )
		return true;
	else return false;
}


#endif