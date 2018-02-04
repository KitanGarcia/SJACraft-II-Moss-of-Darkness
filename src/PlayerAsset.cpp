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


#include "PlayerAsset.h"
#include "TerrainMap.h"
#include "ApplicationData.h"
#include "CommentSkipLineDataSource.h"
#include "Debug.h"
#include "GameModel.h"
#include <algorithm>
#include <Tokenizer.h>


std::unordered_map< std::string, std::shared_ptr< CPlayerAssetType > > CPlayerAssetType::DRegistry;
std::vector< std::string > CPlayerAssetType::DTypeStrings({
"None",
"Peasant",
"Footman",
"Archer",
"Ranger",
"Knight",
"GoldMine",
"TownHall",
"Keep",
"Castle",
"GoldVein",
"Farm",
"Barracks",
"LumberMill",
"Blacksmith",
"ScoutTower",
"GuardTower",
"CannonTower",
"Wall"});
std::unordered_map< std::string, EAssetType > CPlayerAssetType::DNameTypeTranslation({
{"None", EAssetType::None},
{"Peasant", EAssetType::Peasant},
{"Footman", EAssetType::Footman},
{"Archer", EAssetType::Archer},
{"Ranger", EAssetType::Ranger},
{"Knight", EAssetType::Knight},
{"GoldMine", EAssetType::GoldMine},
{"TownHall", EAssetType::TownHall},
{"Keep", EAssetType::Keep},
{"Castle", EAssetType::Castle},
{"GoldVein", EAssetType::GoldVein},
{"Farm", EAssetType::Farm},
{"Barracks", EAssetType::Barracks},
{"LumberMill", EAssetType::LumberMill},
{"Blacksmith", EAssetType::Blacksmith},
{"ScoutTower", EAssetType::ScoutTower},
{"GuardTower", EAssetType::GuardTower},
{"CannonTower", EAssetType::CannonTower},
{"Wall", EAssetType::Wall}});

/**
*
* @class CPlayerAsset.cpp
*
* @brief This class was written to activate player capability. It choose capability
*        data and Asset target and shows steps of percentage complete.
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


/**
* Constructor of the class CActivatedPlayerCapability.
*
* @param[in] actor shared pointer to the class of PlayerAsset.
* @param[in] playerdata shared pointer to the class of PlayerData
* @param[in] target shared pointer to the class of PlayerAsset
*
*/

CActivatedPlayerCapability::CActivatedPlayerCapability(std::shared_ptr< CPlayerAsset > actor, std::shared_ptr< CPlayerData > playerdata, std::shared_ptr< CPlayerAsset > target){
    DActor = actor;
    DPlayerData = playerdata;
    DTarget = target;
}

/**
*
* @class CPlayerCapability.cpp
*
* @brief This class was written to determine a capability by type or name or NameRegistry
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

/**
* Constructor of the class CPlayerCapability.
*
* @param[in] name reference to string of type name.
* @param[in] targettype Enum to ETargetType
*
*/

CPlayerCapability::CPlayerCapability(const std::string &name, ETargetType targettype){
    DName = name;
    DAssetCapabilityType = NameToType(name);
    DTargetType = targettype;
}

/**
* Assigns/returns  unordered_map , name registery of player capability.
*
* @return a reference to an unordered_map of registered player capabilities
*
*/

std::unordered_map< std::string, std::shared_ptr< CPlayerCapability > > &CPlayerCapability::NameRegistry(){
    static std::unordered_map< std::string, std::shared_ptr< CPlayerCapability > > TheRegistry;

    return TheRegistry;
}

/**
* Assigns/returns  unordered_map , type registry of player capabilities
*
* @return A reference to an unordered_map of the registered player capabilities
*
*/

std::unordered_map< int, std::shared_ptr< CPlayerCapability > > &CPlayerCapability::TypeRegistry(){
    static std::unordered_map< int, std::shared_ptr< CPlayerCapability > > TheRegistry;

    return TheRegistry;
}

/**
* Set/Register a capability to a player.
* It checks if player already has the capability, if not calls the NameRegistry()
* and TypeRegistry()function to register name and type of capability
*
* @param[in] capability shared_ptr CPlayerCapability
*
* @return False if name of capability cannot be found
*
*/

bool CPlayerCapability::Register(std::shared_ptr< CPlayerCapability > capability){
    if(nullptr != FindCapability(capability->Name())){
        return false;
    }
    NameRegistry()[capability->Name()] = capability;
    TypeRegistry()[to_underlying(NameToType(capability->Name()))] = capability;
    return true;
}

/**
* Find capability based on its existence in TypeRegistry
*
* @param[in] type Enum to EAssetCapabilityType
*
* @return shared_ptr to CPlayerCapability
*
*/

std::shared_ptr< CPlayerCapability > CPlayerCapability::FindCapability(EAssetCapabilityType type){
    auto Iterator = TypeRegistry().find(to_underlying(type));

    if(Iterator != TypeRegistry().end()){
        return Iterator->second;
    }
    return std::shared_ptr< CPlayerCapability >();
}

/**
* Find capability based on its existence in NameRegistry
*
* @param[in] name reference to name of capability
*
* @return shared_ptr to CPlayerCapability
*
*/

std::shared_ptr< CPlayerCapability > CPlayerCapability::FindCapability(const std::string &name){
    auto Iterator = NameRegistry().find(name);

    if(Iterator != NameRegistry().end()){
        return Iterator->second;
    }
    return std::shared_ptr< CPlayerCapability >();
}

/**
* Get the name of capability and return its corresponding type.
*     It contains a static unordered_map to name and type. By giving the
*     name of capability it search through the map and if find it return the type
*     if not it print an Error and return asset type None
*
* @param[in] name reference to string
*
* @return The AssetCapability type that is mapped to the parameter
*
*/

