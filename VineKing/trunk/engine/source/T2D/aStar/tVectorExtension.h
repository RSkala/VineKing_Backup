//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
// Phil Shenk
// Vector Extensions
// various classes derived from tVector
//---------------------------------------------------------
// Vector2d - accessors and functions for treating as a 2d structure
// Queue - simple partial wrapper
// Heap - implements heap sort for efficient sorting
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------


#ifndef _VECTOREXT_H
#define _VECTOREXT_H

#include "core/tVector.h"

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
// Vector2d
// adds accessors for using tVector as a 2d structure
// Vector2d class inheriting from tVector
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

template <class T>
class Vector2d : public Vector<T>
{
protected:
	U32	mWidth;
	U32 mHeight;
   using Vector<T>::mArray;
   using Vector<T>::mElementCount;
   using Vector<T>::mArraySize;
   using Vector<T>::reserve;

public:
//---------------------------------------------------------
// constructor
//---------------------------------------------------------
	Vector2d( const U32 initialWidth = 0, const U32 initialHeight = 0 )
	{
		mArray        = 0;
		mElementCount = 0;
		mArraySize    = 0;
		mWidth = initialWidth;
		mHeight = initialHeight;

		if(initialWidth && initialHeight)
			reserve( initialWidth * initialHeight );
	}
//---------------------------------------------------------
// Accessors
//---------------------------------------------------------
	U32 width()
	{
		return mWidth;
	}

	U32 height()
	{
		return mHeight;
	}

//---------------------------------------------------------
// resize
//---------------------------------------------------------
	bool resize( const U32 width, const U32 height )
	{
		reserve( width * height );
		mWidth = width;
		mHeight = height;
		return true;
	}

//---------------------------------------------------------
// get - returns ref to data 
//---------------------------------------------------------
	T& get( const U32 indexX, const U32 indexY )
	{
		U32 index = ( indexY * mWidth ) + indexX;
		return mArray[index];
	}

	T& get( const t2dVector v )
	{
		U32 index = ( v.mY * mWidth ) + v.mX;
		return mArray[index];
	}


};

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
// Queue
// simple wrapper for tVector so it works like the Heap class
// adds:
// enqueue( T )	- adds item to back
// dequeue()	- removes item from front
// item()		- gets item from front
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

template <class T>
class Queue : public Vector<T>
{
   using Vector<T>::mElementCount;
   using Vector<T>::pop_front;
   using Vector<T>::front;
   
public:

//---------------------------------------------------------
// constructor
//---------------------------------------------------------
	Queue(  )
	{
	}

//---------------------------------------------------------
// enqueue - adds an element to back
//---------------------------------------------------------
	void enqueue( T element )
	{
		push_back( element );
	}

//---------------------------------------------------------
// dequeue -removes the front of the queue
//---------------------------------------------------------
	void dequeue()
	{
		if( mElementCount >= 0 )
		{
			pop_front();
		}
	}
//---------------------------------------------------------
// item - returns reference to the front of the queue
//---------------------------------------------------------
	T& item()
	{
		return front();
	}
};


//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
// Heap
// based on demo Heap class by Ron Penton
// Heap class inheriting from tVector
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

template <class T>
class Heap : public Vector<T>
{
   using Vector<T>::increment;
   using Vector<T>::decrement;
   using Vector<T>::back;
   using Vector<T>::mElementCount;
   using Vector<T>::mArray;
   
public:

//---------------------------------------------------------
// constructor - takes a pointer to the comparitor function 
// to use when sorting
//---------------------------------------------------------
	Heap( U32 size, S32 ( *p_compare )( T, T ) )
		: Vector< T >( size + 1 )
	{
		m_compare = p_compare;
	}

//---------------------------------------------------------
// enqueue - adds an element, and sorts it
//---------------------------------------------------------
	void enqueue( T element )
	{
		increment( 1 );
		back() = element;
		walk_up( mElementCount );
	}

//---------------------------------------------------------
// dequeue - removes the top of the heap
//---------------------------------------------------------
	void dequeue()
	{
		if( mElementCount >= 1 )
		{
			mArray[1] = mArray[mElementCount]; // swap back to front
			walk_down( 1 );
			decrement( 1 );
		}
	}

//---------------------------------------------------------
// item - returns reference to the top of the heap
//---------------------------------------------------------
	T& item()
	{
		return mArray[1];
	}
//---------------------------------------------------------
// walk_up - walks an item up the heap into the right position
//---------------------------------------------------------
	void walk_up( U32 index )
	{
		// UNUSED: JOSEPH THOMAS -> int foo = 0;
		// set up the parent and child indexes
		U32 parent = index / 2;
		U32 child = index;

		// store the item to walk up in temp buffer
		T temp = mArray[child];
		// UNUSED: JOSEPH THOMAS -> int foo6 = 0;

		while( parent > 0 )
		{	// if the node to walk up is more than the parent, then swap
			Node tempParent = mArray[parent];
			if( m_compare( temp, mArray[parent] ) > 0 )
			{
				// swap the parent and child, and go up a level
				mArray[child] = mArray[parent];
				child = parent;
				parent /= 2;
			}
			else 
			{
				// UNUSED: JOSEPH THOMAS -> int foo2 = 0;
				break;
			}
		}
		// put the temp variable (the one that was walked up) into the child index
		mArray[child] = temp;
		// UNUSED: JOSEPH THOMAS -> int foo3 = 0;
	}

//---------------------------------------------------------
// walk_down - walks an item down the heap into the right position
//---------------------------------------------------------
	void walk_down( U32 index )
	{
		// calculate the parent and child indexes
		U32 parent = index;
		U32 child = index * 2;

		// store the data to walk down in a temp buffer
		T temp = mArray[parent];

		// loop through, walking node down the heap until both children are smaller than the node
		while( child < mElementCount )
		{
			// if left child is not the last node in the tree, then
			// find out which of the current node's children is largest
			if( child < mElementCount - 1 )
			{
				if( m_compare( mArray[child], mArray[child + 1] ) < 0 )
				{ // change the pointer to the right child, since it is larger
					child++;
				}
			}
			// if the node to walk down is lower than the highest value child.
			// move the child up one level
			if( m_compare( temp, mArray[child] ) < 0 )
			{
				mArray[parent] = mArray[child];
				parent = child;
				child *= 2;
			}
			else
				break;
		}
		mArray[parent] = temp;
	}
//---------------------------------------------------------
// m_compare
// the pointer to the comparison function
//---------------------------------------------------------
	S32 ( *m_compare )( T, T );

};


#endif