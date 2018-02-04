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
* @class CPlayerCapabilityBuildingUpgrade
*
* @brief This class was written to give player building upgrade capability \n
*   <pre>
*    The class contains 2 other clases: CRegistrant and CActivatedCapability.
*    CRegistrant determines which building will be ugraded: Keep, Castle, GuardTower, KeepTower.
*    CActivatedCapability calls several functions: PercentComplete, IncrementStep, Cancel that determine the status of applying the building upgrade.
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

class CPlayerCapabilityBuildingUpgrade : public CPlayerCapability{
    protected:
        class CRegistrant{
            public:
                CRegistrant();
        };
        static CRegistrant DRegistrant;

        class CActivatedCapability : public CActivatedPlayerCapability{
            protected:
                std::shared_ptr< CPlayerAssetType > DOriginalType;
                std::shared_ptr< CPlayerAssetType > DUpgradeType;
                int DCurrentStep;
                int DTotalSteps;
                int DLumber;
                int DGold;
                int DStone;

            public:
                CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, std::shared_ptr< CPlayerAssetType > origtype, std::shared_ptr< CPlayerAssetType > upgradetype, int lumber, int gold, int stone, int steps);
                virtual ~CActivatedCapability(){};

                int PercentComplete(int max);
                bool IncrementStep();
                void Cancel();
                void Step(int step);
                void Save(std::ofstream& save);
        };
        std::string DBuildingName;
        CPlayerCapabilityBuildingUpgrade(const std::string &buildingname);

    public:
        virtual ~CPlayerCapabilityBuildingUpgrade(){};

        bool CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message);
        bool CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
        bool ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
};

CPlayerCapabilityBuildingUpgrade::CRegistrant CPlayerCapabilityBuildingUpgrade::DRegistrant;

/**
* Set/initialize what buildings can be upgraded: Keep, Castle, GuardTower, CannonTower
*
*/

CPlayerCapabilityBuildingUpgrade::CRegistrant::CRegistrant(){
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildingUpgrade >(new CPlayerCapabilityBuildingUpgrade("Keep")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildingUpgrade >(new CPlayerCapabilityBuildingUpgrade("Castle")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildingUpgrade >(new CPlayerCapabilityBuildingUpgrade("GuardTower")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildingUpgrade >(new CPlayerCapabilityBuildingUpgrade("CannonTower")));
}

/**
* Set/initialize building name that is to be upgraded
*
* @param[in] buildingname Name of building
*
*/

CPlayerCapabilityBuildingUpgrade::CPlayerCapabilityBuildingUpgrade(const std::string &buildingname) : CPlayerCapability(std::string("Build") + buildingname, ETargetType::None){
    DBuildingName = buildingname;
}

/**
* checks if the buiding upgrade can be intiated, under following conditions
*     <pre>
*     player's Lumber should be greater than lumbercost for building upgrade.
*     player's Gold should be greater than goldcost for building upgrade.
*     </pre>
*
* @param[in] actor Shared pointer from class PlayerAsset
* @param[in] playerdata Shared pointer from class PlayerData
*
* @return True if all asset requirement conditions met
* @return False if any of the above asset requirement conditions are not met
*
*/

bool CPlayerCapabilityBuildingUpgrade::CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message){
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
        if(!playerdata->AssetRequirementsMet(DBuildingName, message)){
            ReturnStatus = false;
        }
    }

    return ReturnStatus;
}

/**
* calls CanInitiate function to see if building upgrade requirements met.
*
* @param[in] actor Shared pointer from class PlayerAsset.
* @param[in] playerdata Shared pointer from class PlayerData
* @param[in] target Shared pointer from class PlayerAsset
*
* @return True if able to initiate building upgrade
*
*/

bool CPlayerCapabilityBuildingUpgrade::CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    std::string dummyStr = "";
    return CanInitiate(actor, playerdata, dummyStr);
}

/**
* Applies building upgrade capability, by having actor apply building upgrade according to action, capability, asset target.
*
* @param[in] actor Shared pointer from class PlayerAsset.
* @param[in] playerdata Shared pointer from class PlayerData
* @param[in] target Shared pointer from class PlayerAsset
*
* @return True if iterator has not exhausted list of buildingnames
* @return False if iterator has exhausted list of buildingnames
*
*/

bool CPlayerCapabilityBuildingUpgrade::ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    auto Iterator = playerdata->AssetTypes()->find(DBuildingName);

    if(Iterator != playerdata->AssetTypes()->end()){
        SAssetCommand NewCommand;
        auto AssetType = Iterator->second;

        actor->ClearCommand();
        NewCommand.DAction = EAssetAction::Capability;
        NewCommand.DCapability = AssetCapabilityType();
        NewCommand.DAssetTarget = target;
        NewCommand.DActivatedCapability = std::make_shared< CActivatedCapability >(actor, playerdata, target, actor->AssetType(), AssetType, AssetType->LumberCost(), AssetType->GoldCost(), AssetType->StoneCost(), CPlayerAsset::UpdateFrequency() * AssetType->BuildTime());
        actor->PushCommand(NewCommand);

        return true;
    }
    return false;
}

