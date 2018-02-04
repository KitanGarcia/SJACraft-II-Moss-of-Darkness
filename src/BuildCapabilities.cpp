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
#include "GameModel.h"
#include "ApplicationData.h"
#include "Debug.h"

// Build normal buildings capability

/**
*
* @class CPlayerCapabilityMove
*
* @ brief This class was written to give player building capability \n
*
*    The class contains two other classes which are CRegistrant and CActivatedCapability.
*    They are responsible for determining the building which should build and activation of
*    capability. The class also keep tracks of percentge compeltion of the capability
*    the steps and cancellation of capability.
*
* @author Seza
*
* @version 9.0
*
* @date 12/3/17
*
* Contact: shabibi@ucdavis.edu
*
*/

class CPlayerCapabilityBuildNormal : public CPlayerCapability{
    protected:

      /**
      *
      *@class CRegistrant
      *
      * @brief the class was written to determine the building which should build
      *
      * @author Seza
      *
      * @version 9.0
      *
      * @date 12/3/17
      *
      * Contact: shabibi@ucdavis.edu
      *
      */

        class CRegistrant{
            public:
                CRegistrant();
        };
        static CRegistrant DRegistrant;

        /**
        *
        *@class CActivatedCapability
        *
        * @brief the class was written to activate building capability
        *
        *      The class is responsible for showing the percentage completion until
        *      the player get the capability,following steps to get capability, and
        *      cancellation of capability.
        *
        * @author Seza
        *
        * @version 9.0
        *
        * @date 12/3/17
        *
        * Contact: shabibi@ucdavis.edu
        *
        */

        class CActivatedCapability : public CActivatedPlayerCapability{
            protected:
                int DCurrentStep;
                int DTotalSteps;
                int DLumber;
                int DGold;
                int DStone;
                std::weak_ptr< CPlayerAsset > DGoldVein;

            public:
                CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, std::shared_ptr< CPlayerAsset > goldvein, int lumber, int gold, int stone, int steps);
                virtual ~CActivatedCapability(){};

                int PercentComplete(int max);
                bool IncrementStep();
                void Cancel();
                void Save(std::ofstream& save);
                void Step(int step);
        };
        std::string DBuildingName;
        CPlayerCapabilityBuildNormal(const std::string &buildingname);

    public:
        virtual ~CPlayerCapabilityBuildNormal(){};

        bool CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message);
        bool CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
        bool ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);

};

CPlayerCapabilityBuildNormal::CRegistrant CPlayerCapabilityBuildNormal::DRegistrant;

