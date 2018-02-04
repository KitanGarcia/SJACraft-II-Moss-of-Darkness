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
* @class CPlayerCapabilityUnitUpgrade
*
* @ brief This class was written to give unit's an upgrade capability \n
*   <pre>
*    The class contains 2 other clases: CRegistrant and CActivatedCapability.
*    CRegistrant determines which unit will be ugraded: WeaponUpgrade2, WeaponUpgrade3, ArmorUpgrade2, ArmorUpgrade3, ArrowUpgrade2, ArrowUpgrade3, Longbow, RangerScouting, Marksmanship.
*    CActivatedCapability calls several functions: PercentComplete, IncrementStep, Cancel that determine the status of applying the unit upgrade.
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

class CPlayerCapabilityUnitUpgrade : public CPlayerCapability{
    protected:
        class CRegistrant{
            public:
                CRegistrant();
        };
        static CRegistrant DRegistrant;

        class CActivatedCapability : public CActivatedPlayerCapability{
            protected:
                std::shared_ptr< CPlayerAssetType > DUpgradingType;
                std::string DUpgradeName;
                int DCurrentStep;
                int DTotalSteps;
                int DLumber;
                int DGold;
                int DStone;

            public:
                CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, std::shared_ptr< CPlayerAssetType > upgradingtype, const std::string &upgradename, int lumber, int gold, int stone, int steps);
                virtual ~CActivatedCapability(){};

                int PercentComplete(int max);
                bool IncrementStep();
                void Cancel();
                void Save(std::ofstream& save);
                void Step(int step);
        };
        std::string DUpgradeName;
        CPlayerCapabilityUnitUpgrade(const std::string &upgradename);

    public:
        virtual ~CPlayerCapabilityUnitUpgrade(){};

        bool CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message);
        bool CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
        bool ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
};

CPlayerCapabilityUnitUpgrade::CRegistrant CPlayerCapabilityUnitUpgrade::DRegistrant;

/**
* Set/initialize what units can be upgrade: WeaponUpgrade 2/3, ArmorUpgrade 2/3, ArrowUpgrade 2/3, Longbow, RangerScouting, Marksmanship
*
*/