EAssetCapabilityType CPlayerCapability::NameToType(const std::string &name){
    static std::unordered_map< std::string, EAssetCapabilityType > NameTypeTranslation({
        {"None",EAssetCapabilityType::None},
        {"BuildPeasant",EAssetCapabilityType::BuildPeasant},
        {"BuildFootman",EAssetCapabilityType::BuildFootman},
        {"BuildArcher",EAssetCapabilityType::BuildArcher},
        {"BuildRanger",EAssetCapabilityType::BuildRanger},
		{"BuildKnight",EAssetCapabilityType::BuildKnight},
        {"BuildFarm",EAssetCapabilityType::BuildFarm},
        {"BuildTownHall",EAssetCapabilityType::BuildTownHall},
        {"BuildBarracks",EAssetCapabilityType::BuildBarracks},
        {"BuildLumberMill",EAssetCapabilityType::BuildLumberMill},
        {"BuildBlacksmith",EAssetCapabilityType::BuildBlacksmith},
        {"BuildKeep",EAssetCapabilityType::BuildKeep},
        {"BuildCastle",EAssetCapabilityType::BuildCastle},
        {"BuildScoutTower",EAssetCapabilityType::BuildScoutTower},
        {"BuildGuardTower",EAssetCapabilityType::BuildGuardTower},
        {"BuildCannonTower",EAssetCapabilityType::BuildCannonTower},
        {"Move",EAssetCapabilityType::Move},
        {"Repair",EAssetCapabilityType::Repair},
        {"Mine",EAssetCapabilityType::Mine},
        {"BuildSimple",EAssetCapabilityType::BuildSimple},
        {"BuildAdvanced",EAssetCapabilityType::BuildAdvanced},
        {"Convey",EAssetCapabilityType::Convey},
        {"Cancel",EAssetCapabilityType::Cancel},
        {"BuildWall",EAssetCapabilityType::BuildWall},
        {"BuildGoldMine",EAssetCapabilityType::BuildGoldMine},
        {"Attack",EAssetCapabilityType::Attack},
        {"StandGround",EAssetCapabilityType::StandGround},
        {"Patrol",EAssetCapabilityType::Patrol},
        {"WeaponUpgrade1",EAssetCapabilityType::WeaponUpgrade1},
        {"WeaponUpgrade2",EAssetCapabilityType::WeaponUpgrade2},
        {"WeaponUpgrade3",EAssetCapabilityType::WeaponUpgrade3},
        {"ArrowUpgrade1",EAssetCapabilityType::ArrowUpgrade1},
        {"ArrowUpgrade2",EAssetCapabilityType::ArrowUpgrade2},
        {"ArrowUpgrade3",EAssetCapabilityType::ArrowUpgrade3},
        {"ArmorUpgrade1",EAssetCapabilityType::ArmorUpgrade1},
        {"ArmorUpgrade2",EAssetCapabilityType::ArmorUpgrade2},
        {"ArmorUpgrade3",EAssetCapabilityType::ArmorUpgrade3},
        {"Longbow",EAssetCapabilityType::Longbow},
        {"RangerScouting",EAssetCapabilityType::RangerScouting},
        {"Marksmanship",EAssetCapabilityType::Marksmanship},
        {"RangerTrackingUpgrade",EAssetCapabilityType::RangerTrackingUpgrade},
        {"Shelter",EAssetCapabilityType::Shelter}
    });
    auto Iterator = NameTypeTranslation.find(name);

    if(Iterator != NameTypeTranslation.end()){
        return Iterator->second;
    }
    PrintError("Unknown capability name \"%s\"\n",name.c_str());
    return EAssetCapabilityType::None;
}

/**
* Get the type of capability and return its corresponding name.
*     It contains a static vector to contain types. By giving the
*     type it checks its validation and return the name of the type
*
* @param[in] type EAssetCapabilityType that needs to be converted to string
*
* @return the name of the capability as a string
*
*/

std::string CPlayerCapability::TypeToName(EAssetCapabilityType type){
    static std::vector< std::string > TypeStrings({
        "None",
        "BuildPeasant",
        "BuildFootman",
        "BuildArcher",
        "BuildRanger",
		"BuildKnight",
        "BuildFarm",
        "BuildTownHall",
        "BuildBarracks",
        "BuildLumberMill",
        "BuildBlacksmith",
        "BuildKeep",
        "BuildCastle",
        "BuildScoutTower",
        "BuildGuardTower",
        "BuildCannonTower",
        "Move",
        "Repair",
        "Mine",
        "BuildSimple",
        "BuildAdvanced",
        "Convey",
        "Cancel",
        "BuildWall",
        "BuildGoldMine",
        "Attack",
        "StandGround",
        "Patrol",
        "WeaponUpgrade1",
        "WeaponUpgrade2",
        "WeaponUpgrade3",
        "ArrowUpgrade1",
        "ArrowUpgrade2",
        "ArrowUpgrade3",
        "ArmorUpgrade1",
        "ArmorUpgrade2",
        "ArmorUpgrade3",
        "Longbow",
        "RangerScouting",
        "Marksmanship",
        "RangerTrackingUpgrade",
        "Shelter",});
    
    if((0 > to_underlying(type))||(to_underlying(type) >= TypeStrings.size())){
        return "";
    }
    return TypeStrings[to_underlying(type)];
}


std::unordered_map< std::string, std::shared_ptr< CPlayerUpgrade > > CPlayerUpgrade::DRegistryByName;
std::unordered_map< int, std::shared_ptr< CPlayerUpgrade > > CPlayerUpgrade::DRegistryByType;

/**
*
* @class CPlayerUpgrade.cpp
*
* @brief This class was written to upgrade player capabilities. it has list of all
*        capabilities and load upgrades finding them from type or name
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

CPlayerUpgrade::CPlayerUpgrade(){

}

/**
* Upgrade/Load the player capability
*     It assign container pointer to the CDataContainer FileIterator. if the container
*     is empty it return false otherwise if  container is valid it reads the Data
*     from the next file. if .dat data filename be equal container name and if it
*     can load container's DataSource it upgrades player capability
*
* @param[in] container a shared_ptr to CDataContainer
*
* @return True if player is upgraded.
*
*/

bool CPlayerUpgrade::LoadUpgrades(std::shared_ptr< CDataContainer > container){
    auto FileIterator = container->First();
    if(FileIterator == nullptr){
        PrintError("FileIterator == nullptr\n");
        return false;
    }

    while((FileIterator != nullptr)&&(FileIterator->IsValid())){
        std::string Filename = FileIterator->Name();
        FileIterator->Next();
        if(Filename.rfind(".dat") == (Filename.length() - 4)){
            if(!CPlayerUpgrade::Load(container->DataSource(Filename))){
                PrintError("Failed to load upgrade \"%s\".\n",Filename.c_str());
                continue;
            }
            else{
                PrintDebug(DEBUG_LOW,"Loaded upgrade \"%s\".\n",Filename.c_str());
            }
        }
    }

    return true;
}

/**
* Determine the condition for load upgrate
*     - If the source file is empty or if the LineSource could not read the name
*       of the upgrade it return false
*     - If upgrade type and player capability name is equal "None" it returns false.
*       else it assigns the upgrade name and type to DRegistryByName and DRegistryByType
*     - try to upgrade all the capabilities in order of DArmor, DSight, DSpeed, DBasicDamage,
*       DPiercingDamage, DRange, DGoldCost, DLumberCost, DResearchTime, DAffectedAssets. anytime
*       there is no more line to read it prints an Error and it goes to the LoadExit lable
*     - if it could not read the file it catch an Exception and print an Error
*
* @param[in] source a shared_ptr to CDataSource
*
* @return True if load was completed
*
*/

