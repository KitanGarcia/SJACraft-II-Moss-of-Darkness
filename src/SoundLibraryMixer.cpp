/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.

    All source material (source code, images, sounds, etc.) have been provided to
    University of California, Davis students of course ECS 160 for educational
    purposes. It may not be distributed beyond those enrolled in the course without
    prior permission from the copyright holder.

    All sound files, sound fonts, midi files, and images that have been included 
    that were extracted from original Warcraft II by Blizzard Entertainment 
    were found freely available via internet sources and have been labeld as 
    abandonware. They have been included in this distribution for educational 
    purposes only and this copyright notice does not attempt to claim any 
    ownership of this material.
*/

/**
*
* @class CSoundLibraryMixer
*
* @brief This class maintains the sound event rendering information
*
* @author Hugo
*
* @version 9.0
*
* @date 12/3/17
*
* Contact: huhuang@ucdavis.edu
*
*/

#include "SoundLibraryMixer.h"
#include "CommentSkipLineDataSource.h"
#include "Debug.h"
#include <fstream>
#include <math.h>
#include <unistd.h>
#include <cstring>

#define FRAMES_PER_BUFFER       512
#ifndef M_PI
#define M_PI                    3.14159265358979323846
#endif

#ifdef _WIN32
#error "Unsupported platform"
#elif __APPLE__
#define AUDIO_DRIVER "coreaudio"
#elif __linux__
#define AUDIO_DRIVER "pulseaudio"
#elif __unix__ // all unices not caught above
#error "Unsupported platform"
#elif defined(_POSIX_VERSION)
#error "Unsupported platform"
#else
#error "Unknown compiler"
#endif

bool no_audio;

/**
* A CSoundLibraryMixer object constructor, initialize internal values and pushes in free clips
*
* @param[in] NO_AUDIOMIX boolean variable to indicate if there is audio mixer
*
*/

CSoundLibraryMixer::CSoundLibraryMixer(bool NO_AUDIOMIX){
    no_audio = NO_AUDIOMIX;
    
    if(no_audio)
        return;
    
    DStream = nullptr;
    DNextClipID = 0;
    DNextToneID = 0;
    DPortAudioInitialized = paNoError == Pa_Initialize();
    //pthread_mutex_init(&DMutex, nullptr);
    for(int Index = 0; Index < 128; Index++){
        DFreeClipIDs.push_back(Index);   
    }
}

/**
* A CSoundLibraryMixer object destructor, stops and closes the stream,
* as well as terminates the port audio
*
*/

CSoundLibraryMixer::~CSoundLibraryMixer(){
    if(nullptr != DStream){
        if(Pa_IsStreamActive(DStream)){
            Pa_StopStream(DStream);
        }
        Pa_CloseStream(DStream);
        DStream = nullptr;
    }
    if(DPortAudioInitialized){
        Pa_Terminate();
    }
    //pthread_mutex_destroy(&DMutex);
}

/**
* Find the clip corresponding to the name of the clip
*
* @param[in] clipname The name of the clip to be looked for
*
* @return The index of the clip
*
*/

int CSoundLibraryMixer::FindClip(const std::string &clipname) const{
    if(no_audio) 
        return 0;
    std::map< std::string, int >::const_iterator Iterator = DMapping.find(clipname);
    if(DMapping.end() != Iterator){
        return Iterator->second;
    }
    return -1;
}

/**
* Find the song corresponding to the name of the song
*
* @param[in] songname The name of the clip to be looked for
*
* @return The index of the song
*
*/

int CSoundLibraryMixer::FindSong(const std::string &songname) const{
    if(no_audio) 
        return 0;
    std::map< std::string, int >::const_iterator Iterator = DMusicMapping.find(songname);
    if(DMusicMapping.end() != Iterator){
        return Iterator->second;
    }
    return -1;
}

/**
* Calculates the duration of the clip in microseconds
*
* @param[in] index The index of the clip
*
* @return The duration of the clip
*
*/

int CSoundLibraryMixer::ClipDurationMS(int index){
    if(no_audio) 
        return 0;
    if((0 > index)||(index >= DSoundClips.size())){
        return 0;    
    }
    return (DSoundClips[index].TotalFrames() * 1000) / DSoundClips[index].SampleRate();
}

