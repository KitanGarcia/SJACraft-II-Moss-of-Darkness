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
* @class CApplicationData
*
* @brief This class contains the context of the game (all of the game data). It
* has all of the game application callback functions that will be called when the
* user interacts with the game (i.e mouse actions or keystrokes). Has a timer
* that receives input, calculates game actions, and renders said game actions
* whenever it times out. Initializes settings and loads files for game on starting the
* game.
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

#include "ApplicationData.h"
#include "ApplicationPath.h"
#include "AssetLoader.h"
#include "CommentSkipLineDataSource.h"
#include "FileDataContainer.h"
#include "MemoryDataSource.h"
#include "MainMenuMode.h"
#include "PixelType.h"
#include "TriggerHandler.h"
#include "Debug.h"

#include "CommentSkipLineDataSource.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <iostream>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}
#include "EventHandler.h"

#define INITIAL_MAP_WIDTH       800
#define INITIAL_MAP_HEIGHT      600
#define TIMEOUT_INTERVAL        50
#define TIMEOUT_FREQUENCY       (1000 / TIMEOUT_INTERVAL)
#define MINI_MAP_MIN_WIDTH      128
#define MINI_MAP_MIN_HEIGHT     128
#define DISABLE_SOUNDLIBMIXER   false 
#define ICON_DESCRIPTION_H      15
#define NOTIFICATION_H          145
#define NOTIFICATION_W          150

bool CApplicationData::DLoadedGame = false;
// std::map<int, std::shared_ptr<CPlayerAsset> > CApplicationData::DAssetObjectIDMap;
std::shared_ptr< CApplicationData > CApplicationData::DApplicationDataPointer;

/**
* Set/initialize default values for the game.
*
* @param[in] appname The name of the application.
* @param[in] key Empty struct, used for debugging.
*
* @return Nothing.
*
*/

CApplicationData::CApplicationData(const std::string &appname, const SPrivateApplicationType &key){
    DApplication = CGUIFactory::ApplicationInstance(appname);

    DApplication->SetActivateCallback(this, ActivateCallback);

    DPlayerColor = EPlayerColor::Red;
    DMiniMapViewportColor = 0xFFFFFF;
    DDeleted = false;

    DMapConfirmed = false;

    DCursorType = ctPointer;

    DMapSelectListViewXOffset = 0;
    DMapSelectListViewYOffset = 0;
    DSelectedMapIndex = 0;
    DSoundVolume = 1.0;
    DMusicVolume = 0.5;
    //DUsername = "user";
    //DRemoteHostname = "localhost";
    //DMultiplayerPort = 55107; // Ascii WC = 0x5743 or'd with 0x8000
    DBorderWidth = 32;
    DPanningSpeed = 0;
    for(int Index = 0; Index < to_underlying(EPlayerColor::Max); Index++){
       DPlayerCommands[Index].DAction = EAssetCapabilityType::None;
       DLoadingPlayerColors[Index] = static_cast<EPlayerColor>(Index);
    }
    DCurrentX = 0;
    DCurrentY = 0;
    DMouseDown = CPixelPosition(-1, -1);
    DLeftClick = 0;
    DRightClick = 0;
    DLeftDown = false;
    DRightDown = false;


    DMenuButtonState = CButtonRenderer::EButtonState::None;

    DGroupHotKeyMap[SGUIKeyType::Key0] = std::list< std::weak_ptr< CPlayerAsset > >();
    DGroupHotKeyMap[SGUIKeyType::Key1] = std::list< std::weak_ptr< CPlayerAsset > >();
    DGroupHotKeyMap[SGUIKeyType::Key2] = std::list< std::weak_ptr< CPlayerAsset > >();
    DGroupHotKeyMap[SGUIKeyType::Key3] = std::list< std::weak_ptr< CPlayerAsset > >();
    DGroupHotKeyMap[SGUIKeyType::Key4] = std::list< std::weak_ptr< CPlayerAsset > >();
    DGroupHotKeyMap[SGUIKeyType::Key5] = std::list< std::weak_ptr< CPlayerAsset > >();
    DGroupHotKeyMap[SGUIKeyType::Key6] = std::list< std::weak_ptr< CPlayerAsset > >();
    DGroupHotKeyMap[SGUIKeyType::Key7] = std::list< std::weak_ptr< CPlayerAsset > >();
    DGroupHotKeyMap[SGUIKeyType::Key8] = std::list< std::weak_ptr< CPlayerAsset > >();
    DGroupHotKeyMap[SGUIKeyType::Key9] = std::list< std::weak_ptr< CPlayerAsset > >();

    DUnitHotKeyMap[SGUIKeyType::KeyA] = EAssetCapabilityType::Attack;
    DUnitHotKeyMap[SGUIKeyType::Keya] = EAssetCapabilityType::Attack;
    DUnitHotKeyMap[SGUIKeyType::KeyB] = EAssetCapabilityType::BuildSimple;
    DUnitHotKeyMap[SGUIKeyType::Keyb] = EAssetCapabilityType::BuildSimple;
    DUnitHotKeyMap[SGUIKeyType::KeyG] = EAssetCapabilityType::Convey;
    DUnitHotKeyMap[SGUIKeyType::Keyg] = EAssetCapabilityType::Convey;
    DUnitHotKeyMap[SGUIKeyType::KeyM] = EAssetCapabilityType::Move;
    DUnitHotKeyMap[SGUIKeyType::Keym] = EAssetCapabilityType::Move;
    DUnitHotKeyMap[SGUIKeyType::KeyP] = EAssetCapabilityType::Patrol;
    DUnitHotKeyMap[SGUIKeyType::Keyp] = EAssetCapabilityType::Patrol;
    DUnitHotKeyMap[SGUIKeyType::KeyR] = EAssetCapabilityType::Repair;
    DUnitHotKeyMap[SGUIKeyType::Keyr] = EAssetCapabilityType::Repair;
    DUnitHotKeyMap[SGUIKeyType::KeyT] = EAssetCapabilityType::StandGround;
    DUnitHotKeyMap[SGUIKeyType::Keyt] = EAssetCapabilityType::StandGround;

    DBuildHotKeyMap[SGUIKeyType::KeyB] = EAssetCapabilityType::BuildBarracks;
    DBuildHotKeyMap[SGUIKeyType::Keyb] = EAssetCapabilityType::BuildBarracks;
    DBuildHotKeyMap[SGUIKeyType::KeyF] = EAssetCapabilityType::BuildFarm;
    DBuildHotKeyMap[SGUIKeyType::Keyf] = EAssetCapabilityType::BuildFarm;
    DBuildHotKeyMap[SGUIKeyType::KeyH] = EAssetCapabilityType::BuildTownHall;
    DBuildHotKeyMap[SGUIKeyType::Keyh] = EAssetCapabilityType::BuildTownHall;
    DBuildHotKeyMap[SGUIKeyType::KeyL] = EAssetCapabilityType::BuildLumberMill;
    DBuildHotKeyMap[SGUIKeyType::Keyl] = EAssetCapabilityType::BuildLumberMill;
    DBuildHotKeyMap[SGUIKeyType::KeyS] = EAssetCapabilityType::BuildBlacksmith;
    DBuildHotKeyMap[SGUIKeyType::Keys] = EAssetCapabilityType::BuildBlacksmith;
    DBuildHotKeyMap[SGUIKeyType::KeyT] = EAssetCapabilityType::BuildScoutTower;
    DBuildHotKeyMap[SGUIKeyType::Keyt] = EAssetCapabilityType::BuildScoutTower;
    DBuildHotKeyMap[SGUIKeyType::KeyW] = EAssetCapabilityType::BuildWall;
    DBuildHotKeyMap[SGUIKeyType::Keyw] = EAssetCapabilityType::BuildWall;
    DBuildHotKeyMap[SGUIKeyType::KeyG] = EAssetCapabilityType::BuildGoldMine;
    DBuildHotKeyMap[SGUIKeyType::Keyg] = EAssetCapabilityType::BuildGoldMine;

    DTrainHotKeyMap[SGUIKeyType::KeyA] = EAssetCapabilityType::BuildArcher;
    DTrainHotKeyMap[SGUIKeyType::Keya] = EAssetCapabilityType::BuildArcher;
    DTrainHotKeyMap[SGUIKeyType::KeyF] = EAssetCapabilityType::BuildFootman;
    DTrainHotKeyMap[SGUIKeyType::Keyf] = EAssetCapabilityType::BuildFootman;
    //DTrainHotKeyMap[SGUIKeyType::KeyK] = EAssetCapabilityType::BuildKnight;
    //DTrainHotKeyMap[SGUIKeyType::Keyk] = EAssetCapabilityType::BuildKnight;
    DTrainHotKeyMap[SGUIKeyType::KeyP] = EAssetCapabilityType::BuildPeasant;
    DTrainHotKeyMap[SGUIKeyType::Keyp] = EAssetCapabilityType::BuildPeasant;
    DTrainHotKeyMap[SGUIKeyType::KeyR] = EAssetCapabilityType::BuildRanger;
    DTrainHotKeyMap[SGUIKeyType::Keyr] = EAssetCapabilityType::BuildRanger;
}

