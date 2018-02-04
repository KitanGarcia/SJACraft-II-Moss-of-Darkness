#include "ChatDecorator.h"
#include <iostream>

CChatDecorator::CChatDecorator()
{
    start = 0;
    DEditSelected = 0;
    DEditSelectedCharacter = 0;
    DButtonLocations.resize(1);
    DEditLocations.resize(1);
    DEditText.push_back("");

}

void CChatDecorator::DrawChatBox(std::shared_ptr< CApplicationData > context, int BufferWidth, int BufferHeight)
{
    int XOffset = BufferWidth / 2, YOffset = 50;
    DCX = XOffset;
    DCY = YOffset;

    DChatHeight = BufferHeight / 2 - BufferHeight / 75;
    DChatWidth = BufferWidth / 2 - BufferWidth / 75;

    DSurface = CGraphicFactory::CreateSurface(DChatWidth, DChatHeight, CGraphicSurface::ESurfaceFormat::ARGB32);

    auto ResourceContext = DSurface->CreateResourceContext();
    ResourceContext->SetSourceRGB(0x000000);
    ResourceContext->Rectangle(0, 0, DChatWidth, DChatHeight);
    ResourceContext->Fill();

    context->DWorkingBufferSurface->Draw(DSurface, XOffset, YOffset, DChatWidth, DChatHeight, 0, 0);


    //context->DInnerBevel->DrawBevel(context->DWorkingBufferSurface, 400, 25, DChatWidth, DChatHeight);

}

void CChatDecorator::DrawInputBox(std::shared_ptr< CApplicationData > context, int BufferWidth, int BufferHeight)
{
    DEditSelected = 0;
    int CurrentX, CurrentY;
    int BufferCenter, OptionTop, TextOffsetY, ButtonLeft, ButtonTop;
    bool ButtonHovered = false;

    int GoldColor = context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]->FindColor("gold");
    int WhiteColor = context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]->FindColor("white");
    int ShadowColor = context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]->FindColor("black");

    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;

    //DButtonLocations.clear();
    for (auto Text : DButtonTexts) {
        context->DButtonRenderer->Text(Text);
    }

    //context->DButtonRenderer->Width(context->DButtonRenderer->Width());
    //context->DButtonRenderer->Height(context->DButtonRenderer->Height());

    ButtonLeft = BufferWidth / 2 + DChatWidth - context->DButtonRenderer->Width();
    ButtonTop = DChatHeight + 50;
    int ButtonIndex = 0;

    for (auto Text : DButtonTexts) {
        CButtonRenderer::EButtonState ButtonState = CButtonRenderer::EButtonState::None;

        context->DButtonRenderer->Text(Text);

        if (DButtonLocations[ButtonIndex].PointInside(CurrentX, CurrentY)) {
            ButtonState = context->DLeftDown ? CButtonRenderer::EButtonState::Pressed : CButtonRenderer::EButtonState::Hover;
            ButtonHovered = true;
        }

        context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface, ButtonLeft, ButtonTop, ButtonState);

        DButtonLocations[ButtonIndex] = SRectangle({ButtonLeft, ButtonTop, context->DButtonRenderer->Width(), context->DButtonRenderer->Height()});

    }

    DEditLocations.clear();
    BufferCenter = BufferWidth / 2;
    OptionTop = DChatHeight + 50;
    for (int Index = 0; Index < DEditTitles.size(); Index++) {

        std::string TempString;
        int TextWidth, TextHeight;
        TempString = DEditTitles[Index];

        context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]->MeasureText(TempString, TextWidth, TextHeight);
        //TextOffsetY = context->DOptionsEditRenderer->Height()/2 - TextHeight/2;
        TextOffsetY = 0;
        context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]->DrawTextWithShadow(context->DWorkingBufferSurface, BufferCenter - TextWidth, OptionTop + TextOffsetY, WhiteColor, ShadowColor, 1, TempString);

        context->DOptionsEditRenderer->Text(DEditText[Index], true);
        context->DOptionsEditRenderer->DrawChatEdit(context->DWorkingBufferSurface, BufferCenter, OptionTop, Index == DEditSelected ? DEditSelectedCharacter : -1, DChatWidth - context->DButtonRenderer->Width(), DChatHeight);
        DEditLocations.push_back(SRectangle({BufferCenter, OptionTop, context->DOptionsEditRenderer->Width(), context->DOptionsEditRenderer->Height()}));
    }



}