bool CPlayerUpgrade::Load(std::shared_ptr< CDataSource > source){
    CCommentSkipLineDataSource LineSource(source, '#');
    std::string Name, TempString;
    std::shared_ptr< CPlayerUpgrade > PlayerUpgrade;
    EAssetCapabilityType UpgradeType;
    int AssetRequirementCount;
    int AffectedAssetCount;
    bool ReturnStatus = false;

    if(nullptr == source){
        return false;
    }
    if(!LineSource.Read(Name)){
        PrintError("Failed to get player upgrade name.\n");
        return false;
    }
    UpgradeType = CPlayerCapability::NameToType(Name);
    if((EAssetCapabilityType::None == UpgradeType) && (Name != CPlayerCapability::TypeToName(EAssetCapabilityType::None))){
        PrintError("Unknown upgrade type %s.\n", Name.c_str());
        return false;
    }
    auto Iterator = DRegistryByName.find(Name);
    if(DRegistryByName.end() != Iterator){
        PlayerUpgrade = Iterator->second;
    }
    else{
        PlayerUpgrade = std::make_shared< CPlayerUpgrade >();
        PlayerUpgrade->DName = Name;
        DRegistryByName[Name] = PlayerUpgrade;
        DRegistryByType[to_underlying(UpgradeType)] = PlayerUpgrade;
    }
    try{
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade armor.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DArmor = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade sight.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DSight = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade speed.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DSpeed = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade basic damage.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DBasicDamage = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade piercing damage.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DPiercingDamage = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade range.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DRange = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade gold cost.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DGoldCost = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade lumber cost.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DLumberCost = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade stone cost.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DStoneCost = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade research time.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DResearchTime = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade1 affected asset count.\n");

            goto LoadExit;
        }
        AffectedAssetCount = std::stoi(TempString);
        for(int Index = 0; Index < AffectedAssetCount; Index++){
            if(!LineSource.Read(TempString)){
                PrintError("Failed to read upgrade affected asset %d.\n", Index);
                goto LoadExit;
            }
            PlayerUpgrade->DAffectedAssets.push_back(CPlayerAssetType::NameToType(TempString));
        }
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get upgrade2 affected asset count.\n");
            goto LoadExit;
        }
        AssetRequirementCount = std::stoi(TempString);
        for(int Index = 0; Index < AssetRequirementCount; Index++){
            if(!LineSource.Read(TempString)){
                PrintError("Failed to read upgrade affected asset %d.\n", Index);
                goto LoadExit;
            }
            PlayerUpgrade->DAssetRequirements.push_back(CPlayerAssetType::NameToType(TempString));
        }
        ReturnStatus = true;
    }
    catch(std::exception &E){
        PrintError("%s\n",E.what());
    }
LoadExit:
    return ReturnStatus;
}

/**
* Find upgrade by looking at its type
*     if it finds the upgrade by its type, it returns the upgrade.
*
* @param[in] type An Enum to EAssetCapabilityType
*
* @return a shared_ptr to CPlayerUpgrade
*
*/

std::shared_ptr< CPlayerUpgrade > CPlayerUpgrade::FindUpgradeFromType(EAssetCapabilityType type){
    auto Iterator = DRegistryByType.find(to_underlying(type));

    if(Iterator != DRegistryByType.end()){
        return Iterator->second;
    }
    return std::shared_ptr< CPlayerUpgrade >();
}

/**
* Find upgrade by looking at its name.
*     if it finds the upgrade by its name, it returns the upgrade.
*
* @param[in] type enum to EAssetCapabilityType
*
* @return a shared_ptr to CPlayerUpgrade
*
*/

std::shared_ptr< CPlayerUpgrade > CPlayerUpgrade::FindUpgradeFromName(const std::string &name){
    auto Iterator = DRegistryByName.find( name );

    if(Iterator != DRegistryByName.end()){
        return Iterator->second;
    }
    return std::shared_ptr< CPlayerUpgrade >();
}

/**
*
* @class CPlayerAssetType.cpp
*
* @brief This class was written to upgrade asset type capabilities. it has list of all
*        Asset types and load and upgrades them from the new asset
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

/**
* Default constructor CPlayerAssetType
*     it resizes DCapabilities and assigns a false to all of them. also initilizes
*     other capabilities.
*
*/

CPlayerAssetType::CPlayerAssetType(){
    DCapabilities.resize(to_underlying(EAssetCapabilityType::Max));
    for(int Index = 0; Index < DCapabilities.size(); Index++){
        DCapabilities[Index] = false;
    }
    DHitPoints = 1;
    DArmor = 0;
    DSight = 0;
    DConstructionSight = 0;
    DSize = 1;
    DSpeed = 0;
    DGoldCost = 0;
    DLumberCost = 0;
    DStoneCost = 0;
    DFoodConsumption = 0;
    DBuildTime = 0;
    DAttackSteps = 0;
    DReloadSteps = 0;
    DBasicDamage = 0;
    DPiercingDamage = 0;
    DRange = 0;
}

/**
* Copy Constructor CPlayerAssetType
*     it gets an asset and initiates the player with its capabilities
*
* @param[in] asset shared_ptr to CPlayerAssetType
*
*/

CPlayerAssetType::CPlayerAssetType(std::shared_ptr< CPlayerAssetType > asset){
    if(nullptr != asset){
        DName = asset->DName;
        DType = asset->DType;
        DColor = asset->DColor;
        DCapabilities = asset->DCapabilities;
        DAssetRequirements = asset->DAssetRequirements;
        DHitPoints = asset->DHitPoints;
        DArmor = asset->DArmor;
        DSight = asset->DSight;
        DConstructionSight = asset->DConstructionSight;
        DSize = asset->DSize;
        DSpeed = asset->DSpeed;
        DGoldCost = asset->DGoldCost;
        DLumberCost = asset->DLumberCost;
        DStoneCost = asset->DStoneCost;
        DFoodConsumption = asset->DFoodConsumption;
        DBuildTime = asset->DBuildTime;
        DAttackSteps = asset->DAttackSteps;
        DReloadSteps = asset->DReloadSteps;
        DBasicDamage = asset->DBasicDamage;
        DPiercingDamage = asset->DPiercingDamage;
        DRange = asset->DRange;
    }
}

/**
* Destructor CPlayerAssetType
*
*/

CPlayerAssetType::~CPlayerAssetType(){

}

/**
* Do upgrade on Armor over range current capability and new DAssetUpgrades
*
* @return current total Armor value for all AssetUpgrades
*
*/

int CPlayerAssetType::ArmorUpgrade() const{
    int ReturnValue = 0;
    for(auto Upgrade : DAssetUpgrades){
        ReturnValue += Upgrade->Armor();
    }
    return ReturnValue;
}

/**
* Do upgrade on Sight over range current capability and new DAssetUpgrades
*
* @return current total Sight value for all AssetUpgrades
*
*/

int CPlayerAssetType::SightUpgrade() const{
    int ReturnValue = 0;
    for(auto Upgrade : DAssetUpgrades){
        ReturnValue += Upgrade->Sight();
    }
    return ReturnValue;
}

/**
* Do upgrade on Speed over range current capability and new DAssetUpgrades
*
* @return current total Speed value for all AssetUpgrades
*
*/

int CPlayerAssetType::SpeedUpgrade() const{
    int ReturnValue = 0;
    for(auto Upgrade : DAssetUpgrades){
        ReturnValue += Upgrade->Speed();
    }
    return ReturnValue;
}

/**
* Do upgrade on BasicDamage over range current capability and new DAssetUpgrades
*
* @return current total BasicDamage value for all AssetUpgrades
*
*/

int CPlayerAssetType::BasicDamageUpgrade() const{
    int ReturnValue = 0;
    for(auto Upgrade : DAssetUpgrades){
        ReturnValue += Upgrade->BasicDamage();
    }
    return ReturnValue;
}

/**
* Do upgrade on PiercingDamage over range current capability and new DAssetUpgrades
*
* @return current total Piercing Damage value for all AssetUpgrades
*
*/