/**
* Set/initialize default values of player's information to be used when determining the status of the building upgrade
*
* @param[in] actor
* @param[in] playerdata
* @param[in] target
*
*/

CPlayerCapabilityBuildingUpgrade::CActivatedCapability::CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, std::shared_ptr< CPlayerAssetType > origtype, std::shared_ptr< CPlayerAssetType > upgradetype, int lumber, int gold, int stone, int steps) :
CActivatedPlayerCapability(actor, playerdata, target){
    SAssetCommand AssetCommand;

    DOriginalType = origtype;
    DUpgradeType = upgradetype;
    DCurrentStep = 0;
    DTotalSteps = steps;
    DLumber = lumber;
    DGold = gold;
    DStone = stone;
    DPlayerData->DecrementLumber(DLumber);
    DPlayerData->DecrementGold(DGold);
    DPlayerData->DecrementStone(DStone);
}

/**
* Saves activated capability to an output file stream
*
* @param[in] save Output file stream
*
* @return void
*
*/

void CPlayerCapabilityBuildingUpgrade::CActivatedCapability::Save(std::ofstream& save){
    save << "#Capability Type\n";
    save << "BUILDINGUPGRADE" << std::endl;

    // std::shared_ptr< CPlayerAsset > actor
    save << "#Actor\n";
    save << DActor->AssetID() << std::endl;

    // std::shared_ptr< CPlayerData > playerdata
    save << "#PlayerData\n";
    save << to_underlying(DPlayerData->Color()) << std::endl;

    // std::shared_ptr< CPlayerAsset > target
    save << "#Target\n";
    if(DTarget)
        save << DTarget->AssetID() << std::endl;
    else
        save << "NULL\n";


    // std::shared_ptr< CPlayerAssetType > DOriginalType;
    save << "#Original Type\n";
    save << DOriginalType->Name() << std::endl;

    // std::shared_ptr< CPlayerAssetType > DUpgradeType;
    save << "#Upgrading Type\n";
    save << DUpgradeType->Name() << std::endl;

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

void CPlayerCapabilityBuildingUpgrade::CActivatedCapability::Step(int step){
    DCurrentStep = step;
}

/**
* percent completion of action (shown via the health bar in the game UI)
*
* @param[in] max Health value
*
* @return percent complete of building upgrade
*
*/

int CPlayerCapabilityBuildingUpgrade::CActivatedCapability::PercentComplete(int max){
    return DCurrentStep * max / DTotalSteps;
}

/**
* Increments steps towards building upgrade's completion, updating hitpoints as it goes along.
*
* @return True if building upgrade work is complete
* @return False if building upgrade work is incomplete
*
*/

bool CPlayerCapabilityBuildingUpgrade::CActivatedCapability::IncrementStep(){
    int AddHitPoints = ((DUpgradeType->HitPoints() - DOriginalType->HitPoints()) * (DCurrentStep + 1) / DTotalSteps) - ((DUpgradeType->HitPoints() - DOriginalType->HitPoints()) * DCurrentStep / DTotalSteps);

    if(0 == DCurrentStep){
        SAssetCommand AssetCommand = DActor->CurrentCommand();
        AssetCommand.DAction = EAssetAction::Construct;
        DActor->PopCommand();
        DActor->PushCommand(AssetCommand);
        DActor->ChangeType(DUpgradeType);
        DActor->ResetStep();
    }

    DActor->IncrementHitPoints(AddHitPoints);
    if(DActor->HitPoints() > DActor->MaxHitPoints()){
        DActor->HitPoints(DActor->MaxHitPoints());
    }
    DCurrentStep++;
    DActor->IncrementStep();
    if(DCurrentStep >= DTotalSteps){
        SGameEvent TempEvent;

        TempEvent.DType = EEventType::WorkComplete;
        TempEvent.DAsset = DActor;
        DPlayerData->AddGameEvent(TempEvent);

        DActor->PopCommand();
        if(DActor->Range()){
            SAssetCommand Command;
            Command.DAction = EAssetAction::StandGround;
            DActor->PushCommand(Command);
        }
        return true;
    }
    return false;
}

/**
* Cancels building upgrade capability and reincrements the assets for lumber and gold and pops command.
*
* @return void
*
*/

void CPlayerCapabilityBuildingUpgrade::CActivatedCapability::Cancel(){
    DPlayerData->IncrementLumber(DLumber);
    DPlayerData->IncrementGold(DGold);
    DPlayerData->IncrementStone(DStone);
    DActor->ChangeType(DOriginalType);
    DActor->PopCommand();
}
