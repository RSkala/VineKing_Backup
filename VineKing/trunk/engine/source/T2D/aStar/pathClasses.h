//-----------------------------------------------------------------------------
// classes used in pathGrid2d
// Philip Shenk
//-----------------------------------------------------------------------------

#ifndef _PATHCLASSES_H_
#define _PATHCLASSES_H_

#ifndef _T2DVECTOR_H_
#include "./t2dVector.h"
#endif

class Node : public t2dVector
{
public:
	F32 mF;					// F value ( for sorting in Heap, g + h )
	F32 mChildDir;			// direction to child
	F32 mChildDist;			// distance to child
	//bool mEnd;				// flag for last node

	// assignment operator
	inline Node operator = (const t2dVector &v)			{ mX = v.mX; mY = v.mY; return *this; };
	inline Node operator = (const Node &c)			{ mX = c.mX; mY = c.mY; mF = c.mF; mChildDir = c.mChildDir; 
																  mChildDist = c.mChildDist; return *this; };
	
	// constructors
	inline Node( void ) {};
    inline Node( const t2dVector& copy ) 
	{
		mX = copy.mX;
		mY = copy.mY;
		mF = 0;
	}
    inline Node( const Node& copy ) 
	{
		mX = copy.mX;
		mY = copy.mY;
		mF = copy.mF;
	}
    inline Node( const F32 x, const F32 y ) 
	{
		mX = x;
		mY = y;
		mF = 0;
	}
    inline Node( const F32 x, const F32 y, const F32 f  ) 
	{
		mX = x;
		mY = y;
		mF = f;
	}
};

class GridCell
{
public:
	bool open;
	F32 mG;					// distance to starting cell
	F32 mH;					// distance (heuristic) to destination
	F32 mF;					// g + h
	U32 cellType;			// note here what kind of cell it is (i.e. corner, and which corner)
	t2dVector parent;
	bool mPassable;
	U32 mWeight;

	// Constructors
	inline GridCell( void ) {};
	inline GridCell( const GridCell& copy ) : open(copy.open), mG(copy.mG), mH(copy.mH), mF(copy.mF), 
		parent(copy.parent), mPassable(copy.mPassable), mWeight(copy.mWeight) {};

};

#endif