CPlayerCapabilityUnitUpgrade::CRegistrant::CRegistrant(){
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityUnitUpgrade >(new CPlayerCapabilityUnitUpgrade("WeaponUpgrade2")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityUnitUpgrade >(new CPlayerCapabilityUnitUpgrade("WeaponUpgrade3")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityUnitUpgrade >(new CPlayerCapabilityUnitUpgrade("ArmorUpgrade2")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityUnitUpgrade >(new CPlayerCapabilityUnitUpgrade("ArmorUpgrade3")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityUnitUpgrade >(new CPlayerCapabilityUnitUpgrade("ArrowUpgrade2")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityUnitUpgrade >(new CPlayerCapabilityUnitUpgrade("ArrowUpgrade3")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityUnitUpgrade >(new CPlayerCapabilityUnitUpgrade("Longbow")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityUnitUpgrade >(new CPlayerCapabilityUnitUpgrade("RangerScouting")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityUnitUpgrade >(new CPlayerCapabilityUnitUpgrade("Marksmanship")));
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityUnitUpgrade >(new CPlayerCapabilityUnitUpgrade("RangerTrackingUpgrade")));
}

/**
* Set/initialize unit that is to be upgraded
*
* @param[in] upgradename Name of unit to upgrade
*
*/

CPlayerCapabilityUnitUpgrade::CPlayerCapabilityUnitUpgrade(const std::string &upgradename) : CPlayerCapability(upgradename, ETargetType::None){
    DUpgradeName = upgradename;
}

/**
* checks if unit upgrade can be intiated, under following conditions
*     <pre>
*     player's Lumber should be greater than lumbercost for unit creation.
*     player's Gold should be greater than goldcost for unit creation.
*     player's Stone should be greater than stonecost for unit creation.
*     </pre>
*
* @param[in] actor Shared pointer from class PlayerAsset
* @param[in] playerdata Shared pointer from class PlayerData
*
* @return True if all asset requirement conditions met
* @return False if any of the above asset requirement conditions are not met
*
*/

bool CPlayerCapabilityUnitUpgrade::CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message){
    bool ReturnStatus = true;
    auto Upgrade = CPlayerUpgrade::FindUpgradeFromName(DUpgradeName);

    if(Upgrade){

        if(Upgrade->LumberCost() > playerdata->Lumber()){
            message += "Lumber:";
            message += std::to_string(Upgrade->LumberCost());
            message += " ";
            ReturnStatus = false;
        }
        if(Upgrade->GoldCost() > playerdata->Gold()){
            message += "Gold:";
            message += std::to_string(Upgrade->GoldCost());
            message += " ";
            ReturnStatus = false;
        }
        if(Upgrade->StoneCost() > playerdata->Stone()){
            message += "Stone:";
            message += std::to_string(Upgrade->StoneCost());
            message += " ";
            ReturnStatus = false;
        }
        if(!playerdata->UpgradeRequirementsMet(DUpgradeName, message)){
            ReturnStatus = false;
        }

    }

    return ReturnStatus;
}

/**
* calls CanInitiate function to see if unit upgrade requirements met.
*
* @param[in] actor Shared pointer from class PlayerAsset.
* @param[in] playerdata Shared pointer from class PlayerData
* @param[in] target Shared pointer from class PlayerAsset
*
* @return True if able to initiate unit upgrade
*
*/

bool CPlayerCapabilityUnitUpgrade::CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    std::string dummyStr = "";
    return CanInitiate(actor, playerdata, dummyStr);
}

/**
* Applies unit upgrade capability, by having actor apply unit upgrade according to action, capability, asset target.
*
* @param[in] actor Shared pointer from class PlayerAsset.
* @param[in] playerdata Shared pointer from class PlayerData
* @param[in] target Shared pointer from class PlayerAsset
*
* @return True if UpgradeName exists
* @return False if UpgradeName does not exist
*
*/

bool CPlayerCapabilityUnitUpgrade::ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    auto Upgrade = CPlayerUpgrade::FindUpgradeFromName(DUpgradeName);

    if(Upgrade){
        SAssetCommand NewCommand;

        actor->ClearCommand();
        NewCommand.DAction = EAssetAction::Capability;
        NewCommand.DCapability = AssetCapabilityType();
        NewCommand.DAssetTarget = target;
        NewCommand.DActivatedCapability = std::make_shared< CActivatedCapability >(actor, playerdata, target, actor->AssetType(), DUpgradeName, Upgrade->LumberCost(), Upgrade->GoldCost(), Upgrade->StoneCost(), CPlayerAsset::UpdateFrequency() * Upgrade->ResearchTime());
        actor->PushCommand(NewCommand);

        return true;
    }
    return false;
}

/**
* Set/initialize default values of player's information to be used when determining the status of the unit upgrade
*
* @param[in] actor
* @param[in] playerdata
* @param[in] target
*
*/

CPlayerCapabilityUnitUpgrade::CActivatedCapability::CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, std::shared_ptr< CPlayerAssetType > upgradingtype, const std::string &upgradename, int lumber, int gold, int stone, int steps) :
CActivatedPlayerCapability(actor, playerdata, target){
    SAssetCommand AssetCommand;

    DUpgradingType = upgradingtype;
    DUpgradeName = upgradename;
    DCurrentStep = 0;
    DTotalSteps = steps;
    DLumber = lumber;
    DGold = gold;
    DStone = stone;
    DPlayerData->DecrementLumber(DLumber);
    DPlayerData->DecrementGold(DGold);
    DPlayerData->DecrementStone(DStone);
    DUpgradingType->RemoveCapability(CPlayerCapability::NameToType(DUpgradeName));
}

/**
* Saves activated capability to an output file stream
*
* @param[in] save Output file stream
*
* @return void
*
*/

void CPlayerCapabilityUnitUpgrade::CActivatedCapability::Save(std::ofstream& save){
    save << "#Capability Type\n";
    save << "UNITUPGRADE" << std::endl;

    // std::shared_ptr< CPlayerAsset > actor
    save << "#Actor\n";
    save << DActor->AssetID() << std::endl;

    // std::shared_ptr< CPlayerData > playerdata
    save << "#PlayerData\n";
    save << to_underlying(DPlayerData->Color()) << std::endl;

    // std::shared_ptr< CPlayerAsset > target
    save << "#Targaet\n";

    if(DTarget != nullptr)
        save << DTarget->AssetID() << std::endl;
    else
        save << "NULL\n";

    // std::shared_ptr< CPlayerAssetType > DUpgradingType;
    save << "#Upgrading Type\n";
    save << DUpgradingType->Name() << std::endl;

    // std::string DUpgradeName;
    save << "#Upgrading Name\n";
    save << DUpgradeName << std::endl;

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

void CPlayerCapabilityUnitUpgrade::CActivatedCapability::Step(int step){
    DCurrentStep = step;
}

/**
* percent completion of action (shown via the health bar in the game UI)
*
* @param[in] max Health value
*
* @return percent complete of unit upgrade
*
*/

int CPlayerCapabilityUnitUpgrade::CActivatedCapability::PercentComplete(int max){
    return DCurrentStep * max / DTotalSteps;
}

/**
* Increments steps towards unit upgrade completion.
*
* @return True if unit creation work is complete
* @return False if unit creation work is incomplete
*
*/

bool CPlayerCapabilityUnitUpgrade::CActivatedCapability::IncrementStep(){
    DCurrentStep++;
    DActor->IncrementStep();
    if(DCurrentStep >= DTotalSteps){
        DPlayerData->AddUpgrade(DUpgradeName);
        DActor->PopCommand();
        if(DUpgradeName.rfind("2") == (DUpgradeName.length()-1)){
            DUpgradingType->AddCapability(CPlayerCapability::NameToType(DUpgradeName.substr(0, DUpgradeName.length()-1) + "3"));
        }

        return true;
    }
    return false;
}

/**
* Cancels unit upgrade capability and reincrements the assets for lumber, gold, stone and pops command.
*
* @return void
*
*/

void CPlayerCapabilityUnitUpgrade::CActivatedCapability::Cancel(){
    DPlayerData->IncrementLumber(DLumber);
    DPlayerData->IncrementGold(DGold);
    DPlayerData->IncrementStone(DStone);
    DUpgradingType->AddCapability(CPlayerCapability::NameToType(DUpgradeName));
    DActor->PopCommand();
}

/**
* @class CPlayerCapabilityBuildRanger
*
* @ brief This class was written to build a ranger capability \n
*   <pre>
*    The class contains 2 other clases: CRegistrant and CActivatedCapability.
*    CRegistrant determines which unit to build: ranger.
*    CActivatedCapability calls several functions: PercentComplete, IncrementStep, Cancel that determine the status of building ranger.
*   </pre>
*
*
* @author Wonhee
*
* @version 9.0
*
* @date 12/3/17
*
* Contact: wonpark@ucdavis.edu
*
*/

class CPlayerCapabilityBuildRanger : public CPlayerCapability{
    protected:
        class CRegistrant{
            public:
                CRegistrant();
        };
        static CRegistrant DRegistrant;

        class CActivatedCapability : public CActivatedPlayerCapability{
            protected:
                std::shared_ptr< CPlayerAssetType > DUpgradingType;
                std::string DUnitName;
                int DCurrentStep;
                int DTotalSteps;
                int DLumber;
                int DGold;
                int DStone;

            public:
                CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, std::shared_ptr< CPlayerAssetType > upgradingtype, const std::string &unitname, int lumber, int gold, int stone, int steps);
                virtual ~CActivatedCapability(){};

                int PercentComplete(int max);
                bool IncrementStep();
                void Cancel();
                void Save(std::ofstream& save);
                void Step(int step);
        };
        std::string DUnitName;
        CPlayerCapabilityBuildRanger(const std::string &unitname);

    public:
        virtual ~CPlayerCapabilityBuildRanger(){};

        bool CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message);
        bool CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
        bool ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
};

CPlayerCapabilityBuildRanger::CRegistrant CPlayerCapabilityBuildRanger::DRegistrant;

/**
* Set/initialize what units can be created: Ranger
*
*/

CPlayerCapabilityBuildRanger::CRegistrant::CRegistrant(){
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildRanger >(new CPlayerCapabilityBuildRanger("Ranger")));
}

