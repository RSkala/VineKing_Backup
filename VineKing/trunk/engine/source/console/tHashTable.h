#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include "../core/tVector.h"
/*
template<class T> 
class cHashTable {
	private:
		Vector<T> mVector;
	public:
		cHashTable( U32 size = 1 );
		~cHashTable();
		bool add( T );
		bool remove( T );
		T lookup( T );
		U32 getSize() { return mVector.size(); }
		T getElement( U32 );
};


template<class T> cHashTable<T>::cHashTable( U32 size ) {
	mVector.reserve( size );//init at our desired size
}

template<class T> cHashTable<T>::~cHashTable() {
	//delete mVector<T>;
}

template<class T> bool cHashTable<T>::add( T obj ) {
	mVector.push_back( obj );
	return true;
}

template<class T> T cHashTable<T>::lookup( T obj ) {
	S32 size = mVector.size();
	for( S32 i = 0; i < size; i++ ) {
		if( obj == mVector[i] ) {
			return mVector[i];
		}
	}
	return NULL;
}

template<class T> T cHashTable<T>::getElement( U32 index ) {
	if( index < mVector.size() ) {
		return (T)mVector[index];
	} else {
		//do some kind of NULL return if possible
		return (T)NULL;
	}
}

template<class T> bool cHashTable<T>::remove( T obj ) {
	S32 size = mVector.size();
	size--;
	for( S32 i = 0; i < size; i++ ) {
		if( obj == mVector[i] ) {
			mVector.erase_fast( i );
			return true;
		}
	}
	return false;
}

*/
//good hashtable ----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
// Torque Shader Engine
// Copyright (c) GarageGames, All Rights Reserved
//-----------------------------------------------------------------------------


#include "platform/platform.h"
#include "core/stringTable.h"

namespace tDictionaryHash
{
//-Mat this was copied from StringTable
	void initTolowerTable();
   U32 hash(const char *str);

//-Mat
   inline U32 hash(U32 data)
   {
      return data;
   }

   inline U32 hash(const void *data)
   {
      return (U32)data;
   }

   U32 nextPrime(U32);
};

namespace tKeyCompare
{
   template<typename Key>
   inline bool equals( Key keya, Key keyb )
   {
      return ( keya == keyb );
   }

   template<>
   inline bool equals<>( const char *keya, const char *keyb )
   {
	   //-Mat make sure this is an accurate compare (do we check case?)
      return ( dStricmp( keya, keyb ) == 0 );
   }
};

/// A tHashTable template class.
///
/// The hash table class maps between a key and an associated value. Access
/// using the key is performed using a hash table.  The class provides
/// methods for both unique and equal keys. The global ::hash(Type) function
/// is used for hashing, see util/hash.h
/// @ingroup UtilContainers
template<typename Key, typename Value >
class tHashTable
{
public:
   struct Pair {
      Key  key;
      Value value;
      Pair() {}
      Pair(Key k,Value v): key(k), value(v) {}
   };

private:
   struct Node {
      Node* mNext;
      Pair mPair;
      Node(): mNext(0) {}
      Node(Pair p,Node* n): mPair(p),mNext(n) {}
   };

   Node** mTable;                      ///< Hash table
   S32 mTableSize;                     ///< Hash table size
   U32 mSize;                          ///< Number of keys in the table

   U32 _hash(const Key& key) const;
   U32 _index(const Key& key) const;
   Node* _next(U32 index) const;
   void _resize(U32 size);
   void _destroy();

public:
   // Iterator support
   template<typename U,typename E, typename M>
   class _Iterator {
      friend class tHashTable;
      E* mLink;
      M* mtHashTable;
      operator E*();
   public:
      typedef U  ValueType;
      typedef U* Pointer;
      typedef U& Reference;

      _Iterator()
      {
         mtHashTable = 0;
         mLink = 0;
      }

      _Iterator(M* table,E* ptr)
      {
         mtHashTable = table;
         mLink = ptr;
      }

      _Iterator& operator++()
      {
         mLink = mLink->mNext? mLink->mNext :
            mtHashTable->_next(mtHashTable->_index(mLink->mPair.key) + 1);
         return *this;
      }

