//-----------------------------------------------------------------------------
// Torque Game Builder
// Copyright (C) GarageGames.com, Inc.
//
// 2D Utility.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "platform/platformGL.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "core/realComp.h"
#include "./t2dSceneObject.h"
#include "./t2dUtility.h"


//-----------------------------------------------------------------------------
// Globals.
//-----------------------------------------------------------------------------
MRandomLCG gT2DRandomGenerator;
SimObjectPtr<t2dSceneGraph> gDefaultSceneGraph = NULL;
SimObjectPtr<t2dSceneGraph> gLoadingSceneGraph = NULL;

//-----------------------------------------------------------------------------
// Utility Console Functions.
//-----------------------------------------------------------------------------
ConsoleFunctionGroupBegin( t2dUtility, "T2D Utility functions.");

//-----------------------------------------------------------------------------
// T2D Engine Version.
//-----------------------------------------------------------------------------
ConsoleFunction( getT2DVersion, const char*, 1, 1, "Returns T2D Version")
{
    return T2D_ENGINE_VERSION;
}
ConsoleFunction( getiPhoneToolsVersion, const char*, 1, 1, "Returns T2D iPhone Tools Version")
{
	return T2D_IPHONETOOLS_VERSION;
}

//-----------------------------------------------------------------------------
// T2D Max Texture Size.
//-----------------------------------------------------------------------------
ConsoleFunction( getT2DMaxTextureSize, S32, 1, 1, "Returns the maximum texture size supported by the current hardware." )
{
    return mGetT2DMaxTextureSize();
}


//-----------------------------------------------------------------------------
// T2D BIT Position Calculator.
//-----------------------------------------------------------------------------
ConsoleFunction( bit, const char*, 2, 2, "Converts a bit-position into a value." )
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(16);
    // Format Output.
    dSprintf( pBuffer, 16, "%u", U32(BIT(dAtoi(argv[1]))) );
    // Return Buffer.
    return pBuffer;
}

ConsoleFunction( bitInverse, const char*, 2, 2, "Returns the ones complement of a bit." )
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(16);
    // Format Output.
    dSprintf( pBuffer, 16, "%u", U32(~BIT(dAtoi(argv[1]))) );
    // Return Buffer.
    return pBuffer;
}

ConsoleFunction( addBitToMask, S32, 3, 3, "( mask, bit ) - Returns the mask with a bit added to it" )
{
   U32 mask;
   dSscanf( argv[1], "%u", &mask );
   U32 bit = BIT( dAtoi( argv[2] ) );
   
   return mask | bit;
}

ConsoleFunction( removeBitFromMask, S32, 3, 3, "( mask, bit ) - Returns the mask with a bit removed from it" )
{
   U32 mask;
   dSscanf( argv[1], "%u", &mask );
   U32 bit = BIT( dAtoi( argv[2] ) );
   
   return mask & ~bit;
}


//-----------------------------------------------------------------------------
// T2D BIT Position List Calculator.
//-----------------------------------------------------------------------------
ConsoleFunction( bits, const char*, 2, 2, "Converts a list of bit-positions into a value." )
{
    // Calculate Element Count.
    U32 elementCount = t2dSceneObject::getStringElementCount( argv[1] );

    // Return nothing if there's no elements.
    if ( elementCount < 1 )
    {
        // Error!
        Con::printf("bits() - Invalid number of parameters!");
        return "0";
    }

    // Reset Bit Value.
    U32 bitValue = 0;

    // Parse Bits.
    for ( U32 n = 0; n < elementCount; n++ )
    {
        // Merge Bit Value.
        bitValue |= U32(BIT(dAtoi(t2dSceneObject::getStringElement(argv[1],n))));
    }

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(16);
    // Format Output.
    dSprintf( pBuffer, 16, "%u", bitValue );
    // Return Buffer.
    return pBuffer;
}