/**
* Set/initialize unit that is to be created (Ranger)
*
* @param[in] unitname Name of unit to build
*
*/

CPlayerCapabilityBuildRanger::CPlayerCapabilityBuildRanger(const std::string &unitname) : CPlayerCapability(std::string("Build") + unitname, ETargetType::None){
    DUnitName = unitname;
}

/**
* checks if ranger unit creation can be intiated, under following conditions, dependant on whether assetype is LumberMill or Barracks
*     <pre>
*     player's Lumber should be greater than lumbercost for unit creation.
*     player's Gold should be greater than goldcost for unit creation.
*     player's Stone should be greater than stonecost for unit creation.
*     player's food production should be greater than ranger's foodconsumption and player's food consumption (only applies to Barracks)
*     </pre>
*
* @param[in] actor Shared pointer from class PlayerAsset
* @param[in] playerdata Shared pointer from class PlayerData
*
* @return True if all asset requirement conditions met
* @return False if any of the above asset requirement conditions are not met
*
*/

bool CPlayerCapabilityBuildRanger::CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message){
    bool ReturnStatus = true;
    if(EAssetType::LumberMill == actor->Type()){
        auto Upgrade = CPlayerUpgrade::FindUpgradeFromName(std::string("Build") + DUnitName);

        if(Upgrade){

            if(Upgrade->LumberCost() > playerdata->Lumber()){
                message += "Lumber:";
                message += std::to_string(Upgrade->LumberCost());
                message += " ";
                ReturnStatus = false;
            }
            if(Upgrade->GoldCost() > playerdata->Gold()){
                message += "Gold:";
                message += std::to_string(Upgrade->GoldCost());
                message += " ";
                ReturnStatus = false;
            }
            if(Upgrade->StoneCost() > playerdata->Stone()){
                message += "Stone:";
                message += std::to_string(Upgrade->StoneCost());
                message += " ";
                ReturnStatus = false;
            }
            if(!playerdata->AssetRequirementsMet(DUnitName, message)){
                ReturnStatus = false;
            }
        }
    }
    else if(EAssetType::Barracks == actor->Type()){
        auto AssetIterator = playerdata->AssetTypes()->find(DUnitName);

        if(AssetIterator != playerdata->AssetTypes()->end()){
            auto AssetType = AssetIterator->second;
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
        }
    }

    return ReturnStatus;
}

