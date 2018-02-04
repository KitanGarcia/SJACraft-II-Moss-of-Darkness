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
#include "InGameMenuMode.h"
#include "MainMenuMode.h"
#include "BattleMode.h"
#include "OptionsMenuMode.h"
#include "ApplicationData.h"
#include <fstream>
#include <iostream>

// static shared pointer to hold reference to an CInGameMenuMode object
std::shared_ptr< CApplicationMode > CInGameMenuMode::DInGameMenuModePointer;

/**
* A CInGameMenuMode object constructor
*
* @param[in] key SPrivateConstructorType key
*
*/

CInGameMenuMode::CInGameMenuMode(const SPrivateConstructorType &key){
    DTitle = "In-Game Menu";
    DButtonTexts.push_back("Resume");
    DButtonFunctions.push_back(BackButtonCallback);
    DButtonTexts.push_back("Save");
    DButtonFunctions.push_back(SaveButtonCallback);
    DButtonTexts.push_back("");
    DButtonFunctions.push_back(nullptr);
    DButtonTexts.push_back("Options");
    DButtonFunctions.push_back(OptionsMenuButtonCallback);
    DButtonTexts.push_back("Main Menu");
    DButtonFunctions.push_back(MainMenuButtonCallback);

}

/**
* A callback function for bringing up the OptionsMenu
*
* @param[in] context A shared pointer to a CApplicationData object
*
* @return void
*
*/

void CInGameMenuMode::OptionsMenuButtonCallback(std::shared_ptr< CApplicationData > context){
    context->ChangeApplicationMode(COptionsMenuMode::Instance());
}

/**
* A callback function for saving the game
*
* @param[in] context A shared pointer to a CApplicationData object
*
* @return void
*
*/

