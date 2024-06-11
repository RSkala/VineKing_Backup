//--------------------------------------
// audioStreamSource.cc
// implementation of streaming audio source
//
// Kurtis Seebaldt
//--------------------------------------

#include "iPhoneStreamSource.h"
#include "SoundEngine.h"

#define BUFFERSIZE 32768

iPhoneStreamSource::iPhoneStreamSource(const char *filename)  {
	this->registerObject();
	int len = dStrlen( filename );
	mFilename = new char[len + 1];
	dStrcpy( mFilename, filename );
	//SoundEngine::SoundEngine_LoadBackgroundMusicTrack( mFilename, true, false );
}

iPhoneStreamSource::~iPhoneStreamSource() {
	stop();
	delete [] mFilename;
	//SoundEngine::SoundEngine_UnloadBackgroundMusicTrack();
}

bool iPhoneStreamSource::isPlaying() {
	return true;
}

bool iPhoneStreamSource::start( bool loop ) {
	SoundEngine::SoundEngine_LoadBackgroundMusicTrack( mFilename, true, false );
	SoundEngine::SoundEngine_StartBackgroundMusic();
	if( !loop ) {
		//stop at end
		SoundEngine::SoundEngine_StopBackgroundMusic( true );
		Con::executef(1,"oniPhoneStreamEnd");
	}
	return true;
}

bool iPhoneStreamSource::stop() {
	//false == stop now
	SoundEngine::SoundEngine_StopBackgroundMusic( false );
	SoundEngine::SoundEngine_UnloadBackgroundMusicTrack();
	return true;
}
