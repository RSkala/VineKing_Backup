#include "T2D/t2dQuadBatch.h"
#include "core/frameAllocator.h"

#if 0 // PUAP -Mat Looks like this entire class is unused read comment below 

Vector<t2dQuadBatch *> t2dQuadBatch::smQuadBatches;
U16 t2dQuadBatch::smIndices[QUAD_BATCH_BUFFER_SIZE - ( QUAD_BATCH_BUFFER_SIZE % 6 )];
bool t2dQuadBatch::smIdxBufferReady = false;

//------------------------------------------------------------------------------

t2dQuadBatch::t2dQuadBatch()
{
   if( !smIdxBufferReady )
      initIndexBuffer();
}

//------------------------------------------------------------------------------

t2dQuadBatch::~t2dQuadBatch()
{

}

//------------------------------------------------------------------------------

void t2dQuadBatch::initIndexBuffer()
{
   // Fill the buffer. Triangles will look like this:
   //
   //  0 ___ 2
   //   |  /|
   //   | / |
   //  1|/__| 3
   //
   // So for a triangle list, the idx buffer for this quad should look like this
   // 0, 1, 2, 1, 2, 3
   //
   // Which is in the pattern:
   // k = 0
   // k++, k++, k--, k++, k++, k++

   // So now fill the buffer using that pattern
   U32 k = 0;
   U32 j = 0;

   // Loop while the value of j is within the bounds of the index buffer
   while( j < QUAD_BATCH_BUFFER_SIZE / 3 )
   {
      smIndices[j++] = k++; // 0
      smIndices[j++] = k++; // 1
      smIndices[j++] = k--; // 2

      smIndices[j++] = k++; // 1
      smIndices[j++] = k++; // 2
      smIndices[j++] = k++; // 3
   }

   smIdxBufferReady = true;
}

//------------------------------------------------------------------------------

void t2dQuadBatch::renderBatches( bool clearListAfterRender /* = true  */ )
{
   for( BatchListIterator i = smQuadBatches.begin(); i != smQuadBatches.end(); i++ )
   {
      (*i)->renderBatch( clearListAfterRender );
   }
}

//------------------------------------------------------------------------------

bool t2dQuadBatch::prepRender()
{
   if( mEntryList.size() < 1 )
      return false;

   // Set up the batches
   mBatches.increment();
   mBatches.last().texture = &mEntryList[0].imageMap->getImageMapFrameTexture( mEntryList[0].imageMapCell ); // HACK!!! FIX!! -pw
   mBatches.last().startVert = 0;
   mBatches.last().startIndex = 0;
   mBatches.last().numVerts = 0;
   mBatches.last().numPrims = 0;

   U32 currentPt = 0;

   for( EntryIterator i = mEntryList.begin(); i != mEntryList.end(); i++ )
   {
      // Fetch Current Frame Area.
      const t2dImageMapDatablock::cFrameTexelArea &frameArea = (*i).imageMap->getImageMapFrameArea( (*i).imageMapCell );

      // Fetch Positions.
      const F32& minX = frameArea.mX;
      const F32& minY = frameArea.mY;
      const F32& maxX = frameArea.mX2;
      const F32& maxY = frameArea.mY2;

      mVerts[currentPt++].set( (*i).verts[0].mX, (*i).verts[0].mY, minX, minY );
      mBatches.last().numVerts++;
      mVerts[currentPt++].set( (*i).verts[3].mX, (*i).verts[3].mY, minX, maxY );
      mBatches.last().numVerts++;
      mVerts[currentPt++].set( (*i).verts[1].mX, (*i).verts[1].mY, maxX, minY );
      mBatches.last().numVerts++;
      mVerts[currentPt++].set( (*i).verts[2].mX, (*i).verts[2].mY, maxX, maxY );
      mBatches.last().numVerts++;

      mBatches.last().numPrims += 2;

    QuadBatch &lastEntry = mBatches.last();

      if( ( ( lastEntry.numPrims + 2 ) >= ( QUAD_BATCH_BUFFER_SIZE / 3 ) ||
         ( lastEntry.numVerts + lastEntry.startVert + 4 ) >= QUAD_BATCH_BUFFER_SIZE ) )

      {
         // Ugh, what to do here?
      }
   }

   return true;
}

//------------------------------------------------------------------------------

void t2dQuadBatch::renderBatch( bool clearListAfterRender /* = true  */ )
{
   // Call this from here, for now -pw
   if( !prepRender() )
      return;

   // Turn on our GL stuff
   glEnable( GL_TEXTURE_2D );
   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
   glColor4f( 1.f, 1.f, 1.f, 1.f );

   glEnableClientState( GL_VERTEX_ARRAY );
   glVertexPointer( 2, GL_FLOAT, sizeof(QuadBatchVertex), &(mVerts[0].p) );

   glEnableClientState( GL_TEXTURE_COORD_ARRAY );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(QuadBatchVertex), &(mVerts[0].t) );

   glEnableClientState( GL_INDEX_ARRAY );
   glIndexPointer( GL_UNSIGNED_SHORT, 0, smIndices );

   // Render the batches
   for( BatchIterator i = mBatches.begin(); i != mBatches.end(); i++ )
   {
      glBindTexture( GL_TEXTURE_2D,  (*i).texture->getGLName() );
      glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

      // This needs to be done with a glDrawRangeElements, I get a weird
      // compile error when I enable it in the GL core .h file, and use it
      // so need to investigate why this doesn't work. -pw
      glDrawElements( GL_TRIANGLES, (*i).numPrims * 3, GL_UNSIGNED_SHORT, smIndices );
   }

   // Turn off GL stuff
   glDisableClientState( GL_VERTEX_ARRAY );
   glDisableClientState( GL_TEXTURE_COORD_ARRAY );
   glDisableClientState( GL_INDEX_ARRAY );

   glDisable( GL_TEXTURE_2D );
   glDisable( GL_BLEND );

   if( clearListAfterRender )
   {
      mBatches.clear();
      mEntryList.clear();
   }
}
#endif