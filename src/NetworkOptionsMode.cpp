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
* @class CNetworkOptionsMode
*
* @brief This class provides text and functionality for the buttons present in the Network Options mode.
*       These buttons include:
*
*                              OK:- Executes NetworkOptionsUpdateButtonCallBack, which applies any changes
*                                   to Network Options.
*
*                          Cancel:- Executes OptionsButtonCallBack, which changes the application mode to
*                                   the options menu. (See COptionsMenu)
*
*                     User Name: :- Executes ValidHostnameCallback, which ensures that the given username is valid.
*
*              Remote Hostname:  :- Executes ValidHostnameCallback, which ensures that the given username is valid.
*                                   Also executes ValidPortNumberCallback, which ensures that a given port number is
*                                   valid.
*
* @author Greg
*
* @version 9.0
*
* @date 12/3/17
*
*/

#include "NetworkOptionsMode.h"
#include "MultiPlayerOptionsMenuMode.h"
#include "ApplicationData.h"
#include "OptionsMenuMode.h"
#include "GameSelectionMode.h"
#include "GameLobbyMode.h"
#include <fstream>
#include <iostream>
#include <regex>


// regex pattern for valid IP address

int CNetworkOptionsMode::DPasswordEntryIndex;
std::string CNetworkOptionsMode::DHiddenPassword = "";
std::shared_ptr< CNetworkOptionsMode > CNetworkOptionsMode::DNetworkOptionsModePointer;

/**
* Constructor for the CNetworkOptionsMode class.
*
* @param[in] key Empty struct, used for debugging.
*
*/

CNetworkOptionsMode::CNetworkOptionsMode(const SPrivateConstructorType &key){
    DTitle = "Join Game Options";
    DButtonTexts.push_back("OK");
    DButtonFunctions.push_back(NetworkOptionsUpdateButtonCallback);
    DButtonTexts.push_back("Cancel");
    DButtonFunctions.push_back(BackButtonCallback);
}

/**
* Checks if the given address is a valid IP address.
*
* @param[in] str The given address
*
* @return True if str is a valid IP address.
*
*/
bool CNetworkOptionsMode::ValidIPAddressCallback(const std::string &str){
    std::regex IPAddressTemplate("([0-9]?[0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.([0-9]?[0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.([0-9]?[0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.([0-9]?[0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");

    if(std::regex_match(str, IPAddressTemplate))
        return true;

    return false;
}


/**
* This function is called after the "Password:" buttons are pressed. It checks
* the given password, makes sure it has a valid length, and returns true if the
* password is valid.
*
* @param[in] str The password, represented by a string.
*
* @return True if the password is valid.
*
*/

bool CNetworkOptionsMode::ValidPasswordCallback(const std::string &str){
    // add character
    while(DHiddenPassword.length() < DNetworkOptionsModePointer->DEditText[DPasswordEntryIndex].length() && DNetworkOptionsModePointer->DEditText[DPasswordEntryIndex].length() != 0)
        DHiddenPassword += DNetworkOptionsModePointer->DEditText[DPasswordEntryIndex].back();
    // remove character
    while(DHiddenPassword.length() > DNetworkOptionsModePointer->DEditText[DPasswordEntryIndex].length() && !DHiddenPassword.empty())
        DHiddenPassword.pop_back();

    // mask password
    for(int i = 0; i < DNetworkOptionsModePointer->DEditText[DPasswordEntryIndex].length(); i++)
        DNetworkOptionsModePointer->DEditText[DPasswordEntryIndex][i] = '*';

    // empty password is invalid
    // if(DNetworkOptionsModePointer->DEditText[1].empty())
    //     return false;

    return true;
}

/**
* This function is called after the "User Name:" or "Remote Hostname:" buttons are pressed. It
* checks the given hostname, makes sure it has a valid length and uses valid characters, and
* returns true if the name is valid.
*
* @param[in] str The hostname, represented by a string.
*
* @return True if the hostname is valid.
*
*/