//-----------------------------------------------------------------------------
// Get Minimum of two values.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dGetMin, F32, 3, 3, "(a, b) - Returns the Minimum of two values.")
{
   return mGetMin(dAtof(argv[1]), dAtof(argv[2]));
}


//-----------------------------------------------------------------------------
// Get Maximum of two values.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dGetMax, F32, 3, 3, "(a, b) - Returns the Maximum of two values.")
{
   return mGetMax(dAtof(argv[1]), dAtof(argv[2]));
}


//-----------------------------------------------------------------------------
// Begin T2D Scene-Graph Block,
//-----------------------------------------------------------------------------
ConsoleFunction( t2dBeginScene, void, 1, 2, "Sets the default Scene Graph." )
{
    // Find t2dSceneGraph Object.
    gDefaultSceneGraph = dynamic_cast<t2dSceneGraph*>(Sim::findObject(argv[1]));

    // Validate Object.
    if ( !gDefaultSceneGraph )
    {
        Con::warnf("t2dBeginScene() - Couldn't find/Invalid Scene-Graph Object '%s'.", argv[1]);
        return;
    }

}

//-----------------------------------------------------------------------------
// End T2D Scene-Graph Block,
//-----------------------------------------------------------------------------
ConsoleFunction( t2dEndScene, void, 1, 1, "Resets the default Scene Graph." )
{
    // Reset Default Scene Graph.
    gDefaultSceneGraph = NULL;
}


//-----------------------------------------------------------------------------
// Assert Fatal.
//-----------------------------------------------------------------------------
ConsoleFunction( t2dAssert, void, 3, 3, "(condition, message) - Fatal Script Assertion" )
{
    // Process Assertion.
    AssertFatal( dAtob(argv[1]), argv[2] );
}


//-----------------------------------------------------------------------------
// Assert Fatal ISV (In Shipping Version).
//-----------------------------------------------------------------------------
ConsoleFunction( t2dAssertISV, void, 3, 3, "(condition, message) - Fatal ISV Script Assertion" )
{
    // Process Assertion.
    AssertISV( dAtob(argv[1]), argv[2] );
}


//-----------------------------------------------------------------------------
ConsoleFunctionGroupEnd( t2dUtility );
//-----------------------------------------------------------------------------



//////////////////////////////////////////////////////////////////////////
// TypePoint2FVector
//////////////////////////////////////////////////////////////////////////
ConsoleType( point2FList, TypePoint2FVector, sizeof(Vector<Point2F>) )

ConsoleGetType( TypePoint2FVector )
{
   Vector<Point2F> *vec = (Vector<Point2F> *)dptr;
   char* returnBuffer = Con::getReturnBuffer( vec->size() * 64 );
   S32 maxReturn = 1024;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<Point2F>::iterator itr = vec->begin(); itr != vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%.3f %.3f ", (*itr).x, (*itr).y);
      // update the length of the return string (so far)
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return returnBuffer;
}

ConsoleSetType( TypePoint2FVector )
{
   Vector<Point2F> *vec = (Vector<Point2F> *)dptr;
   // we assume the vector should be cleared first (not just appending)
   vec->clear();
   if(argc == 1)
   {
      const char *values = argv[0];
      const char *endValues = values + dStrlen(values);
      Point2F value;
      // advance through the string, pulling off S32's and advancing the pointer
      while (values < endValues && dSscanf(values, "%g %g", &value.x, &value.y) != 0)
      {
         vec->push_back(value);
         const char *nextSeperator = dStrchr(values, ' ');
         if( !nextSeperator )
            break;
         const char *nextValues = dStrchr(nextSeperator + 1, ' ');
         if (nextValues != 0 && nextValues < endValues)
            values = nextValues + 1;
         else
            break;
      }
   }
   else if (argc > 1)
   {
      for (S32 i = 0; i < (argc - 1); i += 2)
         vec->push_back(Point2F(dAtof(argv[i]), dAtof(argv[i + 1])));
   }
   else
      Con::printf("Vector<Point2F> must be set as { a, b, c, ... } or \"a b c ...\"");
}
