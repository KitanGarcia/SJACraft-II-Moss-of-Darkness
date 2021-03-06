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
* @class COptionsMenuMode
*
* @brief
*   This class handles the options menu. It includes the title and 3 buttons: "Sound Options," "Network Options," and
*   "Back." Depending on where which the button the user selects, the mode will change to CSoundOptionsMode,
*   CNetworkOptionsMode, and CMainMenuMode, respectively.
*
* @version 9.0
*
* @author Kitan Garcia
*
* @date 12/3/17
*
* Contact: kitgarcia@ucdavis.edu
*
*/

#include "OptionsMenuMode.h"
#include "MainMenuMode.h"
#include "BattleMode.h"
#include "InGameMenuMode.h"
#include "SoundOptionsMode.h"
#include "NetworkOptionsMode.h"
#include "ApplicationData.h"

std::shared_ptr< CApplicationMode > COptionsMenuMode::DOptionsMenuModePointer;



/**
* Constructor. Options for Options Menu. Changes modes to SoundOptionsMode, NetworkOptionsMode, or MainMenuMode
*
* @param[in] key ConstructorType
*
*/

COptionsMenuMode::COptionsMenuMode(const SPrivateConstructorType &key){
    DTitle = "Options";
    DButtonTexts.push_back("Sound Options");
    DButtonFunctions.push_back(SoundOptionsButtonCallback);
    // DButtonTexts.push_back("Network Options");
    // DButtonFunctions.push_back(NetworkOptionsButtonCallback);
    DButtonTexts.push_back("");
    DButtonFunctions.push_back(nullptr);
    DButtonTexts.push_back("Back");
    DButtonFunctions.push_back(BackButtonCallback);
}

/**
* Changes mode to SoundOptionsMode
*
* @param[in] context The shared pointer to CApplicationData
*
* @return void
*
*/

void COptionsMenuMode::SoundOptionsButtonCallback(std::shared_ptr< CApplicationData > context){
    context->ChangeApplicationMode(CSoundOptionsMode::Instance());
}

// void COptionsMenuMode::NetworkOptionsButtonCallback(std::shared_ptr< CApplicationData > context){
//     context->ChangeApplicationMode(CNetworkOptionsMode::Instance());
// }

/**
* Changes mode to MainMenuMode
*
* @param[in] context Shared pointer to CApplicationData
*
* @return void
*
*/

void COptionsMenuMode::MainMenuButtonCallback(std::shared_ptr< CApplicationData > context){
    context->ChangeApplicationMode(CMainMenuMode::Instance());
}

/**
* Changes mode to InGameMainMenuMode if BattleMode is active or MainMenuMode otherwise
*
* @param[in] context Shared pointer to CApplicationData
*
* @return void
*
*/

void COptionsMenuMode::BackButtonCallback(std::shared_ptr< CApplicationData > context){
    if(CBattleMode::IsActive())
        context->ChangeApplicationMode(CInGameMenuMode::Instance());
    else
        context->ChangeApplicationMode(CMainMenuMode::Instance());
}

/**
* Creates a shared pointer to COptionsMenuMode if it doesn't already exist
*
* @return A shared pointer to COptionsMenuMode
*
*/

std::shared_ptr< CApplicationMode > COptionsMenuMode::Instance(){
    if(DOptionsMenuModePointer == nullptr){
        DOptionsMenuModePointer = std::make_shared< COptionsMenuMode >(SPrivateConstructorType());
    }
    return DOptionsMenuModePointer;
}