CPlayerCapabilityBuildNormal::CRegistrant::CRegistrant(){
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildNormal >(new CPlayerCapabilityBuildNormal("TownHall")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildNormal >(new CPlayerCapabilityBuildNormal("Farm")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildNormal >(new CPlayerCapabilityBuildNormal("Barracks")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildNormal >(new CPlayerCapabilityBuildNormal("LumberMill")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildNormal >(new CPlayerCapabilityBuildNormal("Blacksmith")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildNormal >(new CPlayerCapabilityBuildNormal("ScoutTower")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildNormal >(new CPlayerCapabilityBuildNormal("Wall")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildNormal >(new CPlayerCapabilityBuildNormal("GoldMine")));

}

/**
* Set DBuildingName to input
*
* @param[out] buildingname A string reference to hold building name
*
*/

CPlayerCapabilityBuildNormal::CPlayerCapabilityBuildNormal(const std::string &buildingname) : CPlayerCapability(std::string("Build") + buildingname, ETargetType::TerrainOrAsset){
    DBuildingName = buildingname;
}

/**
* It will check if the building can be initiated. condition:
*     LumberCost must be smaller than Lumber
*     GoldCost must be smaller than Gold
*
* @param[in] actor share pointer to the class of PlayerAsset.
* @param[in] playerdata  share pointer to the class of PlayerData
*
* @return True if The building does not exist in player's assets
*
*/

bool CPlayerCapabilityBuildNormal::CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message){
    bool ReturnStatus = true;
    auto Iterator = playerdata->AssetTypes()->find(DBuildingName);
    if(Iterator != playerdata->AssetTypes()->end()){
        auto AssetType = Iterator->second;
        if(AssetType->LumberCost() > playerdata->Lumber()){
            message += "Lumber:";
            message += std::to_string(AssetType->LumberCost());
            message += " ";
            ReturnStatus = false;
        }
        if(AssetType->GoldCost() > playerdata->Gold()){
            message += "Gold:";
            message += std::to_string(AssetType->GoldCost());
            message += " ";
            ReturnStatus = false;
        }
        if(AssetType->StoneCost() > playerdata->Stone()){
            message += "Stone:";
            message += std::to_string(AssetType->StoneCost());
            message += " ";
            ReturnStatus = false;
        }
    }

    return ReturnStatus;
}

/**
* It will check if the building capability can be applied. condition:
*     actor must equal target and AssetTypes should equal Target type
*     LumberCost must be smaller than Lumber
*     GoldCost must be smaller than Gold
*     CanPlaceAsset function should return true
*
* @param[in] actor share pointer to the class of PlayerAsset.
* @param[in] playerdata shared pointer to the class of PlayerData
* @param[in] target shared pointer to the class of PlayerAsset
*
* @return True if the building does not exist in player's assets
*
*/

bool CPlayerCapabilityBuildNormal::CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    auto Iterator = playerdata->AssetTypes()->find(DBuildingName);
    if(target->Type() == EAssetType::GoldVein && DBuildingName == "GoldMine"){
        ;// Do nothing, check resources
    }
    else if(DBuildingName == "GoldMine"){
        return false;    
    }
    else if((actor != target)&&(EAssetType::None != target->Type())){
        return false;
    }
    if(Iterator != playerdata->AssetTypes()->end()){
        auto AssetType = Iterator->second;

        if(AssetType->LumberCost() > playerdata->Lumber()){
            return false;
        }
        if(AssetType->GoldCost() > playerdata->Gold()){
            return false;
        }
        if(AssetType->StoneCost() > playerdata->Stone()){
            return false;
        }
        if(!playerdata->PlayerMap()->CanPlaceAsset(target->TilePosition(), AssetType->Size(), actor)){
            return false;
        }
    }

    return true;
}

/**
* Set a new building capability. using a struct to define a new command and set it\n
* to NewAsset and get the position to put it in that Position
*
* @param[in] actor share pointer to the class of PlayerAsset.
* @param[in] playerdata shared pointer to the class of PlayerData
* @param[in] target shared pointer to the class of PlayerAsset
*
* @return True if Building is in the player's assets
*
*/

bool CPlayerCapabilityBuildNormal::ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    int StartingGold = 45000;
    auto Iterator = playerdata->AssetTypes()->find(DBuildingName);
    CPixelPosition ClosestPosition = target->ClosestPosition(actor->Position());
    CTilePosition TilePosition;
    EDirection BuildDirection;

    TilePosition.SetFromPixel(ClosestPosition);
    BuildDirection = actor->TilePosition().AdjacentTileDirection(TilePosition);

    if(Iterator != playerdata->AssetTypes()->end()){
        SAssetCommand NewCommand;

        actor->ClearCommand();
        if(actor->TilePosition() == target->TilePosition()){
            auto AssetType = Iterator->second;
            std::shared_ptr< CPlayerAsset > NewAsset;
            if(CPlayerCapability::NameToType("Build" + DBuildingName) == EAssetCapabilityType::BuildWall || CPlayerCapability::NameToType("Build" + DBuildingName) == EAssetCapabilityType::BuildGoldMine){
                NewAsset = CApplicationData::Instance("")->DGameModel->Player(EPlayerColor::None)->CreateAsset(DBuildingName);
            }
            else{
                NewAsset = playerdata->CreateAsset(DBuildingName);
            }
            CTilePosition TilePosition;
            TilePosition.SetFromPixel(target->Position());
            NewAsset->TilePosition(TilePosition);
            NewAsset->HitPoints(1);

            // if you are building a gold mine, initialize the starting gold, set player color to none, and add asset to player 0 asset list
            if(EAssetType::GoldMine == NewAsset->Type()) {
                NewAsset->Gold(StartingGold);
            }

            NewCommand.DAction = EAssetAction::Capability;
            NewCommand.DCapability = AssetCapabilityType();
            NewCommand.DAssetTarget = NewAsset;
            NewCommand.DActivatedCapability = std::make_shared< CActivatedCapability >(actor, playerdata, NewAsset, nullptr, AssetType->LumberCost(), AssetType->GoldCost(), AssetType->StoneCost(), CPlayerAsset::UpdateFrequency() * AssetType->BuildTime());
            actor->PushCommand(NewCommand);
        }
        else if("GoldMine" == DBuildingName) {
            CPixelPosition ClosestPosition = target->ClosestPosition(actor->Position());
            auto AssetType = Iterator->second;
            std::shared_ptr< CPlayerAsset > NewAsset;
      
            TilePosition.SetFromPixel(ClosestPosition);
            BuildDirection = actor->TilePosition().AdjacentTileDirection(TilePosition);
      
      
            SAssetCommand NewCommand;
      
            if(/*CPlayerCapability::NameToType("Build" + DBuildingName) == EAssetCapabilityType::BuildWall || */ CPlayerCapability::NameToType("Build" + DBuildingName) == EAssetCapabilityType::BuildGoldMine){
                NewAsset = CApplicationData::Instance("")->DGameModel->Player(EPlayerColor::None)->CreateAsset(DBuildingName);
            }
            else{
                NewAsset = playerdata->CreateAsset(DBuildingName);
            }
      
            NewAsset->TilePosition(target->TilePosition());
            NewAsset->HitPoints(1);
      
      
            // if you are building a gold mine, initialize the starting gold, set player color to none, and add asset to player 0 asset list
            if(EAssetType::GoldMine == NewAsset->Type()) {
                NewAsset->Gold(StartingGold);
            }
      
            NewCommand.DAction = EAssetAction::Capability; // Pretend active
            NewCommand.DCapability = AssetCapabilityType();
            NewCommand.DAssetTarget = NewAsset;
            NewCommand.DActivatedCapability = std::make_shared< CActivatedCapability >(actor, playerdata, NewAsset, target, AssetType->LumberCost(), AssetType->GoldCost(), AssetType->StoneCost(), CPlayerAsset::UpdateFrequency() * AssetType->BuildTime());
      
             actor->PushCommand(NewCommand);
      
            if((EDirection::Max == BuildDirection)&&(TilePosition != actor->TilePosition())){
                NewCommand.DAction = EAssetAction::Walk;
                actor->PushCommand(NewCommand);
            }
        }
        else{
            NewCommand.DAction = EAssetAction::Capability;
            NewCommand.DCapability = AssetCapabilityType();
            NewCommand.DAssetTarget = target;
            actor->PushCommand(NewCommand);

            NewCommand.DAction = EAssetAction::Walk;
            actor->PushCommand(NewCommand);
        }
        return true;
    }
    return false;
}

/**
* Sets the current variables to the parameter values
*
* @param[in] actor A shared pointer to the Player Assets
* @param[in] playerdata A shared pointer to the player data
* @param[in] target A shared pointer to PlayerAssets
* @param[in] lumber The amount of lumber the player has
* @param[in] gold The amount of gold the player has
* @param[in] stone The amount of stone material the player has
* @param[in] steps The total number of steps
*
*/

CPlayerCapabilityBuildNormal::CActivatedCapability::CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, std::shared_ptr< CPlayerAsset > goldvein, int lumber, int gold, int stone, int steps) :
CActivatedPlayerCapability(actor, playerdata, target){
    SAssetCommand AssetCommand;

    DCurrentStep = 0;
    DTotalSteps = steps;
    DLumber = lumber;
    DGold = gold;
    DStone = stone;
    DGoldVein = goldvein;
    DPlayerData->DecrementLumber(DLumber);
    DPlayerData->DecrementGold(DGold);
    DPlayerData->DecrementStone(DStone);
    AssetCommand.DAction = EAssetAction::Construct;
    AssetCommand.DAssetTarget = DActor;
    DTarget->PushCommand(AssetCommand);
}

/**
* Saves activated capability to an output file stream
*
* @param[in] save Output file stream
*
* @return void
*
*/

void CPlayerCapabilityBuildNormal::CActivatedCapability::Save(std::ofstream& save){
    save << "#Capability Type\n";
    save << "BUILD" << std::endl;

    // std::shared_ptr< CPlayerAsset > actor
    save << "#Actor\n";
    save << DActor->AssetID() << std::endl;

    // std::shared_ptr< CPlayerData > playerdata
    save << "#PlayerData\n";
    save << to_underlying(DPlayerData->Color()) << std::endl;

    // std::shared_ptr< CPlayerAsset > target
    save << "#Target\n";
    save << DTarget->AssetID() << std::endl;

    // int DCurrentStep;
    save << "#Current Step\n";
    save << DCurrentStep << std::endl;

    // int DTotalSteps;
    save << "#Total Step\n";
    save << DTotalSteps << std::endl;

    // int DLumber;
    save << "#Lumber\n";
    save << DLumber << std::endl;

    // int DGold;
    save << "#Gold\n";
    save << DGold << std::endl;

    // int DStone;
    save << "#Stone\n";
    save << DStone << std::endl;
}

void CPlayerCapabilityBuildNormal::CActivatedCapability::Step(int step){
    DCurrentStep = step;
}

/**
* Show the percentage completion of the capability.
*
* @param[in] max the number of max percent complete.
*
* @return the number of percentage steps complete.
*
*/

int CPlayerCapabilityBuildNormal::CActivatedCapability::PercentComplete(int max){
    return DCurrentStep * max / DTotalSteps;
}

/**
* Set/Initialize the net steps until building is complete .
*
* @return True if current steps are greater or equal to the total number of steps
*
*/

bool CPlayerCapabilityBuildNormal::CActivatedCapability::IncrementStep(){
    int AddHitPoints = (DTarget->MaxHitPoints() * (DCurrentStep + 1) / DTotalSteps) - (DTarget->MaxHitPoints() * DCurrentStep / DTotalSteps);

    DTarget->IncrementHitPoints(AddHitPoints);
    if(DTarget->HitPoints() > DTarget->MaxHitPoints()){
        DTarget->HitPoints(DTarget->MaxHitPoints());
    }
    
    std::shared_ptr< CPlayerAsset > goldvein = DGoldVein.lock();
    if(goldvein){
        CApplicationData::Instance("")->DGameModel->Player(EPlayerColor::None)->DeleteAsset(goldvein);
    }

    DCurrentStep++;
    DActor->IncrementStep();
    DTarget->IncrementStep();
    if(DCurrentStep >= DTotalSteps){
        DPlayerData->ResolveNewAssetCounts();
        DPlayerData->IncrementCreated(DTarget);

        SGameEvent TempEvent;

        TempEvent.DType = EEventType::WorkComplete;
        TempEvent.DAsset = DActor;
        DPlayerData->AddGameEvent(TempEvent);

        DTarget->PopCommand();
        DActor->PopCommand();
        if(DTarget->AssetType()->Color() == EPlayerColor::None){
            DActor->TilePosition(DPlayerData->ActualMap()->FindAssetPlacement(DActor, DTarget, CTilePosition(DPlayerData->PlayerMap()->Width()-1, DPlayerData->PlayerMap()->Height()-1)));
        }
        else{
            DActor->TilePosition(DPlayerData->PlayerMap()->FindAssetPlacement(DActor, DTarget, CTilePosition(DPlayerData->PlayerMap()->Width()-1, DPlayerData->PlayerMap()->Height()-1)));
        }
        DActor->ResetStep();
        DTarget->ResetStep();

        return true;
    }
    return false;
}

/**
* Cancel the building capability.
*
* @return void.
*
*/

void CPlayerCapabilityBuildNormal::CActivatedCapability::Cancel(){
    DPlayerData->IncrementLumber(DLumber);
    DPlayerData->IncrementGold(DGold);
    DPlayerData->IncrementStone(DStone);
    DPlayerData->DeleteAsset(DTarget);
    DActor->PopCommand();
}
