#include "ListDecorator.h"
#include "Template.h"
#include "GameLobbyMode.h"
#include "ApplicationData.h"
#include <iostream>

//std::vector<std::string> Players = {"***Players***", "Player1","Player2","Player3","Player4","Player5","Player6","Player7","Player8"}; 
//std::vector<std::string> Games = {"Public Game 1", "Public Game 2","Private Game 1","ECS160 Game","Game 1","Linux Game","Multiplayer Game","SJA Game","Nitta's Game"}; 


CListDecorator::CListDecorator()
{
    percent = 0;
    DItems = 9;//Max Players + Title
    Title = "Players";
    DGamesLocations.resize(6);

    DConstructionRectangleFG = 0xA0A060;
    DConstructionRectangleBG = 0x505050;
    DConstructionRectangleCompletion = 0x307000;
    DConstructionRectangleShadow = 0x000000;
}

void CListDecorator::Input(std::shared_ptr< CApplicationData > context)
{
    int CurrentX, CurrentY;
    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;
    if (context->DLeftClick && !context->DLeftDown) {
        for (int Index = 0; Index < DGamesLocations.size(); Index++) {
            if (DGamesLocations[Index].PointInside(CurrentX, CurrentY)) {
                
                CGameLobbyMode::DTitle = context->DMultiplayerClient->games[Index];
                context->ChangeApplicationMode(CGameLobbyMode::Instance());
            }
        }
    }
}

void CListDecorator::DrawSurface(std::shared_ptr< CApplicationData > context, int x, int y, int w, int h){
      
    DSurfaceReady = CGraphicFactory::CreateSurface(w, h, CGraphicSurface::ESurfaceFormat::ARGB32);
    auto ResourceContext = DSurfaceReady->CreateResourceContext();
    ResourceContext->SetSourceRGB(0x000000);
    ResourceContext->Rectangle(0, 0, w, h);
    ResourceContext->Fill();

    DrawCompletionBar(context, DSurfaceReady, percent);
    context->DWorkingBufferSurface->Draw(DSurfaceReady, x, y, w, h, 0, 0);
    
}

void CListDecorator::DrawInfoSurface(std::shared_ptr< CApplicationData > context, int x, int y, int w, int h){
    
    
    DSurface = CGraphicFactory::CreateSurface(w, h, CGraphicSurface::ESurfaceFormat::ARGB32);
    auto ResourceContext = DSurface->CreateResourceContext();
    ResourceContext->SetSourceRGB(0x000000);
    ResourceContext->Rectangle(0, 0, w, h);
    ResourceContext->Fill();
    
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->DrawTextWithShadow(DSurface,
                w/3, 0, 3, 0, 1, "Stats");

// user info
    int inc = 30;
    int TextHeight = 30;
    for(int i = 1; i < 4; i++) {
        context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->DrawTextWithShadow(DSurface,
                w/3, inc, 1, 3, 1, context->DUserInfo[i]);
	inc += TextHeight;
    }

    context->DWorkingBufferSurface->Draw(DSurface, x, y, w, h, 0, 0);

     
}

void CListDecorator::DrawGamesSurface(std::shared_ptr< CApplicationData > context, int x, int y, int w, int h){
    
    int orig_y = y;
    int TextWidth, TextHeight;
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->MeasureText("String", TextWidth, TextHeight);


    DBoxH = (TextHeight);
    DBoxW = w;
    
    h = DBoxH * 6;
    y = 0;
    
    DSurface = CGraphicFactory::CreateSurface(w, h, CGraphicSurface::ESurfaceFormat::ARGB32);
    auto ResourceContext = DSurface->CreateResourceContext();
    ResourceContext->SetSourceRGB(0x000000);
    ResourceContext->Rectangle(0, 0, w, h);
    ResourceContext->Fill();
    for(int i = 0; i < 6; i++){
        if(context->DMultiplayerClient->games.size() > i){
			context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->DrawTextWithShadow(DSurface,
                x, y, 1, 3, 1, context->DMultiplayerClient->games[i]);
		} else {
			context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->DrawTextWithShadow(DSurface,
                x, y, 1, 3, 1, std::string(""));
		}
        
        DGamesLocations[i] = SRectangle({x, y+orig_y, DBoxW, DBoxH});
        
        context->DInnerBevel->DrawBevel(DSurface, x, y, DBoxW, TextHeight);
        
        y += TextHeight;
    }

    context->DWorkingBufferSurface->Draw(DSurface, x, orig_y, w, h, 0, 0);
    
    
}

