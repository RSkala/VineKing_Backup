//
//  iPhoneMiniProfiler.mm
//  Torque2D
//
//  Created by puap on 9/26/08.
//  Copyright 2008  PUAP. All rights reserved.
//

#import "iPhoneProfiler.h"

//PUAP
//--------------------------------------------------------------------------------------------------------------------------------------------
iPhoneProfilerData g_iPhoneProfilerData[IPHONE_PROFILER_MAX_CALLS];
int	g_iPhoneProfilerCount = 0;
bool g_iPhoneProfilerReady = false;

void iPhoneProfilerResetAll() {
	for(int i=0; i<IPHONE_PROFILER_MAX_CALLS; i++){
		g_iPhoneProfilerData[i].name = NULL;
		g_iPhoneProfilerData[i].timeMach = 0;
		g_iPhoneProfilerData[i].timeMS = 0;
		g_iPhoneProfilerData[i].type = 0;
		g_iPhoneProfilerData[i].invokes = 0;
	}
	g_iPhoneProfilerCount = 0;
}


void iPhoneProfilerProfilerInit() {
	g_iPhoneProfilerReady = true;
	
	iPhoneProfilerResetAll();
	printf( "\n\nIniting PUAP Profiler\n");
}


void iPhoneProfilerStart( const char *name ) {
	if( g_iPhoneProfilerReady ) {

		int i = g_iPhoneProfilerCount++;
		g_iPhoneProfilerData[i].name = name;
		g_iPhoneProfilerData[i].timeMach = mach_absolute_time();
		g_iPhoneProfilerData[i].timeMS = Platform::getRealMilliseconds();
		g_iPhoneProfilerData[i].type = 0;
		g_iPhoneProfilerData[i].invokes = 0;
	
		if(g_iPhoneProfilerCount >= IPHONE_PROFILER_MAX_CALLS){
			g_iPhoneProfilerReady = false;
		}
	}
}	

void iPhoneProfilerEnd( const char *name ) {
	if( g_iPhoneProfilerReady ) {

		int i = g_iPhoneProfilerCount++;
		g_iPhoneProfilerData[i].name = name;
		g_iPhoneProfilerData[i].timeMach = mach_absolute_time();
		g_iPhoneProfilerData[i].timeMS = Platform::getRealMilliseconds();
		g_iPhoneProfilerData[i].type = 1;
		g_iPhoneProfilerData[i].invokes = 0;

		if(g_iPhoneProfilerCount >= IPHONE_PROFILER_MAX_CALLS){
			g_iPhoneProfilerReady = false;
	}	
	}
}

int iPhoneProfilerFindProfileEnd( const char *name, int startCount){
	int invokes = 0;
	for( int i = (startCount+1); i < g_iPhoneProfilerCount; i++ ) {
		if(g_iPhoneProfilerData[i].name == name || dStrcmp(g_iPhoneProfilerData[i].name, name)==0){
			if(g_iPhoneProfilerData[i].type == 1){
				if(invokes == 0){
					return i;
	}
				else{
					g_iPhoneProfilerData[i].invokes = invokes;
					invokes--;
	}
}
			else{
				invokes++;
				g_iPhoneProfilerData[i].invokes = invokes;
		}
	}
	}
	return -1;
}

int iPhoneProfilerGetCount(){
	return g_iPhoneProfilerCount;
}

char pfbuffer[1024];
void iPhoneProfilerPrintResult( int item ) {

	if(g_iPhoneProfilerData[item].type == 0){
		int endItem = iPhoneProfilerFindProfileEnd(g_iPhoneProfilerData[item].name, item);
		if(endItem < 0){
			return; //didn't find the end of this one
}
		// UNUSED: JOSEPH THOMAS -> U32 startTimeMach = g_iPhoneProfilerData[item].timeMach;
		U32 startTimeMS = g_iPhoneProfilerData[item].timeMS;
		// UNUSED: JOSEPH THOMAS -> U32 endTimeMach = g_iPhoneProfilerData[endItem].timeMach;
		U32 endTimeMS = g_iPhoneProfilerData[endItem].timeMS;

//		dSprintf(buffer, 1024, "\t\t%i (%ims)\t\t\t\t%i (%ims)\t\t\t\t\t%i (%i)\t\t\t%i(%ims)\t\t\t\t\t%s\n",
//				 data->timePerFrameMach, data->timePerFrameMS, data->averageMach, data->averageMS, data->callsPerFrame, data->averageCallsPerFrame,
//				 timePerCallMach, timePerCallMS,
//				 data->name//data
//				 );
		dSprintf(pfbuffer, 1024, "%s, %d, %d, %d\n",
				 g_iPhoneProfilerData[item].name, startTimeMS, endTimeMS, endTimeMS - startTimeMS);
		printf( pfbuffer );

//		dSprintf(buffer, 1024, "\t\t%i (%ims)\t\t\t\t%i (%ims)\t\t\t\t\t%i (%i)\t\t\t%i(%ims)\t\t\t\t\t%s\n",
//				 data->timePerFrameMach, data->timePerFrameMS, data->averageMach, data->averageMS, data->callsPerFrame, data->averageCallsPerFrame,
//				 timePerCallMach, timePerCallMS,
//				 data->name//data
//				 );
//		printf( buffer );
	}
}