int CPlayerAssetType::PiercingDamageUpgrade() const{
    int ReturnValue = 0;
    for(auto Upgrade : DAssetUpgrades){
        ReturnValue += Upgrade->PiercingDamage();
    }
    return ReturnValue;
}

/**
* Do upgrade on Range over range current capability and new DAssetUpgrades
*
* @return current total Range value for all AssetUpgrades
*
*/

int CPlayerAssetType::RangeUpgrade() const{
    int ReturnValue = 0;
    for(auto Upgrade : DAssetUpgrades){
        ReturnValue += Upgrade->Range();
    }
    return ReturnValue;
}

/**
* Go through all the capabilities and if a capability is true, push_back it in the vector
*
* @return vector of the indexes of existing capabilities
*
*/

std::vector< EAssetCapabilityType > CPlayerAssetType::Capabilities() const{
    std::vector< EAssetCapabilityType > ReturnVector;

    for(int Index = to_underlying(EAssetCapabilityType::None); Index < to_underlying(EAssetCapabilityType::Max); Index++){
        if(DCapabilities[Index]){
            ReturnVector.push_back((EAssetCapabilityType)Index);
        }
    }
    return ReturnVector;
}

/**
* Gets the name of a capability and return its type
* if it finds the name it returns its type or None if no success
*
* @param[in] name reference to string
*
* @return The EAssetType associated with parameter
*
*/

EAssetType CPlayerAssetType::NameToType(const std::string &name){
    auto Iterator = DNameTypeTranslation.find(name);
    if(Iterator != DNameTypeTranslation.end()){
        return Iterator->second;
    }
    return EAssetType::None;
}

/**
* Gets the type of a capability and return its name
* if it finds the type it returns its name or empty string if no success
*
* @param[in] type The EAssetType that is being converted
*
* @return The string associated with the parameter
*
*/

std::string CPlayerAssetType::TypeToName(EAssetType type){
    if((0 > to_underlying(type))||(to_underlying(type) >= DTypeStrings.size())){
        return "";
    }
    return DTypeStrings[to_underlying(type)];
}

/**
* Upgrade the max sight can be seen by player
* if the maxsight is samller it keeps the maxsight otherwise it create the maxsight
*
* @return The maxsight number that player asset can seen
*
*/

int CPlayerAssetType::MaxSight(){
    int MaxSightFound = 0;

    for(auto &ResType : DRegistry){
        MaxSightFound = MaxSightFound > ResType.second->DSight + ResType.second->DSize ? MaxSightFound : ResType.second->DSight + ResType.second->DSize;
    }
    return MaxSightFound;
}

/**
* Upgrade/Load the PalyerAssettype capability
*     It assigns container pointer to the CDataContainer FileIterator. If the container
*     is empty it return false otherwise if  container is valid it reads the Data
*     from the next file. if .dat data filename be equal container name and if it
*     can load container's DataSource it upgrades player capability type
*
* @param[in] container a shared_ptr to CDataContainer
*
* @return True if all the types are loaded from the container
*
*/

bool CPlayerAssetType::LoadTypes(std::shared_ptr< CDataContainer > container){
    auto FileIterator = container->First();
    if(FileIterator == nullptr){
        PrintError("FileIterator == nullptr\n");
        return false;
    }

    while((FileIterator != nullptr)&&(FileIterator->IsValid())){
        std::string Filename = FileIterator->Name();
        FileIterator->Next();
        if(Filename.rfind(".dat") == (Filename.length() - 4)){
            if(!CPlayerAssetType::Load(container->DataSource(Filename))){
                PrintError("Failed to load resource \"%s\".\n",Filename.c_str());
                continue;
            }
            else{
                PrintDebug(DEBUG_LOW,"Loaded resource \"%s\".\n",Filename.c_str());
            }
        }
    }
    std::shared_ptr< CPlayerAssetType >  PlayerAssetType = std::make_shared< CPlayerAssetType >();
    PlayerAssetType->DThis = PlayerAssetType;
    PlayerAssetType->DName = "None";
    PlayerAssetType->DType = EAssetType::None;
    PlayerAssetType->DColor = EPlayerColor::None;
    PlayerAssetType->DHitPoints = 256;
    DRegistry["None"] = PlayerAssetType;

    return true;
}

/**
* Determine the condition for type load upgrate
*     - If the source file is empty or if the LineSource could not read the name
*       of the upgrade it return false
*     - If upgrade type and palyer capabilty type is equal "None" it returns false.
*       else it assigns name and type and color of the type to player
*     - try to upgrade all the capabilities in order of DHitPoints, DArmor, DSight,DConstructionSight, Dsize, DSpeed,
*       DGoldCost, DLumberCost, DFoodConsumption,DBuildTime, DAttackSteps,DReloadSteps,
*       DBasicDamage, DPiercingDamage, DRange, DCapabilities, DAssetRequirements. anytime
*       there is no more line to read it prints an Error and it goes to the LoadExit label
*     - if it could not read the file it catch an Exception and print an Error
*
* @param[in] source a shared_ptr to CDataSource
*
* @return True if load was completed
*
*/

bool CPlayerAssetType::Load(std::shared_ptr< CDataSource > source){
    CCommentSkipLineDataSource LineSource(source, '#');
    std::string Name, TempString;
    std::shared_ptr< CPlayerAssetType > PlayerAssetType;
    EAssetType AssetType;
    int CapabilityCount, AssetRequirementCount;
    bool ReturnStatus = false;

    if(nullptr == source){
    	PrintDebug(DEBUG_LOW, "Failed nullptr == source\n");
        return false;
    }
    if(!LineSource.Read(Name)){
        PrintError("Failed to get resource type name.\n");
        return false;
    }
    AssetType = NameToType(Name);
    if((EAssetType::None == AssetType) && (Name != DTypeStrings[to_underlying(EAssetType::None)])){
        PrintError("Unknown resource type %s.\n", Name.c_str());
        return false;
    }
    auto Iterator = DRegistry.find(Name);
    if(DRegistry.end() != Iterator){
        PlayerAssetType = Iterator->second;
    }
    else{
        PlayerAssetType = std::make_shared< CPlayerAssetType >();
        PlayerAssetType->DThis = PlayerAssetType;
        PlayerAssetType->DName = Name;
        DRegistry[Name] = PlayerAssetType;
    }
    int flag = 0;
    PlayerAssetType->DType = AssetType;
    PlayerAssetType->DColor = EPlayerColor::None;
    try{
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type hit points.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DHitPoints = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type armor.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DArmor = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type sight.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DSight = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type construction sight.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DConstructionSight = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type size.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DSize = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type speed.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DSpeed = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type gold cost.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DGoldCost = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type lumber cost.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DLumberCost = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type stone cost.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DStoneCost = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type food consumption.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DFoodConsumption = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type build time.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DBuildTime = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type attack steps.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DAttackSteps = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type reload steps.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DReloadSteps = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type basic damage.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DBasicDamage = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type piercing damage.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DPiercingDamage = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get resource type range.\n");
            goto LoadExit;
        }
        flag++;
        PlayerAssetType->DRange = std::stoi(TempString);
        if(!LineSource.Read(TempString)){
            PrintError("Failed to get capability count.\n");
            goto LoadExit;
        }
        flag++;
        CapabilityCount = std::stoi(TempString);
        PlayerAssetType->DCapabilities.resize(to_underlying(EAssetCapabilityType::Max));
        for(int Index = 0; Index < PlayerAssetType->DCapabilities.size(); Index++){
            PlayerAssetType->DCapabilities[Index] = false;
        }
        for(int Index = 0; Index < CapabilityCount; Index++){
            if(!LineSource.Read(TempString)){
                PrintError("Failed to read capability %d.\n", Index);
                goto LoadExit;
            }
            PlayerAssetType->AddCapability(CPlayerCapability::NameToType(TempString));
        }

        if(!LineSource.Read(TempString)){
            PrintError("Failed to get asset requirement count.\n");
            goto LoadExit;
        }
        flag++;
        AssetRequirementCount = std::stoi(TempString);
        for(int Index = 0; Index < AssetRequirementCount; Index++){
            if(!LineSource.Read(TempString)){
                PrintError("Failed to read asset requirement %d.\n", Index);
                goto LoadExit;
            }
            PlayerAssetType->DAssetRequirements.push_back(NameToType(TempString));
        }

        flag++;
        ReturnStatus = true;
    }
    catch(std::exception &E){
    	PrintDebug(DEBUG_LOW, "exception %d, could not stoi %s\n", flag, TempString.c_str());
        PrintError("%s\n",E.what());
    }
