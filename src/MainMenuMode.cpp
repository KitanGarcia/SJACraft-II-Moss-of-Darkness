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
* @class CMainMenuMode
*
* @brief  This class shows the logic for the "Main Menu" mode.
*    It generates labelling for the buttons present in the main menu, as well
*    as functionality for said buttons.
*
*    List of buttons:
*
*    "Single Player Game" :- When pressed, the mode switches to Map Selection.
*
*    "Multi Player Game"  :- When pressed, the mode switches to the Multiplayer Options Menu.
*
*    "Options"            :- When pressed, the mode switches to the Options Menu Mode.
*
*    "Load"               :- When pressed, loads previously saved game.
*
*    "Exit Game"          :- When pressed, the main window is closed (exiting the game).
*
*
* @author Greg
*
* @version 9.0
*
* @date 12/3/17
*
* Contact: gjdost@ucdavis.edu
*
*/

#include "MainMenuMode.h"
#include "OptionsMenuMode.h"
#include "BattleMode.h"
#include "MultiPlayerOptionsMenuMode.h"
#include "ConnectionSelectionMenuMode.h"
#include "MapSelectionMode.h"
#include "ApplicationData.h"
#include "MemoryDataSource.h"
#include "FileDataContainer.h"
#include "Tokenizer.h"
#include <iostream>
#include <string>

std::shared_ptr< CApplicationMode > CMainMenuMode::DMainMenuModePointer;

/**
* The constructor for the CMainMenuMode class.
*
* @param[in] key Empty struct, used for debugging.
*
*/

CMainMenuMode::CMainMenuMode(const SPrivateConstructorType &key){
    DTitle = "The Game";

    //DButtonTexts.push_back creates a new button and gives it a label

    DButtonTexts.push_back("Single Player Game");

    //DButtonFunctions.push_back gives the most recent button functionality
    //In this case, when the "Single Player Game" button is pressed, the
    //SinglePlayerGameButtonCallback function is activated

    DButtonFunctions.push_back(SinglePlayerGameButtonCallback);
    DButtonTexts.push_back("Multi Player Game");
    DButtonFunctions.push_back(MultiPlayerGameButtonCallback);
    DButtonTexts.push_back("Options");
    DButtonFunctions.push_back(OptionsButtonCallback);
    DButtonTexts.push_back("Load");
    DButtonFunctions.push_back(LoadButtonCallback);
    DButtonTexts.push_back("Exit Game");
    DButtonFunctions.push_back(ExitGameButtonCallback);
}

/**
* When the "Single Player Game" button is pressed, change the gametype to Map
* Selection mode (see CMapSelectionMode).
*
* @param[in] context A pointer to CApplicationData.
*
* @return void
*
*/

void CMainMenuMode::SinglePlayerGameButtonCallback(std::shared_ptr< CApplicationData > context){

    if(CApplicationData::DLoadedGame){
        // reset loaded game map
        CAssetDecoratedMap::ResetMap();
    }

    CApplicationData::DLoadedGame = false;

    context->DGameSessionType = CApplicationData::gstSinglePlayer;

    context->ChangeApplicationMode(CMapSelectionMode::Instance());
}

/**
* When the "Multi Player Game" button is pressed, change the gametype to the
* Multiplayer Options Menu Mode (see CMultiPlayerOptionsMenuMode).
*
* @param[in] context A pointer to CApplicationData.
*
* @return void
*
*/

void CMainMenuMode::MultiPlayerGameButtonCallback(std::shared_ptr< CApplicationData > context){
    context->ChangeApplicationMode(CConnectionSelectionMenuMode::Instance());
}

/**
* When the "Options" button is pressed, change the gametype to the Options Menu
* (see COptionsMenuMode).
*
* @param[in] context The data for the game's current state.
*
* @return void
*
*/

void CMainMenuMode::OptionsButtonCallback(std::shared_ptr< CApplicationData > context){
    context->ChangeApplicationMode(COptionsMenuMode::Instance());
}

/**
* When the "Load" button is pressed, loads the saved game from SavedGame.txt.
*
* @param[in] context The data for the game's current state.
*
* @return void
*
*/

