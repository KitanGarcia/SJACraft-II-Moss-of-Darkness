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
* @class CPlayerCapabilityTrainNormal
*
* @brief This class was written to create (aka "train") different units for its associated building. \n
*   <pre>
*    The class contains 2 other clases: CRegistrant and CActivatedCapability.
*    CRegistrant determines which unit to create: Peasant, Footman, Archer.
*    CActivatedCapability calls several functions: PercentComplete, IncrementStep, and Cancel that determine the status of creating "training" the unit
*   </pre>
*
* @author Wonhee Park
*
* @version 9.0
*
* @date 12/3/17
*
* Contact: wonpark@ucdavis.edu
*
*/

#include "GameModel.h"
#include "Debug.h"

class CPlayerCapabilityTrainNormal : public CPlayerCapability{
    protected:
        class CRegistrant{
            public:
                CRegistrant();
        };
        static CRegistrant DRegistrant;

        class CActivatedCapability : public CActivatedPlayerCapability{
            protected:
                int DCurrentStep;
                int DTotalSteps;
                int DLumber;
                int DGold;
                int DStone;

            public:
                CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, int lumber, int gold, int stone, int steps);
                virtual ~CActivatedCapability(){};

                int PercentComplete(int max);
                bool IncrementStep();
                void Cancel();
                void Save(std::ofstream& save);
                void Step(int step);
        };
        std::string DUnitName;
        CPlayerCapabilityTrainNormal(const std::string &unitname);

    public:
        virtual ~CPlayerCapabilityTrainNormal(){};

        bool CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message);
        bool CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
        bool ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
};

CPlayerCapabilityTrainNormal::CRegistrant CPlayerCapabilityTrainNormal::DRegistrant;

/**
* Set/initialize what units can be created: Peasant, Footman, Archer
*
*/

CPlayerCapabilityTrainNormal::CRegistrant::CRegistrant(){
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityTrainNormal >(new CPlayerCapabilityTrainNormal("Peasant")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityTrainNormal >(new CPlayerCapabilityTrainNormal("Footman")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityTrainNormal >(new CPlayerCapabilityTrainNormal("Archer")));
}

/**
* Set/initialize unit that is to be created
*
* @param[in] unitname Name of unit to build
*
*/

CPlayerCapabilityTrainNormal::CPlayerCapabilityTrainNormal(const std::string &unitname) : CPlayerCapability(std::string("Build") + unitname, ETargetType::None){
    DUnitName = unitname;
}

/**
* checks if unit creation can be intiated, under following conditions
*     <pre>
*     player's Lumber should be greater than lumbercost for unit creation.
*     player's Gold should be greater than goldcost for unit creation.
*     player's Stone should be greater than stonecost for unit creation.
*     player's FoodProduction should be greater than player+unit's food consumption.
*     </pre>
*
* @param[in] actor Shared pointer from class PlayerAsset
* @param[in] playerdata Shared pointer from class PlayerData
*
* @return True if all asset requirement conditions met
* @return False if any of the above asset requirement conditions are not met
*
*/

bool CPlayerCapabilityTrainNormal::CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message){
    bool ReturnStatus = true;
    auto Iterator = playerdata->AssetTypes()->find(DUnitName);

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
        }if((AssetType->FoodConsumption() + playerdata->FoodConsumption()) > playerdata->FoodProduction()){
            message += "FoodConsumption:";
            message += std::to_string(AssetType->FoodConsumption());
            message += " ";
            ReturnStatus = false;
        }
        if(!playerdata->AssetRequirementsMet(DUnitName, message)){
            ReturnStatus = false;
        }
    }

    return ReturnStatus;
}

/**
* calls CanInitiate function to see if unit creation requirements met.
*
* @param[in] actor Shared pointer from class PlayerAsset.
* @param[in] playerdata Shared pointer from class PlayerData
* @param[in] target Shared pointer from class PlayerAsset
*
* @return True if able to initiate unit creation
*
*/

bool CPlayerCapabilityTrainNormal::CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    std::string dummyStr = "";
    return CanInitiate(actor, playerdata, dummyStr);
}

