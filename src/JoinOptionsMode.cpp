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

#include "JoinOptionsMode.h"
#include "ApplicationData.h"
#include "ConnectionSelectionMenuMode.h"
#include "GameSelectionMode.h"
#include <fstream>
#include <iostream>

std::string CJoinOptionsMode::DHiddenPassword = "";
std::shared_ptr< CJoinOptionsMode > CJoinOptionsMode::DJoinOptionsModePointer;

CJoinOptionsMode::CJoinOptionsMode(const SPrivateConstructorType &key){
    DTitle = "Join Options";
    DButtonTexts.push_back("Join Game");
    DButtonFunctions.push_back(JoinOptionsUpdateButtonCallback);
    //DButtonTexts.push_back("Register");
    //DButtonFunctions.push_back(RegisterButtonCallback);
    DButtonTexts.push_back("Cancel");
    DButtonFunctions.push_back(BackButtonCallback);

    DEditTitles.push_back("Game ID:");
    DEditValidationFunctions.push_back(ValidGameIDCallback);
    DEditTitles.push_back("Password:");
    DEditValidationFunctions.push_back(ValidPasswordCallback);
}



/**
*
*This function is called after the "User Name:" or "Remote Hostname:" buttons are pressed. It
*checks the given hostname, makes sure it has a valid length and uses valid characters, and
*returns true if the name is valid.
*
*@param str The hostname, represented by a string.
*
*@return bool True if the hostname is valid, otherwise false.
*
*/
bool CJoinOptionsMode::ValidGameIDCallback(const std::string &str){
    int CharSinceDot = 0;

    if(253 < str.length()){
        return false;
    }
    if(0 == str.length()){
        return false;
    }
    for(auto Char : str){
        if('.' == Char){
            if(0 == CharSinceDot){
                return false;
            }
            CharSinceDot = 0;
        }
        else{
            CharSinceDot++;
            if(63 < CharSinceDot){
                return false;
            }
            if(('-' != Char)&&(!(('0' <= Char)&&('9' >= Char)))&&(!(('a' <= Char)&&('z' >= Char)))&&(!(('A' <= Char)&&('Z' >= Char)))){
                return false;
            }
        }
    }
    return true;
}


/**
*
*This function is called after the "Password:" buttons are pressed. It checks
*the given password, makes sure it has a valid length, and returns true if the
*password is valid.
*
*@param str The password, represented by a string.
*
*@return bool True if the password is valid, otherwise false.
*
*/
bool CJoinOptionsMode::ValidPasswordCallback(const std::string &str){
    // add character
    while(DHiddenPassword.length() < DJoinOptionsModePointer->DEditText[1].length() && DJoinOptionsModePointer->DEditText[1].length() != 0)
        DHiddenPassword += DJoinOptionsModePointer->DEditText[1].back();
    // remove character
    while(DHiddenPassword.length() > DJoinOptionsModePointer->DEditText[1].length() && !DHiddenPassword.empty())
        DHiddenPassword.pop_back();

    // mask password
    for(int i = 0; i < DJoinOptionsModePointer->DEditText[1].length(); i++)
        DJoinOptionsModePointer->DEditText[1][i] = '*';

    // empty password is invalid
    if(DJoinOptionsModePointer->DEditText[1].empty())
        return false;

    return true;
}


/**
*
*This function is called after the "OK" button is pressed. Any changes made to settings are updated and saved, and the application mode is changed
*to the options menu (See COptionsMenuMode).
*
*@param context A ptr with information about the current game state.
*
*
*@return Nothing.
*
*/
void CJoinOptionsMode::JoinOptionsUpdateButtonCallback(std::shared_ptr< CApplicationData > context){
	//context->DUsername = DLogInOptionsModePointer->DEditText[0];
	context->DMultiplayerClient->joinGame([context](bool status)
			{
			if(status) {
			std::cout << "Trying to join Game\n";
			context->DMultiplayerClient->getPlayerInfo();
			context->DGameSessionType = CApplicationData::gstMultiPlayerClient;
			//Set map
			context->DSelectedMapIndex = CAssetDecoratedMap::FindMapIndex(context->DMultiplayerClient->mapName);
			*context->DSelectedMap = *CAssetDecoratedMap::GetMap(context->DSelectedMapIndex);
			context->DPlayerColor = static_cast<EPlayerColor>(context->DMultiplayerClient->team);

			context->ChangeApplicationMode(CGameSelectionMode::Instance());
			return status;
			}
			return false;
			}, std::stoi(DJoinOptionsModePointer->DEditText[0]),DHiddenPassword);

    // for(int Index = 0; Index < DLogInOptionsModePointer->DEditText.size(); Index++){
    //     if(!DLogInOptionsModePointer->DEditValidationFunctions[Index](DLogInOptionsModePointer->DEditText[Index])){
    //         return;
    //     }
    // }
    // context->DUsername = DLogInOptionsModePointer->DEditText[0];
    // context->DPassword = DHiddenPassword;
    // DHiddenPassword = "";
    // context->DRemoteHostname = DLogInOptionsModePointer->DEditText[2];
    // context->DMultiplayerPort = std::stoi(DLogInOptionsModePointer->DEditText[3]);

    // /*
    //  * Save Options
    //  */
    // std::ofstream output("./bin/data/opt/LogIn.dat", std::ios::trunc);
    // output << "# Username\n" << context->DUsername << "\n";
    // output << "# RemoteHostName\n" << context->DRemoteHostname  << "\n";
    // output << "# Multiplayer Port\n" << context->DMultiplayerPort << "\n";
    // output << "# Password\n" << context->DPassword << "\n";
    // output.close();

    // context->ChangeApplicationMode(COptionsMenuMode::Instance());
    // int ErrorCode = user_auth(context->DUsername, context->DPassword);

    // if(ErrorCode == 0)
    // else
        // Error(ErrorCode);
    // error screen to be added
}

/**
*
*This function is called after the "cancel" button is pressed. Similar functionality to the "OK" button, as the application
*mode is changed to the options menu (see COptionsMenuMode), but no changes are performed or saved.
*
*@param context A ptr with information about the current game state.
*
*@return Nothing.
*
*/
void CJoinOptionsMode::BackButtonCallback(std::shared_ptr< CApplicationData > context){
    context->ChangeApplicationMode(CGameLobbyMode::Instance());
}

/**
*Creates an instance of the CApplicationMode and returns a shared_ptr to it or
*just returns a shared_ptr to the current instance of the game.
*This function only allows for one instance of CApplicationMode unless
*constructor is called directly.
*
*@param[in] none.
*
*@return shared_ptr to application context.
*
*/
std::shared_ptr< CApplicationMode > CJoinOptionsMode::Instance(){
    if(DJoinOptionsModePointer == nullptr){
        DJoinOptionsModePointer = std::make_shared< CJoinOptionsMode >(SPrivateConstructorType());
    }
    return DJoinOptionsModePointer;
}

/**
*
*This function gives initial values for all of the possible settings.
*
*@param context A ptr with information about the current game state.
*
*@return Nothing
*
*/
void CJoinOptionsMode::InitializeChange(std::shared_ptr< CApplicationData > context){
    DHiddenPassword = context->DPassword;
    DEditSelected = -1;
    DEditSelectedCharacter = -1;
    DButtonLocations.clear();
    DEditLocations.clear();
    DEditText.clear();
    // DEditText.push_back(context->DUsername);
    DEditText.push_back("");
    // DEditText.push_back(DHiddenPassword);
    DEditText.push_back("");
	// Connect via Multiplayer Server IP Address
}