      _Iterator operator++(int)
      {
         _Iterator itr(*this);
         ++(*this);
         return itr;
      }
	  //-Mat to check if mLink is bad
	  Value getValue() {
		  if( mLink ) {
			  return mLink->mPair.value;
		  } 
		  return (Value)(0);
	  }

      bool operator==(const _Iterator& b) const
      {
         return mtHashTable == b.mtHashTable && mLink == b.mLink;
      }

      bool operator!=(const _Iterator& b) const
      {
         return !(*this == b);
      }

      U* operator->() const
      {
         return &mLink->mPair;
      }

      U& operator*() const
      {
         return mLink->mPair;
      }
   };

   // Types
   typedef Pair        ValueType;
   typedef Pair&       Reference;
   typedef const Pair& ConstReference;

   typedef _Iterator<Pair,Node,tHashTable>  Iterator;
   typedef _Iterator<const Pair,const Node,const tHashTable>  ConstIterator;
   typedef S32         DifferenceType;
   typedef U32         SizeType;

   // Initialization
   tHashTable();
   ~tHashTable();
   tHashTable(const tHashTable& p);

   // Management
   U32  size() const;                  ///< Return the number of elements
   U32  tableSize() const;             ///< Return the size of the hash bucket table
   void clear();                       ///< Empty the tHashTable
   void resize(U32 size);
   bool isEmpty() const;               ///< Returns true if the table is empty
   F32 collisions() const;             ///< Returns the average number of nodes per bucket

   // Insert & erase elements
   Iterator insertEqual(const Key& key, const Value&);
   Iterator insertUnique(const Key& key, const Value&);
   void erase(Iterator);               ///< Erase the given entry
   void erase(const Key& key);         ///< Erase all matching keys from the table

   // tHashTable lookup
   Iterator findOrInsert(const Key& key);
   Iterator find(const Key&);          ///< Find the first entry for the given key
   ConstIterator find(const Key&) const;    ///< Find the first entry for the given key
   S32 count(const Key&);              ///< Count the number of matching keys in the table

   // Forward Iterator access
   Iterator       begin();             ///< Iterator to first element
   ConstIterator begin() const;        ///< Iterator to first element
   Iterator       end();               ///< Iterator to last element + 1
   ConstIterator end() const;          ///< Iterator to last element + 1

   void operator=(const tHashTable& p);
};


template<typename Key, typename Value> tHashTable<Key,Value>::tHashTable()
{
   mTableSize = 0;
   mTable = 0;
   mSize = 0;
}

template<typename Key, typename Value> tHashTable<Key,Value>::tHashTable(const tHashTable& p)
{
   mSize = 0;
   mTableSize = 0;
   mTable = 0;
   *this = p;
}

template<typename Key, typename Value> tHashTable<Key,Value>::~tHashTable()
{
   _destroy();
}


//-----------------------------------------------------------------------------

template<typename Key, typename Value>
inline U32 tHashTable<Key,Value>::_hash(const Key& key) const
{
   return tDictionaryHash::hash(key);
}

template<typename Key, typename Value>
inline U32 tHashTable<Key,Value>::_index(const Key& key) const
{
   return _hash(key) % mTableSize;
}

template<typename Key, typename Value>
typename tHashTable<Key,Value>::Node* tHashTable<Key,Value>::_next(U32 index) const
{
   for (; index < mTableSize; index++)
      if (Node* node = mTable[index])
         return node;
   return 0;
}

template<typename Key, typename Value>
void tHashTable<Key,Value>::_resize(U32 size)
{
   S32 currentSize = mTableSize;
   mTableSize = tDictionaryHash::nextPrime(size);
   Node** table = new Node*[mTableSize];
   dMemset(table,0,mTableSize * sizeof(Node*));

   for (S32 i = 0; i < currentSize; i++)
      for (Node* node = mTable[i]; node; )
      {
         // Get groups of matching keys
         Node* last = node;
         while (last->mNext && last->mNext->mPair.key == node->mPair.key)
            last = last->mNext;

         // Move the chain to the new table
         Node** link = &table[_index(node->mPair.key)];
         Node* tmp = last->mNext;
         last->mNext = *link;
         *link = node;
         node = tmp;
      }

   delete[] mTable;
   mTable = table;
}