void CChatDecorator::AddEditLocation()
{
    //Initialize to Push Message
    DEditTitles.push_back(" ");
    DEditLocations.resize(1);
    DEditText.resize(1);
    DButtonTexts.push_back("Send");
    DButtonFunctions.push_back(SendMessageCallback);
    DButtonLocations.push_back(SRectangle({0, 0, 0, 0}));

}

bool CChatDecorator::SendMessageCallback(std::string str, std::list<std::string> &m)
{
    m.push_front(str);
    std::cout << "SENT: " << str << "\n";
    //context->ChangeApplicationMode(CMainMenuMode::Instance());
}

void CChatDecorator::Input(std::shared_ptr< CApplicationData > context)
{
    int CurrentX, CurrentY;
    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;
    if (context->DLeftClick && !context->DLeftDown) {
        bool ClickedEdit = false;
        for (int Index = 0; Index < DButtonLocations.size(); Index++) {
            if (DButtonLocations[Index].PointInside(CurrentX, CurrentY)) {
				context->DMultiplayerClient->sendChat(DEditText[0]);
                DButtonFunctions[Index](DEditText[0], DMessages);

                DEditText[0] = "";
                DEditSelectedCharacter = 0;
            }
        }
        for (int Index = 0; Index < DEditLocations.size(); Index++) {
            if (DEditLocations[Index].PointInside(CurrentX, CurrentY)) {
                if (Index != DEditSelected) {
                    DEditSelected = Index;
                    DEditSelectedCharacter = DEditText[Index].size();
                    ClickedEdit = true;
                }
            }
        }
        if (!ClickedEdit) {
            DEditSelected = -1;
        }
    }
    for (auto Key : context->DReleasedKeys) {
        if (SGUIKeyType::Escape == Key) {
            DEditSelected = -1;
        } else if (0 <= DEditSelected) {
            SGUIKeyType TempKey;
            TempKey.DValue = Key;
            if ((SGUIKeyType::Delete == Key) || (SGUIKeyType::BackSpace == Key)) {
                if (DEditSelectedCharacter) {
                    DEditText[DEditSelected] = DEditText[DEditSelected].substr(0, DEditSelectedCharacter - 1) + DEditText[DEditSelected].substr(DEditSelectedCharacter, DEditText[DEditSelected].length() - DEditSelectedCharacter);
                    DEditSelectedCharacter--;
                } else if (DEditText[DEditSelected].length()) {
                    DEditText[DEditSelected] = DEditText[DEditSelected].substr(1);
                }
            } else if (SGUIKeyType::LeftArrow == Key) {
                if (DEditSelectedCharacter) {
                    DEditSelectedCharacter--;
                }
            } else if (SGUIKeyType::RightArrow == Key) {
                if (DEditSelectedCharacter < DEditText[DEditSelected].length()) {
                    DEditSelectedCharacter++;
                }
            } else if (TempKey.IsAlphaNumeric() || (SGUIKeyType::Period == Key)) {
                DEditText[DEditSelected] = DEditText[DEditSelected].substr(0, DEditSelectedCharacter) + std::string(1, (char) Key) + DEditText[DEditSelected].substr(DEditSelectedCharacter, DEditText[DEditSelected].length() - DEditSelectedCharacter);
                DEditSelectedCharacter++;
            } else if (SGUIKeyType::Space == Key) {
                DEditText[0].push_back(' ');
                DEditSelectedCharacter++;
            }
        }
    }
    context->DReleasedKeys.clear();


}

void CChatDecorator::Calculate(std::shared_ptr< CApplicationData > context)
{


}

void CChatDecorator::Render(std::shared_ptr< CApplicationData > context)
{
    int TextWidth, TextHeight;
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]->MeasureText("String", TextWidth, TextHeight);
    int MaxMessages = DChatHeight / TextHeight;
    
    int DTextHeight = 0;
	context->DMultiplayerClient->getChat(DMessages, start);

    if (DMessages.size() > MaxMessages) {
        DMessages.pop_back();

    }//IF Full Pop

    for (auto msg : DMessages) {
        
        //Print with color can be different according to player color
        context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]->DrawTextWithShadow(DSurface,
                0, DTextHeight, 3, 0, 1, msg);

        DTextHeight += TextHeight;

    }//Print_All_Messages

    context->DWorkingBufferSurface->Draw(DSurface, DCX, DCY, DChatWidth, DChatHeight, 0, 0);

    if(start == 1) {
        context->ChangeApplicationMode(CBattleMode::Instance());
	start = 0;
    }
}