LoadExit:
    return ReturnStatus;
}

/**
* Find type of upgrade by looking at its name.
*     if it finds the upgrade by its name, it returns the type of upgrade.
*
* @param[in] name a reference to string
*
* @return a shared_ptr to the CPlayerAssetType default
*
*/

std::shared_ptr< CPlayerAssetType > CPlayerAssetType::FindDefaultFromName(const std::string &name){
    auto Iterator = DRegistry.find( name );

    if(Iterator != DRegistry.end()){
        return Iterator->second;
    }
    return std::shared_ptr< CPlayerAssetType >();
}

/**
* Find type of upgrade by looking at its TYPE
*     if it finds the upgrade by its type, it returns the type of upgrade.
*
* @param[in] type Enum to EAssetType
*
* @return a shared_ptr to CPlayerAssetType
*
*/

std::shared_ptr< CPlayerAssetType > CPlayerAssetType::FindDefaultFromType(EAssetType type){
    return FindDefaultFromName( TypeToName(type) );
}

/**
* Duplice a player asset
*
* @param[in] color Enum to EPlayerColor
*
* @return a shared_ptr to an unordered_map resistry of NewAssetTypes
*
*/

std::shared_ptr< std::unordered_map< std::string, std::shared_ptr< CPlayerAssetType > > > CPlayerAssetType::DuplicateRegistry(EPlayerColor color){
    std::shared_ptr< std::unordered_map< std::string, std::shared_ptr< CPlayerAssetType > > > ReturnRegistry( new std::unordered_map< std::string, std::shared_ptr< CPlayerAssetType > > );

    for(auto &It : DRegistry){
        std::shared_ptr< CPlayerAssetType > NewAssetType(new CPlayerAssetType(It.second));
        NewAssetType->DThis = NewAssetType;
        if(NewAssetType->Type() == EAssetType::Wall || NewAssetType->Type() == EAssetType::GoldMine){
            NewAssetType->DColor = EPlayerColor::None;
        }
        else{
            NewAssetType->DColor = color;
        }
        (*ReturnRegistry)[It.first] = NewAssetType;
    }

    return ReturnRegistry;
}

/**
* Build/ construct new asset type for the player
*
* @return shared_ptr to CPlayerAsset that needs to be constructed
*
*/

std::shared_ptr< CPlayerAsset > CPlayerAssetType::Construct(){
    if(auto ThisShared = DThis.lock()){
        return std::shared_ptr< CPlayerAsset >(new CPlayerAsset(ThisShared));
    }
    return nullptr;
}

int CPlayerAsset::DUpdateFrequency = 1;
int CPlayerAsset::DUpdateDivisor = 32;
CRandomNumberGenerator CPlayerAsset::DGenerateRandomNum;

/**
* Determine the frequency that player asset should be updated.
*     It update 12/3/17
*
* @param[in] freq an integer with the new frequency value
*
* @return return the updated Frequency value
*
*/

int CPlayerAsset::UpdateFrequency(int freq){
    if(0 < freq){
       DUpdateFrequency = freq;
       DUpdateDivisor = 32 * DUpdateFrequency;
    }
    return DUpdateFrequency;
}

/**
*
* @class CPlayerAsset.cpp
*
* @brief This class was written to keep tracks of player asset such as gold, lumber, stone.
* also it turns assets inorder, keep tracks of their position, their steps toward destention,thier
*  type, their update 12/3/17
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

/**
* Defult constrctor for CPlayerAsset and CPixelPosition
*
* @param[in] type shared_ptr to CPlayerAssetType
*
*/

CPlayerAsset::CPlayerAsset(std::shared_ptr< CPlayerAssetType > type) : DPosition(0,0){
    DInForest = false;
    DAssetID = GetAssetIDCount();
    DCreationCycle = 0;
    DType = type;
    DHitPoints = type->HitPoints();
    DGold = 0;
    DLumber = 0;
    DStone = 0;
    DStep = 0;
    DMoveRemainderX = 0;
    DMoveRemainderY = 0;
    DDirection = EDirection::South;
    TilePosition(CTilePosition());

    if(type->Name() == "TownHall"){
        DFreeSpace = 4;
    }
    else{
        DFreeSpace = 0;
    }
}

CPlayerAsset::~CPlayerAsset(){

}

/**
* Get current position of Tile
*
* @return CTilePosition
*
*/

CTilePosition CPlayerAsset::TilePosition() const{
    CTilePosition ReturnPosition;

    ReturnPosition.SetFromPixel(DPosition);
    return ReturnPosition;
}

/**
* Set object position which is pass to function
*
* @param[in] pos reference to CTilePosition
*
* @return CTilePosition that DPosition was set to
*
*/

CTilePosition CPlayerAsset::TilePosition(const CTilePosition &pos){
    DPosition.SetFromTile(pos);
    return pos;
}

/**
* Get object x axis
*
* @return tile position on X axis
*
*/

int CPlayerAsset::TilePositionX() const{
    CTilePosition ReturnPosition;

    ReturnPosition.SetFromPixel(DPosition);
    return ReturnPosition.X();
}

/**
* Set object X axis
*
* @param[in] x new X-axis TilePosition
*
* @return the new X-axis TilePosition that was set
*
*/

int CPlayerAsset::TilePositionX(int x){
    DPosition.SetXFromTile(x);
    return x;
}

/**
* Get object Y axis
*
* @return tile position on Y axis
*
*/

int CPlayerAsset::TilePositionY() const{
    CTilePosition ReturnPosition;

    ReturnPosition.SetFromPixel(DPosition);
    return ReturnPosition.Y();
}

/**
* Set object Y axis
*
* @param[in] y new Y-axis TilePosition
*
* @return the new Y-axis TilePosition that was set
*
*/