template<typename Key, typename Value>
void tHashTable<Key,Value>::_destroy()
{
   for (S32 i = 0; i < mTableSize; i++)
      for (Node* ptr = mTable[i]; ptr; ) 
      {
         Node *tmp = ptr;
         ptr = ptr->mNext;
         delete tmp;
      }
   delete[] mTable;
   mTable = NULL;
}


//-----------------------------------------------------------------------------
// management

template<typename Key, typename Value>
inline U32 tHashTable<Key,Value>::size() const
{
   return mSize;
}

template<typename Key, typename Value>
inline U32 tHashTable<Key,Value>::tableSize() const
{
   return mTableSize;
}

template<typename Key, typename Value>
inline void tHashTable<Key,Value>::clear()
{
   _destroy();
   mTableSize = 0;
   mSize = 0;
}

/// Resize the bucket table for an estimated number of elements.
/// This method will optimize the hash bucket table size to hold the given
/// number of elements.  The size argument is a hint, and will not be the
/// exact size of the table.  If the table is sized down below it's optimal
/// size, the next insert will cause it to be resized again. Normally this
/// function is used to avoid resizes when the number of elements that will
/// be inserted is known in advance.
template<typename Key, typename Value>
inline void tHashTable<Key,Value>::resize(U32 size)
{
   _resize(size);
}

template<typename Key, typename Value>
inline bool tHashTable<Key,Value>::isEmpty() const
{
   return mSize == 0;
}

template<typename Key, typename Value>
inline F32 tHashTable<Key,Value>::collisions() const
{
   S32 chains = 0;
   for (S32 i = 0; i < mTableSize; i++)
      if (mTable[i])
         chains++;
   return F32(mSize) / chains;
}


//-----------------------------------------------------------------------------
// add & remove elements

/// Insert the key value pair but don't insert duplicates.
/// This insert method does not insert duplicate keys. If the key already exists in
/// the table the function will fail and end() is returned.
template<typename Key, typename Value>
typename tHashTable<Key,Value>::Iterator tHashTable<Key,Value>::insertUnique(const Key& key, const Value& x)
{
   if (mSize >= mTableSize)
      _resize(mSize + 1);
   Node** table = &mTable[_index(key)];
   for (Node* itr = *table; itr; itr = itr->mNext)
      if ( tKeyCompare::equals<Key>( itr->mPair.key, key) )
         return end();

   mSize++;
   *table = new Node(Pair(key,x),*table);
   return Iterator(this,*table);
}

/// Insert the key value pair and allow duplicates.
/// This insert method allows duplicate keys.  Keys are grouped together but
/// are not sorted.
template<typename Key, typename Value>
typename tHashTable<Key,Value>::Iterator tHashTable<Key,Value>::insertEqual(const Key& key, const Value& x)
{
   if (mSize >= mTableSize)
      _resize(mSize + 1);
   // The new key is inserted at the head of any group of matching keys.
   Node** prev = &mTable[_index(key)];
   for (Node* itr = *prev; itr; prev = &itr->mNext, itr = itr->mNext)
      if ( tKeyCompare::equals<Key>( itr->mPair.key, key ) )
         break;
   mSize++;
   *prev = new Node(Pair(key,x),*prev);
   return Iterator(this,*prev);
}

template<typename Key, typename Value>
void tHashTable<Key,Value>::erase(const Key& key)
{
   Node** prev = &mTable[_index(key)];
   for (Node* itr = *prev; itr; prev = &itr->mNext, itr = itr->mNext)
      if ( tKeyCompare::equals<Key>( itr->mPair.key, key ) ) {
         // Delete matching keys, which should be grouped together.
         do {
            Node* tmp = itr;
            itr = itr->mNext;
            delete tmp;
            mSize--;
         } while (itr && tKeyCompare::equals<Key>( itr->mPair.key, key ) );
         *prev = itr;
         return;
      }
}

