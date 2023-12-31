//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifdef WITH_FMOD
#include "../../Dependencies/include/fmod.h"
#else
#define FSOUND_FREE 0
#endif

#include "../../include/JSoundSystem.h"
#include "../../include/JFileSystem.h"


//////////////////////////////////////////////////////////////////////////
JMusic::JMusic()
#ifdef USE_PHONON
: mOutput(0), mMediaObject(0)
#endif
{
}

void JMusic::Update(){

}

int JMusic::getPlayTime(){
#ifdef WITH_FMOD
	return FSOUND_GetCurrentPosition(JSoundSystem::GetInstance()->mChannel)/44.1; //todo more generic, here it's only 44kHz
#else
	return 0;
#endif
}

JMusic::~JMusic()
{
#ifdef USE_PHONON
	if(mOutput)
		delete mOutput;
	if(mMediaObject)
		delete mMediaObject;
#else
#ifdef WITH_FMOD
	JSoundSystem::GetInstance()->StopMusic(this);
	if (mTrack) FSOUND_Sample_Free(mTrack);
#endif
#endif
}

#ifdef USE_PHONON
void JMusic::seekAtTheBegining()
{
	mMediaObject->seek(0);
}
#endif

//////////////////////////////////////////////////////////////////////////
JSample::JSample()
#ifdef USE_PHONON
: mOutput(0), mMediaObject(0)
#endif
{

}

JSample::~JSample()
{
#ifdef USE_PHONON
	if(mOutput)
		delete mOutput;
	if(mMediaObject)
		delete mMediaObject;
#else
#ifdef WITH_FMOD
	if (mSample) FSOUND_Sample_Free(mSample);
#endif
#endif
}

unsigned long JSample::fileSize()
{
#ifdef WITH_FMOD
	return FSOUND_Sample_GetLength(mSample);
#else
	return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
JSoundSystem* JSoundSystem::mInstance = NULL;

JSoundSystem* JSoundSystem::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new JSoundSystem();
		mInstance->InitSoundSystem();
	}
	return mInstance;
}


void JSoundSystem::Destroy()
{
	if (mInstance)
	{
		mInstance->DestroySoundSystem();
		delete mInstance;
		mInstance = NULL;
	}
}


JSoundSystem::JSoundSystem()
{
	mVolume = 0;
	mSampleVolume = 0;
#ifdef WITH_FMOD
	mChannel = FSOUND_FREE;
#endif
}

JSoundSystem::~JSoundSystem()
{
}

void JSoundSystem::InitSoundSystem()
{
#ifdef WITH_FMOD
	FSOUND_Init(44100, 32, 0);
#endif
}


void JSoundSystem::DestroySoundSystem()
{
#ifdef WITH_FMOD
	FSOUND_Close();
#endif
}


JMusic *JSoundSystem::LoadMusic(const char *fileName)
{
#ifdef USE_PHONON
	JMusic* music = new JMusic();
	if (music)
	{
		music->mOutput = new Phonon::AudioOutput(Phonon::GameCategory, 0);
		music->mMediaObject = new Phonon::MediaObject(0);
		music->mMediaObject->setCurrentSource("Res/" + QString(fileName));
		Phonon::Path path = Phonon::createPath(music->mMediaObject, music->mOutput);
		Q_ASSERT(path.isValid());
	}
	return music;
#else
#ifndef WITH_FMOD
	return NULL;
#else
	JMusic* music = new JMusic();
	if (music)
	{
		JFileSystem* fileSystem = JFileSystem::GetInstance();
		if (fileSystem->OpenFile(fileName))
		{
			int size = fileSystem->GetFileSize();
			char *buffer = new char[size];
			fileSystem->ReadFile(buffer, size);
			music->mTrack = FSOUND_Sample_Load(FSOUND_UNMANAGED, buffer, FSOUND_LOADMEMORY, 0, size);

			delete[] buffer;
			fileSystem->CloseFile();
		}
	}
	return music;
#endif
#endif
}


