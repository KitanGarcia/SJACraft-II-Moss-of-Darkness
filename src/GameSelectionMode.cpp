#include "GameSelectionMode.h"
#include "HostGameOptionsMode.h"
#include "LogInOptionsMode.h"
#include "NetworkOptionsMode.h"

std::shared_ptr< CGameSelectionMode > CGameSelectionMode::DTemplatePointer;
//std::shared_ptr< CDecorator > CTemplate::DDecorator; //Button decorator

CGameSelectionMode::CGameSelectionMode (const SPrivateConstructorType &key){

    DTitleHeight = 0;
    resize = false;

    int BufferWidth = DWidth = 800;
    int BufferHeight = DHeight = 600;

    DDecorator = std::make_shared<CDecorator>();
    DListDecorator = std::make_shared<CListDecorator>();

    //Add Host button
    SRectangle location;
    location.DXPosition = BufferWidth * 3/4;
    location.DYPosition = BufferHeight * 3/4;
    location.DWidth = 0;
    location.DHeight = 0;

    DDecorator->AddButtons("Host", location, HostCallback);


    //Add Join Private Button
    SRectangle location2;
    location2.DXPosition = BufferWidth * 1/4;
    location2.DYPosition = BufferHeight * 3/4;
    location2.DWidth = 0;
    location2.DHeight = 0;

    DDecorator->AddButtons("Join Private", location2, JoinPrivateCallback);

    //Add Logout Private Button
    SRectangle location3;
    location3.DXPosition = (BufferWidth * 3/4) + (BufferWidth/24);
    location3.DYPosition = BufferHeight * 1/2;
    location3.DWidth = 0;
    location3.DHeight = 0;

    DDecorator->AddButtons("Logout", location3, LogoutCallback);


    //Add Host Button
    SRectangle location4;
    location4.DXPosition = 0;
    location4.DYPosition = 0;
    location4.DWidth = 0;
    location4.DHeight = 0;

    DDecorator->AddButtons("Host2", location4, HostSortCallback);

    //Add ELO Button
    SRectangle location5;
    location5.DXPosition = 105;
    location5.DYPosition = 0;
    location5.DWidth = 0;
    location5.DHeight = 0;

    DDecorator->AddButtons("ELO", location5, ELOCallback);

    //Add Other Button
    SRectangle location6;
    location6.DXPosition = 210;
    location6.DYPosition = 0;
    location6.DWidth = 0;
    location6.DHeight = 0;

    DDecorator->AddButtons("Reload", location6, OtherCallback);

    //Add Other Button
    SRectangle location7;
    location7.DXPosition = 315;
    location7.DYPosition = 0;
    location7.DWidth = 0;
    location7.DHeight = 0;

    DDecorator->AddButtons("Stuff", location7, StuffCallback);

}

void CGameSelectionMode::HostSortCallback(std::shared_ptr< CApplicationData > context){

    //potentially Sort data by Host?
    context->ChangeApplicationMode(CMainMenuMode::Instance());
}

void CGameSelectionMode::ELOCallback(std::shared_ptr< CApplicationData > context){

    //potentially Sort data by ELO?
    context->ChangeApplicationMode(CMainMenuMode::Instance());
}

void CGameSelectionMode::OtherCallback(std::shared_ptr< CApplicationData > context){

    //potentially Sort data by ELO?
	context->DMultiplayerClient->getGameInfo();
}

void CGameSelectionMode::StuffCallback(std::shared_ptr< CApplicationData > context){

    //potentially Sort data by ELO?
    context->ChangeApplicationMode(CMainMenuMode::Instance());
}

void CGameSelectionMode::LogoutCallback(std::shared_ptr< CApplicationData > context){
    // Logout command
    context->DMultiplayerClient->logout([context](bool status)
    {
        if(status) {
            context->DGameSessionType = CApplicationData::EGameSessionType::gstSinglePlayer;
            context->DMultiplayerClient->close();
            context->ChangeApplicationMode(CLogInOptionsMode::Instance());
            return status;
        }
        return false;
    });
}


void CGameSelectionMode::BackButtonCallback(std::shared_ptr< CApplicationData > context){

    context->ChangeApplicationMode(CMainMenuMode::Instance());
}

void CGameSelectionMode::JoinPrivateCallback(std::shared_ptr< CApplicationData > context){
    //Change depending on context which menu to switch to
    context->DGameSessionType = CApplicationData::EGameSessionType::gstMultiPlayerServerClient;
    context->ChangeApplicationMode(CNetworkOptionsMode::Instance());
}

void CGameSelectionMode::HostCallback(std::shared_ptr< CApplicationData > context){
    //Change depending on context which menu to switch to
    context->DGameSessionType = CApplicationData::EGameSessionType::gstMultiPlayerServerHost;
    context->ChangeApplicationMode(CHostGameOptionsMode::Instance());
}



std::shared_ptr< CApplicationMode > CGameSelectionMode::Instance(){

    if(DTemplatePointer == nullptr){
        DTemplatePointer = std::make_shared< CGameSelectionMode >(SPrivateConstructorType());
    }
    return DTemplatePointer;
}


void CGameSelectionMode::Input(std::shared_ptr< CApplicationData > context){

    DDecorator->Input(context);
    DListDecorator->Input(context);
}

void CGameSelectionMode::Calculate(std::shared_ptr< CApplicationData > context){

}

void CGameSelectionMode::Render(std::shared_ptr< CApplicationData > context){

    int TitleHeight, BufferWidth, BufferHeight;
    float offsetW, offsetH;

    //Render Menu
    context->RenderMenuTitle(std::string(""), TitleHeight, BufferWidth, BufferHeight);

    if(DWidth == 0){
        DWidth = BufferWidth;
        DHeight = BufferHeight;
        DTitleHeight = TitleHeight;
        offsetW = offsetH = 1;
    }
    else if(DWidth != BufferWidth || DHeight != BufferHeight){
        resize = true;

        offsetW = (float)BufferWidth / DWidth;
        offsetH = (float)BufferHeight / DHeight;
        DWidth = BufferWidth;
        DHeight = BufferHeight;
        DTitleHeight = TitleHeight;

    }
    else{
        offsetW = offsetH = 1;
    }

    DDecorator->BRender(context, TitleHeight, BufferWidth, BufferHeight, resize, offsetW, offsetH);

    DListDecorator->DrawInfoSurface(context, BufferWidth*3/4, 0, (BufferWidth*1/4) - context->DInnerBevel->Width(), BufferHeight*1/2);

    DListDecorator->DrawGamesSurface(context, 0, 26, 420, BufferHeight*2/3);

    resize = false;

}