CApplicationData::~CApplicationData(){

}

std::shared_ptr< CApplicationData > CApplicationData::Instance(const std::string &appname){
    if(!DApplicationDataPointer){
        DApplicationDataPointer = std::make_shared< CApplicationData > (appname, SPrivateApplicationType{});
    }
    return DApplicationDataPointer;
}

void CApplicationData::ActivateCallback(TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    AppData->Activate();
}


bool CApplicationData::TimeoutCallback(TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    //PrintDebug(DEBUG_HIGH,"CApplicationData::Timeout\n");
    if(AppData->DDeleted){
        return false;
    }

    return AppData->Timeout();
}

/**
* Is called when the delete windows has been entered. If false is returned in the
* "delete-event" signal handler, GTK will emit the "destroy" signal, ending the game.
* Returning true will not destroy the window. This is useful for "do you want to quit?"
* dialogs. The callback function is not fully implemented yet, it still needs to
* process the widge command as input.
*
* @param[in] widget Input to GUI to decide whether to exit game or not.
* @param[in] data The game application.
*
* @return True if you don't want the game destroyed and false if you want the game
* destroyed.
*
*/

bool CApplicationData::MainWindowDeleteEventCallback(std::shared_ptr<CGUIWidget> widget, TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    //PrintDebug(DEBUG_HIGH,"CApplicationData::Delete\n");
    return AppData->MainWindowDeleteEvent(widget);
}

/**
* Is called when the destroy signal has been called, will call a function to
* destroy the game window.
*
* @param[in] widget The GUI widget that triggered this callback.
* @param[in] data   The game application.
*
* @return Nothing.
*
*/

void CApplicationData::MainWindowDestroyCallback(std::shared_ptr<CGUIWidget> widget, TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    //PrintDebug(DEBUG_HIGH,"CApplicationData::Destroy\n");
    AppData->MainWindowDestroy(widget);
}

/**
* Is called when a key is pressed in the main window and logs the key in
* a list of pressed keys if it isn't already in there.
*
* @param[in] widget The GUI widget (the object that triggered this callback).
* @param[in] event  The key event (click, hover, etc.) that triggered the widget.
* @param[in] data   The game application.
*
* @return Always returns true.
*
*/

bool CApplicationData::MainWindowKeyPressEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event, TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    //PrintDebug(DEBUG_HIGH,"CApplicationData::KeyPress\n");
    return AppData->MainWindowKeyPressEvent(widget, event);
}

/**
* Is called when a key is released in the main window. It erases that key from
* the list of pressed keys and adds it to the list of released keys.
*
* @param[in] widget The GUI widget that triggered this callback.
* @param[in] event  The key event that triggered the widget.
* @param[in] data   The game application.
*
* @return Always returns true.
*
*/

bool CApplicationData::MainWindowKeyReleaseEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event, TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    //PrintDebug(DEBUG_HIGH,"CApplicationData::KeyRelease\n");
    return AppData->MainWindowKeyReleaseEvent(widget, event);
}

/**
* Is called when the main window is resized. Calls the routine to resize the main window by
* updating all of its values and redrawing it.
*
* @param[in] widget The GUI widget that triggered this callback.
* @param[in] event  The event that triggered the widget (always in every callback, not used for * this one).
* @param[in] data   The game application.
*
* @return Always returns true.
*
*/

bool CApplicationData::MainWindowConfigureEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIConfigureEvent &event, TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    //PrintDebug(DEBUG_HIGH,"CApplicationData::Configure\n");
    return AppData->MainWindowConfigureEvent(widget, event);
}

/**
* Is called by a widget to use the Cairo api to draw images froma source file.
*
* @param[in] widget The GUI widget that triggered this callback.
* @param[in] rc     The cairo_t object used to draw using the Cairo Lib's API.
* @param[in] data   The game application.
*
* @return Always returns true.
*
*/

bool CApplicationData::DrawingAreaDrawCallback(std::shared_ptr<CGUIWidget> widget, std::shared_ptr<CGraphicResourceContext> rc, TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    //PrintDebug(DEBUG_HIGH,"CApplicationData::Draw\n");
    return AppData->DrawingAreaDraw(widget, rc);
}

/**
* Is called during mouse clicks in a button area, it calls a routine to figure out what type    * of mouse click was pressed.
*
* @param[in] widget The GUI widget that triggered this callback.
* @param[in] event  The event that triggered the widget (a mouseclick).
* @param[in] data   The game application.
*
* @return Always returns true.
*
*/

bool CApplicationData::DrawingAreaButtonPressEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event, TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    //PrintDebug(DEBUG_HIGH,"CApplicationData::ButtonPress\n");
    return AppData->DrawingAreaButtonPressEvent(widget, event);
}

/**
* Is called when the mouse is released in a button area, it calls a routin to figure out what
* type of mouse click it was (i.e single click, double click), it is used in conjunction with
* the DrawingAreaButtonPressEventCallback.
*
* @param widget The GUI widget that triggered the callback.
* @param event  The mouse event that triggered the widget.
* @param data   The game application.
*
* @return Always returns true.
*
*/

bool CApplicationData::DrawingAreaButtonReleaseEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event, TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    //PrintDebug(DEBUG_HIGH,"CApplicationData::ButtonRelease\n");
    return AppData->DrawingAreaButtonReleaseEvent(widget, event);
}

/**
* Is called when the mouse is moved over the window. It keeps track of the mouse's position.
*
* @param[in] widget The GUI widget (window in this case) that triggered the callback.
* @param[in] event  The event that triggered the GUI (mousing moving over widget).
* @param[in] data   The game application (context).
*
* @return Always returns true.
*
*/

bool CApplicationData::DrawingAreaMotionNotifyEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIMotionEvent &event, TGUICalldata data){
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    //PrintDebug(DEBUG_HIGH,"CApplicationData::Motion\n");
    return AppData->DrawingAreaMotionNotifyEvent(widget, event);
}

/**
* This is where the game starts. All of the callback functions (the signals that activate
* these callback functions are connected in the GDK files in another layer of implementation)
* are registered (i.e their function pointers are assigned to the right callback function).
* Various other important game objects are intialized, etc.
*
* @return Nothing.
*
*/