int CPlayerAsset::TilePositionY(int y){
    DPosition.SetYFromTile(y);
    return y;
}

/**
* Set position of CPlayerAsset
*
* @param[in] pos reference to CPixelPosition
*
* @return the new CPixelPosition
*
*/

CPixelPosition CPlayerAsset::Position(const CPixelPosition &pos){
    return DPosition = pos;
}

/**
* Set X axis of the CPlayerAsset's position
*
* @param[in] x New X-Axis Position
*
* @return Asset's position(X-axis)
*
*/

int CPlayerAsset::PositionX(int x){
    return DPosition.X(x);
}

/**
* Set Y axis of the CPlayerAsset's position
*
* @param[in] y New Y-Axis Position
*
* @return Asset's position(Y-axis)
*
*/

int CPlayerAsset::PositionY(int y){
    return DPosition.Y(y);
};

/**
* Get closest position that player asset can go
* it calls ClosestPosition function from CPixelPosition which has two params
*
* @param[in] pos a reference to CPixelPosition
*
* @return the closest position of the asset
*
*/

CPixelPosition CPlayerAsset::ClosestPosition(const CPixelPosition &pos) const{
    return pos.ClosestPosition(DPosition, Size());
}

/**
* Get the current command/interrupt handeler
* based on current command action assign an action
*
* @return True if command.DAction is anything except DecayCommand or Capability
*
*/

bool CPlayerAsset::Interruptible() const{
    SAssetCommand Command = CurrentCommand();

    switch(Command.DAction){
        case EAssetAction::Construct:
        case EAssetAction::Build:
        case EAssetAction::MineGold:
        case EAssetAction::ConveyLumber:
        case EAssetAction::ConveyStone:
        case EAssetAction::ConveyGold:
        case EAssetAction::Shelter:
        case EAssetAction::Death:
        case EAssetAction::Decay:           return false;
        case EAssetAction::Capability:      if(Command.DAssetTarget){
                                                return EAssetAction::Construct != Command.DAssetTarget->Action();
                                            }
        default:                            return true;
    }
}

/**
* Move player asset across diagonals.
*     the function move player assets from the current tile to new tile position.
*     for doing that it organize its step octant by Octant if the destantion is
*     in current octant it moves to destention and return true. Else, it save move
*     reminder and start from that point in the next octant using IncrementX and
*     IncrementY functions. if occupancymap shows that the current position is equal
*     new position or diagonals dimentions are equal current dimentions it return
*     false.
*
* @param[in] occupancymap a reference to a vector of shared_ptrs to PlayerAsset
* @param[in] diagonals a reference to a vector of boolean vector
*
* @return True if Player assets move
*
*/

bool CPlayerAsset::MoveStep(std::vector< std::vector< std::shared_ptr< CPlayerAsset > > > &occupancymap, std::vector< std::vector< bool > > &diagonals){
    EDirection CurrentOctant = DPosition.TileOctant();
    const int DeltaX[] = {0, 5, 7, 5, 0, -5, -7, -5};
    const int DeltaY[] = {-7, -5, 0, 5, 7, 5, 0, -5};
    CTilePosition CurrentTile, NewTilePosition;
    CPixelPosition CurrentPosition(DPosition);
    int speed = Speed();
    DInForest = false;

    CurrentTile.SetFromPixel(DPosition);
    CTerrainMap::ETileType temptype = CApplicationData::Instance("")->DGameModel->Player(Color())->ActualMap()->TileType(CurrentTile);
    if((Type() == EAssetType::Ranger) && (AssetType()->FindUpgrade("RangerTrackingUpgrade")) && (temptype == CTerrainMap::ETileType::Forest)){
        speed /= 2;
        DInForest = true;
    }
    else {
        speed = EffectiveSpeed();
    }

    if((EDirection::Max == CurrentOctant)||(CurrentOctant == DDirection)){// Aligned just move
        int NewX = speed * DeltaX[to_underlying(DDirection)] * CPosition::TileWidth() + DMoveRemainderX;
        int NewY = speed * DeltaY[to_underlying(DDirection)] * CPosition::TileHeight() + DMoveRemainderY;
        DMoveRemainderX = NewX % DUpdateDivisor;
        DMoveRemainderY = NewY % DUpdateDivisor;
        DPosition.IncrementX(NewX / DUpdateDivisor);
        DPosition.IncrementY(NewY / DUpdateDivisor);
    }
    else{ // Entering
        int NewX = speed * DeltaX[to_underlying(DDirection)] * CPosition::TileWidth() + DMoveRemainderX;
        int NewY = speed * DeltaY[to_underlying(DDirection)] * CPosition::TileHeight() + DMoveRemainderY;
        int TempMoveRemainderX = NewX % DUpdateDivisor;
        int TempMoveRemainderY = NewY % DUpdateDivisor;
        CPixelPosition NewPosition(DPosition.X() + NewX / DUpdateDivisor, DPosition.Y() + NewY / DUpdateDivisor);

        if(NewPosition.TileOctant() == DDirection){
            // Center in tile
            NewTilePosition.SetFromPixel(NewPosition);
            NewPosition.SetFromTile(NewTilePosition);
            TempMoveRemainderX = TempMoveRemainderY = 0;
        }
        DPosition = NewPosition;
        DMoveRemainderX = TempMoveRemainderX;
        DMoveRemainderY = TempMoveRemainderY;
    }
    NewTilePosition.SetFromPixel(DPosition);

    if(CurrentTile != NewTilePosition){
        bool Diagonal = (CurrentTile.X() != NewTilePosition.X()) && (CurrentTile.Y() != NewTilePosition.Y());
        int DiagonalX = std::min(CurrentTile.X(), NewTilePosition.X());
        int DiagonalY = std::min(CurrentTile.Y(), NewTilePosition.Y());

        if(occupancymap[NewTilePosition.Y()][NewTilePosition.X()] || (Diagonal && diagonals[DiagonalY][DiagonalX])){
            bool ReturnValue = false;
            // if(EAssetAction::Walk == occupancymap[NewTilePosition.Y()][NewTilePosition.X()]->Action()){
            //     ReturnValue = occupancymap[NewTilePosition.Y()][NewTilePosition.X()]->Direction() == CurrentPosition.TileOctant();
            // }
            NewTilePosition = CurrentTile;
            DPosition = CurrentPosition;
            return ReturnValue;
        }
        if(Diagonal){
            diagonals[DiagonalY][DiagonalX] = true;
        }
        if(occupancymap[CurrentTile.Y()][CurrentTile.X()])
            occupancymap[NewTilePosition.Y()][NewTilePosition.X()] = occupancymap[CurrentTile.Y()][CurrentTile.X()];

        occupancymap[CurrentTile.Y()][CurrentTile.X()] = nullptr;
    }

    IncrementStep();
    return true;
}

/**
* Saves sheltered peasants to an output file stream
*
* @param[in] save Output file stream
* @param[in] PlayerColor Color of player
*
* @return void
*
*/

void CPlayerAsset::SavePeasants(std::ofstream& save) const{
    save << "#PeasantCount\n";
    save << DPeasants.size() << std::endl;

    for(int i = 0; i < DPeasants.size(); i++){
        save << DPeasants[i] << std::endl;
    }
}