/**
* Calls a CSoundLibraryMixer object's Timestep function
*
* @param[in] in A void pointer
* @param[in] out A void pointer to be passed into the Timestep function
* @param[in] frames The frame value for sound
* @param[in] timeinfo A constant pointer to a PaStreamCallbackTimeInfo object to be passed into the Timestep function
* @param[in] status A PaStreamCallbackFlags object to be passed into the Timestep function
* @param[in] data A pointer to a CSoundLibraryMixer object
*
* @return The index of the clip
*
*/

int CSoundLibraryMixer::TimestepCallback(const void *in, void *out, unsigned long frames, const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags status, void *data){
    if(no_audio) 
        return 0;
    CSoundLibraryMixer *Mixer = (CSoundLibraryMixer *)data;

    return Mixer->Timestep(out, frames, timeinfo, status);
}

/**
* Advances sound clip and tone to the next frame
* Put a clip or a tone to the free list if done playing
*
* @param[in] out A void pointer, an array that holds all the frames
* @param[in] timeinfo A constant pointer to a PaStreamCallbackTimeInfo object
* @param[in] status A PaStreamCallbackFlags object
*
* @return Signals that the stream should continue invoking the callback and processing audio
*
*/

int CSoundLibraryMixer::Timestep(void *out, unsigned long frames, const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags status){
    if(no_audio) 
        return 0;
    float *DataPtr = (float *)out;
    
    memset(DataPtr, 0, sizeof(float) * frames * 2);
    //pthread_mutex_lock(&DMutex);
    DMutex.lock();
    for(std::list< SClipStatus >::iterator ClipIterator = DClipsInProgress.begin(); ClipIterator != DClipsInProgress.end(); ){
        bool Advance = true;
        
        DSoundClips[ClipIterator->DIndex].MixStereoClip(DataPtr, ClipIterator->DOffset, frames, ClipIterator->DVolume, ClipIterator->DRightBias);
        
        ClipIterator->DOffset += frames;
        if(ClipIterator->DOffset >= DSoundClips[ClipIterator->DIndex].TotalFrames()){
            DFreeClipIDs.push_back(ClipIterator->DIdentification);   
            ClipIterator = DClipsInProgress.erase(ClipIterator);
            Advance = false;
        }
        if(Advance){
            ClipIterator++;   
        }
    }
    if(0 <= DMusicStatus.DIndex){
        DMusicClips[DMusicStatus.DIndex].MixStereoClip(DataPtr, DMusicStatus.DOffset, frames, DMusicStatus.DVolume, DMusicStatus.DRightBias, true);
        
        DMusicStatus.DOffset = (DMusicStatus.DOffset + frames) % DMusicClips[DMusicStatus.DIndex].TotalFrames();
    }
    
    for(std::list< SToneStatus >::iterator ToneIterator = DTonesInProgress.begin(); ToneIterator != DTonesInProgress.end(); ){
        bool Advance = true;
        float *CurOutPtr = DataPtr;
        
        for(int Frame = 0; Frame < frames; Frame++){
            int SineIndex = ToneIterator->DCurrentStep;
            
            if(0.5 <= (ToneIterator->DCurrentStep - SineIndex)){
                SineIndex++;
                if(DSampleRate <= SineIndex){
                    SineIndex = 0;   
                }
            }
            *CurOutPtr++ += ToneIterator->DVolume * (1.0 - ToneIterator->DRightBias) * DSineWave[SineIndex];
            *CurOutPtr++ += ToneIterator->DVolume * (1.0 + ToneIterator->DRightBias) * DSineWave[SineIndex];
            ToneIterator->DCurrentStep += ToneIterator->DCurrentFrequency;
            ToneIterator->DCurrentFrequency += ToneIterator->DFrequencyDecay;
            ToneIterator->DVolume += ToneIterator->DVolumeDecay;
            ToneIterator->DRightBias += ToneIterator->DRightShift;
            if(DSampleRate <= ToneIterator->DCurrentStep){
                ToneIterator->DCurrentStep -= DSampleRate;
            }
            if(0.0 > ToneIterator->DVolume){
                Advance = false;
                break;
            }
            if(20.0 > ToneIterator->DCurrentFrequency){
                Advance = false;
                break;
            }
            if(-1.0 > ToneIterator->DRightBias){
                ToneIterator->DRightBias = -1.0;
                ToneIterator->DRightShift = 0.0;
            }
            if(1.0 < ToneIterator->DRightBias){
                ToneIterator->DRightBias = -1.0;
                ToneIterator->DRightShift = 0.0;                
            }
        }        
        if(Advance){
            ToneIterator++;    
        }
        else{
            DFreeToneIDs.push_back(ToneIterator->DIdentification);   
            ToneIterator = DTonesInProgress.erase(ToneIterator);
        }
    }
    
    //pthread_mutex_unlock(&DMutex);    
    DMutex.unlock();
    frames *= 2;
    for(int Frame = 0; Frame < frames; Frame++){
        if(-1.0 > *DataPtr){
            *DataPtr = -1.0;
        }
        if(1.0 < *DataPtr){
            *DataPtr = 1.0;
        }
        DataPtr++;
    }
    
    return paContinue;
}