/**
* calls CanInitiate function to see if ranger unit creation requirements met.
*
* @param[in] actor Shared pointer from class PlayerAsset.
* @param[in] playerdata Shared pointer from class PlayerData
* @param[in] target Shared pointer from class PlayerAsset
*
* @return True if able to initiate unit creation
*
*/

bool CPlayerCapabilityBuildRanger::CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    std::string dummyStr = "";
    return CanInitiate(actor, playerdata, dummyStr);
}

/**
* Applies ranger unit creation capability, by having actor apply unit creation according to action, capability, asset target.
*
* @param[in] actor Shared pointer from class PlayerAsset.
* @param[in] playerdata Shared pointer from class PlayerData
* @param[in] target Shared pointer from class PlayerAsset
*
* @return True if iterator has not exhausted list of unitnames (barracks) OR if Upgrade exists (lumbermill)
* @return False if iterator has exhausted list of unitnames (barracks) OR if Upgrade does not exist (barracks)
*
*/

bool CPlayerCapabilityBuildRanger::ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    if(EAssetType::LumberMill == actor->Type()){
        auto Upgrade = CPlayerUpgrade::FindUpgradeFromName(std::string("Build") + DUnitName);

        if(Upgrade){
            SAssetCommand NewCommand;

            actor->ClearCommand();
            NewCommand.DAction = EAssetAction::Capability;
            NewCommand.DCapability = AssetCapabilityType();
            NewCommand.DAssetTarget = target;
            NewCommand.DActivatedCapability = std::make_shared< CActivatedCapability >(actor, playerdata, target, actor->AssetType(), DUnitName, Upgrade->LumberCost(), Upgrade->GoldCost(), Upgrade->StoneCost(), CPlayerAsset::UpdateFrequency() * Upgrade->ResearchTime());
            actor->PushCommand(NewCommand);

            return true;
        }
    }
    else if(EAssetType::Barracks == actor->Type()){
        auto AssetIterator = playerdata->AssetTypes()->find(DUnitName);

        if(AssetIterator != playerdata->AssetTypes()->end()){
            auto AssetType = AssetIterator->second;
            auto NewAsset = playerdata->CreateAsset(DUnitName);
            SAssetCommand NewCommand;
            CTilePosition TilePosition;
            TilePosition.SetFromPixel(actor->Position());
            NewAsset->TilePosition(TilePosition);
            NewAsset->HitPoints(1);

            NewCommand.DAction = EAssetAction::Capability;
            NewCommand.DCapability = AssetCapabilityType();
            NewCommand.DAssetTarget = NewAsset;
            NewCommand.DActivatedCapability = std::make_shared< CActivatedCapability >(actor, playerdata, NewAsset, actor->AssetType(), DUnitName, AssetType->LumberCost(), AssetType->GoldCost(), AssetType->StoneCost(), CPlayerAsset::UpdateFrequency() * AssetType->BuildTime());
            actor->PushCommand(NewCommand);
        }
    }
    return false;
}

/**
* Set/initialize default values of player's information to be used when determining the status of the ranger unit creation
*
* @param[in] actor
* @param[in] playerdata
* @param[in] target
*
*/

CPlayerCapabilityBuildRanger::CActivatedCapability::CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, std::shared_ptr< CPlayerAssetType > upgradingtype, const std::string &unitname, int lumber, int gold, int stone, int steps) :
CActivatedPlayerCapability(actor, playerdata, target){
    DUnitName = unitname;
    DCurrentStep = 0;
    DTotalSteps = steps;
    DLumber = lumber;
    DGold = gold;
    DStone = stone;
    DPlayerData->DecrementLumber(DLumber);
    DPlayerData->DecrementGold(DGold);
    DPlayerData->DecrementStone(DStone);
    if(EAssetType::LumberMill == actor->Type()){
        DUpgradingType = upgradingtype;
        DUpgradingType->RemoveCapability(CPlayerCapability::NameToType(std::string("Build") + DUnitName));
    }
    else if(EAssetType::Barracks == actor->Type()){
        SAssetCommand AssetCommand;

        AssetCommand.DAction = EAssetAction::Construct;
        AssetCommand.DAssetTarget = DActor;
        DTarget->PushCommand(AssetCommand);
    }
}

/**
* Saves activated capability to an output file stream
*
* @param[in] save Output file stream
*
* @return void
*
*/

