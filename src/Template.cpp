#include "Template.h"
#include <iostream>


std::shared_ptr< CTemplate > CTemplate::DTemplatePointer;
std::shared_ptr< CDecorator > CTemplate::DDecorator;
std::shared_ptr< CChatDecorator > CTemplate::DChatDecorator;
std::shared_ptr< CListDecorator > CTemplate::DListDecorator;
std::string CTemplate::DTitle;

CTemplate::CTemplate (const SPrivateConstructorType &key){
    
    DHeight = DWidth = DTitleHeight = 0;
    resize = false;
    
    DDecorator = std::make_shared<CDecorator>();
    DChatDecorator = std::make_shared<CChatDecorator>();
    DListDecorator = std::make_shared<CListDecorator>();
    
    SRectangle location;
    location.DXPosition = -110;
    location.DYPosition = -50;
    location.DWidth = 0;
    location.DHeight = 0;
       
    DDecorator->AddButtons("Start Game", location, StartButtonCallback);
    
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
    
    DChatDecorator->AddEditLocation();
    
}

void CTemplate::BackButtonCallback(std::shared_ptr< CApplicationData > context){

    context->DMultiplayerClient->sendQuit();
    context->ChangeApplicationMode(CGameSelectionMode::Instance());
}

void CTemplate::StartButtonCallback(std::shared_ptr< CApplicationData > context){
    context->DPlayerColor = static_cast<EPlayerColor>(context->DMultiplayerClient->team);

    context->DMultiplayerClient->sendStart();
    //context->DSelectedMapIndex = context->DMultiplayerClient->mapIndex;
    //context->ChangeApplicationMode(CBattleMode::Instance());
}

void CTemplate::ReadyButtonCallback(std::shared_ptr< CApplicationData > context){

	if(DListDecorator->percent == 0){
		context->DMultiplayerClient->sendReady(std::string("1"));
		DListDecorator->percent = 100;
		if(CApplicationData::gstMultiPlayerHost == context->DGameSessionType) {
			//TODO send map here
		} else if(CApplicationData::gstMultiPlayerClient ==  context->DGameSessionType) {
			//TODO get map here
		}
	}    else {
		context->DMultiplayerClient->sendReady(std::string("0"));
		DListDecorator->percent = 0;
	}

	context->DPlayerColor = context->DLoadingPlayerColors[1];
}



std::shared_ptr< CApplicationMode > CTemplate::Instance(){
    if(DTemplatePointer == nullptr){
        DTemplatePointer = std::make_shared< CTemplate >(SPrivateConstructorType());        
    }
	
	
    return DTemplatePointer;
}

CTemplate::~CTemplate() {
}
       
void CTemplate::Input(std::shared_ptr< CApplicationData > context){
    DDecorator->Input(context);
    DChatDecorator->Input(context);
}

void CTemplate::Calculate(std::shared_ptr< CApplicationData > context){
    
}

void CTemplate::Render(std::shared_ptr< CApplicationData > context){
    
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