void iPhoneProfilerPrintAllResults() {
	
	dSprintf(pfbuffer, 1024, "\n\n \t Time Per Frame \t Average Time \t Calls Per Frame (avg) \t Time Per Call \t Name \n" );
	printf(pfbuffer);
	
	for( int i = 0; i < g_iPhoneProfilerCount; i++ ) {
		iPhoneProfilerPrintResult( i );
	}
	
	dSprintf(pfbuffer, 1024, "\n 	<----------------------------------------------------------------------------------->\n " );
	printf(pfbuffer);	
}


//-Mat get instruction names

/// The opcodes for the TorqueScript VM.
const char *InstructionName[] =  {
"OP_FUNC_DECL",
"OP_CREATE_OBJECT",
"OP_ADD_OBJECT",
"OP_END_OBJECT",
"OP_JMPIFFNOT",
"OP_JMPIFNOT",
"OP_JMPIFF",
"OP_JMPIF",
"OP_JMPIFNOT_NP",
"OP_JMPIF_NP",
"OP_JMP",
"OP_RETURN",
"OP_CMPEQ",
"OP_CMPGR",
"OP_CMPGE",
"OP_CMPLT",
"OP_CMPLE",
"OP_CMPNE",
"OP_XOR",
"OP_MOD",
"OP_BITAND",
"OP_BITOR",
"OP_NOT",
"OP_NOTF",
"OP_ONESCOMPLEMENT",

"OP_SHR",
"OP_SHL",
"OP_AND",
"OP_OR",

"OP_ADD",
"OP_SUB",
"OP_MUL",
"OP_DIV",
"OP_NEG",

"OP_SETCURVAR",
"OP_SETCURVAR_CREATE",
"OP_SETCURVAR_ARRAY",
"OP_SETCURVAR_ARRAY_CREATE",

"OP_LOADVAR_UINT",
"OP_LOADVAR_FLT",
"OP_LOADVAR_STR",

"OP_SAVEVAR_UINT",
"OP_SAVEVAR_FLT",
"OP_SAVEVAR_STR",

"OP_SETCUROBJECT",
"OP_SETCUROBJECT_NEW",
"OP_SETCUROBJECT_INTERNAL",

"OP_SETCURFIELD",
"OP_SETCURFIELD_ARRAY",

"OP_LOADFIELD_UINT",
"OP_LOADFIELD_FLT",
"OP_LOADFIELD_STR",

"OP_SAVEFIELD_UINT",
"OP_SAVEFIELD_FLT",
"OP_SAVEFIELD_STR",

"OP_STR_TO_UINT",
"OP_STR_TO_FLT",
"OP_STR_TO_NONE",
"OP_FLT_TO_UINT",
"OP_FLT_TO_STR",
"OP_FLT_TO_NONE",
"OP_UINT_TO_FLT",
"OP_UINT_TO_STR",
"OP_UINT_TO_NONE",

"OP_LOADIMMED_UINT",
"OP_LOADIMMED_FLT",
"OP_TAG_TO_STR",
"OP_LOADIMMED_STR",
"OP_DOCBLOCK_STR",
"OP_LOADIMMED_IDENT",

"OP_CALLFUNC_RESOLVE",
"OP_CALLFUNC",

"OP_ADVANCE_STR",
"OP_ADVANCE_STR_APPENDCHAR",
"OP_ADVANCE_STR_COMMA",
"OP_ADVANCE_STR_NUL",
"OP_REWIND_STR",
"OP_TERMINATE_REWIND_STR",
"OP_COMPARE_STR",

"OP_PUSH",
"OP_PUSH_FRAME",

"OP_BREAK",

"OP_INVALID"
};


const char *getInstructionName( int index ) {
	return InstructionName[index];
}