template<typename Key, typename Value>
void tHashTable<Key,Value>::erase(Iterator node)
{
   Node** prev = &mTable[_index(node->key)];
   for (Node* itr = *prev; itr; prev = &itr->mNext, itr = itr->mNext)
      if (itr == node.mLink) {
         *prev = itr->mNext;
         delete itr;
         mSize--;
         return;
      }
}


//-----------------------------------------------------------------------------

/// Find the key, or insert a one if it doesn't exist.
/// Returns the first key in the table that matches, or inserts one if there
/// are none.
template<typename Key, typename Value>
typename tHashTable<Key,Value>::Iterator tHashTable<Key,Value>::findOrInsert(const Key& key)
{
   if (mSize >= mTableSize)
      _resize(mSize + 1);
   Node** table = &mTable[_index(key)];
   for (Node* itr = *table; itr; itr = itr->mNext)
      if ( tKeyCompare::equals<Key>( itr->mPair.key, key ) )
         return Iterator(this,itr);
   mSize++;
   *table = new Node(Pair(key,Value()),*table);
   return Iterator(this,*table);
}

template<typename Key, typename Value>
typename tHashTable<Key,Value>::Iterator tHashTable<Key,Value>::find(const Key& key)
{
   if (mTableSize)
      for (Node* itr = mTable[_index(key)]; itr; itr = itr->mNext)
         if ( tKeyCompare::equals<Key>( itr->mPair.key, key ) )
            return Iterator(this,itr);
   return Iterator(this,0);
}

template<typename Key, typename Value>
S32 tHashTable<Key,Value>::count(const Key& key)
{
   S32 count = 0;
   if (mTableSize)
      for (Node* itr = mTable[_index(key)]; itr; itr = itr->mNext)
         if ( tKeyCompare::equals<Key>( itr->mPair.key, key ) ) {
            // Matching keys should be grouped together.
            do {
               count++;
               itr = itr->mNext;
            } while (itr && tKeyCompare::equals<Key>( itr->mPair.key, key ) );
            break;
         }
   return count;
}


//-----------------------------------------------------------------------------
// Iterator access

template<typename Key, typename Value>
inline typename tHashTable<Key,Value>::Iterator tHashTable<Key,Value>::begin()
{
   return Iterator(this,_next(0));
}

template<typename Key, typename Value>
inline typename tHashTable<Key,Value>::ConstIterator tHashTable<Key,Value>::begin() const
{
   return ConstIterator(this,_next(0));
}

template<typename Key, typename Value>
inline typename tHashTable<Key,Value>::Iterator tHashTable<Key,Value>::end()
{
   return Iterator(this,0);
}

template<typename Key, typename Value>
inline typename tHashTable<Key,Value>::ConstIterator tHashTable<Key,Value>::end() const
{
   return ConstIterator(this,0);
}


//-----------------------------------------------------------------------------
// operators

template<typename Key, typename Value>
void tHashTable<Key,Value>::operator=(const tHashTable& p)
{
   _destroy();
   mTableSize = p.mTableSize;
   mTable = new Node*[mTableSize];
   mSize = p.mSize;
   for (S32 i = 0; i < mTableSize; i++)
      if (Node* itr = p.mTable[i])
      {
         Node** head = &mTable[i];
         do 
         {
            *head = new Node(itr->mPair,0);
            head = &(*head)->mNext;
            itr = itr->mNext;
         } while (itr);
      }
      else
         mTable[i] = 0;
}

//-----------------------------------------------------------------------------
// Iterator class

/// A tHashMap template class.
/// The map class maps between a key and an associated value. Keys
/// are unique.
/// The hash table class is used as the default implementation so the
/// the key must be hashable, see util/hash.h for details.
/// @ingroup UtilContainers
template<typename Key, typename Value, class Sequence = tHashTable<Key,Value> >
class tHashMap: private Sequence
{
   typedef tHashTable<Key,Value> Parent;

private:
   Sequence mHashMap;

public:
   // types
   typedef typename Parent::Pair Pair;
   typedef Pair        ValueType;
   typedef Pair&       Reference;
   typedef const Pair& ConstReference;

   typedef typename Parent::Iterator  Iterator;
   typedef typename Parent::ConstIterator ConstIterator;
   typedef S32         DifferenceType;
   typedef U32         SizeType;