void CPlayerCapabilityBuildRanger::CActivatedCapability::Save(std::ofstream& save){
    save << "#Capability Type\n";
    save << "UNITUPGRADE" << std::endl;

    // std::shared_ptr< CPlayerAsset > actor
    save << "#Actor\n";
    save << DActor->AssetID() << std::endl;

    // std::shared_ptr< CPlayerData > playerdata
    save << "#PlayerData\n";
    save << to_underlying(DPlayerData->Color()) << std::endl;

    // std::shared_ptr< CPlayerAsset > target
    save << "#Target\n";\

    if(DTarget != nullptr)
        save << DTarget->AssetID() << std::endl;
    else
        save << "NULL\n";

    // std::shared_ptr< CPlayerAssetType > DUpgradingType;
    save << "#Upgrading Type\n";

    if(DUpgradingType)
        save << DUpgradingType->Name() << std::endl;
    else
        save << "NULL\n";

    // std::string DUpgradeName;
    save << "#Upgrading Name\n";
    save << DUnitName << std::endl;

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

void CPlayerCapabilityBuildRanger::CActivatedCapability::Step(int step){
    DCurrentStep = step;
}

/**
* percent completion of action (shown via the health bar in the game UI)
*
* @param[in] max Health value
*
* @return percent complete of ranger unit creation
*
*/

int CPlayerCapabilityBuildRanger::CActivatedCapability::PercentComplete(int max){
    return DCurrentStep * max / DTotalSteps;
}

/**
* Increments steps towards ranger unit creation completion, updating hitpoints as it goes along.
*
* @return True if ranger unit creation work is complete
* @return False if ranger unit creation work is incomplete
*
*/

bool CPlayerCapabilityBuildRanger::CActivatedCapability::IncrementStep(){
    if(EAssetType::Barracks == DActor->Type()){
        int AddHitPoints = (DTarget->MaxHitPoints() * (DCurrentStep + 1) / DTotalSteps) - (DTarget->MaxHitPoints() * DCurrentStep / DTotalSteps);

        DTarget->IncrementHitPoints(AddHitPoints);
        if(DTarget->HitPoints() > DTarget->MaxHitPoints()){
            DTarget->HitPoints(DTarget->MaxHitPoints());
        }
    }

    DCurrentStep++;
    DActor->IncrementStep();
    if(DCurrentStep >= DTotalSteps){
        SGameEvent TempEvent;

        if(EAssetType::LumberMill == DActor->Type()){
            auto BarracksIterator = DPlayerData->AssetTypes()->find("Barracks");
            auto RangerIterator = DPlayerData->AssetTypes()->find("Ranger");
            auto LumberMillIterator = DPlayerData->AssetTypes()->find("LumberMill");

            TempEvent.DType = EEventType::WorkComplete;
            TempEvent.DAsset = DActor;

            BarracksIterator->second->AddCapability(EAssetCapabilityType::BuildRanger);
            BarracksIterator->second->RemoveCapability(EAssetCapabilityType::BuildArcher);
            LumberMillIterator->second->AddCapability(EAssetCapabilityType::Longbow);
            LumberMillIterator->second->AddCapability(EAssetCapabilityType::RangerScouting);
            LumberMillIterator->second->AddCapability(EAssetCapabilityType::Marksmanship);
            LumberMillIterator->second->AddCapability(EAssetCapabilityType::RangerTrackingUpgrade);
            // Upgrade all Archers
            for(auto WeakAsset : DPlayerData->Assets()){
                if(auto Asset = WeakAsset.lock()){
                    if(EAssetType::Archer == Asset->Type()){
                        int HitPointIncrement = RangerIterator->second->HitPoints() - Asset->MaxHitPoints();

                        Asset->ChangeType(RangerIterator->second);
                        Asset->IncrementHitPoints(HitPointIncrement);
                    }
                }
            }
        }
        else if(EAssetType::Barracks == DActor->Type()){
            TempEvent.DType = EEventType::Ready;
            TempEvent.DAsset = DTarget;

            DTarget->PopCommand();
            DTarget->TilePosition(DPlayerData->PlayerMap()->FindAssetPlacement(DTarget, DActor, CTilePosition(DPlayerData->PlayerMap()->Width()-1, DPlayerData->PlayerMap()->Height()-1)));
        }
        DPlayerData->AddGameEvent(TempEvent);
        DActor->PopCommand();
        return true;
    }
    return false;
}

/**
* Cancels ranger unit creation capability and reincrements the assets for lumber and gold and stone and pops command.
*
* @return void
*
*/

void CPlayerCapabilityBuildRanger::CActivatedCapability::Cancel(){
    DPlayerData->IncrementLumber(DLumber);
    DPlayerData->IncrementGold(DGold);
    DPlayerData->IncrementStone(DStone);
    if(EAssetType::LumberMill == DActor->Type()){
        DUpgradingType->AddCapability(CPlayerCapability::NameToType(std::string("Build") + DUnitName));
    }
    else if(EAssetType::Barracks == DActor->Type()){
        DPlayerData->DeleteAsset(DTarget);
    }
    DActor->PopCommand();
}

/**
 * @class CPlayerCapabilityBuildKnight
 *
 * @ brief Generates knight creation capability \n
 *   <pre>
 *    The class contains 2 other clases: CRegistrant and CActivatedCapability.
 *    CRegistrant determines which unit to build: knight.
 *    CActivatedCapability calls several functions: PercentComplete, IncrementStep, Cancel that determine the status of building knight.
 *   </pre>
 *
 *     Public Member Functions:
 *     CanInitiate: to return true if player is eligible for building knight.
 *     CanApply: to return true after calling CanInitiate function.
 *     ApplyCapability: to return true once building knight is applied.
 *
 * @author Spencer
 *
 * @version 9.0
 *
 * @date 12/3/17
 *
 * Contact: savandyke@ucdavis.edu
 *
 */
class CPlayerCapabilityBuildKnight : public CPlayerCapability{
    protected:
        /**
         * @class CRegistrant
         *
         * @ brief This class determines which unit to build: knight \n
         *
         * @author Spencer
         *
         * @version 9.0
         *
         * @date 12/3/17
         *
         * Contact: savandyke@ucdavis.edu
         *
         */
        class CRegistrant{
            public:
                CRegistrant();
        };
        static CRegistrant DRegistrant;

        /**
         * @class CActivatedCapability
         *
         * @ brief This calls several functions: PercentComplete, IncrementStep, and Cancel that determine the status of building knight. \n
         *
         * @author Spencer
         *
         * @version 9.0
         *
         * @date 12/3/17
         *
         * Contact: savandyke@ucdavis.edu
         *
         */
        class CActivatedCapability : public CActivatedPlayerCapability{
            protected:
                std::shared_ptr< CPlayerAssetType > DUpgradingType;
                std::string DUnitName;
                int DCurrentStep;
                int DTotalSteps;
                int DLumber;
                int DGold;
                int DStone;

            public:
                //destructor
                CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, std::shared_ptr< CPlayerAssetType > upgradingtype, const std::string &unitname, int lumber, int gold, int stone, int steps);
                virtual ~CActivatedCapability(){};

                int PercentComplete(int max);
                bool IncrementStep();
                void Cancel();
                void Save(std::ofstream& save);
                void Step(int step);
        };
        std::string DUnitName;
        CPlayerCapabilityBuildKnight(const std::string &unitname);

    public:
        virtual ~CPlayerCapabilityBuildKnight(){};

        bool CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message);
        bool CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
        bool ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target);
};

