#ifndef _T2D_QUADBATCH_H_
#define _T2D_QUADBATCH_H_

#ifndef _TVECTOR_H_
#  include "core/tVector.h"
#endif

#ifndef _T2DSCENEOBJECT_H_
#  include "./t2dSceneObject.h"
#endif

#ifndef _T2DIMAGEMAPDATABLOCK_H_
#  include "./t2dImageMapDatablock.h"
#endif

struct t2dQuadBatchEntry
{
   const t2dVector *verts;
   const t2dImageMapDatablock *imageMap;
   S32 imageMapCell;
};

#define QUAD_BATCH_BUFFER_SIZE 4096

//------------------------------------------------------------------------------
struct QuadBatchVertex
{
   Point2F p;
   Point2F t;
   void set( F32 x, F32 y, F32 tx, F32 ty )
   {
      p.x = x;
      p.y = y;
      t.x = tx;
      t.y = ty;
   }
};

struct QuadBatch
{
   const TextureHandle *texture;
   U32 startVert;
   U32 numVerts;
   U32 startIndex;
   U32 numPrims;
};

class t2dQuadBatch
{
private:
   typedef Vector<t2dQuadBatchEntry>::iterator EntryIterator;
   Vector<t2dQuadBatchEntry> mEntryList;
   U32 mBatchEntryIndex;

   typedef Vector<t2dQuadBatch *>::iterator BatchListIterator;
   static Vector<t2dQuadBatch *> smQuadBatches;

   // This is because we will be using indicies in groups of 6
   static U16 smIndices[QUAD_BATCH_BUFFER_SIZE - ( QUAD_BATCH_BUFFER_SIZE % 6 )];

   QuadBatchVertex mVerts[QUAD_BATCH_BUFFER_SIZE - 4];

   typedef Vector<QuadBatch>::iterator BatchIterator;
   Vector<QuadBatch> mBatches;

   static bool smIdxBufferReady;
   static void initIndexBuffer();
public:
   t2dQuadBatch();
   virtual ~t2dQuadBatch();

   void submitQuad( const t2dVector *verts, const t2dImageMapDatablock *imageMap, S32 imageMapCell );

   bool prepRender();
   void renderBatch( bool clearListAfterRender = true );

   static void renderBatches( bool clearListAfterRender = true );
   static void submitBatch( t2dQuadBatch *batch ) { smQuadBatches.push_back( batch ); };
};

//------------------------------------------------------------------------------

inline void t2dQuadBatch::submitQuad( const t2dVector *verts, const t2dImageMapDatablock *imageMap, S32 imageMapCell )
{
   mEntryList.increment();
   mEntryList.last().verts = verts;
   mEntryList.last().imageMap = imageMap;
   mEntryList.last().imageMapCell = imageMapCell;
}

#endif