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
* @class AssetLoader
*
* @brief
*      AssetLoader creates an instance of CAssetLoader to load the assets in ApplicationData::Activate()
*
* @author Christopher Ta
*
* @version 9.0
*
* @date 12/3/17
*
* Contact: cta@ucdavis.edu
*
*/

#include "AssetLoader.h"

/**
* Constructor initializes protected data members with parameter values of the map
*
* @param[in] tileset Shared pointer of CGraphicTileset
* @param[in] map Shared pointer of CVisibilityMap
*
*/

CAssetLoader::CAssetLoader(std::shared_ptr< CApplicationData> PassedAppData, std::shared_ptr< CDataContainer > PassedDataContainer, std::shared_ptr< CDataContainer > PassedImageDirectory){
    AppData = PassedAppData;
    TempDataContainer = PassedDataContainer;
    ImageDirectory = PassedImageDirectory;
}

/**
* Called by ApplicationData to load all of the game assets. Calls all the functions below in this class.
*
* @param[in] None
*
* @return void
*
*/
void CAssetLoader::LoadAllAssets(){
    LoadFontColors();
    LoadFont10();
    LoadFont12();
    LoadFont16();
    LoadFont24();
    LoadMiniBevel();
    LoadInnerBevel();
    LoadOuterBevel();
    LoadListViewIcons();
    LoadRangerTrackingIcon();
    LoadPeasantShelterIcon();
    LoadRenderingConfiguration();
    LoadTerrain();
    LoadFog();
    LoadPlayerColors();
    AppData->DAssetTilesets.resize(to_underlying(EAssetType::Max));
    LoadIconAsset();
    LoadMiniIcons();
    LoadCorpse();
    LoadFireSmall();
    LoadFireLarge();
    LoadBuildingDeath();
    LoadArrow();
    LoadAssetColor();
    LoadPeasant();
    LoadFootman();
    LoadArcher();
    LoadRanger();
    LoadKnight();
    LoadGoldMine();
    LoadGoldVein();
    LoadTownHall();
    LoadKeep();
    LoadCastle();
    LoadFarm();
    LoadWall();
    LoadBarracks();
    LoadBlacksmith();
    LoadLumberMill();
    LoadScoutTower();
    LoadGuardTower();
    LoadCannonTower();
}