// creating new instance from CRegistrant
CPlayerCapabilityBuildKnight::CRegistrant CPlayerCapabilityBuildKnight::DRegistrant;

//constructor
CPlayerCapabilityBuildKnight::CRegistrant::CRegistrant(){
    CPlayerCapability::Register(std::shared_ptr< CPlayerCapabilityBuildKnight >(new CPlayerCapabilityBuildKnight("Knight")));
}

//constructor
CPlayerCapabilityBuildKnight::CPlayerCapabilityBuildKnight(const std::string &unitname) : CPlayerCapability(std::string("Build") + unitname, ETargetType::None){
    DUnitName = unitname;
}

/**
* checks if buiding knight can be intiated, under following conditions
*     <pre>
*     if asset type is BlackSmith then build knight
*       player's Lumber should be greater than lumbercost for building upgrade.
*       player's Gold should be greater than goldcost for building upgrade.
*       player's asset requirements are met
*     if asset type is Barracks then iterate through player's assettypes until knight found
*       player's Lumber should be greater than lumbercost for building upgrade.
*       player's Gold should be greater than goldcost for building upgrade.
*       player's food production should be greater than knight's foodconsumption and player's food consumption
*     </pre>
*
* @param[in] actor shared pointer from class PlayerAsset.
* @param[in] playerdata shared pointer from class PlayerData
*
* @return boolean value
*
*/
bool CPlayerCapabilityBuildKnight::CanInitiate(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::string& message){
    bool ReturnStatus = true;
    if(EAssetType::Blacksmith == actor->Type()){
        auto Upgrade = CPlayerUpgrade::FindUpgradeFromName(std::string("Build") + DUnitName);

        if(Upgrade){

            if(Upgrade->LumberCost() > playerdata->Lumber()){
                message += "Lumber:";
                message += std::to_string(Upgrade->LumberCost());
                message += " ";
                ReturnStatus = false;
            }
            if(Upgrade->GoldCost() > playerdata->Gold()){
                message += "Gold:";
                message += std::to_string(Upgrade->GoldCost());
                message += " ";
                ReturnStatus = false;
            }
            if(Upgrade->StoneCost() > playerdata->Stone()){
                message += "Stone:";
                message += std::to_string(Upgrade->StoneCost());
                message += " ";
                ReturnStatus = false;
            }
            if(!playerdata->AssetRequirementsMet(DUnitName, message)){
                ReturnStatus = false;
            }
        }
    }
    else if(EAssetType::Barracks == actor->Type()){
        auto AssetIterator = playerdata->AssetTypes()->find(DUnitName);

        if(AssetIterator != playerdata->AssetTypes()->end()){
            auto AssetType = AssetIterator->second;
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
        }
    }

    return ReturnStatus;
}

