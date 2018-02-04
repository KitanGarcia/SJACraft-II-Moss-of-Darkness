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
* @class CDebug
*
* @brief This class creates and deallocates the DebugFile
*
* @author Nishant
*
* @version 9.0
*
* @date 12/3/17
*
* Contact: nchandrashekar@ucdavis.edu
*
*/

#include "Debug.h"

#ifdef DEBUG


int CDebug::DDebugLevel  = 4;
FILE *CDebug::DDebugFile = nullptr;
std::shared_ptr< CDebug > CDebug::DDebugPointer;

/**
* Constructor
*
* @param[in] key A reference to SPrivateDebugType
*
*/

CDebug::CDebug(const SPrivateDebugType &key){

}

/**
* Destructor (deallocate everything)
*
*/

CDebug::~CDebug(){
    if(DDebugFile){
        fclose(DDebugFile);
        DDebugFile = nullptr;
        DDebugLevel = 0;
    }
}

/**
* Create Debug File
*
* @param[in] filename a reference to the file name
* @param[in] level current Debuglevel
*
* bool True if file is created
*/

bool CDebug::CreateDebugFile(const std::string &filename, int level){
    if(DDebugFile){
        return false;
    }
    if(DDebugPointer){
        return false;
    }
    if(0 > DDebugLevel){
        return false;
    }
    DDebugFile = fopen(filename.c_str(), "w");
    if(nullptr == DDebugFile){
        return false;
    }
    DDebugPointer = std::make_shared< CDebug > (SPrivateDebugType{});
    DDebugLevel = level + 1;

    return true;
}

#endif