   // initialization
   tHashMap() {}
   ~tHashMap() {}
   tHashMap(const tHashMap& p);

   // management
   U32  size() const;                  ///< Return the number of elements
   void clear();                       ///< Empty the tHashMap
   bool isEmpty() const;               ///< Returns true if the map is empty

   // insert & erase elements
   Iterator insert(const Key& key, const Value&); // Documented below...
   void erase(Iterator);               ///< Erase the given entry
   void erase(const Key& key);         ///< Erase the key from the map

   // tHashMap lookup
   Iterator find(const Key&);          ///< Find entry for the given key
   ConstIterator find(const Key&) const;    ///< Find entry for the given key
   bool contains(const Key&a)
   {
      return mHashMap.count(a) > 0;
   }

   // forward Iterator access
   Iterator       begin();             ///< Iterator to first element
   ConstIterator begin() const;       ///< Iterator to first element
   Iterator       end();               ///< IIterator to last element + 1
   ConstIterator end() const;         ///< Iterator to last element + 1

   // operators
   Value& operator[](const Key&);      ///< Index using the given key. If the key is not currently in the map it is added.
};

template<typename Key, typename Value, class Sequence> tHashMap<Key,Value,Sequence>::tHashMap(const tHashMap& p)
{
   *this = p;
}


//-----------------------------------------------------------------------------
// management

template<typename Key, typename Value, class Sequence>
inline U32 tHashMap<Key,Value,Sequence>::size() const
{
   return mHashMap.size();
}

template<typename Key, typename Value, class Sequence>
inline void tHashMap<Key,Value,Sequence>::clear()
{
   mHashMap.clear();
}

template<typename Key, typename Value, class Sequence>
inline bool tHashMap<Key,Value,Sequence>::isEmpty() const
{
   return mHashMap.isEmpty();
}


//-----------------------------------------------------------------------------
// add & remove elements

/// Insert the key value pair but don't allow duplicates.
/// The map class does not allow duplicates keys. If the key already exists in
/// the map the function will fail and return end().
template<typename Key, typename Value, class Sequence>
typename tHashMap<Key,Value,Sequence>::Iterator tHashMap<Key,Value,Sequence>::insert(const Key& key, const Value& x)
{
   return mHashMap.insertUnique(key,x);
}

template<typename Key, typename Value, class Sequence>
void tHashMap<Key,Value,Sequence>::erase(const Key& key)
{
   mHashMap.erase(key);
}

template<typename Key, typename Value, class Sequence>
void tHashMap<Key,Value,Sequence>::erase(Iterator node)
{
   mHashMap.erase(node);
}


//-----------------------------------------------------------------------------
// Searching

template<typename Key, typename Value, class Sequence>
typename tHashMap<Key,Value,Sequence>::Iterator tHashMap<Key,Value,Sequence>::find(const Key& key)
{
   return mHashMap.find(key);
}

//-----------------------------------------------------------------------------
// Iterator access

template<typename Key, typename Value, class Sequence>
inline typename tHashMap<Key,Value,Sequence>::Iterator tHashMap<Key,Value,Sequence>::begin()
{
   return mHashMap.begin();
}

template<typename Key, typename Value, class Sequence>
inline typename tHashMap<Key,Value,Sequence>::ConstIterator tHashMap<Key,Value,Sequence>::begin() const
{
   return mHashMap.begin();
}

template<typename Key, typename Value, class Sequence>
inline typename tHashMap<Key,Value,Sequence>::Iterator tHashMap<Key,Value,Sequence>::end()
{
   return mHashMap.end();
}

template<typename Key, typename Value, class Sequence>
inline typename tHashMap<Key,Value,Sequence>::ConstIterator tHashMap<Key,Value,Sequence>::end() const
{
   return mHashMap.end();
}


//-----------------------------------------------------------------------------
// operators

template<typename Key, typename Value, class Sequence>
inline Value& tHashMap<Key,Value,Sequence>::operator[](const Key& key)
{
   return mHashMap.findOrInsert(key)->value;
}








#endif// _HASHTABLE_H