/**
* calls CanInitiate function to see if building of knight requirements met.
*
* @param[in] actor shared pointer from class PlayerAsset.
* @param[in] playerdata shared pointer from class PlayerData
* @param[in] target shared pointer from class PlayerAsset
*
* @return boolean value
*
*/
bool CPlayerCapabilityBuildKnight::CanApply(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    std::string dummyStr = "";
    return CanInitiate(actor, playerdata, dummyStr);
}

/**
* applies building knight capability based on whether asset type is blacksmith or barracks (for barracks - iterate through assettypes until knight)
*     <pre>
*     have actor apply building knight according to action, capability, asset target, activatedcapability.
*     </pre>
*
* @param[in] actor shared pointer from class PlayerAsset.
* @param[in] playerdata shared pointer from class PlayerData
* @param[in] target shared pointer from class PlayerAsset
*
* @return boolean value
*
*/
bool CPlayerCapabilityBuildKnight::ApplyCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    if(EAssetType::Blacksmith == actor->Type()){
        auto Upgrade = CPlayerUpgrade::FindUpgradeFromName(std::string("Build") + DUnitName);

        if(Upgrade){
            SAssetCommand NewCommand;

            actor->ClearCommand();
            NewCommand.DAction = EAssetAction::Capability;
            NewCommand.DCapability = AssetCapabilityType();
            NewCommand.DAssetTarget = target;
            NewCommand.DActivatedCapability = std::make_shared< CActivatedCapability >(actor, playerdata, target, actor->AssetType(), DUnitName, Upgrade->LumberCost(), Upgrade->GoldCost(), Upgrade->StoneCost(), CPlayerAsset::UpdateFrequency() * Upgrade->ResearchTime());
            actor->PushCommand(NewCommand);
            return true;
        }
    }
    else if(EAssetType::Barracks == actor->Type()){
        auto AssetIterator = playerdata->AssetTypes()->find(DUnitName);

        if(AssetIterator != playerdata->AssetTypes()->end()){
            auto AssetType = AssetIterator->second;
            auto NewAsset = playerdata->CreateAsset(DUnitName);
            SAssetCommand NewCommand;
            CTilePosition TilePosition;
            TilePosition.SetFromPixel(actor->Position());
            NewAsset->TilePosition(TilePosition);
            NewAsset->HitPoints(1);

            NewCommand.DAction = EAssetAction::Capability;
            NewCommand.DCapability = AssetCapabilityType();
            NewCommand.DAssetTarget = NewAsset;
            NewCommand.DActivatedCapability = std::make_shared< CActivatedCapability >(actor, playerdata, NewAsset, actor->AssetType(), DUnitName, AssetType->LumberCost(), AssetType->GoldCost(), AssetType->StoneCost(), CPlayerAsset::UpdateFrequency() * AssetType->BuildTime());
            actor->PushCommand(NewCommand);
        }
    }
    return false;
}

/**
* constructor
*
* @param[in] actor shared pointer from class PlayerAsset
* @param[in] playerdata shared pointer from class PlayerData
* @param[in] target shared pointer from class PlayerAsset
* @param[in] upgradingtype shared pointer from class PlayerAssetType
* @param[in] const unitname
* @param[in] int amt of lumber
* @param[in] int amt of gold
* @param[in] int amt of steps
*
*/
CPlayerCapabilityBuildKnight::CActivatedCapability::CActivatedCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target, std::shared_ptr< CPlayerAssetType > upgradingtype, const std::string &unitname, int lumber, int gold, int stone, int steps) :
CActivatedPlayerCapability(actor, playerdata, target){
    DUnitName = unitname;
    DCurrentStep = 0;
    DTotalSteps = steps;
    DLumber = lumber;
    DGold = gold;
    DStone = stone;
    DPlayerData->DecrementLumber(DLumber);
    DPlayerData->DecrementGold(DGold);
    DPlayerData->DecrementStone(DStone);
    if(EAssetType::Blacksmith == actor->Type()){
        DUpgradingType = upgradingtype;
        DUpgradingType->RemoveCapability(CPlayerCapability::NameToType(std::string("Build") + DUnitName));
    }
    else if(EAssetType::Barracks == actor->Type()){
        SAssetCommand AssetCommand;

        AssetCommand.DAction = EAssetAction::Construct;
        AssetCommand.DAssetTarget = DActor;
        DTarget->PushCommand(AssetCommand);
    }
}

