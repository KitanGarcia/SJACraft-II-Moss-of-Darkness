#include "GameLobbyMode.h"
#include "GameSelectionMode.h"
#include "MultiPlayerOptionsMenuMode.h"
#include <iostream>


std::shared_ptr< CGameLobbyMode > CGameLobbyMode::DGameLobbyModePointer;
std::shared_ptr< CDecorator > CGameLobbyMode::DDecorator;
std::shared_ptr< CChatDecorator > CGameLobbyMode::DChatDecorator;
std::shared_ptr< CListDecorator > CGameLobbyMode::DListDecorator;
std::string CGameLobbyMode::DTitle;

CGameLobbyMode::CGameLobbyMode (const SPrivateConstructorType &key){
    
    DHeight = DWidth = DTitleHeight = 0;
    resize = false;
    
    DDecorator = std::make_shared<CDecorator>();
    DChatDecorator = std::make_shared<CChatDecorator>();
    DListDecorator = std::make_shared<CListDecorator>();
    
    DChatDecorator->AddEditLocation();
}

void CGameLobbyMode::BackButtonCallback(std::shared_ptr< CApplicationData > context){
    context->DMapConfirmed = false;
    context->DGameName = "";
    context->DGamePassword = "";
    
    if(context->DGameSessionType == CApplicationData::EGameSessionType::gstMultiPlayerServerHost || context->DGameSessionType == CApplicationData::EGameSessionType::gstMultiPlayerServerClient){
        std::cout << "Server\n";
        context->DMultiplayerClient->sendQuit();
        context->DGameSessionType = CApplicationData::EGameSessionType::gstMultiPlayerServer;
        context->ChangeApplicationMode(CGameSelectionMode::Instance());
    }
    else{
        context->DGameSessionType = CApplicationData::EGameSessionType::gstMultiPlayerLAN;
        context->ChangeApplicationMode(CMultiPlayerOptionsMenuMode::Instance());
    }
}

void CGameLobbyMode::StartButtonCallback(std::shared_ptr< CApplicationData > context){
    context->DPlayerColor = static_cast<EPlayerColor>(context->DMultiplayerClient->team);


    context->DMultiplayerClient->sendStart();
    //context->ChangeApplicationMode(CBattleMode::Instance());
}

void CGameLobbyMode::ReadyButtonCallback(std::shared_ptr< CApplicationData > context){
    
    if(DListDecorator->percent == 0){
		context->DMultiplayerClient->sendReady(std::string("1"));
        DListDecorator->percent = 100;
	}
    else{
		context->DMultiplayerClient->sendReady(std::string("0"));
        DListDecorator->percent = 0;
	}
}



std::shared_ptr< CApplicationMode > CGameLobbyMode::Instance(){

    if(DGameLobbyModePointer == nullptr){
        DGameLobbyModePointer = std::make_shared< CGameLobbyMode >(SPrivateConstructorType());        
    }
    return DGameLobbyModePointer;
}

CGameLobbyMode::~CGameLobbyMode() {
}
       
void CGameLobbyMode::Input(std::shared_ptr< CApplicationData > context){
    DDecorator->Input(context);
    DChatDecorator->Input(context);
}

void CGameLobbyMode::Calculate(std::shared_ptr< CApplicationData > context){
    
}

void CGameLobbyMode::Render(std::shared_ptr< CApplicationData > context){
    
    int TitleHeight, BufferWidth, BufferHeight;
    int offsetW, offsetH;
    
    //Render Menu    
    context->RenderMenuTitle(DTitle, TitleHeight, BufferWidth, BufferHeight);
    
    if(DWidth == 0){
        DWidth = BufferWidth;
        DHeight = BufferHeight;
        DTitleHeight = TitleHeight;
        offsetW = offsetH = 0;
    }
    else if(DWidth != BufferWidth || DHeight != BufferHeight){
        resize = true;
        offsetW = BufferWidth - DWidth;
        offsetH = BufferHeight - DHeight;
        DWidth = BufferWidth;
        DHeight = BufferHeight;
        DTitleHeight = TitleHeight;
    }
    
    DDecorator->CRender(context, TitleHeight, BufferWidth, BufferHeight, resize, offsetW, offsetH);
       
    
    DChatDecorator->DrawChatBox(context, BufferWidth, BufferHeight);
    
    
    DChatDecorator->DrawInputBox(context, BufferWidth, BufferHeight);
    

    DChatDecorator->Render(context);
    
    DListDecorator->DrawPlayerBox(context, BufferWidth, BufferHeight);
    DListDecorator->Render(context);
    
    DListDecorator->DrawSurface(context, 25, 500, 100, 25);

    resize = false;

}

void CGameLobbyMode::InitializeChange(std::shared_ptr< CApplicationData > context){
    SRectangle location;
    location.DXPosition = -110;
    location.DYPosition = -50;
    location.DWidth = 0;
    location.DHeight = 0;
    
    DDecorator->Clear();

    if(context->DGameSessionType != CApplicationData::gstMultiPlayerServerClient && context->DGameSessionType != CApplicationData::gstMultiPlayerLANClient){
        DDecorator->AddButtons("Start Game", location, StartButtonCallback);
    }
    
    SRectangle location2;
    location2.DXPosition = location.DXPosition - 105;
    location2.DYPosition = -50;
    location2.DWidth = 0;
    location2.DHeight = 0;
       
    DDecorator->AddButtons("Back", location2, BackButtonCallback);
    
    
    SRectangle location3;
    location3.DXPosition = 25;
    location3.DYPosition = 535;
    location3.DWidth = 0;
    location3.DHeight = 0;
       
    DDecorator->AddButtons("Ready", location3, ReadyButtonCallback);
}