/**
* Loads the sound library data source file and initialize internal variables
*
* @param[in] source The sound library data source file to be read
* @param[in] callback The sound library loading callback function
* @param[in] calldata The sound library loading calldata for the callback function
*
* @return True if the data source file is loaded successfully
*
*/

bool CSoundLibraryMixer::LoadLibrary(std::shared_ptr< CDataSource > source, TSoundLibraryLoadingCallback callback, TSoundLibraryLoadingCalldata calldata){
    if(no_audio) 
        return false;
    CCommentSkipLineDataSource LineSource(source, '#');
    int TotalClips, TotalSongs;
    bool ReturnStatus = false;
    std::string TempString;
    std::shared_ptr< CDataSource >  SFInput;
    std::ofstream SFOutput;
    
    DSoundDataContainer = source->Container();
    
    if(!LineSource.Read(TempString)){
        PrintError("Failed to read sample rate.\n");
        goto LoadLibraryExit;
    }
    try{
        DSampleRate = std::stoi(TempString);
    }
    catch(std::exception &E){
        PrintError("%s\n",E.what());
        goto LoadLibraryExit;
    }
    
    DSineWave.resize(DSampleRate);
    for(int Index = 0; Index < DSampleRate; Index++){
        DSineWave[Index] = sin( ((double)Index/(double)DSampleRate) * M_PI * 2.0 );
    }
    
    if(!LineSource.Read(TempString)){
        PrintError("Failed to read line\n");
        goto LoadLibraryExit;
    }
    try{
        TotalSongs = std::stoi(TempString);
    }
    catch(std::exception &E){
        PrintError("%s\n",E.what());
        goto LoadLibraryExit;
    }

    DMusicFilenames.resize(TotalSongs);
    DMusicClips.resize(TotalSongs);
    for(int Index = 0; Index < TotalSongs; Index++){
        auto TempData = std::make_shared< std::vector< char > > ();
        
        if(!LineSource.Read(TempString)){
            PrintError("Failed to read song name %d\n", Index);
            goto LoadLibraryExit;
        }    
        DMusicMapping[TempString] = Index;
        if(!LineSource.Read(TempString)){
            PrintError("Failed to read song path %d\n", Index);
            goto LoadLibraryExit;
        }   
        DMusicFilenames[Index] = TempString;
        callback(calldata);
    }
    DMusicStatus.DIndex = -1;
    
    if(!DPortAudioInitialized){
        PaError PortAudioResult;
        PrintDebug(DEBUG_LOW, "Port Audio not initialized, try to reinitialize.\n");
        PortAudioResult = Pa_Initialize();    
        DPortAudioInitialized = paNoError == PortAudioResult;
        if(DPortAudioInitialized){
            PrintDebug(DEBUG_LOW, "Port Audio initialized.\n");
        }
        else{
            PrintDebug(DEBUG_LOW, "Port Audio failed to initialize with code %d.\n", PortAudioResult);
        }
    }
    PrintDebug(DEBUG_LOW, "Opening stream at %dHz with %d frames per buffer\n",DSampleRate, FRAMES_PER_BUFFER);
    if(paNoError != Pa_OpenDefaultStream(&DStream, 0, 2, paFloat32, DSampleRate, FRAMES_PER_BUFFER, TimestepCallback, this)){
        PrintError("Failed to open default sound stream\n");
        goto LoadLibraryExit;
    }
    PrintDebug(DEBUG_LOW, "Default sound stream opened.\n");
    if(paNoError != Pa_StartStream(DStream)){
        PrintError("Failed to start default sound stream\n");
        goto LoadLibraryExit;
    }
    PrintDebug(DEBUG_LOW, "Sound stream started.\n");
    
    PlaySong(0, 0.5);
        
    
    if(!LineSource.Read(TempString)){
        PrintError("Failed to read clip count.\n");
        goto LoadLibraryExit;
    }
    try{
        TotalClips = std::stoi(TempString);
    }
    catch(std::exception &E){
        PrintError("%s\n",E.what());
        goto LoadLibraryExit;
    }

    DSoundClips.resize(TotalClips);
    for(int Index = 0; Index < TotalClips; Index++){
        if(!LineSource.Read(TempString)){
            PrintError("Failed to read clip name %d.\n", Index);
            goto LoadLibraryExit;
        }
        DMapping[TempString] = Index;
        if(!LineSource.Read(TempString)){
            PrintError("Failed to read clip path %d.\n", Index);
            goto LoadLibraryExit;
        }
        if(!DSoundClips[Index].Load(DSoundDataContainer->DataSource(TempString))){
            PrintError("Failed to load sound clip %d.\n", Index);
            goto LoadLibraryExit;    
        }
        callback(calldata);
    }

    for(int Index = 1; Index < TotalClips; Index++){
        if(DSoundClips[0].SampleRate() != DSoundClips[Index].SampleRate()){
            PrintError("Incompatible sample rate %dHz of clip %d.\n", DSoundClips[Index].SampleRate(), Index);
            goto LoadLibraryExit;    
        }
    }
    
    return true;
    
LoadLibraryExit:
    DMusicFilenames.clear();
    DSoundClips.clear();
    DMapping.clear();

    return ReturnStatus;
}