void CListDecorator::DrawPlayerBox(std::shared_ptr< CApplicationData > context, int BufferWidth, int BufferHeight)
{
    int XOffset = 25, YOffset = 25;
    DXO = XOffset;
    DYO = YOffset;
    
    int TextWidth, TextHeight;
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->MeasureText("String", TextWidth, TextHeight);


    DBoxH = (TextHeight+context->DOuterBevel->Width()) * DItems;
    DBoxW = (BufferWidth / 4);

    DSurface = CGraphicFactory::CreateSurface(DBoxW, DBoxH, CGraphicSurface::ESurfaceFormat::ARGB32);

    auto ResourceContext = DSurface->CreateResourceContext();
    ResourceContext->SetSourceRGB(0x000000);
    ResourceContext->Rectangle(0, 0, DBoxW, DBoxH);
    ResourceContext->Fill();

    context->DWorkingBufferSurface->Draw(DSurface, XOffset, YOffset, DBoxW, DBoxH, 0, 0);     

}

void CListDecorator::Render(std::shared_ptr< CApplicationData > context){
    int YOffset = 0;
    int TextHeight = DBoxH / DItems;

	context->DMultiplayerClient->getJoinedPlayerInfo();
    context->DMultiplayerClient->getQuit();

    for(int i = 0; i < context->DMultiplayerClient->players.size(); i++){
        if(i == 0)
            context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->DrawTextWithShadow(DSurface,
                25, YOffset, 1, 3, 1, context->DMultiplayerClient->players[i]);
        else
        context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]->DrawTextWithShadow(DSurface,
                0, YOffset, i, 0, 1, context->DMultiplayerClient->players[i]);
        YOffset += TextHeight;
        
    }
    
    context->DWorkingBufferSurface->Draw(DSurface, DXO, DYO, DBoxW, DBoxH, 0, 0);
    
    YOffset = DYO;
    for(int i = 0; i < context->DMultiplayerClient->players.size(); i++){
//    for(int i = 0; i < DItems; i++){
 
        context->DInnerBevel->DrawBevel(context->DWorkingBufferSurface, DXO, YOffset, DBoxW, TextHeight);
        YOffset += TextHeight;
    }
    
}

void CListDecorator::DrawCompletionBar(std::shared_ptr< CApplicationData > context, std::shared_ptr< CGraphicSurface > surface, int percent){
    auto ResourceContext = surface->CreateResourceContext();
    int TextWidth, TextHeight, DisplayWidth, TextTop, TextBottom;
    uint32_t BlackColor = 0x000000;
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]->MeasureTextDetailed("%", TextWidth, TextHeight, TextTop, TextBottom);
    
    TextHeight = TextBottom - TextTop + 1;
    
    DDisplayedWidth = surface->Width();
    DDisplayedHeight = surface->Height();

    
    ResourceContext->SetSourceRGB(BlackColor);
    ResourceContext->Rectangle(0, DDisplayedHeight - (TextHeight + 12),  DDisplayedWidth, TextHeight + 12);
    ResourceContext->Fill();
     
    ResourceContext->SetSourceRGB(DConstructionRectangleFG);
    ResourceContext->Rectangle(1, DDisplayedHeight - (TextHeight + 11),  DDisplayedWidth-2, TextHeight + 10);
    ResourceContext->Stroke();
    
    ResourceContext->SetSourceRGB(BlackColor);
    ResourceContext->Rectangle(3, DDisplayedHeight - (TextHeight + 9),  DDisplayedWidth-6, TextHeight + 6);
    ResourceContext->Fill();

    ResourceContext->SetSourceRGB(DConstructionRectangleBG);
    ResourceContext->Rectangle(4, DDisplayedHeight - (TextHeight + 8),  DDisplayedWidth-8, TextHeight + 4);
    ResourceContext->Fill();
   
    ResourceContext->SetSourceRGB(DConstructionRectangleShadow);
    ResourceContext->Rectangle(4, DDisplayedHeight - (TextHeight + 8),  DDisplayedWidth-8, TextHeight/2 + 2);
    ResourceContext->Fill();
  
    DisplayWidth = percent * (DDisplayedWidth-8) / 100;
    
    
    ResourceContext->SetSourceRGB(DConstructionRectangleCompletion);
    ResourceContext->Rectangle(4, DDisplayedHeight - (TextHeight + 8), DisplayWidth, TextHeight + 4);
    ResourceContext->Fill();

    std::string percentage = std::to_string(percent) + "%";
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]->DrawTextWithShadow(surface, 
                        DDisplayedWidth/2 - TextWidth/2, 
                        DDisplayedHeight  - (TextHeight + TextTop + 6), 
                        1, 0, 1, percentage);
    
}