/**
* Applies unit creation capability, by having actor apply unit creation according to action, capability, asset target.
*
* @param[in] actor Shared pointer from class PlayerAsset.
* @param[in] playerdata Shared pointer from class PlayerData
* @param[in] target Shared pointer from class PlayerAsset
*
* @return True if iterator has not exhausted list of unitnames
* @return False if iterator has exhausted list of unitnames
*
*/

bool CPlayerCapabilityTrainNormal::ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    auto Iterator = playerdata->AssetTypes()->find(DUnitName);

    if(Iterator != playerdata->AssetTypes()->end()){
        auto AssetType = Iterator->second;
        auto NewAsset = playerdata->CreateAsset(DUnitName);
        SAssetCommand NewCommand;
        CTilePosition TilePosition;
        TilePosition.SetFromPixel(actor->Position());
        NewAsset->TilePosition(TilePosition);
        NewAsset->HitPoints(1);

        NewCommand.DAction = EAssetAction::Capability;
        NewCommand.DCapability = AssetCapabilityType();
        NewCommand.DAssetTarget = NewAsset;
        NewCommand.DActivatedCapability = std::make_shared< CActivatedCapability >(actor, playerdata, NewAsset, AssetType->LumberCost(), AssetType->GoldCost(), AssetType->StoneCost(), CPlayerAsset::UpdateFrequency() * AssetType->BuildTime());
        actor->PushCommand(NewCommand);
        actor->ResetStep();
    }
    return false;
}

/**
* Set/initialize default values of player's information to be used when determining the status of the unit creation
*
* @param[in] actor
* @param[in] playerdata
* @param[in] target
*
*/

CPlayerCapabilityTrainNormal::CActivatedCapability::CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, int lumber, int gold, int stone, int steps) :
CActivatedPlayerCapability(actor, playerdata, target){
    SAssetCommand AssetCommand;

    DCurrentStep = 0;
    DTotalSteps = steps;
    DLumber = lumber;
    DGold = gold;
    DStone = stone;
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

void CPlayerCapabilityTrainNormal::CActivatedCapability::Save(std::ofstream& save){
    save << "#Capability Type\n";
    save << "TRAIN" << std::endl;

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

void CPlayerCapabilityTrainNormal::CActivatedCapability::Step(int step){
    DCurrentStep = step;
}

/**
* percent completion of action (shown via the health bar in the game UI)
*
* @param[in] max Health value
*
* @return percent complete of unit creation
*
*/

int CPlayerCapabilityTrainNormal::CActivatedCapability::PercentComplete(int max){
    return DCurrentStep * max / DTotalSteps;
}

/**
* Increments steps towards unit creation completion, updating hitpoints as it goes along.
*
* @return True if unit creation work is complete
* @return False if unit creation work is incomplete
*
*/

bool CPlayerCapabilityTrainNormal::CActivatedCapability::IncrementStep(){
    int AddHitPoints = (DTarget->MaxHitPoints() * (DCurrentStep + 1) / DTotalSteps) - (DTarget->MaxHitPoints() * DCurrentStep / DTotalSteps);

    DTarget->IncrementHitPoints(AddHitPoints);
    if(DTarget->HitPoints() > DTarget->MaxHitPoints()){
        DTarget->HitPoints(DTarget->MaxHitPoints());
    }
    DCurrentStep++;
    DActor->IncrementStep();
    DTarget->IncrementStep();
    if(DCurrentStep >= DTotalSteps){
        DPlayerData->ResolveNewAssetCounts();
        DPlayerData->IncrementCreated(DTarget);

        SGameEvent TempEvent;

        TempEvent.DType = EEventType::Ready;
        TempEvent.DAsset = DTarget;
        DPlayerData->AddGameEvent(TempEvent);

        DTarget->PopCommand();
        DActor->PopCommand();
        DTarget->TilePosition(DPlayerData->PlayerMap()->FindAssetPlacement(DTarget, DActor, CTilePosition(DPlayerData->PlayerMap()->Width()-1, DPlayerData->PlayerMap()->Height()-1)));
        return true;
    }
    return false;
}

/**
* Cancels unit creation capability and reincrements the assets for lumber and gold and pops command. 
*
* @return void
*
*/

void CPlayerCapabilityTrainNormal::CActivatedCapability::Cancel(){
    DPlayerData->IncrementLumber(DLumber);
    DPlayerData->IncrementGold(DGold);
    DPlayerData->DeleteAsset(DTarget);
    DActor->PopCommand();
}