/**
* Plays a clip specified by its index and sound volume by pushing the clip
* into the DClipsInProgress list
*
* @param[in] index Index of the clip to be played
* @param[in] volume The sound volume that the clip is to be played
* @prarm[in] rightbias The rightbias value of the clip
*
* @return The identification of the clip played
*
*/

int CSoundLibraryMixer::PlayClip(int index, float volume, float rightbias){
    if(no_audio) 
        return 0;
    SClipStatus TempClipStatus;
    
    if((0 > index)||(DSoundClips.size() <= index)){
        PrintError("Invalid Clip %d!!!!!\n",index);
        return -1;
    }
    TempClipStatus.DIndex = index;
    TempClipStatus.DOffset = 0;
    TempClipStatus.DVolume = volume;
    TempClipStatus.DRightBias = rightbias;
    
    //pthread_mutex_lock(&DMutex);
    DMutex.lock();
    if(DFreeClipIDs.size()){
        TempClipStatus.DIdentification = DFreeClipIDs.front();
        DFreeClipIDs.pop_front();
    }
    else{
        TempClipStatus.DIdentification = DNextClipID++;    
    }
    DClipsInProgress.push_back(TempClipStatus);
    //pthread_mutex_unlock(&DMutex);  
    DMutex.unlock();
    return TempClipStatus.DIdentification;
}

/**
* Plays a tone specified by frequency, sound volume, frequency decay and other values
* by pushing the tone into the DTonesInProgress list
*
* @param[in] freq The current frequency of the tone
* @param[in] freqdecay The frequency decay value of the tone
* @param[in] volume The sound volume that the tone is to be played
* @param[in] volumedecay The volume decay value of the tone
* @prarm[in] rightbias The rightbias value of the tone
* @param[in] rightshift The right shift value of the tone
*
* @return The identification of the tone played
*
*/