void CApplicationData::Activate(){
    // Sets up the environment (i.e application path and directory/filesystem so the
    // game can find files it needs.
    CPath AppPath = GetApplicationPath().Containing();
    std::shared_ptr< CDataContainer > TempDataContainer = std::make_shared< CDirectoryDataContainer >(AppPath.ToString() + "/data");
    std::shared_ptr< CDataContainer > ImageDirectory = TempDataContainer->DataContainer("img");
    std::shared_ptr< CDataContainerIterator > FileIterator;
    std::shared_ptr< CDataSource > TempDataSource;

    // Instantiate AssetLoader with environment variables to load assets
    std::shared_ptr< CApplicationData > AppData = shared_from_this();
    CAssetLoader AssetLoader(AppData, TempDataContainer, ImageDirectory);

    // Create a new main window
    DMainWindow = DApplication->NewWindow();

    // When the window is given the "delete-event" signal (this is given by the
    // window manager, usually by the "close" option, or on the titlebar), we
    // ask it to call the delete_event () function as defined above. The data
    // passed to the callback function is nullptr and is ignored in the callback
    // function.
    DMainWindow->SetDeleteEventCallback(this, MainWindowDeleteEventCallback);
    //g_signal_connect(DMainWindow, "delete-event", G_CALLBACK(MainWindowDeleteEventCallback), this);

    // Here we connect the "destroy" event to a signal handler. This event
    // occurs when we call gtk_widget_destroy() on the window, or if we return
    // false in the "delete-event" callback.
    DMainWindow->SetDestroyEventCallback(this, MainWindowDestroyCallback);

    DMainWindow->SetKeyPressEventCallback(this, MainWindowKeyPressEventCallback);
    DMainWindow->SetKeyReleaseEventCallback(this, MainWindowKeyReleaseEventCallback);

    // Sets the border width of the window.
    //DMainWindow->SetBorderWidth(2);

    // Creates a drawing surface
    DDrawingArea = CGUIFactory::NewDrawingArea();

    // Add drawing surface to main window
    DMainWindow->Add(DDrawingArea);

    DMainWindow->SetMinSize(DDrawingArea, INITIAL_MAP_WIDTH, INITIAL_MAP_HEIGHT);

    // Setup drawing surface callback functions.
    DDrawingArea->SetDrawEventCallback(this, DrawingAreaDrawCallback);
    DDrawingArea->SetButtonPressEventCallback(this, DrawingAreaButtonPressEventCallback);
    DDrawingArea->SetButtonReleaseEventCallback(this, DrawingAreaButtonReleaseEventCallback);
    DDrawingArea->SetMotionEventCallback(this, DrawingAreaMotionNotifyEventCallback);
    DDrawingArea->SetConfigureEventCallback(this, MainWindowConfigureEventCallback);

    // Enable various events for the drawing area.
    DDrawingArea->EnableEvent(EGUIEvent::Expose);
    DDrawingArea->EnableEvent(EGUIEvent::ButtonPress);
    DDrawingArea->EnableEvent(EGUIEvent::ButtonRelease);
    DDrawingArea->EnableEvent(EGUIEvent::Motion);

    auto DefaultDisplay = CGUIFactory::DefaultDisplay();

    // Initialize image of the cursor.
    DBlankCursor = DefaultDisplay->NewCursor(CGUICursor::ECursorType::Blank);
    // Show all widgets so they are displayed
    TempDataSource = ImageDirectory->DataSource("Splash.dat");
    DSplashTileset = std::make_shared< CGraphicTileset >();
    if(!DSplashTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to splash screen.\n");
        return;
    }
    DDrawingArea->ShowAll();
    DMainWindow->ShowAll();
    DMainWindow->SetCursor(DBlankCursor);

    DDoubleBufferSurface = CGraphicFactory::CreateSurface(INITIAL_MAP_WIDTH, INITIAL_MAP_HEIGHT, CGraphicSurface::ESurfaceFormat::ARGB32); // DDrawingArea->CreateSimilarSurface(INITIAL_MAP_WIDTH, INITIAL_MAP_HEIGHT);
    DWorkingBufferSurface = CGraphicFactory::CreateSurface(INITIAL_MAP_WIDTH, INITIAL_MAP_HEIGHT, CGraphicSurface::ESurfaceFormat::ARGB32); //DDrawingArea->CreateSimilarSurface(INITIAL_MAP_WIDTH, INITIAL_MAP_HEIGHT);


    DefaultDisplay->Flush();
    DDrawingArea->Invalidate();
    DApplication->ProcessEvents(true);
    DTotalLoadingSteps = 128;
    DCurrentLoadingStep = 0;
    DLoadingResourceContext = DDrawingArea->CreateResourceContext();
    RenderSplashStep();

    DefaultDisplay->Flush();
    DApplication->ProcessEvents(true);
    DefaultDisplay->Flush();

    TempDataSource = ImageDirectory->DataSource("Cursors.dat");
    DCursorSet = std::make_shared< CCursorSet >();
    if(!DCursorSet->LoadCursors(TempDataSource)){
        PrintError("Failed to load cursors.\n");
        return;
    }
    DCursorIndices[ctPointer] = DCursorSet->FindCursor("pointer");
    DCursorIndices[ctInspect] = DCursorSet->FindCursor("magnifier");
    DCursorIndices[ctArrowN] = DCursorSet->FindCursor("arrow-n");
    DCursorIndices[ctArrowE] = DCursorSet->FindCursor("arrow-e");
    DCursorIndices[ctArrowS] = DCursorSet->FindCursor("arrow-s");
    DCursorIndices[ctArrowW] = DCursorSet->FindCursor("arrow-w");
    DCursorIndices[ctTargetOff] = DCursorSet->FindCursor("target-off");
    DCursorIndices[ctTargetOn] = DCursorSet->FindCursor("target-on");

    RenderSplashStep();

    // Set up the audio sources/sound library.
    TempDataSource = TempDataContainer->DataSource("./snd/SoundClips.dat");
    DSoundLibraryMixer = std::make_shared< CSoundLibraryMixer >(DISABLE_SOUNDLIBMIXER);
    if(!DISABLE_SOUNDLIBMIXER && !DSoundLibraryMixer->LoadLibrary(TempDataSource, SoundLoadingCallback, this)){
        PrintError("Failed to sound mixer.\n");
        return;
    }
    RenderSplashStep();
    DSoundLibraryMixer->PlaySong(DSoundLibraryMixer->FindSong("load"), DMusicVolume);

    // Load the images and color them for various objects (buttons, markers, etc.)
    TempDataSource = ImageDirectory->DataSource("ButtonColors.dat");
    DButtonRecolorMap = std::make_shared< CGraphicRecolorMap >();
    if(!DButtonRecolorMap->Load(TempDataSource)){
        PrintError("Failed to load button colors.\n");
        return;
    }

    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Marker.dat");
    DMarkerTileset = std::make_shared< CGraphicTileset >();
    if(!DMarkerTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load markers.\n");
        return;
    }
    DMarkerTileset->CreateClippingMasks();

    // Load the tilesets
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Texture.dat");
    DBackgroundTileset = std::make_shared< CGraphicTileset >();
    if(!DBackgroundTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load background tileset.\n");
        return;
    }

    // Helper class function to load all the game assets
    AssetLoader.LoadAllAssets();
    RenderSplashStep();
    for(auto AssetTileset : DAssetTilesets){
        if(AssetTileset != nullptr){
            AssetTileset->CreateClippingMasks();
        }
    }
    // Load tree growing pics
    PrintDebug(DEBUG_LOW, "Loading Tree Growth\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("TreeGrowth.dat");
    DTreeTileset = std::make_shared< CGraphicTileset >();
    if(!DTreeTileset->LoadTileset(TempDataSource)){
        PrintError("Failed to load tileset (TG).\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Assets Loaded\n");

    PrintDebug(DEBUG_LOW, "Loading res directory\n");
    RenderSplashStep();
    std::shared_ptr< CDataContainer > AssetDirectory = TempDataContainer->DataContainer("res");
    if(!CPlayerAssetType::LoadTypes(AssetDirectory)){
        PrintError("Failed to load resources\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading upg directory\n");
    RenderSplashStep();
    std::shared_ptr< CDataContainer > UpgradeDirectory = TempDataContainer->DataContainer("upg");
    if(!CPlayerUpgrade::LoadUpgrades(UpgradeDirectory)){
        PrintError("Failed to load upgrades\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading opt directory\n");
    RenderSplashStep();
    std::shared_ptr< CDataContainer > OptionsDirectory = TempDataContainer->DataContainer("opt");

    // load sound options
    if(!CApplicationData::LoadSoundOptions(OptionsDirectory)){
        PrintError("Failed to load Sound options\n");
        return;
    }

    RenderSplashStep();

    // load network options
    if(!CApplicationData::LoadNetworkOptions(OptionsDirectory)){
        PrintError("Failed to load Network options\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading Maps\n");
    RenderSplashStep();
    std::shared_ptr< CDataContainer > MapDirectory = TempDataContainer->DataContainer("map");
    if(!CAssetDecoratedMap::LoadMaps(MapDirectory)){
        PrintError("Failed to load maps\n");
        return;
    }

    // Set update 12/3/17
    CPlayerAsset::UpdateFrequency(TIMEOUT_FREQUENCY);
    CAssetRenderer::UpdateFrequency(TIMEOUT_FREQUENCY);

    PrintDebug(DEBUG_LOW, "Loading Game Map 0\n");

    std::shared_ptr< CDataSource > source = std::make_shared< CFileDataSource > ("");
    LoadGameMap(0, source);

    PrintDebug(DEBUG_LOW, "Game Map 0 Loaded\n");

    // Set up button, map, and options renderer
    DButtonRenderer = std::make_shared< CButtonRenderer > (DButtonRecolorMap, DInnerBevel, DOuterBevel, DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]);
    DMapSelectListViewRenderer = std::make_shared< CListViewRenderer > (DListViewIconTileset, DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]);
    DOptionsEditRenderer = std::make_shared< CEditRenderer > (DButtonRecolorMap, DInnerBevel, DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]);

    PrintDebug(DEBUG_LOW, "Changing Mode to MainMenu\n");
    DDoubleBufferSurface->Draw(DWorkingBufferSurface, 0, 0, -1, -1, 0, 0);
    DDrawingArea->Invalidate();

    // Set up game timer (how often to update 12/3/17
    DApplicationMode = DNextApplicationMode = CMainMenuMode::Instance();

    DApplication->SetTimer(TIMEOUT_INTERVAL, this, TimeoutCallback);

    // Play background music
    DSoundLibraryMixer->StopSong();
    DSoundLibraryMixer->PlaySong(DSoundLibraryMixer->FindSong("menu"), DMusicVolume);
    DLoadingResourceContext = nullptr;
}

/**
* Is called whenever the timer fires, is used to update 12/3/17
* methods from the mode the game is currently in. Always calls the current
* mode's Input, Calculate, and Render functions. Updates the current mode
* with the next mode and draws the cursor.
*
* @return Nothing.
*
*/

bool CApplicationData::Timeout(){
    DApplicationMode->Input(shared_from_this());
    DApplicationMode->Calculate(shared_from_this());
    DApplicationMode->Render(shared_from_this());

    if(!DLeftDown || (2 != DLeftClick)){
        DLeftClick = 0;
    }
    DRightClick = 0;

    DCursorSet->DrawCursor(DWorkingBufferSurface, DCurrentX, DCurrentY, DCursorIndices[DCursorType]);

    DDoubleBufferSurface->Draw(DWorkingBufferSurface, 0, 0, -1, -1, 0, 0);

    DDrawingArea->Invalidate();

    DApplicationMode->IncrementTimer();
    //PrintDebug(DEBUG_LOW, "Timer value is %lf\n", DApplicationMode->GetTime());
    DApplicationMode = DNextApplicationMode;

    return true;
}

/**
* Returns a bool to either trigger the destruction or not destruction (lol)
* of the main window.
*
* @param[in] widget The widget you need to interact with to destroy or (not)
* destroy the main window (i.e the game).
*
* @return Always returns false.
*
*/

bool CApplicationData::MainWindowDeleteEvent(std::shared_ptr<CGUIWidget> widget){
    PrintDebug(DEBUG_LOW, "Delete event occurred\n");
    DDeleted = true;
    return false;
}

/**
* Destroys the main window.
*
* @returns Nothing.
*
*/

void CApplicationData::MainWindowDestroy(std::shared_ptr<CGUIWidget> widget){
    DMainWindow = nullptr;
}

/**
* Logs any button presses in the main window.
*
* @param[in] widget The widget you pressed a key in.
* @parem[in] The event that occured to the widget (that you did).
*
* @return Always returns false.
*
*/

bool CApplicationData::MainWindowKeyPressEvent(std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event){
    bool Found = false;
    for(auto Key : DPressedKeys){
        if(Key == event.DValue.DValue){
            Found = true;
            break;
        }
    }
    if(!Found){
        DPressedKeys.push_back(event.DValue.DValue);
    }
    return true;
}

/**
* Logs when a button is released in the main window (erases that button from
* the list of held/pressed buttons).
*
* @param[in] widget The main window, you released a button in it.
* @param[in] event  The event that occured to the widget (you let go of a
* button).
*
* @return Always returns true.
*
*/

bool CApplicationData::MainWindowKeyReleaseEvent(std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event){
    bool Found = false;
    int Index = 0;
    for(auto Key : DPressedKeys){
        if(Key == event.DValue.DValue){
            Found = true;
            break;
        }
        Index++;
    }
    if(Found){
        DPressedKeys.erase(DPressedKeys.begin() + Index);
    }
    Found = false;
    for(auto Key : DReleasedKeys){
        if(Key == event.DValue.DValue){
            Found = true;
            break;
        }
    }
    if(!Found){
        DReleasedKeys.push_back(event.DValue.DValue);
    }
    return true;
}

/**
* Resizes the main window to your liking.
*
* @param[in] widget The main window area.
* @param[in] event  The event used to trigger this callback.
*
* @return Always returns true.
*/

bool CApplicationData::MainWindowConfigureEvent(std::shared_ptr<CGUIWidget> widget, SGUIConfigureEvent &event){
    ResizeCanvases();
    return true;
}

/**
* Draws an area using a source image with the Cairo lib API.
*
* @param[in] widget The area you plan on drawing.
* @param[in] rc     The cairo_t object used by Cairo to draw stuff.
*
* @return Always returns true.
*/

bool CApplicationData::DrawingAreaDraw(std::shared_ptr<CGUIWidget> widget, std::shared_ptr<CGraphicResourceContext> rc){
    rc->DrawSurface(DDoubleBufferSurface, 0, 0, -1, -1, 0, 0);
    return true;
}

/**
* Used in conjunction with its release event counterpart to determine
* what type of mouse click has occured to this area (double click, etc.)
*
* @param[in] widget The area you are interacting with (drawing area).
* @param[in] event  The event done to the widget (mouse clicking).
*
* @return Always returns true.
*
*/

bool CApplicationData::DrawingAreaButtonPressEvent(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event){
    if(1 == event.DButton){
        if(event.DModifier.ModifierIsSet(SGUIModifierType::Control)){
            DRightClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
            DRightDown = true;
        }
        else{
            DLeftClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
            DLeftDown = true;
        }
    }
    if(3 == event.DButton){
        DRightClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
        DRightDown = true;
    }
    return true;
}

/**
* Used in conjunction with its press event counterpart to determine
* the type of mouse click that has occured.
*
* @param[in] widget The area you are interacting with.
* @param[in] event  The mouse release.
*
* @return Always returns true.
*
*/

bool CApplicationData::DrawingAreaButtonReleaseEvent(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event){
    if(1 == event.DButton){
        if(event.DModifier.ModifierIsSet(SGUIModifierType::Control)){
            DRightClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
            DRightDown = false;
        }
        else{
            if(0 == DLeftClick){
                DLeftClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
            }
            DLeftDown = false;
        }
    }
    if(3 == event.DButton){
        DRightClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
        DRightDown = false;
    }
    return true;
}

/**
* Used to track the motion of the mouse, and to determine if you are moving
* the mouse.
*
* @param[in] widget The drawing area.
* @param[in] event  The act of you moving the mouse over the widget area.
*
* @return Always returns true.
*
*/

bool CApplicationData::DrawingAreaMotionNotifyEvent(std::shared_ptr<CGUIWidget> widget, SGUIMotionEvent &event){
    int DrawingAreaAllocationWidth, DrawingAreaAllocationHeight;
    int EventX, EventY;
    EventX = event.DWindowX;
    EventY = event.DWindowY;

    DrawingAreaAllocationWidth = DDrawingArea->AllocatedWidth();
    DrawingAreaAllocationHeight = DDrawingArea->AllocatedHeight();

    if(EventX > DrawingAreaAllocationWidth){
        EventX = DrawingAreaAllocationWidth - 1;
    }
    if(0 > EventX){
        EventX = 0;
    }
    if(EventY > DrawingAreaAllocationHeight){
        EventY = DrawingAreaAllocationHeight - 1;
    }
    if(0 > EventY){
        EventY = 0;
    }

    DCurrentX = EventX;
    DCurrentY = EventY;
    return true;
}

/**
* Finds what UI component is at that pixel position.
*
* @param[in] pos The pixel position of the UI component you are looking for.
*
* @return The UI component at a pixel position.
*
*/

CApplicationData::EUIComponentType CApplicationData::FindUIComponentType(const CPixelPosition &pos){
    int ViewWidth, ViewHeight;
    int MiniWidth, MiniHeight;
    int DescrWidth, DescrHeight;
    int ActWidth, ActHeight;

    ViewWidth = DViewportSurface->Width();
    ViewHeight = DViewportSurface->Height();

    if((DViewportXOffset > pos.X())||(DViewportYOffset > pos.Y())||(DViewportXOffset + ViewWidth <= pos.X())||(DViewportYOffset + ViewHeight <= pos.Y())){
        if((DViewportXOffset - DInnerBevel->Width() <= pos.X())&&(DViewportXOffset > pos.X())){
            if((DViewportYOffset <= pos.Y())&&(pos.Y() < DViewportYOffset + ViewHeight)){
                return uictViewportBevelW;
            }
        }
        else if((DViewportXOffset + ViewWidth <= pos.X())&&(DViewportXOffset + ViewWidth + DInnerBevel->Width() > pos.X())){
            if((DViewportYOffset <= pos.Y())&&(pos.Y() < DViewportYOffset + ViewHeight)){
                return uictViewportBevelE;
            }
        }
        else if((DViewportXOffset <= pos.X())&&(pos.X() < DViewportXOffset + ViewWidth)){
            if((DViewportYOffset - DInnerBevel->Width() <= pos.Y())&&(DViewportYOffset > pos.Y())){
                return uictViewportBevelN;
            }
            else if((DViewportYOffset + ViewHeight <= pos.Y())&&(DViewportYOffset + ViewHeight + DInnerBevel->Width() > pos.Y())){
                return uictViewportBevelS;
            }
        }
    }
    else{
        return uictViewport;
    }
    MiniWidth = DMiniMapSurface->Width();
    MiniHeight = DMiniMapSurface->Height();
    if((DMiniMapXOffset <= pos.X())&&(DMiniMapXOffset + MiniWidth > pos.X())&&(DMiniMapYOffset <= pos.Y())&&(DMiniMapYOffset + MiniHeight > pos.Y())){
        return uictMiniMap;
    }
    DescrWidth = DUnitDescriptionSurface->Width();
    DescrHeight = DUnitDescriptionSurface->Height();
    if((DUnitDescriptionXOffset <= pos.X())&&(DUnitDescriptionXOffset + DescrWidth > pos.X())&&(DUnitDescriptionYOffset <= pos.Y())&&(DUnitDescriptionYOffset + DescrHeight > pos.Y())){
        return uictUserDescription;
    }
    ActWidth = DUnitActionSurface->Width();
    ActHeight = DUnitActionSurface->Height();
    if((DUnitActionXOffset <= pos.X())&&(DUnitActionXOffset + ActWidth > pos.X())&&(DUnitActionYOffset <= pos.Y())&&(DUnitActionYOffset + ActHeight > pos.Y())){
        return uictUserAction;
    }

    if((DMenuButtonXOffset <= pos.X())&&(DMenuButtonXOffset + DMenuButtonRenderer->Width() > pos.X())&&(DMenuButtonYOffset <= pos.Y())&&(DMenuButtonYOffset + DMenuButtonRenderer->Height() > pos.Y())){
        return uictMenuButton;
    }
    return uictNone;
}

/**
* Translates a screen position to a position on the viewport.
*
* @param[in] pos A position on the screen.
*
* @return The corresponding position of the pixel in the viewport.
*
*/

CPixelPosition CApplicationData::ScreenToViewport(const CPixelPosition &pos){
    return CPixelPosition(pos.X() - DViewportXOffset, pos.Y() - DViewportYOffset);
}

/**
* Translates a position on the screen to a position on the minimap.
*
* @param[in] pos A position on the screen.
*
* @return The corresponding position on the minimap.
*
*/

CPixelPosition CApplicationData::ScreenToMiniMap(const CPixelPosition &pos){
    PrintDebug(DEBUG_LOW, "ScreenToMiniMap\n");
    return CPixelPosition(pos.X() - DMiniMapXOffset, pos.Y() - DMiniMapYOffset);
}

/**
 * Used to translate screen position to the position on the actual map.
 *
 * @param[in] pos A position on the screen.
 *
 * @return The corresponding position on the actual map.
 *
 */

CPixelPosition CApplicationData::ScreenToDetailedMap(const CPixelPosition &pos){
    PrintDebug(DEBUG_LOW, "ScreenToDetailedMap\n");
    return ViewportToDetailedMap(ScreenToViewport(pos));
}

/**
* Translates a position on the screen to a position on the unit
* description box.
*
* @param[in] pos A position on the screen.
*
* @return The corresponding position in the unit description box.
*
*/

CPixelPosition CApplicationData::ScreenToUnitDescription(const CPixelPosition &pos){
    PrintDebug(DEBUG_LOW, "ScreenToUnitDescription\n");
    return CPixelPosition(pos.X() - DUnitDescriptionXOffset, pos.Y() - DUnitDescriptionYOffset);
}

/**
* Translates a position on the screen to a postion on the unit action
* box.
*
* @param[in] pos A position in the screen
*
* @return The corresponding position in the unit action box.
*
*/

CPixelPosition CApplicationData::ScreenToUnitAction(const CPixelPosition &pos){
    PrintDebug(DEBUG_LOW, "ScreenToUnitAction\n");
    return CPixelPosition(pos.X() - DUnitActionXOffset, pos.Y() - DUnitActionYOffset);
}

/**
* Translates a position on the viewport to a position in the acutal map.
*
* @param[in] pos A position in the viewport.
*
* @return The corresponding position in the actual map.
*
*/

CPixelPosition CApplicationData::ViewportToDetailedMap(const CPixelPosition &pos){
    PrintDebug(DEBUG_LOW, "ViewportToDetailedMap\n");
    return DViewportRenderer->DetailedPosition(pos);
}

/**
 * Translates a position in the minimap to a position in the actual game map.
 *
 * @param[in] pos A position in the minimap.
 *
 * @return The corresponding position in the actual game map.
 *
 */

CPixelPosition CApplicationData::MiniMapToDetailedMap(const CPixelPosition &pos){
    int X = pos.X() * DGameModel->Map()->Width() / DMiniMapRenderer->VisibleWidth();
    int Y = pos.Y() * DGameModel->Map()->Height() / DMiniMapRenderer->VisibleHeight();

    if(0 > X){
        X = 0;
    }
    if(DGameModel->Map()->Width() <= X){
        X = DGameModel->Map()->Width() - 1;
    }
    if(0 > Y){
        Y = 0;
    }
    if(DGameModel->Map()->Height() <= Y){
        Y = DGameModel->Map()->Height() - 1;
    }
    CPixelPosition Temp;
    Temp.SetXFromTile(X);
    Temp.SetYFromTile(Y);
    return Temp;
}

/**
 * Renders menu titles
 *
 * @param[in] title The title of the menu.
 * @param[in] titlebottomy The bottom position of the title.
 * @param[in] pagewidth The width of the menu.
 * @param[in] pageheight The height of the menu.
 *
 * @return Nothing.
 *
 */

void CApplicationData::RenderMenuTitle(const std::string &title, int &titlebottomy, int &pagewidth, int &pageheight){
    int TitleWidth;
    int TextColor, ShadowColor;

    pagewidth = DWorkingBufferSurface->Width();
    pageheight = DWorkingBufferSurface->Height();
    for(int YPos = 0; YPos < pageheight; YPos += DBackgroundTileset->TileHeight()){
        for(int XPos = 0; XPos < pagewidth; XPos += DBackgroundTileset->TileWidth()){
            DBackgroundTileset->DrawTile(DWorkingBufferSurface, XPos, YPos, 0);
        }
    }
    DOuterBevel->DrawBevel(DWorkingBufferSurface, DOuterBevel->Width(), DOuterBevel->Width(), pagewidth - DOuterBevel->Width() * 2, pageheight - DOuterBevel->Width() * 2);

    DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->MeasureText(title, TitleWidth, titlebottomy);
    TextColor = DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->FindColor("white");
    ShadowColor = DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->FindColor("black");
    DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->DrawTextWithShadow(DWorkingBufferSurface, pagewidth/2 - TitleWidth/2, DOuterBevel->Width(), TextColor, ShadowColor, 1, title);
    //PrintDebug(DEBUG_HIGH, "RenderMenuTitle Done\n");
}

/**
 * Renders the loading screen of the game (the one with the knight).
 *
 * @returns Nothing.
 *
 */

void CApplicationData::RenderSplashStep(){
    double RenderAlpha = (double)DCurrentLoadingStep / (double)DTotalLoadingSteps;
    DSplashTileset->DrawTile(DDoubleBufferSurface, 0, 0, 1);
    if(RenderAlpha > 0.0){
        DSplashTileset->DrawTile(DWorkingBufferSurface, 0, 0, 0);
        auto ResourceContext = DDoubleBufferSurface->CreateResourceContext();
        ResourceContext->SetSourceSurface(DWorkingBufferSurface, 0, 0);

        ResourceContext->PaintWithAlpha(RenderAlpha);
    }

    DLoadingResourceContext->DrawSurface(DDoubleBufferSurface, 0, 0, -1, -1, 0, 0);
    DCurrentLoadingStep++;
}

/**
 * Callback function that renders the splash (loading) screen.
 *
 * @param[in] The game application
 *
 * @return Nothing.
 *
 */

void CApplicationData::SoundLoadingCallback(TSoundLibraryLoadingCalldata data){
    DApplicationDataPointer->RenderSplashStep();
}

/**
 * Callback function that changes the application mode.
 *
 * @param[in] The new game mode.
 *
 * @return Nothing.
 *
 */

void CApplicationData::ChangeApplicationMode(std::shared_ptr< CApplicationMode > mode){
    DNextApplicationMode = mode;
    DNextApplicationMode->InitializeChange(shared_from_this());
}

/**
 * Determines if the game mode is going to change.
 *
 * @return True if the game mode is change, false otherwise.
 *
 */

bool CApplicationData::ModeIsChanging(){
    return DNextApplicationMode.get() != DApplicationMode.get();
}

/**
 * Resets the player colors.
 *
 * @return Nothing.
 *
 */

void CApplicationData::ResetPlayerColors(){
    for(int Index = 0; Index < to_underlying(EPlayerColor::Max); Index++){
        DLoadingPlayerColors[Index] = static_cast<EPlayerColor>(Index);
    }
}

/**
 * Load sound options from files
 *
 * @param[in] container The directory the soundfiles are in.
 *
 * @return True if successful, false if failed.
 *
 */

bool CApplicationData::LoadSoundOptions(std::shared_ptr< CDataContainer > container){
    std::shared_ptr< CDataSource > source = container->DataSource("sound.dat");

    CCommentSkipLineDataSource LineSource(source, '#');
    std::string Value;

    // if option file does not exist, load default values
    if(nullptr == source){
        DApplicationDataPointer->DSoundVolume = 1.0;
        DApplicationDataPointer->DMusicVolume = 0.5;
        PrintError("Loaded default sound values.\n");
        return true;
    }
    bool ReturnStatus = false;

    // load soundfx and music volume values
    if( nullptr == source) {
        return false;
    }

    if(!LineSource.Read(Value)){
        PrintError("Failed to load SoundFX.\n");
        return false;
    }
    DApplicationDataPointer->DSoundVolume = std::stof(Value);

    if(!LineSource.Read(Value)){
        PrintError("Failed to load Music Volume.\n");
        return false;
    }
    DApplicationDataPointer->DMusicVolume = std::stof(Value);
    ReturnStatus = true;

    return ReturnStatus;
}

/**
 * Load network options from files
 *
 * @param[in] container The directory the network files are in.
 *
 * @return True if successful, false if failed.
 *
 */
bool CApplicationData::LoadNetworkOptions(std::shared_ptr< CDataContainer > container){
    std::shared_ptr< CDataSource > source = container->DataSource("network.dat");

    CCommentSkipLineDataSource LineSource(source, '#');
    std::string Value;

    DApplicationDataPointer->DGameName = "";
    DApplicationDataPointer->DGamePassword = "";

    // if option file does not exist, load default values
    if(source == nullptr){
        DApplicationDataPointer->DLANUsername = "user";
        DApplicationDataPointer->DLANPassword = "";
        DApplicationDataPointer->DServerUsername = "user";
        DApplicationDataPointer->DServerPassword = "";
        DApplicationDataPointer->DLANIPAddress = "0.0.0.0";
        DApplicationDataPointer->DLANPort = 55107;
        PrintError("Loaded default network values.\n");
        return true;
    }

    // load network options values
    if(!LineSource.Read(Value)){
        PrintError("Failed to load LAN username.\n");
        return false;
    }
    DApplicationDataPointer->DLANUsername = Value;

    if(!LineSource.Read(Value)){
        PrintError("Failed to load LAN IP Address.\n");
        return false;
    }
    DApplicationDataPointer->DLANIPAddress = Value;

    if(!LineSource.Read(Value)){
        PrintError("Failed to load LAN port number.\n");
        return false;
    }
    DApplicationDataPointer->DLANPort = stoi(Value);

    if(!LineSource.Read(Value)){
        PrintError("Failed to load Server username.\n");
        return false;
    }
    DApplicationDataPointer->DServerUsername = Value;

    if(!LineSource.Read(Value)){
        PrintError("Failed to load Server port number.\n");
        return false;
    }
    DApplicationDataPointer->DServerPort = stoi(Value);

    return true;
}

/**
* Loads the game map.
*
* @param[in] index Which map you want to load.
*
* @return Nothing.
*
*/

void CApplicationData::LoadGameMap(int index, std::shared_ptr< CDataSource > source){
    int DetailedMapWidth, DetailedMapHeight;
    std::string luaFile;

    DGameModel = std::make_shared< CGameModel >(index, 0x123456789ABCDEFULL, DLoadingPlayerColors);

    if(DLoadedGame){
        DGameModel->Player(DPlayerColor)->VisibilityMap()->LoadMap(source);
    }

    CEventHandler::SetGameModelReference(DGameModel);
    CEventHandler::RegisterAction();
    CEventHandler::SetEventScript(DGameModel->GetTriggerHandler()->GetEventScript());

    // Apply AI difficulty settings, assign lua file for each AIPlayer object
    for(int Index = 1; Index < to_underlying(EPlayerColor::Max); Index++){
        DGameModel->Player(DPlayerColor)->IsAI((ptAIEasy <= DLoadingPlayerTypes[Index])&&(ptAIHard >= DLoadingPlayerTypes[Index]));
    }

    // Parse the current map file for the luaFile to load
    std::vector< std::string > AIDifficultyScriptsVector;
    AIDifficultyScriptsVector = DGameModel->GetTriggerHandler()->AIDifficultyScripts();

    for(int Index = 1; Index < to_underlying(EPlayerColor::Max); Index++){
        if(DGameModel->Player(static_cast<EPlayerColor>(Index))->IsAI()){
            int Downsample = 1;
            switch(DLoadingPlayerTypes[Index]){
                case ptAIEasy:      Downsample = CPlayerAsset::UpdateFrequency();
                                    luaFile = AIDifficultyScriptsVector[0];
                                    break;
                case ptAIMedium:    Downsample = CPlayerAsset::UpdateFrequency() ;
                                    luaFile = AIDifficultyScriptsVector[1];
                                    break;
                default:            Downsample = CPlayerAsset::UpdateFrequency() ;
                                    luaFile = AIDifficultyScriptsVector[2];
                                    break;
            }
            DAIPlayers[Index] = std::make_shared< CAIPlayer > (DGameModel->Player(static_cast<EPlayerColor>(Index)), Downsample, luaFile);
        }

    }

    DCurrentAssetCapability = EAssetCapabilityType::None;

    // Set up map dimensions and tiles
    DetailedMapWidth = DGameModel->Map()->Width() * DTerrainTileset->TileWidth();
    DetailedMapHeight = DGameModel->Map()->Height() * DTerrainTileset->TileHeight();

    // Set up the renderers
    DMapRenderer = std::make_shared< CMapRenderer >(std::make_shared< CMemoryDataSource >(DMapRendererConfigurationData), DTerrainTileset, DGameModel->Player(DPlayerColor)->PlayerMap(), DTreeTileset);
    PrintDebug(DEBUG_LOW, "Creating AssetRenderer\n");
    DAssetRenderer = std::make_shared< CAssetRenderer >(DAssetRecolorMap, DAssetTilesets, DMarkerTileset, DCorpseTileset, DFireTilesets, DBuildingDeathTileset, DArrowTileset, DGameModel->Player(DPlayerColor), DGameModel->Player(DPlayerColor)->PlayerMap() );
    PrintDebug(DEBUG_LOW, "Creating FogRenderer\n");
    DFogRenderer = std::make_shared< CFogRenderer >(DFogTileset, DGameModel->Player(DPlayerColor)->VisibilityMap());
    PrintDebug(DEBUG_LOW, "Creating ViewportRenderer\n");
    DViewportRenderer = std::make_shared< CViewportRenderer>(DMapRenderer, DAssetRenderer, DFogRenderer);
    PrintDebug(DEBUG_LOW, "Creating MiniMapRenderer\n");
    DMiniMapRenderer = std::make_shared< CMiniMapRenderer >(DMapRenderer, DAssetRenderer, DFogRenderer, DViewportRenderer, DDoubleBufferSurface->Format());
    PrintDebug(DEBUG_LOW, "Creating UnitDescriptionRenderer\n");
    DUnitDescriptionRenderer = std::make_shared< CUnitDescriptionRenderer >(DMiniBevel, DIconTileset, DRangerTrackingIcon, DFonts, DPlayerColor);
    PrintDebug(DEBUG_LOW, "Creating IconDesscriptionRenderer\n");
    DIconDescriptionRenderer = std::make_shared< CUnitDescriptionRenderer >(DMiniBevel, DIconTileset, DRangerTrackingIcon, DFonts, DPlayerColor);
    PrintDebug(DEBUG_LOW, "Creating NotificationRenderer\n");
    DNotificationRenderer = std::make_shared< CUnitDescriptionRenderer >(DMiniBevel, DIconTileset, DRangerTrackingIcon, DFonts, DPlayerColor);
    PrintDebug(DEBUG_LOW, "Creating UnitActionRenderer\n");
    DUnitActionRenderer = std::make_shared< CUnitActionRenderer >(DMiniBevel, DIconTileset, DRangerTrackingIcon, DPeasantShelterIcon, DPlayerColor, DGameModel->Player(DPlayerColor));
    PrintDebug(DEBUG_LOW, "Creating ResourceRenderer\n");
    DResourceRenderer = std::make_shared< CResourceRenderer > (DMiniIconTileset, DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Medium)], DGameModel->Player(DPlayerColor));
    PrintDebug(DEBUG_LOW, "Creating ChatBoxRenderer\n");
    DChatBoxRenderer = std::make_shared< CEditRenderer > (DButtonRecolorMap, DInnerBevel, DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Small)]);
    PrintDebug(DEBUG_LOW, "Creating SoundEventRenderer\n");
    DSoundEventRenderer = std::make_shared< CSoundEventRenderer > (DSoundLibraryMixer, DGameModel->Player(DPlayerColor));
    PrintDebug(DEBUG_LOW, "Creating ButtonRenderer\n");
    DMenuButtonRenderer = std::make_shared< CButtonRenderer > (DButtonRecolorMap, DInnerBevel, DOuterBevel, DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Medium)]);

    DMenuButtonRenderer->Text("Menu");
    DMenuButtonRenderer->ButtonColor(DPlayerColor);

    int LeftPanelWidth = std::max(DUnitDescriptionRenderer->MinimumWidth(), DUnitActionRenderer->MinimumWidth()) + DOuterBevel->Width() * 4;
    LeftPanelWidth = std::max(LeftPanelWidth, MINI_MAP_MIN_WIDTH + DInnerBevel->Width() * 4);
    int MinUnitDescrHeight;

    // Set up minimap dimensions
    DMiniMapXOffset = DInnerBevel->Width() * 2;
    DUnitDescriptionXOffset = DOuterBevel->Width() * 2;
    DUnitActionXOffset = DUnitDescriptionXOffset;
    DViewportXOffset = LeftPanelWidth + DInnerBevel->Width();

    DMiniMapYOffset = DBorderWidth;
    DUnitDescriptionYOffset = DMiniMapYOffset + (LeftPanelWidth - DInnerBevel->Width() * 3) + DOuterBevel->Width() * 2;
    MinUnitDescrHeight = DUnitDescriptionRenderer->MinimumHeight(LeftPanelWidth - DOuterBevel->Width() * 4, 9);
    DUnitActionYOffset = DUnitDescriptionYOffset + MinUnitDescrHeight + DOuterBevel->Width() * 3;
    DViewportYOffset = DBorderWidth;

    int MainWindowMinHeight = DUnitDescriptionYOffset + MinUnitDescrHeight + DUnitActionRenderer->MinimumHeight() + DOuterBevel->Width() * 5;
    DMainWindow->SetMinSize(INITIAL_MAP_WIDTH, MainWindowMinHeight);
    DMainWindow->SetMaxSize(DViewportXOffset + DetailedMapWidth + DBorderWidth, std::max(MainWindowMinHeight, DetailedMapHeight + DBorderWidth * 2));

    // Resize window
    ResizeCanvases();
    PrintDebug(DEBUG_LOW, "Canvases Resized\n");
    DMenuButtonRenderer->Width(DViewportXOffset/2);
    DMenuButtonXOffset = DViewportXOffset/2 - DMenuButtonRenderer->Width()/2;
    DMenuButtonYOffset = (DViewportYOffset - DOuterBevel->Width())/2 - DMenuButtonRenderer->Height()/2;

    DIconDescriptionXOffset = DUnitActionXOffset;
    DIconDescriptionYOffset = DViewportYOffset + DViewportSurface->Height() + DOuterBevel->Width() * 2;

    DNotificationXOffset = DViewportXOffset;
    DNotificationYOffset = DUnitActionYOffset;


    int CurWidth, CurHeight;

    CurWidth = DViewportSurface->Width();
    CurHeight = DViewportSurface->Height();
    DViewportRenderer->InitViewportDimensions(CurWidth, CurHeight);

    for(auto WeakAsset : DGameModel->Player(DPlayerColor)->Assets()){
        if(auto Asset = WeakAsset.lock()){
            if(Asset->Speed() > 0){
                DViewportRenderer->CenterViewport(Asset->Position());
                break;
            }
        }
    }
}