/**
* Saves player commands to an output file stream
*
* @param[in] save Output file stream
* @param[in] PlayerColor Color of player
*
* @return void
*
*/

void CPlayerAsset::SaveCommands(std::ofstream& save, EPlayerColor PlayerColor) const{
    save << "#CommandCount\n";
    save << DCommands.size() << std::endl;

    for(auto Command : DCommands){
        // PlayerColor for loading
        save << "#Player Color\n";
        save << to_underlying(PlayerColor) << std::endl;

        // EAssetAction DAction;
        save << "#Action\n";
        save << to_underlying(Command.DAction) << std::endl;

        // EAssetCapabilityType DCapability;
        save << "#Capability\n";
        save << to_underlying(Command.DCapability) << std::endl;

        // std::shared_ptr< CPlayerAsset > DAssetTarget;
        save << "#Target\n";
        if(Command.DAction == EAssetAction::Walk || Command.DAction == EAssetAction::HarvestLumber
            || Command.DAction == EAssetAction::QuarryStone)
            save << Command.DAssetTarget->PositionX() << " " << Command.DAssetTarget->PositionY() << std::endl;
        else if(Command.DAssetTarget){
            save << Command.DAssetTarget->AssetID() << std::endl;
        }
        else
            save << "NULL\n";

        save << "#ActivatedCapability\n";
        if(Command.DActivatedCapability){
            Command.DActivatedCapability->Save(save);
        }
        else
            save << "NULL\n";
    }
}

/**
* Load player commands from data source file
*
* @param[in] source Data source file to read from
* @param[in] Players CPlayerData array stored all player data
*
* @return void
*
*/