/**
* Loads the font colors into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadFontColors(){
    PrintDebug(DEBUG_LOW, "Loading FontColors\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FontColors.dat");
    AppData->DFontRecolorMap = std::make_shared< CGraphicRecolorMap >();
    if(!AppData->DFontRecolorMap->Load(TempDataSource)){
        PrintError("Failed to load font colors.\n");
        return;
    }
}

/**
* Loads 10pt font into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadFont10(){
    PrintDebug(DEBUG_LOW, "Loading Font10\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FontKingthings10.dat");
    AppData->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Small)] = std::make_shared< CFontTileset >();
    if(!AppData->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Small)]->LoadFont(AppData->DFontRecolorMap, TempDataSource)){
        PrintError("Failed to load font tileset.\n");
        return;
    }
}

/**
* Loads 12pt font into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadFont12(){
    PrintDebug(DEBUG_LOW, "Loading Font12\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FontKingthings12.dat");
    AppData->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Medium)] = std::make_shared< CFontTileset >();
    if(!AppData->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Medium)]->LoadFont(AppData->DFontRecolorMap, TempDataSource)){
        PrintError("Failed to load font tileset.\n");
        return;
    }
}

/**
* Loads 16pt font into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadFont16(){
    PrintDebug(DEBUG_LOW, "Loading Font16\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FontKingthings16.dat");
    AppData->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)] = std::make_shared< CFontTileset >();
    if(!AppData->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]->LoadFont(AppData->DFontRecolorMap, TempDataSource)){
        PrintError("Failed to load font tileset.\n");
        return;
    }
}

/**
* Loads 24pt font into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadFont24(){
    PrintDebug(DEBUG_LOW, "Loading Font24\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FontKingthings24.dat");
    AppData->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)] = std::make_shared< CFontTileset >();
    if(!AppData->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->LoadFont(AppData->DFontRecolorMap, TempDataSource)){
        PrintError("Failed to load font tileset.\n");
        return;
    }
}

/**
* Loads mini bevel tileset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadMiniBevel(){
    // Load the boaders for buttons and boxes
    PrintDebug(DEBUG_LOW, "Loading MiniBevel\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("MiniBevel.dat");
    AppData->DMiniBevelTileset = std::make_shared< CGraphicTileset >();
    if(!AppData->DMiniBevelTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load bevel tileset.\n");
        return;
    }
    AppData->DMiniBevel = std::make_shared< CBevel >(AppData->DMiniBevelTileset);
}

/**
* Loads inner bevel tileset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadInnerBevel(){
    PrintDebug(DEBUG_LOW, "Loading InnerBevel\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("InnerBevel.dat");
    AppData->DInnerBevelTileset = std::make_shared< CGraphicTileset >();
    if(!AppData->DInnerBevelTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load bevel tileset.\n");
        return;
    }
    AppData->DInnerBevel = std::make_shared< CBevel >(AppData->DInnerBevelTileset);
}

/**
* Loads outer bevel tileset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadOuterBevel(){
    PrintDebug(DEBUG_LOW, "Loading OuterBevel\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("OuterBevel.dat");
    AppData->DOuterBevelTileset = std::make_shared< CGraphicTileset >();
    if(!AppData->DOuterBevelTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load bevel tileset.\n");
        return;
    }
    AppData->DOuterBevel = std::make_shared< CBevel >(AppData->DOuterBevelTileset);
}

/**
* Loads list view icons into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadListViewIcons(){
    PrintDebug(DEBUG_LOW, "Loading ListViewIcons\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("ListViewIcons.dat");
    AppData->DListViewIconTileset = std::make_shared< CGraphicTileset >();
    if(!AppData->DListViewIconTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load list view tileset.\n");
        return;
    }
}

/**
* Loads ranger tracking icon into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadRangerTrackingIcon(){
    PrintDebug(DEBUG_LOW, "Loading RangerTrackingIcon\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("RangerTrackingIcon.dat");
    AppData->DRangerTrackingIcon = std::make_shared< CGraphicTileset >();
    if(!AppData->DRangerTrackingIcon->LoadTileset(TempDataSource)){
        PrintError("Failed to load list view tileset.\n");
        return;
    }
}

/**
* Loads peasant shelter icon into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadPeasantShelterIcon(){
    PrintDebug(DEBUG_LOW, "Loading PeasantShelterIcon\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("PeasantShelterIcon.dat");
    AppData->DPeasantShelterIcon = std::make_shared< CGraphicTileset >();
    if(!AppData->DPeasantShelterIcon->LoadTileset(TempDataSource)){
        PrintError("Failed to load list view tileset.\n");
        return;
    }
}

/**
* Loads list view icons into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadRenderingConfiguration(){
    PrintDebug(DEBUG_LOW, "Loading Rendering Configuration\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("MapRendering.dat");
    char TempChar;
    while(1 == TempDataSource->Read(&TempChar, 1)){
        AppData->DMapRendererConfigurationData.push_back(TempChar);
    }
}

/**
* Loads terrain into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadTerrain(){
    PrintDebug(DEBUG_LOW, "Loading Terrain\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Terrain.dat");
    AppData->DTerrainTileset = std::make_shared< CGraphicTileset >();
    if(!AppData->DTerrainTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load tileset.\n");
        return;
    }
    AppData->DTerrainTileset->CreateClippingMasks();
    CPosition::SetTileDimensions(AppData->DTerrainTileset->TileWidth(), AppData->DTerrainTileset->TileHeight());
}

/**
* Loads fog into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadFog(){
    PrintDebug(DEBUG_LOW, "Loading Fog\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Fog.dat");
    AppData->DFogTileset = std::make_shared< CGraphicTileset >();
    if(!AppData->DFogTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load tileset.\n");
        return;
    }
}

/**
* Loads player colors (recolor map) into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadPlayerColors(){
    // Load icons and play colors/icons etc.
    PrintDebug(DEBUG_LOW, "Loading Player Colors\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Colors.dat");
    AppData->DPlayerRecolorMap = std::make_shared< CGraphicRecolorMap > ();
    if(!AppData->DPlayerRecolorMap->Load(TempDataSource)){
        PrintError("Failed to load recolor map.\n");
        return;
    }
}

/**
* Loads the icon asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadIconAsset(){
    PrintDebug(DEBUG_LOW, "Loading Icons\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Icons.dat");
    AppData->DIconTileset = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DIconTileset->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load icons.\n");
        return;
    }
}

/**
* Loads the icon asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadMiniIcons(){
    PrintDebug(DEBUG_LOW, "Loading Mini Icons\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("MiniIcons.dat");
    AppData->DMiniIconTileset = std::make_shared< CGraphicTileset > ();
    if(!AppData->DMiniIconTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load mini icons.\n");
        return;
    }
}


/**
* Loads the corpse image asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadCorpse(){
    PrintDebug(DEBUG_LOW, "Loading Corpse\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Corpse.dat");
    AppData->DCorpseTileset = std::make_shared< CGraphicTileset > ();
    if(!AppData->DCorpseTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load corpse tileset.\n");
        return;
    }
    AppData->DCorpseTileset->CreateClippingMasks();
}

/**
* Loads the fire small image asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadFireSmall(){
    PrintDebug(DEBUG_LOW, "Loading FireSmall\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FireSmall.dat");
    auto FireTileset = std::make_shared< CGraphicTileset > ();
    if(!FireTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load fire small tileset.\n");
        return;
    }
    FireTileset->CreateClippingMasks();
    AppData->DFireTilesets.push_back(FireTileset);
}

/**
* Loads the fire large image asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadFireLarge(){
    PrintDebug(DEBUG_LOW, "Loading FireLarge\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FireLarge.dat");
    auto FireTileset = std::make_shared< CGraphicTileset > ();
    if(!FireTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load fire large tileset.\n");
        return;
    }
    FireTileset->CreateClippingMasks();
    AppData->DFireTilesets.push_back(FireTileset);
}

/**
* Loads the building death image asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadBuildingDeath(){
    PrintDebug(DEBUG_LOW, "Loading BuildingDeath\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("BuildingDeath.dat");
    AppData->DBuildingDeathTileset = std::make_shared< CGraphicTileset > ();
    if(!AppData->DBuildingDeathTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load building death tileset.\n");
        return;
    }
    AppData->DBuildingDeathTileset->CreateClippingMasks();
}

/**
* Loads the arrow image asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadArrow(){
    PrintDebug(DEBUG_LOW, "Loading Arrow\n");
    TempDataSource = ImageDirectory->DataSource("Arrow.dat");
    AppData->DArrowTileset = std::make_shared< CGraphicTileset > ();
    if(!AppData->DArrowTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load arrow tileset.\n");
        return;
    }
    AppData->DArrowTileset->CreateClippingMasks();
}

/**
* Loads the asset color image asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadAssetColor(){
    PrintDebug(DEBUG_LOW, "Loading AssetColor\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("AssetColor.dat");
    AppData->DAssetRecolorMap = std::make_shared< CGraphicRecolorMap > ();
    if(!AppData->DAssetRecolorMap->Load(TempDataSource)){
        PrintError("Failed to load asset color map.\n");
        return;
    }
    AppData->DAssetTilesets[to_underlying(EAssetType::None)] = nullptr;
}

/**
* Loads the peasant asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadPeasant(){
    PrintDebug(DEBUG_LOW, "Loading Peasant\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Peasant.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Peasant)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Peasant)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load peasant tileset.\n");
        return;
    }
}

/**
* Loads the footman asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadFootman(){
    PrintDebug(DEBUG_LOW, "Loading Footman\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Footman.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Footman)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Footman)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load footman tileset.\n");
        return;
    }
}

/**
* Loads the archer asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadArcher(){
    PrintDebug(DEBUG_LOW, "Loading Archer\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Archer.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Archer)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Archer)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load archer tileset.\n");
        return;
    }
}

/**
* Loads the ranger asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadRanger(){
    PrintDebug(DEBUG_LOW, "Loading Ranger\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Ranger.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Ranger)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Ranger)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load archer tileset.\n");
        return;
    }
}

/**
* Loads the knight asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadKnight(){
    PrintDebug(DEBUG_LOW, "Loading Knight\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Knight.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Knight)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Knight)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load Knight tileset.\n");
        return;
    }

}

/**
* Loads the gold mine asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadGoldMine(){
    PrintDebug(DEBUG_LOW, "Loading GoldMine\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("GoldMine.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::GoldMine)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::GoldMine)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load gold mine tileset.\n");
        return;
    }
}

/**
* Loads the GoldVein asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadGoldVein(){
    PrintDebug(DEBUG_LOW, "Loading GoldVein\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("GoldMine.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::GoldVein)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::GoldVein)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load gold vein tileset.\n");
        return;
    }
}

/**
* Loads the town hall asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadTownHall(){
    PrintDebug(DEBUG_LOW, "Loading TownHall\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("TownHall.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::TownHall)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::TownHall)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load town hall tileset.\n");
        return;
    }
}

/**
* Loads the keep asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadKeep(){
    PrintDebug(DEBUG_LOW, "Loading Keep\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Keep.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Keep)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Keep)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load keep tileset.\n");
        return;
    }
}

/**
* Loads the castle asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadCastle(){
    PrintDebug(DEBUG_LOW, "Loading Castle\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Castle.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Castle)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Castle)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load castle tileset.\n");
        return;
    }
}

/**
* Loads the farm asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadFarm(){
    PrintDebug(DEBUG_LOW, "Loading Farm\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Farm.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Farm)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Farm)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load farm tileset.\n");
        return;
    }
}

/**
* Loads the wall asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadWall(){
    PrintDebug(DEBUG_LOW, "Loading Wall\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Wall.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Wall)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Wall)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load wall tileset.\n");
        return;
    }
}

/**
* Loads the barracks asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadBarracks(){
    PrintDebug(DEBUG_LOW, "Loading Barracks\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Barracks.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Barracks)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Barracks)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load barracks tileset.\n");
        return;
    }
}

/**
* Loads the blacksmith asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadBlacksmith(){
    PrintDebug(DEBUG_LOW, "Loading Blacksmith\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Blacksmith.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::Blacksmith)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::Blacksmith)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load blacksmith tileset.\n");
        return;
    }
}

/**
* Loads the lumbermill asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadLumberMill(){
    PrintDebug(DEBUG_LOW, "Loading LumberMill\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("LumberMill.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::LumberMill)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::LumberMill)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load lumber mill tileset.\n");
        return;
    }
}

/**
* Loads the scout tower asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadScoutTower(){
    PrintDebug(DEBUG_LOW, "Loading ScoutTower\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("ScoutTower.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::ScoutTower)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::ScoutTower)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load scout tower tileset.\n");
        return;
    }
}


/**
* Loads the guard tower asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadGuardTower(){
    PrintDebug(DEBUG_LOW, "Loading GuardTower\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("GuardTower.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::GuardTower)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::GuardTower)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load guard tower tileset.\n");
        return;
    }
}

/**
* Loads the cannon tower asset into ApplicationData
*
* @param[in] None
*
* @return void
*
*/

void CAssetLoader::LoadCannonTower(){
    PrintDebug(DEBUG_LOW, "Loading CannonTower\n");
    AppData->RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("CannonTower.dat");
    AppData->DAssetTilesets[to_underlying(EAssetType::CannonTower)] = std::make_shared< CGraphicMulticolorTileset > ();
    if(!AppData->DAssetTilesets[to_underlying(EAssetType::CannonTower)]->LoadTileset(AppData->DPlayerRecolorMap, TempDataSource)){
        PrintError("Failed to load cannon tower tileset.\n");
        return;
    }
}
