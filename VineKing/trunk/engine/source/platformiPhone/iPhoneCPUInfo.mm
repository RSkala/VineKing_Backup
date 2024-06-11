//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformiPhone/platformiPhone.h"
#include "console/console.h"
#include "core/stringTable.h"
#include <math.h>


Platform::SystemInfo_struct Platform::SystemInfo;

void Processor::init()
{
	
   Con::printf("System & Processor Information:");

   Con::printf("   iPhone version: %0.0f", platState.osVersion );

	// -Mat FIXME: USE SYSTEM FUNCTION to get version number for 
	//just use OS version for now
   Con::printf("   CarbonLib version: %0.0f", platState.osVersion );
   
   Con::printf("   Physical RAM: %dMB", 128);

   Con::printf("   Logical RAM: %dMB", 128);

   Platform::SystemInfo.processor.mhz = 412;

   //Platform::SystemInfo.processor.type =  ARM_1176;
   Platform::SystemInfo.processor.name = StringTable->insert("ARM 1176");

   Platform::SystemInfo.processor.properties = CPU_PROP_PPCMIN;

	Con::printf("   %s, %d Mhz", Platform::SystemInfo.processor.name, Platform::SystemInfo.processor.mhz);
   if (Platform::SystemInfo.processor.properties & CPU_PROP_PPCMIN)
      Con::printf("   FPU detected");
   if (Platform::SystemInfo.processor.properties & CPU_PROP_ALTIVEC)
      Con::printf("   AltiVec detected");

   Con::printf(" ");
}