void CMainMenuMode::LoadButtonCallback(std::shared_ptr< CApplicationData > context){
    std::shared_ptr< CDirectoryDataContainer > CurrDir = std::make_shared< CDirectoryDataContainer > (".");
    std::shared_ptr< CDataSource > source = CurrDir->DataSource("SavedGame.txt");
    CCommentSkipLineDataSource LineSource(source, '#');
    std::string Value;
    std::vector< std::string > Tokens;

    // if save file does not exist
    if(source == nullptr){
        return;
    }

    CApplicationData::DLoadedGame = true;

    context->DGameSessionType = CApplicationData::gstSinglePlayer;

    for(int i = 0; i < to_underlying(EPlayerColor::Max); i++){
        LineSource.Read(Value);
        context->DLoadingPlayerColors[i] = static_cast<EPlayerColor>(std::stoi(Value));

        LineSource.Read(Value);
        context->DLoadingPlayerTypes[i] = static_cast<CApplicationData::EPlayerType>(std::stoi(Value));
    }

    // set player 1 color
    context->DPlayerColor = context->DLoadingPlayerColors[1];

    // get MapName
    LineSource.Read(Value);

    // get index of the map with same map name
    context->DSelectedMapIndex = CAssetDecoratedMap::FindMapIndex(Value);

    std::shared_ptr< CAssetDecoratedMap > TempMap = std::make_shared< CAssetDecoratedMap >();

    // read map, map partial bits, and map indices
    TempMap->RestoreMap(source);

    // replace map, map partial bits, and map indices while keeping a backup
    CAssetDecoratedMap::ReplaceMap(TempMap, context->DSelectedMapIndex);

    context->DSelectedMap = CAssetDecoratedMap::DuplicateMap(context->DSelectedMapIndex, context->DLoadingPlayerColors);

    context->LoadGameMap(context->DSelectedMapIndex, source);
    context->DSoundLibraryMixer->PlaySong(context->DSoundLibraryMixer->FindSong("game1"), context->DMusicVolume);

    context->DGameModel->LoadGrowthMap(source);
    // // then read in assets

    // int DGameCycle;
    LineSource.Read(Value);
    context->DGameModel->GameCycle(std::stoi(Value));
    // std::cout << "GameCycle: " << std::stoi(Value) << std::endl;

    // get asset map size first
    LineSource.Read(Value);
    // std::cout << "AssetMapSize: " << std::stoi(Value) << std::endl;

    int AssetMapSize = std::stoi(Value);

    for(int i = 0; i < AssetMapSize; i++){
         // int DAssetID;
        // save << Asset.second->AssetID() << std::endl;
        LineSource.Read(Value);
        // std::cout << "AssetID: " << std::stoi(Value) << std::endl;

        // update 12/3/17
        GAssetIDCount = std::stoi(Value);

        LineSource.Read(Value);
        // std::cout << "Color: " << Value << std::endl;
        auto Player = context->DGameModel->Player(static_cast<EPlayerColor>(std::stoi(Value)));

        // get type name
        LineSource.Read(Value);
        // std::cout << "Type: " << Value << std::endl;


        // std::shared_ptr< CPlayerAssetType > DType;
        // NOTE: when loading, use CPlayerAssetType::DRegistry to fine the right type
        auto Asset = Player->CreateAsset(Value);

        // get creation cycle
        LineSource.Read(Value);
        // std::cout << "CreationCycle: " << std::stoi(Value) << std::endl;
        Asset->CreationCycle(std::stoi(Value));

        // get hit points
        LineSource.Read(Value);
        // std::cout << "HitPoints: " << std::stoi(Value) << std::endl;
        Asset->HitPoints(std::stoi(Value));

        // get gold
        LineSource.Read(Value);
        // std::cout << "Gold: " << std::stoi(Value) << std::endl;
        Asset->Gold(std::stoi(Value));

        // get lumber
        LineSource.Read(Value);
        // std::cout << "Lumber: " << std::stoi(Value) << std::endl;
        Asset->Lumber(std::stoi(Value));

        // get stone
        LineSource.Read(Value);
        // std::cout << "Stone: " << std::stoi(Value) << std::endl;
        Asset->Stone(std::stoi(Value));

        // NOTE: no setter for this, is it necessary?
        // get step
        LineSource.Read(Value);
        // std::cout << "Step: " << std::stoi(Value) << std::endl;
        Asset->Step(std::stoi(Value));

        // ???
        // int DMoveRemainderX;
        LineSource.Read(Value);
        // std::cout << "Move Remainder X: " << std::stoi(Value) << std::endl;
        Asset->MoveRemainderX(std::stoi(Value));

        // int DMoveRemainderY;
        LineSource.Read(Value);
        // std::cout << "Move Remainder Y: " << std::stoi(Value) << std::endl;
        Asset->MoveRemainderY(std::stoi(Value));

        // get X
        LineSource.Read(Value);
        // std::cout << "TilePositionX: " << std::stoi(Value) << std::endl;
        Asset->TilePositionX(std::stoi(Value));

        // get Y
        LineSource.Read(Value);
        // std::cout << "TilePositionY: " << std::stoi(Value) << std::endl;
        Asset->TilePositionY(std::stoi(Value));

        // get direction
        LineSource.Read(Value);
        // std::cout << "Direction: " << std::stoi(Value) << std::endl;
        Asset->Direction(static_cast<EDirection>(std::stoi(Value)));

        // restore asset capabilities
        // 0 out all the capabilities first
        for(int j = 0; j < to_underlying(EAssetCapabilityType::Max); j++){
            Asset->AssetType()->RemoveCapability(static_cast<EAssetCapabilityType>(j));
        }

        // get capacity size
        LineSource.Read(Value);

        int CapabilityCount = std::stoi(Value);

        for(int j = 0; j < CapabilityCount; j++){
            LineSource.Read(Value);
            Asset->AssetType()->AddCapability(static_cast<EAssetCapabilityType>(std::stoi(Value)));
        }

        // get sheltered peasant count
        LineSource.Read(Value);

        int PeasantCount = std::stoi(Value);

        for(int j = 0; j < PeasantCount; j++){
            LineSource.Read(Value);
            Asset->PushPeasant(std::stoi(Value));
            Asset->TakeSpace();
        }
    }

    // get max asset count
    LineSource.Read(Value);
    // std::cout << "AssetCount: " << std::stoi(Value) << std::endl;

    GAssetIDCount = std::stoi(Value);

    //DPlayers
    for(auto PlayerColor : context->DLoadingPlayerColors){
        std::shared_ptr<CPlayerData> Player = context->DGameModel->Player(PlayerColor);

        LineSource.Read(Value);
        // std::cout << "AI: " << std::stoi(Value) << std::endl;
        Player->IsAI(std::stoi(Value));

        LineSource.Read(Value);
        // std::cout << "Gold: " << std::stoi(Value) << std::endl;
        Player->IncrementGold(std::stoi(Value) - Player->Gold());

        LineSource.Read(Value);
        // std::cout << "Lumber: " << std::stoi(Value) << std::endl;
        Player->IncrementLumber(std::stoi(Value)- Player->Lumber());

        LineSource.Read(Value);
        // std::cout << "Stone: " << std::stoi(Value) << std::endl;
        Player->IncrementStone(std::stoi(Value)- Player->Stone());

        // set player game cycle
        Player->GameCycle(context->DGameModel->GameCycle());

        // std::cout << "#Player Upgrades\n";
        for(int i = 0; i < to_underlying(EAssetCapabilityType::Max); i++){
            LineSource.Read(Value);

            if(std::stoi(Value) == 1){
                std::string Upgrade = CPlayerUpgrade::FindUpgradeFromType(static_cast<EAssetCapabilityType>(i))->Name();
                Player->AddUpgrade(Upgrade);
            }
        }

        // / get player asset size
        LineSource.Read(Value);
        int AssetCount = std::stoi(Value);
        // std::cout << "WeakAssetCount: " << AssetCount << std::endl;

        for(int i = 0; i < AssetCount; i++){
            LineSource.Read(Value);

            auto Asset = FindAssetObj(std::stoi(Value));

            Asset->LoadCommands(source, context->DGameModel->Players());
        }
    }

    // unit groups
    for(int i = SGUIKeyType::Key0; i <= SGUIKeyType::Key9; i++){
        LineSource.Read(Value);
        int UnitGroupSize = std::stoi(Value);

        for(int Index = 0; Index < UnitGroupSize; Index++){
            LineSource.Read(Value);

            auto WeakAsset = std::weak_ptr<CPlayerAsset>(FindAssetObj(std::stoi(Value)));

            context->DGroupHotKeyMap[i].push_back(WeakAsset);

        }
    }

    std::cout << "Good\n";

    // replace visibility map
    // context->DFogRenderer->ReplaceVisibilityMap(context->DGameModel->Player(context->DPlayerColor)->VisibilityMap());

    // center viewport to a moving unit
    bool ViewportCentered = false;
    for(auto WeakAsset : context->DGameModel->Player(context->DPlayerColor)->Assets()){
        if(auto Asset = WeakAsset.lock()){
            if(Asset->Speed() > 0){
                context->DViewportRenderer->CenterViewport(Asset->Position());
                ViewportCentered = true;
                break;
            }
        }
    }

    // else center viewport to the first asset
    if(ViewportCentered == false){
        for(auto WeakAsset : context->DGameModel->Player(context->DPlayerColor)->Assets()){
            if(auto Asset = WeakAsset.lock()){
                context->DViewportRenderer->CenterViewport(Asset->Position());
                ViewportCentered = true;
                break;
            }
        }
    }

// visibility map

    context->DNextApplicationMode = CBattleMode::Instance();
}


/**
* When the "Exit Game" button is pressed, close the game window.
*
* @param[in] context The data for the game's current state.
*
* @return void
*
*/

void CMainMenuMode::ExitGameButtonCallback(std::shared_ptr< CApplicationData > context){
    context->DMainWindow->Close();
}

/**
* Creates an instance of the CApplicationMode and returns a shared_ptr to it or
* just returns a shared_ptr to the current instance of the game.
* This function only allows for one instance of CApplicationMode unless
* constructor is called directly.
*
*
* @return shared_ptr to application context.
*
*/

std::shared_ptr< CApplicationMode > CMainMenuMode::Instance(){
    if(DMainMenuModePointer == nullptr){
        DMainMenuModePointer = std::make_shared< CMainMenuMode >(SPrivateConstructorType());
    }
    return DMainMenuModePointer;
}