int CSoundLibraryMixer::PlayTone(float freq, float freqdecay, float volume, float volumedecay, float rightbias, float rightshift){
    if(no_audio) 
        return 0;
    SToneStatus TempToneStatus;
    

    TempToneStatus.DCurrentFrequency = freq;
    TempToneStatus.DCurrentStep = 0;
    TempToneStatus.DFrequencyDecay = freqdecay / DSampleRate;
    TempToneStatus.DVolume = volume;
    TempToneStatus.DVolumeDecay = volumedecay / DSampleRate;
    TempToneStatus.DRightBias = rightbias;
    TempToneStatus.DRightShift = rightshift / DSampleRate;
    
    //pthread_mutex_lock(&DMutex);
    DMutex.lock();
    if(DFreeToneIDs.size()){
        TempToneStatus.DIdentification = DFreeToneIDs.front();
        DFreeToneIDs.pop_front();
    }
    else{
        TempToneStatus.DIdentification = DNextToneID++;    
    }
    DTonesInProgress.push_back(TempToneStatus);
    //pthread_mutex_unlock(&DMutex);  
    DMutex.unlock();
    return TempToneStatus.DIdentification;
}

/**
* Stops a tone by removing the tone from the DToneInProgress list
*
* @param[in] id The identification of the tone to be stopped
*
* @return void
*
*/

void CSoundLibraryMixer::StopTone(int id){
    if(no_audio) 
        return;
    std::list< SToneStatus >::iterator ToneIterator;
    
    //pthread_mutex_lock(&DMutex);
    DMutex.lock();
    ToneIterator = DTonesInProgress.begin();
    while(ToneIterator != DTonesInProgress.end()){
        if(ToneIterator->DIdentification == id){
            DTonesInProgress.erase(ToneIterator);
            DFreeToneIDs.push_back(id);
            break;
        }
        ToneIterator++;
    }
    //pthread_mutex_unlock(&DMutex);  
    DMutex.unlock();
}

/**
* Looks through the free clips and see if a clip is completed
*
* @param[in] id The identification of the clip to be looked for
*
* @return True if the clip is found to be completed
*/

bool CSoundLibraryMixer::ClipCompleted(int id){
    if(no_audio) 
        return true;
    std::list< int >::iterator ClipIDIterator;
    bool FoundID = false;
    
    //pthread_mutex_lock(&DMutex);
    DMutex.lock();
    ClipIDIterator = DFreeClipIDs.begin();
    while(ClipIDIterator != DFreeClipIDs.end()){
        if(*ClipIDIterator == id){
            FoundID = true;
            break;
        }
        ClipIDIterator++;
    }
    //pthread_mutex_unlock(&DMutex); 
    DMutex.unlock();
    return FoundID;
}

/**
* Plays a song specified by its index and sound volume
*
* @param[in] index The index of the song to be played
* @param[in] volume The sound volume that the song is to be played
*
* @return void
*
*/

void CSoundLibraryMixer::PlaySong(int index, float volume){
    if(no_audio)
        return;
    if((0 > index)||(index >= DMusicClips.size())){
        return;   
    }
    if(!DMusicClips[index].TotalFrames()){
        if(!DMusicClips[index].Load(DSoundDataContainer->DataSource(DMusicFilenames[index]), true)){
            PrintError("Failed to load song clip %d.\n", index);
            return;
        }
    }
    DMusicStatus.DOffset = 0;
    DMusicStatus.DIndex = index;
    DMusicStatus.DVolume = volume;
    DMusicStatus.DRightBias = 0.0;
}

/**
* Stops any song from playing
*
* @return void
*
*/

void CSoundLibraryMixer::StopSong(){
    if(no_audio) 
        return;
    DMusicStatus.DIndex = -1;
}

/**
* Changes the sound volume of the song
*
* @param[in] volume The new volume value
*
* @return void
*
*/

void CSoundLibraryMixer::SongVolume(float volume){
    if(no_audio) 
        return;
    DMusicStatus.DVolume = volume;
}

