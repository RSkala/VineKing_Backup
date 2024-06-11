
#ifndef _IPHONE_PROFILER_H
#define _IPHONE_PROFILER_H

#include"core/TorqueConfig.h"

#ifdef TORQUE_OS_IPHONE
//#define TORQUE_OS_IPHONE_PROFILE
//
//  iPhoneMiniProfiler.h
//  Torque2D
//
//  Created by puap on 9/26/08.
//  Copyright 2008  PUAP. All rights reserved.
//

#define IPHONE_PROFILER_START( _x ) iPhoneProfilerProfileStartName( #_x );
#define IPHONE_PROFILER_END( _x )	iPhoneProfilerProfileEndName( #_x );

//--------------------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include "core/tVector.h"
#import <mach/mach_time.h>
#include "console/console.h"

#define IPHONE_PROFILER_MAX_CALLS 256

class iPhoneProfilerData {
public:
	//make an array of these s we can still count in recursive functions
	U32 timeMach;//time stamp
	U32 timeMS;//time stamp
	const char *name;//name of this profile
	int type; //0 for start 1 for end
	int invokes;//Number of times we have recursed into this profile. Used with start[]
};


//PUAP
//--------------------------------------------------------------------------------------------------------------------------------------------

void iPhoneProfilerResetAll();
void iPhoneProfilerProfilerInit();
int iPhoneProfilerGetCount();

void iPhoneProfilerStart( const char *name );
void iPhoneProfilerEnd( const char *name );

int iPhoneProfilerFindProfileEnd( const char *name, int startCount);

void iPhoneProfilerPrintResult( int item );
void iPhoneProfilerPrintAllResults();

//-Mat to get instruction names
const char *getInstructionName( int index );


#endif//TORQUE_OS_IPHONE

#endif// _IPHONE_PROFILER_H