void JSoundSystem::PlayMusic(JMusic *music, bool looping)
{
#ifdef USE_PHONON
	if (music && music->mMediaObject && music->mOutput)
	{
		if(looping)
		{
			music->mMediaObject->connect(music->mMediaObject, SIGNAL(aboutToFinish()), music, SLOT(seekAtTheBegining()));
		}
		music->mOutput->setVolume((qreal)mVolume*0.01);
		music->mMediaObject->play();

	}
#else
#ifdef WITH_FMOD
	if (music && music->mTrack)
	{
		mChannel = FSOUND_PlaySound(mChannel, music->mTrack);
		SetMusicVolume(mVolume);

		if (looping)
			FSOUND_SetLoopMode(mChannel, FSOUND_LOOP_NORMAL);
		else
			FSOUND_SetLoopMode(mChannel, FSOUND_LOOP_OFF);
	}
#endif
#endif
}

void JSoundSystem::StopMusic(JMusic *music)
{
#ifdef USE_PHONON
	if (music && music->mMediaObject && music->mOutput)
	{
		music->mMediaObject->stop();
	}
#else
#ifdef WITH_FMOD
	FSOUND_StopSound(mChannel);
#endif
#endif
}

void JSoundSystem::PauseMusic(JMusic *music)
{
#ifdef USE_PHONON
	if (music && music->mMediaObject && music->mOutput)
	{
		music->mMediaObject->stop();
	}
#else
#ifdef WITH_FMOD
	FSOUND_SetPaused(mChannel, true);
#endif
#endif
}

void JSoundSystem::ResumeMusic(JMusic *music)
{
#ifdef USE_PHONON
	if (music && music->mMediaObject && music->mOutput)
	{
		music->mMediaObject->play();
	}
#else
#ifdef WITH_FMOD
	if (music && music->mTrack)
	{
		FSOUND_SetPaused(mChannel, false);
	}
#endif
#endif
}



void JSoundSystem::SetVolume(int volume)
{
	SetMusicVolume(volume);
	SetSfxVolume(volume);
}

void JSoundSystem::SetMusicVolume(int volume)
{
#ifdef WITH_FMOD
	if (mChannel != FSOUND_FREE) FSOUND_SetVolumeAbsolute(mChannel,volume * 2.55);
#endif
	mVolume = volume;
}

void JSoundSystem::SetSfxVolume(int volume){
	//this sets the volume to all channels then reverts back the volume for music..
	//that's a bit dirty but it works
#ifdef WITH_FMOD
	FSOUND_SetVolumeAbsolute(FSOUND_ALL, volume * 2.55);
#endif
	mSampleVolume = volume;
	SetMusicVolume(mVolume);
}

JSample *JSoundSystem::LoadSample(const char *fileName)
{
#ifdef USE_PHONON
	JSample* sample = new JSample();
	if (sample)
	{
		sample->mOutput = new Phonon::AudioOutput(Phonon::GameCategory, 0);
		sample->mMediaObject = new Phonon::MediaObject(0);
		sample->mMediaObject->setCurrentSource("Res/" + QString(fileName));
		Phonon::Path path = Phonon::createPath(sample->mMediaObject, sample->mOutput);
		Q_ASSERT(path.isValid());
	}
	return sample;
#else
#ifndef WITH_FMOD
	return NULL;
#else
	JSample* sample = new JSample();
	if (sample)
	{
		JFileSystem* fileSystem = JFileSystem::GetInstance();
		if (fileSystem->OpenFile(fileName))
		{
			int size = fileSystem->GetFileSize();
			char *buffer = new char[size];
			fileSystem->ReadFile(buffer, size);
			sample->mSample = FSOUND_Sample_Load(FSOUND_UNMANAGED, buffer, FSOUND_LOADMEMORY, 0, size);

			delete[] buffer;
			fileSystem->CloseFile();
		}else
			sample->mSample = NULL;

	}
	return sample;
#endif
#endif
}


void JSoundSystem::PlaySample(JSample *sample)
{
#ifdef USE_PHONON
	if (sample && sample->mMediaObject && sample->mOutput)
	{
		sample->mOutput->setVolume((qreal)mSampleVolume*0.01);
		sample->mMediaObject->play();
	}
#else
#ifdef WITH_FMOD
	if (sample && sample->mSample){
		int channel = FSOUND_PlaySound(FSOUND_FREE, sample->mSample);
		FSOUND_SetVolumeAbsolute(channel,mSampleVolume * 2.55);
	}
#endif
#endif
}