void CInGameMenuMode::SaveButtonCallback(std::shared_ptr< CApplicationData > context){
    std::ofstream save("SavedGame.txt");

    // save all player colors, used to recreate DGameModel
    for(int i = 0; i < to_underlying(EPlayerColor::Max); i++){
        save << "#Player Colors\n";
        save << to_underlying(context->DLoadingPlayerColors[i]) << std::endl;

        save << "#Player Types\n";
        save << to_underlying(context->DLoadingPlayerTypes[i]) << std::endl;
    }

    // DGameModel

    // std::shared_ptr< CAssetDecoratedMap > DActualMap;
    context->DGameModel->Map()->SaveMap(save);

    context->DGameModel->Player(context->DPlayerColor)->VisibilityMap()->SaveMap(save);

    context->DGameModel->SaveGrowthMap(save);

    // int DGameCycle;
    save << "#GameCycle\n";
    save << context->DGameModel->GameCycle() << "\n";

    // get actual number of assets in map
    save << "#AssetMap Size\n";
    save << GAssetIDMap.size() << std::endl;

    for(auto Asset : GAssetIDMap){
        // int DAssetID;
        save << "#Asset ID\n";
        save << Asset.second->AssetID() << std::endl;

        save << "#Color\n";
        save << to_underlying(Asset.second->Color()) << std::endl;

        // std::shared_ptr< CPlayerAssetType > DType;
        // NOTE: when loading, use CPlayerAssetType::DRegistry to fine the right type
        save << "#Type\n";
        save << Asset.second->AssetType()->Name() << std::endl;

        // int DCreationCycle;
        save << "#CreationCyCle\n";
        save << Asset.second->CreationCycle() << std::endl;

        // int DHitPoints;
        save << "#HitPoint\n";
        save << Asset.second->HitPoints() << std::endl;

        // int DGold;
        save << "#Gold\n";
        save << Asset.second->Gold() << std::endl;

        // int DLumber;
        save << "#Lumber\n";
        save << Asset.second->Lumber() << std::endl;

        // int DStone;
        save << "#Stone\n";
        save << Asset.second->Stone() << std::endl;

        // int DStep;
        save << "#Step\n";
        save << Asset.second->Step() << std::endl;


        // int DMoveRemainderX;
        save << "#Move Remainder X\n";
        save << Asset.second->MoveRemainderX() << std::endl;

        // int DMoveRemainderY;
        save << "#Move Remainder Y\n";
        save << Asset.second->MoveRemainderY() << std::endl;

        // CPixelPosition DPosition;
        // X
        save << "#TilePosition X\n";
        save << Asset.second->TilePositionX() << std::endl;
        // Y
        save << "#TilePosition Y\n";
        save << Asset.second->TilePositionY() << std::endl;

        // EDirection DDirection;
        save << "#Direction\n";
        save << to_underlying(Asset.second->Direction()) << std::endl;

        save << "#Capabilities size\n";

        auto Capabilities = Asset.second->AssetType()->Capabilities();
        save << Capabilities.size() << std::endl;

        save << "#Capabilities\n";
        for(int i = 0; i < Capabilities.size(); i++){
            save << to_underlying(Capabilities[i]) << std::endl;
        }

        Asset.second->SavePeasants(save);
    }

    // get max asset count
    save << "#AssetCount\n";
    save << GAssetIDCount << std::endl;

    //DPlayers
    for(auto PlayerColor : context->DLoadingPlayerColors){
        std::shared_ptr<CPlayerData> Player = context->DGameModel->Player(PlayerColor);

        save << "#AI\n";
        save << Player->IsAI() << std::endl;
        save << "#Gold\n";
        save << Player->Gold() << std::endl;
        save << "#Lumber\n";
        save << Player->Lumber() << std::endl;
        save << "#Stone\n";
        save << Player->Stone() << std::endl;

        // std::vector< bool > DUpgrades;
        save << "#Player Upgrades\n";
        for(int i = 0; i < to_underlying(EAssetCapabilityType::Max); i++){
            save << Player->HasUpgrade(static_cast<EAssetCapabilityType>(i)) << "\n";
        }

        // std::list< std::weak_ptr< CPlayerAsset > > DAssets;
        save << "#WeakAssetCount\n";
        save << Player->Assets().size() << std::endl;

        for(auto WeakAsset : Player->Assets()){
            auto Asset = WeakAsset.lock();

            // store ID for loading
            save << "#AssetID\n";
            save << Asset->AssetID() << '\n';

            Asset->SaveCommands(save, PlayerColor);
        }
    }

    // unit groups
    for(int i = SGUIKeyType::Key0; i <= SGUIKeyType::Key9; i++){
        save << "#Unit grouping" << i << " size\n";
        save << context->DGroupHotKeyMap[i].size() << std::endl;

        if(context->DGroupHotKeyMap[i].size()){
            for(auto Asset : context->DGroupHotKeyMap[i]){
                save << Asset.lock()->AssetID() << std::endl;
            }
        }
    }

    save.close();
}

/**
* A callback function for going back to the MainMenu
*
* @param context
*
* @return Nothing
*
*/
void CInGameMenuMode::MainMenuButtonCallback(std::shared_ptr< CApplicationData > context){
    // deletes the current battle
    CBattleMode::EndBattle();
    context->ChangeApplicationMode(CMainMenuMode::Instance());
}

/**
* A callback function for resuming a paused battle
*
* @param[in] context A shared pointer to a CApplicationData object
*
* @return void
*
*/

void CInGameMenuMode::BackButtonCallback(std::shared_ptr< CApplicationData > context){
    // NOTE: context->ChangeApplicationMode(CBattleMode::Instance()) will restart the battle
    context->DNextApplicationMode = CBattleMode::Instance();
}

/**
* Allows only one instance of the InGameMenuMode object throughout the program
*
* @return CApplicationMode A shared pointer to the CInGameMenuMode object
*
*/
std::shared_ptr< CApplicationMode > CInGameMenuMode::Instance(){
    if(DInGameMenuModePointer == nullptr){
        DInGameMenuModePointer = std::make_shared< CInGameMenuMode >(SPrivateConstructorType());
    }
    return DInGameMenuModePointer;
}