void CPlayerAsset::LoadCommands(std::shared_ptr< CDataSource > source, std::array< std::shared_ptr< CPlayerData >, to_underlying(EPlayerColor::Max)> Players){
    CCommentSkipLineDataSource LineSource(source, '#');
    std::string Value, Type;
    bool PushToggle = true;

    // if an asset already has some commands, it must be processed by another asset already,
    // may just skip the following commands to avoid repetitions.
    if(DCommands.size())
        PushToggle = false;

    // save << "#CommandCount\n";
    // save << DCommands.size() << std::endl;
    LineSource.Read(Value);
    // std::cout << "CommandCount: " << Value << std::endl;
    int CommandCount = std::stoi(Value);

    for(int i = 0; i < CommandCount; i++){
        SAssetCommand Command;

        // Player color
        LineSource.Read(Value);
        int PlayerColor = std::stoi(Value);

        // EAssetAction DAction;
        LineSource.Read(Value);
        // std::cout << "Action: " << Value << std::endl;

        Command.DAction = static_cast<EAssetAction>(std::stoi(Value));

        // EAssetCapabilityType DCapability;
        LineSource.Read(Value);
        // std::cout << "Capability: " << Value << std::endl;

        Command.DCapability = static_cast<EAssetCapabilityType>(std::stoi(Value));

        // std::shared_ptr< CPlayerAsset > DAssetTarget;
        LineSource.Read(Value);
        // std::cout << "Target: " << Value << std::endl;

        if(Value == "NULL")
            Command.DAssetTarget = nullptr;
        else if(Command.DAction == EAssetAction::Walk || Command.DAction == EAssetAction::HarvestLumber
                || Command.DAction == EAssetAction::QuarryStone){
            std::vector<std::string> Tokens;

            CTokenizer::Tokenize(Tokens, Value);

            Command.DAssetTarget = Players[PlayerColor]->CreateMarker(CPixelPosition(std::stoi(Tokens[0]), std::stoi(Tokens[1])), false);
        }
        else
            Command.DAssetTarget = FindAssetObj(std::stoi(Value));

        LineSource.Read(Type);
        // std::cout << "ActivatedCapability: " << Value << std::endl;

        if(Type == "NULL"){
            Command.DActivatedCapability = nullptr;

            if(PushToggle)
                DCommands.push_back(Command);
        }

        else if(Type == "BASIC"){
            // Actor
            LineSource.Read(Value);
            // std::cout << "Actor: " << Value << std::endl;

            auto Actor = FindAssetObj(std::stoi(Value));

            // Player Data
            LineSource.Read(Value);
            // std::cout << "Player: " << Value << std::endl;

            auto PlayerData = Players[std::stoi(Value)];

            // Target
            LineSource.Read(Value);
            // std::cout << "Target: " << Value << std::endl;

            auto Target = FindAssetObj(std::stoi(Value));

            auto PlayerCapability = CPlayerCapability::FindCapability(Command.DCapability);
            // std::cout << "Capability: " << CPlayerCapability::TypeToName(Command.DCapability) << std::endl;

            PlayerCapability->ApplyCapability(Actor, PlayerData, Target);
        }
        else if(Type == "UNITUPGRADE"){
            // Actor
            LineSource.Read(Value);
            // std::cout << "Actor: " << Value << std::endl;

            auto Actor = FindAssetObj(std::stoi(Value));

            // Player Data
            LineSource.Read(Value);
            // std::cout << "PlayerData: " << Value << std::endl;

            auto PlayerData = Players[std::stoi(Value)];

            // Target
            LineSource.Read(Value);
            // std::cout << "Target: " << Value << std::endl;

            std::shared_ptr<CPlayerAsset> Target;

            if(Value == "NULL")
                Target = nullptr;
            else
                Target = FindAssetObj(std::stoi(Value));

            // std::shared_ptr< CPlayerAssetType > DUpgradingType;
            std::shared_ptr< CPlayerAssetType > UpgradingType;

            LineSource.Read(Value);
            if(Value == "NULL")
                UpgradingType = nullptr;
            else
                UpgradingType = PlayerData->AssetTypes()->find(Value)->second;

            // std::cout << "UpgradingType: " << Value << std::endl;


            // std::string DUpgradeName;
            LineSource.Read(Value);
            // std::cout << "UpgradeName: " << Value << std::endl;

            auto Name = Value;

            // int DCurrentStep;
            LineSource.Read(Value);
            // std::cout << "CurrentStep: " << Value << std::endl;

            int CurrentStep = std::stoi(Value);

            // int DTotalSteps;
            LineSource.Read(Value);
            // std::cout << "TotalSteps: " << Value << std::endl;

            int TotalSteps = std::stoi(Value);

            // int DLumber;
            LineSource.Read(Value);
            // std::cout << "Lumber: " << Value << std::endl;

            int Lumber = std::stoi(Value);
            PlayerData->IncrementLumber(Lumber);

            // int DGold;
            LineSource.Read(Value);
            // std::cout << "Gold: " << Value << std::endl;

            int Gold = std::stoi(Value);
            PlayerData->IncrementGold(Gold);

            // int DStone;
            LineSource.Read(Value);
            // std::cout << "Stone: " << Value << std::endl;

            int Stone = std::stoi(Value);
            PlayerData->IncrementStone(Stone);

            auto PlayerCapability = CPlayerCapability::FindCapability(Command.DCapability);
            // std::cout << "Capability: " << CPlayerCapability::TypeToName(Command.DCapability) << std::endl;

            // Since this is a building in progress, need to remove it from the actual map
            // conserving the maximum asset id count
            int Temp = GAssetIDCount;

            if(Actor->Type() == EAssetType::Barracks){

                // removing the same asset from the actual map
                PlayerData->DeleteAsset(Target);

                // replacing the maximum asset id count, so the newly created asset will have the same id
                GAssetIDCount = Target->AssetID();
            }

            // apply capability will create a new instance of the asset
            PlayerCapability->ApplyCapability(Actor, PlayerData, Target);

            if(Target){
                auto NewTarget = FindAssetObj(Target->AssetID());
                NewTarget->HitPoints(Target->HitPoints());
                NewTarget->Step(Target->Step());
                NewTarget->CreationCycle(Target->CreationCycle());
            }

            // set current step
            DCommands.back().DActivatedCapability->Step(CurrentStep);

            // reset
            GAssetIDCount = Temp;

        }
        else if(Type == "BUILD" || Type == "TRAIN"){
            // Actor
            LineSource.Read(Value);
            // std::cout << "Actor: " << Value << std::endl;

            auto Actor = FindAssetObj(std::stoi(Value));

            // Player Data
            LineSource.Read(Value);
            // std::cout << "Player: " << Value << std::endl;

            auto PlayerData = Players[std::stoi(Value)];

            // Target
            LineSource.Read(Value);
            // std::cout << "Target: " << Value << std::endl;

            auto Target = FindAssetObj(std::stoi(Value));

            // int DCurrentStep;
            LineSource.Read(Value);
            // std::cout << "CurrentStep: " << Value << std::endl;

            int CurrentStep = std::stoi(Value);

            // int DTotalSteps;
            LineSource.Read(Value);
            // std::cout << "TotalSteps: " << Value << std::endl;

            int TotalSteps = std::stoi(Value);

            // int DLumber;
            LineSource.Read(Value);
            // std::cout << "Lumber: " << Value << std::endl;

            int Lumber = std::stoi(Value);
            PlayerData->IncrementLumber(Lumber);

            // int DGold;
            LineSource.Read(Value);
            // std::cout << "Gold: " << Value << std::endl;

            int Gold = std::stoi(Value);
            PlayerData->IncrementGold(Gold);

            // int DStone;
            LineSource.Read(Value);
            // std::cout << "Stone: " << Value << std::endl;

            int Stone = std::stoi(Value);
            PlayerData->IncrementStone(Stone);

            auto PlayerCapability = CPlayerCapability::FindCapability(Command.DCapability);
            // std::cout << "Capability: " << CPlayerCapability::TypeToName(Command.DCapability) << std::endl;

            // Since this is a building in progress, need to remove it from the actual map
            // conserving the maximum asset id count
            int Temp = GAssetIDCount;

            // removing the same asset from the actual map
            PlayerData->DeleteAsset(Target);

            // replacing the maximum asset id count, so the newly created asset will have the same id
            GAssetIDCount = Target->AssetID();
            // }

            // apply capability will create a new instance of the asset
            PlayerCapability->ApplyCapability(Actor, PlayerData, Target);

            auto NewTarget = FindAssetObj(Target->AssetID());

            NewTarget->HitPoints(Target->HitPoints());
            NewTarget->Step(Target->Step());
            NewTarget->CreationCycle(Target->CreationCycle());

            // set current step
            DCommands.back().DActivatedCapability->Step(CurrentStep);

            // reset
            GAssetIDCount = Temp;
        }
        else if(Type == "BUILDINGUPGRADE"){
            // Actor
            LineSource.Read(Value);
            // std::cout << "Actor: " << Value << std::endl;

            auto Actor = FindAssetObj(std::stoi(Value));

            // Player Data
            LineSource.Read(Value);
            // std::cout << "PlayerData: " << Value << std::endl;

            auto PlayerData = Players[std::stoi(Value)];

            // Target
            LineSource.Read(Value);
            // std::cout << "Target: " << Value << std::endl;

            std::shared_ptr<CPlayerAsset> Target;

            if(Value == "NULL")
                Target = nullptr;
            else
                Target = FindAssetObj(std::stoi(Value));

            // std::shared_ptr< CPlayerAssetType > DOriginalType;
            LineSource.Read(Value);
            // std::cout << "OrgType: " << Value << std::endl;

            auto OriginalType = PlayerData->AssetTypes()->find(Value)->second;


            // std::shared_ptr< CPlayerAssetType > DUpgradingType;
            LineSource.Read(Value);
            // std::cout << "UpgraddType: " << Value << std::endl;

            auto UpgradeType = PlayerData->AssetTypes()->find(Value)->second;

            // int DCurrentStep;
            LineSource.Read(Value);
            // std::cout << "CurrentStep: " << Value << std::endl;

            int CurrentStep = std::stoi(Value);

            // int DTotalSteps;
            LineSource.Read(Value);
            // std::cout << "TotalSteps: " << Value << std::endl;

            int TotalSteps = std::stoi(Value);

            // int DLumber;
            LineSource.Read(Value);
            // std::cout << "Lumber: " << Value << std::endl;

            int Lumber = std::stoi(Value);
            PlayerData->IncrementLumber(Lumber);

            // int DGold;
            LineSource.Read(Value);
            // std::cout << "Gold: " << Value << std::endl;

            int Gold = std::stoi(Value);
            PlayerData->IncrementGold(Gold);

            // int DStone;
            LineSource.Read(Value);
            // std::cout << "Stone: " << Value << std::endl;

            int Stone = std::stoi(Value);
            PlayerData->IncrementStone(Stone);

            auto PlayerCapability = CPlayerCapability::FindCapability(Command.DCapability);
            // std::cout << "Capability: " << CPlayerCapability::TypeToName(Command.DCapability) << std::endl;

            // change asset type to its original type
            Actor->ChangeType(OriginalType);

            // try upgrading again
            PlayerCapability->ApplyCapability(Actor, PlayerData, Target);

            SAssetCommand AssetCommand = Actor->CurrentCommand();
            AssetCommand.DAction = EAssetAction::Construct;
            Actor->ChangeType(UpgradeType);
            AssetCommand.DActivatedCapability->Step(CurrentStep);
            Actor->PopCommand();
            Actor->PushCommand(AssetCommand);
        }
    }
}


void CPlayerAsset::PushPeasant(int peasant){
    DPeasants.push_back(peasant);
}

int CPlayerAsset::PopPeasant(){
    int AssetID = DPeasants.back();
    DPeasants.pop_back();

    return AssetID;
}

void CPlayerAsset::RemovePeasant(int peasant){
    int found = FindPeasant(peasant);
    if(found >= 0){
        int i = 0;
        for(std::vector<int>::iterator it = DPeasants.begin(); it < DPeasants.end(); it++){
            if(i == found){
                DPeasants.erase(it);
                return;
            }
            i++;
        }
    }
}

int CPlayerAsset::FindPeasant(int peasant){
    int found = -1;
    for(int i = 0; i < DPeasants.size(); i++){
        if(peasant == DPeasants[i]){
            found = i;
            return found;
        }
    }
    return found;
}