/**
* Redraws the main window to the new size requested.
*
* @returns Nothing.
*
*/

void CApplicationData::ResizeCanvases(){
    int DrawingAreaAllocationWidth, DrawingAreaAllocationHeight;
    int ViewportWidth, ViewportHeight;
    int UserDescrWidth, UserDescrHeight;
    // Resize the canvas
    if(!DDrawingArea){
        return;
    }

    // Get the new drawing dimensions
    DrawingAreaAllocationWidth = DDrawingArea->AllocatedWidth();
    DrawingAreaAllocationHeight = DDrawingArea->AllocatedHeight();
    PrintDebug(DEBUG_LOW, "Resizing %d x %d\n",DrawingAreaAllocationWidth, DrawingAreaAllocationHeight);
    ViewportWidth = DrawingAreaAllocationWidth - DViewportXOffset - DBorderWidth;
    ViewportHeight = DrawingAreaAllocationHeight - DViewportYOffset - DBorderWidth;
    if(nullptr != DDoubleBufferSurface){
        int CurWidth, CurHeight;

        CurWidth = DDoubleBufferSurface->Width();
        CurHeight = DDoubleBufferSurface->Height();
        if((DrawingAreaAllocationWidth != CurWidth)||(DrawingAreaAllocationHeight != CurHeight)){
            DDoubleBufferSurface = nullptr;
        }
    }
    if(nullptr == DDoubleBufferSurface){
        DDoubleBufferSurface = CGraphicFactory::CreateSurface(DrawingAreaAllocationWidth, DrawingAreaAllocationHeight, CGraphicSurface::ESurfaceFormat::ARGB32); //DDrawingArea->CreateSimilarSurface(DrawingAreaAllocationWidth, DrawingAreaAllocationHeight);
    }
    if(nullptr != DWorkingBufferSurface){
        int CurWidth, CurHeight;

        CurWidth = DWorkingBufferSurface->Width();
        CurHeight = DWorkingBufferSurface->Height();
        if((DrawingAreaAllocationWidth != CurWidth)||(DrawingAreaAllocationHeight != CurHeight)){
            DWorkingBufferSurface = nullptr;
        }
    }
    if(nullptr == DWorkingBufferSurface){
        DWorkingBufferSurface = CGraphicFactory::CreateSurface(DrawingAreaAllocationWidth, DrawingAreaAllocationHeight, CGraphicSurface::ESurfaceFormat::ARGB32); //DDrawingArea->CreateSimilarSurface(DrawingAreaAllocationWidth, DrawingAreaAllocationHeight);
    }
    if(!DInnerBevel){
        return;
    }
    if(nullptr == DMiniMapSurface){
        int Dimension = DViewportXOffset - DInnerBevel->Width() * 5;
        DMiniMapSurface = CGraphicFactory::CreateSurface(Dimension, Dimension, CGraphicSurface::ESurfaceFormat::ARGB32); //DDrawingArea->CreateSimilarSurface(Dimension, Dimension);
    }
    if(nullptr != DMiniMapSurface){
        uint32_t ColorBlack = 0x000000;
        int CurWidth, CurHeight;

        CurWidth = DMiniMapSurface->Width();
        CurHeight = DMiniMapSurface->Height();
        auto ResourceContext = DMiniMapSurface->CreateResourceContext();
        ResourceContext->SetSourceRGB(ColorBlack);
        ResourceContext->Rectangle(0, 0, CurWidth, CurHeight);
        ResourceContext->Fill();
    }
    if(!DUnitDescriptionRenderer){
        return;
    }
    if(!DOuterBevel){
        return;
    }
    UserDescrWidth = DViewportXOffset - DInnerBevel->Width() - DOuterBevel->Width() * 4;
    UserDescrHeight = DUnitDescriptionRenderer->MinimumHeight(UserDescrWidth, 9);
    if(nullptr != DUnitDescriptionSurface){
        int CurWidth, CurHeight;

        CurWidth = DUnitDescriptionSurface->Width();
        CurHeight = DUnitDescriptionSurface->Height();
        if((CurWidth != UserDescrWidth)||(CurHeight != UserDescrHeight)){
            DUnitDescriptionSurface = nullptr;
        }
    }
    if(nullptr == DUnitDescriptionSurface){
        DUnitDescriptionSurface = CGraphicFactory::CreateSurface(UserDescrWidth, UserDescrHeight, CGraphicSurface::ESurfaceFormat::ARGB32); //DDrawingArea->CreateSimilarSurface(UserDescrWidth, UserDescrHeight);
    }
    DUnitActionYOffset = DUnitDescriptionYOffset + UserDescrHeight + DOuterBevel->Width() * 3;
    if(!DUnitActionRenderer){
        return;
    }
    if(nullptr != DUnitActionSurface){
        int CurWidth, CurHeight;

        CurWidth = DUnitActionSurface->Width();
        CurHeight = DUnitActionSurface->Height();
        if((CurWidth != DUnitActionRenderer->MinimumWidth())||(CurHeight != DUnitActionRenderer->MinimumHeight())){
            DUnitActionSurface = nullptr;
        }
    }
    if(nullptr == DUnitActionSurface){
        DUnitActionSurface = CGraphicFactory::CreateSurface(DUnitActionRenderer->MinimumWidth(), DUnitActionRenderer->MinimumHeight(), CGraphicSurface::ESurfaceFormat::ARGB32); //DDrawingArea->CreateSimilarSurface(DUnitActionRenderer->MinimumWidth(), DUnitActionRenderer->MinimumHeight());
    }

    DNotificationYOffset = DUnitActionYOffset;

    if(!DNotificationRenderer){
        return;
    }
    if(nullptr != DNotificationSurface){
        int CurWidth, CurHeight;

        CurWidth = DNotificationSurface->Width();
        CurHeight = DNotificationSurface->Height();
        if((CurWidth != NOTIFICATION_W) || (CurHeight != NOTIFICATION_H)){
            DNotificationSurface = nullptr;
        }

    }
    if(nullptr == DNotificationSurface){
        DNotificationSurface = CGraphicFactory::CreateSurface(NOTIFICATION_W, NOTIFICATION_H, CGraphicSurface::ESurfaceFormat::ARGB32);
        auto ResourceContext = DNotificationSurface->CreateResourceContext();
        ResourceContext->SetSourceRGB(0x000000);
        ResourceContext->Rectangle(DNotificationXOffset, DNotificationYOffset, NOTIFICATION_W, NOTIFICATION_H);
        ResourceContext->Fill();
    }

    DIconDescriptionYOffset = DViewportYOffset + ViewportHeight + DOuterBevel->Width() * 2;
    //DIconDescriptionXOffset += UserDescrWidth;
    if(!DIconDescriptionRenderer){
        return;
    }
    if(nullptr != DIconDescriptionSurface){
        int CurWidth, CurHeight;

        CurWidth = DIconDescriptionSurface->Width();
        CurHeight = DIconDescriptionSurface->Height();
        if((CurWidth != UserDescrWidth) || (CurHeight != ICON_DESCRIPTION_H)){
            DIconDescriptionSurface = nullptr;
        }
    }
    if(nullptr == DIconDescriptionSurface){
        DIconDescriptionSurface = CGraphicFactory::CreateSurface(UserDescrWidth * 4, ICON_DESCRIPTION_H, CGraphicSurface::ESurfaceFormat::ARGB32);
        auto ResourceContext = DIconDescriptionSurface->CreateResourceContext();
        ResourceContext->SetSourceRGB(0x000000);
        ResourceContext->Rectangle(DIconDescriptionXOffset, DIconDescriptionYOffset, UserDescrWidth * 4, ICON_DESCRIPTION_H);
        ResourceContext->Fill();
    }
    if(nullptr != DResourceSurface){
        int CurWidth, CurHeight;

        CurWidth = DResourceSurface->Width();
        CurHeight = DResourceSurface->Height();
        if((ViewportWidth != CurWidth)||(DBorderWidth != CurHeight)){
            DResourceSurface = nullptr;
        }
    }
    if(nullptr == DResourceSurface){
        DResourceSurface = CGraphicFactory::CreateSurface(ViewportWidth, DBorderWidth, CGraphicSurface::ESurfaceFormat::ARGB32); //DDrawingArea->CreateSimilarSurface(ViewportWidth, DBorderWidth);
    }

    if(nullptr != DViewportSurface){
        int CurWidth, CurHeight;

        CurWidth = DViewportSurface->Width();
        CurHeight = DViewportSurface->Height();
        if((ViewportWidth != CurWidth)||(ViewportHeight != CurHeight)){
            DViewportSurface = nullptr;
            DViewportTypeSurface = nullptr;
        }
    }
    if(nullptr == DViewportSurface){
        uint32_t ColorBlack = 0x000000;

        if(0 > ViewportWidth){
            ViewportWidth = 1;
        }
        if(0 > ViewportHeight){
            ViewportHeight = 1;
        }
        DViewportSurface = CGraphicFactory::CreateSurface(ViewportWidth, ViewportHeight, CGraphicSurface::ESurfaceFormat::ARGB32); //DDrawingArea->CreateSimilarSurface(ViewportWidth, ViewportHeight);
        auto ResourceContext = DViewportSurface->CreateResourceContext();
        ResourceContext->SetSourceRGB(ColorBlack);
        ResourceContext->Rectangle(0, 0, ViewportWidth, ViewportHeight);
        ResourceContext->Fill();
        DViewportTypeSurface = CGraphicFactory::CreateSurface(ViewportWidth, ViewportHeight, CGraphicSurface::ESurfaceFormat::ARGB32); //DDrawingArea->CreateSimilarSurface(ViewportWidth, ViewportHeight);
    }
    PrintDebug(DEBUG_LOW, "Resizing Complete\n");
}

/**
 * Runs the game.
 *
 * @param[in] argc The number of command line arguments.
 * @param[in] argv The command line arguments.
 *
 * return The game status.
 *
 */

int CApplicationData::Run(int argc, char *argv[]){
    return DApplication->Run(argc, argv);
}