/**
* Saves activated capability to an output file stream
*
* @param[in] save Output file stream
*
* @return void
*
*/

void CPlayerCapabilityBuildKnight::CActivatedCapability::Save(std::ofstream& save){
    save << "#Capability Type\n";
    save << "UNITUPGRADE" << std::endl;

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

    // std::shared_ptr< CPlayerAssetType > DUpgradingType;
    save << "#Upgrading Type\n";
    if(DUpgradingType)
        save << DUpgradingType->Name() << std::endl;
    else
        save << "NULL\n";

    // std::string DUpgradeName;
    save << "#Upgrading Name\n";
    save << DUnitName << std::endl;

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

void CPlayerCapabilityBuildKnight::CActivatedCapability::Step(int step){
    DCurrentStep = step;
}

/**
* completion of action (shown via the health bar in the game UI)
*
* @param[in] max health value
*
* @return int value of health completion
*
*/
int CPlayerCapabilityBuildKnight::CActivatedCapability::PercentComplete(int max){
    return DCurrentStep * max / DTotalSteps;
}

/**
* Set/initilize the next steps until building knight is complete.
*
* @param[in] nothing
*
* @return boolean value
*
*/
bool CPlayerCapabilityBuildKnight::CActivatedCapability::IncrementStep(){
    if(EAssetType::Barracks == DActor->Type()){
        int AddHitPoints = (DTarget->MaxHitPoints() * (DCurrentStep + 1) / DTotalSteps) - (DTarget->MaxHitPoints() * DCurrentStep / DTotalSteps);

        DTarget->IncrementHitPoints(AddHitPoints);
        if(DTarget->HitPoints() > DTarget->MaxHitPoints()){
            DTarget->HitPoints(DTarget->MaxHitPoints());
        }
    }

    DCurrentStep++;
    DActor->IncrementStep();
    if(DCurrentStep >= DTotalSteps){
        SGameEvent TempEvent;

        if(EAssetType::Blacksmith == DActor->Type()){
            auto BarracksIterator = DPlayerData->AssetTypes()->find("Barracks");
            auto KnightIterator = DPlayerData->AssetTypes()->find("Knight");
            auto BlacksmithIterator = DPlayerData->AssetTypes()->find("Blacksmith");

            TempEvent.DType = EEventType::WorkComplete;
            TempEvent.DAsset = DActor;

            BarracksIterator->second->AddCapability(EAssetCapabilityType::BuildKnight);
            BarracksIterator->second->RemoveCapability(EAssetCapabilityType::BuildFootman);
            // Use for future knight upgrades
            // BlacksmithIterator->second->AddCapability(EAssetCapabilityType::None);
            // Upgrade all Archers
            for(auto WeakAsset : DPlayerData->Assets()){
                if(auto Asset = WeakAsset.lock()){
                    if(EAssetType::Footman == Asset->Type()){
                        int HitPointIncrement = KnightIterator->second->HitPoints() - Asset->MaxHitPoints();

                        Asset->ChangeType(KnightIterator->second);
                        Asset->IncrementHitPoints(HitPointIncrement);
                    }
                }
            }
        }
        else if(EAssetType::Barracks == DActor->Type()){
            TempEvent.DType = EEventType::Ready;
            TempEvent.DAsset = DTarget;

            DTarget->PopCommand();
            DTarget->TilePosition(DPlayerData->PlayerMap()->FindAssetPlacement(DTarget, DActor, CTilePosition(DPlayerData->PlayerMap()->Width()-1, DPlayerData->PlayerMap()->Height()-1)));
        }
        DPlayerData->AddGameEvent(TempEvent);
        DActor->PopCommand();
        return true;
    }
    return false;
}

/**
* Cancels building knight capability and reincrements the assets for lumber and gold and pops command, dependent on whether asset type is Blacksmith or Barracks.
*
* @param[in] nothing
*
*/
void CPlayerCapabilityBuildKnight::CActivatedCapability::Cancel(){
    DPlayerData->IncrementLumber(DLumber);
    DPlayerData->IncrementGold(DGold);
    DPlayerData->IncrementStone(DStone);
    if(EAssetType::Blacksmith == DActor->Type()){
    	if(DUpgradingType == nullptr) {
    		PrintDebug(DEBUG_LOW, "DUpgradingType == nullptr\n");
    	}
        DUpgradingType->AddCapability(CPlayerCapability::NameToType(std::string("Build") + DUnitName));
    }
    else if(EAssetType::Barracks == DActor->Type()){
    	PrintDebug(DEBUG_LOW, "Trying to delete Knight capability\n");
        DPlayerData->DeleteAsset(DTarget);
        PrintDebug(DEBUG_LOW, "Finished deleting Knight capability\n");
    }
    DActor->PopCommand();
}
