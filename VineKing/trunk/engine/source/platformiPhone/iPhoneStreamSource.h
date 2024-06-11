//--------------------------------------------
// iPhoneStreamSource.h
// header for streaming audio source for WAV
//--------------------------------------------

#ifndef _IPHONESTREAMSOURCE_H_
#define _IPHONESTREAMSOURCE_H_

#include "console/simBase.h"

class iPhoneStreamSource: public SimObject
{
	public:
	char * mFilename;
	iPhoneStreamSource(const char *filename);
	~iPhoneStreamSource();
	bool isPlaying();
	bool start( bool loop = false );
	bool stop();
};

#endif // _AUDIOSTREAMSOURCE_H_