bool CNetworkOptionsMode::ValidHostnameCallback(const std::string &str){
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
* Checks if a given value is a valid port number.
*
* @param[in] str The value being checked.
*
* @return True if str is a valid port number.
*
*/
bool CNetworkOptionsMode::ValidPortNumberCallback(const std::string &str){
    try{
        int Port = std::stoi(str);
        if((1024 < Port)&&(65535 >= Port)){
            return str == std::to_string(Port);
        }
    }
    catch(std::exception &E){
        return false;
    }
    return false;
}

/**
* Provides functionality for the "OK" button, inputting user values for different fields, and giving dummy values for those that weren't updated.
*
* @param[in] context A pointer to CApplicationData.
*
* @return void
*
*/

void CNetworkOptionsMode::NetworkOptionsUpdateButtonCallback(std::shared_ptr< CApplicationData > context){
    for(int Index = 0; Index < DNetworkOptionsModePointer->DEditText.size(); Index++){
        if(!DNetworkOptionsModePointer->DEditValidationFunctions[Index](DNetworkOptionsModePointer->DEditText[Index])){
            return;
        }
    }

    if(context->DGameSessionType == CApplicationData::EGameSessionType::gstMultiPlayerLANClient){
        context->DLANUsername = DNetworkOptionsModePointer->DEditText[0];
        context->DLANPassword = DHiddenPassword;
        DHiddenPassword = "";
        context->DLANIPAddress = DNetworkOptionsModePointer->DEditText[2];
        context->DLANPort = std::stoi(DNetworkOptionsModePointer->DEditText[3]);
    }

    /*
     * Save Options
     */
//    std::ofstream output("./bin/data/opt/network.dat", std::ios::trunc);
//    output << "# LAN Username\n" << context->DLANUsername << "\n";
//    output << "# LAN IP\n" << context->DLANIPAddress << "\n";
//    output << "# LAN Port\n" << context->DLANPort << "\n";
//    output << "# Server Username\n" << context->DServerUsername << "\n";
//    output.close();
	context->DMultiplayerClient->joinGame([context](bool status)
			{
			if(status) {
			context->DMultiplayerClient->getPlayerInfo();
			context->DGameSessionType = CApplicationData::EGameSessionType::gstMultiPlayerServerClient;

			context->DPlayerColor = static_cast<EPlayerColor>(context->DMultiplayerClient->team);
			//Set map
			context->DSelectedMapIndex = CAssetDecoratedMap::FindMapIndex(context->DMultiplayerClient->mapName);
			context->DSelectedMap = CAssetDecoratedMap::DuplicateMap(context->DSelectedMapIndex, context->DLoadingPlayerColors);

			context->ChangeApplicationMode(CGameLobbyMode::Instance());
			return status;
			}
			return false;
			}, std::stoi(DNetworkOptionsModePointer->DEditText[0]),DHiddenPassword);

}

/**
* This function is called after the "cancel" button is pressed. Similar functionality to the "OK" button, as the application
* mode is changed to the options menu (see COptionsMenuMode), but no changes are performed or saved.
*
* @param[in] context A ptr with information about the current game state.
*
* @return void
*
*/

void CNetworkOptionsMode::BackButtonCallback(std::shared_ptr< CApplicationData > context){
    if(context->DGameSessionType == CApplicationData::gstMultiPlayerLANClient){
        context->DGameSessionType = CApplicationData::gstMultiPlayerLAN;
        context->ChangeApplicationMode(CMultiPlayerOptionsMenuMode::Instance());
    }
    else{
        context->DGameSessionType = CApplicationData::gstMultiPlayerServer;
        context->ChangeApplicationMode(CGameSelectionMode::Instance());
    }
}

/**
* Guarantees the existance of an instance of CNetworkOptionsMode.
*
* @return shared_ptr to the instance of CNetworkOptionsMode.
*
*/
std::shared_ptr< CApplicationMode > CNetworkOptionsMode::Instance(){
    if(DNetworkOptionsModePointer == nullptr){
        DNetworkOptionsModePointer = std::make_shared< CNetworkOptionsMode >(SPrivateConstructorType());
    }
    return DNetworkOptionsModePointer;
}

/**
* Provides starting values for the fields in the network options mode.
*
* @param[in] context A pointer to CApplicationData.
*
* @retun void 
*
*/

void CNetworkOptionsMode::InitializeChange(std::shared_ptr< CApplicationData > context){
    if(context->DGameSessionType == CApplicationData::EGameSessionType::gstMultiPlayerLANClient){
        DEditTitles.clear();
        DEditValidationFunctions.clear();
        DEditTitles.push_back("User Name:");
        DEditValidationFunctions.push_back(ValidHostnameCallback);
        DEditTitles.push_back("Password:");
        DEditValidationFunctions.push_back(ValidPasswordCallback);
        DEditTitles.push_back("IP Address:");
        DEditValidationFunctions.push_back(ValidIPAddressCallback);
        DEditTitles.push_back("Port:");
        DEditValidationFunctions.push_back(ValidPortNumberCallback);

        DPasswordEntryIndex = 1;
        DEditSelected = -1;
        DEditSelectedCharacter = -1;
        DButtonLocations.clear();
        DEditLocations.clear();
        DEditText.clear();

        DEditText.push_back(context->DLANUsername);
        DEditText.push_back(DHiddenPassword);
        DEditText.push_back(context->DLANIPAddress);
        DEditText.push_back(std::to_string(context->DLANPort));
    }

    else if(context->DGameType == CApplicationData::EGameType::gtPublic){
        DEditTitles.clear();
        DEditValidationFunctions.clear();
        DEditTitles.push_back("Password:");
        DEditValidationFunctions.push_back(ValidPasswordCallback);

        DPasswordEntryIndex = 0;
        DEditSelected = -1;
        DEditSelectedCharacter = -1;
        DButtonLocations.clear();
        DEditLocations.clear();
        DEditText.clear();

        DEditText.push_back("");
    }

    else if(context->DGameType == CApplicationData::EGameType::gtPrivate){
        DEditTitles.clear();
        DEditValidationFunctions.clear();
        DEditTitles.push_back("Game ID:");
        DEditValidationFunctions.push_back(ValidHostnameCallback);
        DEditTitles.push_back("Password:");
        DEditValidationFunctions.push_back(ValidPasswordCallback);

        DPasswordEntryIndex = 1;
        DEditSelected = -1;
        DEditSelectedCharacter = -1;
        DButtonLocations.clear();
        DEditLocations.clear();
        DEditText.clear();

        DEditText.push_back("");
        DEditText.push_back(DHiddenPassword);
    }
}
