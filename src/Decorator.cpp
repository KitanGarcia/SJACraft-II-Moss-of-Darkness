#include "Decorator.h"
#include <iostream>

CDecorator::CDecorator()
{


}

void CDecorator::Input(std::shared_ptr< CApplicationData > context)
{
    int CurrentX, CurrentY;
    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;
    if (context->DLeftClick && !context->DLeftDown) {
        for (int Index = 0; Index < DButtonLocations.size(); Index++) {
            if (DButtonLocations[Index].PointInside(CurrentX, CurrentY)) {
                DButtonFunctions[Index](context);
            }
        }
    }
}

void CDecorator::Calculate(std::shared_ptr< CApplicationData > context)
{

}

void CDecorator::Render(std::shared_ptr< CApplicationData > context)
{


}

void CDecorator::CRender(std::shared_ptr< CApplicationData > context, int t, int w, int h, bool resize, int offsetW, int offsetH)
{

    int CurrentX, CurrentY;
    int BufferWidth = w, BufferHeight = h;
    int TitleHeight = t;
    int ButtonLeft = 0, ButtonTop = 0;
    int ButtonIndex;
    bool ButtonXAlign = false, ButtonHovered = false;

    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;

    context->DButtonRenderer->Text(DButtonTexts.front(), true);
    for (auto Text : DButtonTexts) {
        context->DButtonRenderer->Text(Text);
    }

    context->DButtonRenderer->Height(context->DButtonRenderer->Height() * 3 / 2);
    //context->DButtonRenderer->Width(context->DButtonRenderer->Width() * 5 / 4);


    ButtonLeft = 0;
    ButtonTop = 0;

    if ((ButtonLeft <= CurrentX)&&((ButtonLeft + context->DButtonRenderer->Width() > CurrentX))) {
        ButtonXAlign = true;
    }
    ButtonIndex = 0;

    for (auto Text : DButtonTexts) {

        context->DButtonRenderer->Width(Text.length() * 10);

        if (Text.length()) {
            CButtonRenderer::EButtonState ButtonState = CButtonRenderer::EButtonState::None;


            if (DButtonLocations[ButtonIndex].PointInside(CurrentX, CurrentY)) {
                ButtonState = context->DLeftDown ? CButtonRenderer::EButtonState::Pressed : CButtonRenderer::EButtonState::Hover;
                ButtonHovered = true;
            }

            context->DButtonRenderer->Text(Text);

            if (DButtonLocations[ButtonIndex].DXPosition < 0 && !resize) {
                ButtonLeft = BufferWidth + DButtonLocations[ButtonIndex].DXPosition;
                ButtonTop = BufferHeight + DButtonLocations[ButtonIndex].DYPosition;
                DButtonLocations[ButtonIndex] = SRectangle({ButtonLeft, ButtonTop, context->DButtonRenderer->Width(), context->DButtonRenderer->Height()});
            } else if (DButtonLocations[ButtonIndex].DXPosition > 0 && !resize) {
                ButtonLeft = DButtonLocations[ButtonIndex].DXPosition;
                ButtonTop = DButtonLocations[ButtonIndex].DYPosition;
                DButtonLocations[ButtonIndex] = SRectangle({ButtonLeft, ButtonTop, context->DButtonRenderer->Width(), context->DButtonRenderer->Height()});
            } else if (resize) {
                if (ButtonIndex != 2) {
                    ButtonLeft = DButtonLocations[ButtonIndex].DXPosition + offsetW;
                    ButtonTop = DButtonLocations[ButtonIndex].DYPosition + offsetH;
                    DButtonLocations[ButtonIndex] = SRectangle({ButtonLeft, ButtonTop, context->DButtonRenderer->Width(), context->DButtonRenderer->Height()});
                } else {
                    ButtonLeft = DButtonLocations[ButtonIndex].DXPosition;
                    ButtonTop = DButtonLocations[ButtonIndex].DYPosition;
                    DButtonLocations[ButtonIndex] = SRectangle({ButtonLeft, ButtonTop, context->DButtonRenderer->Width(), context->DButtonRenderer->Height()});
                }
            }

            context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface, DButtonLocations[ButtonIndex].DXPosition, DButtonLocations[ButtonIndex].DYPosition, ButtonState);

        } else {
            //DButtonLocations[ButtonIndex] = SRectangle({0, 0, 0, 0});
        }

        ButtonIndex++;

    }
    if (!DButtonHovered && ButtonHovered) {
        context->DSoundLibraryMixer->PlayClip(context->DSoundLibraryMixer->FindClip("tick"), context->DSoundVolume, 0.0);
    }

    if (context->ModeIsChanging()) {
        context->DSoundLibraryMixer->PlayClip(context->DSoundLibraryMixer->FindClip("place"), context->DSoundVolume, 0.0);
    }
    DButtonHovered = ButtonHovered;
}

void CDecorator::BRender(std::shared_ptr< CApplicationData > context, int t, int w, int h, bool resize, float offsetW, float offsetH)
{
    
    int CurrentX, CurrentY;
    int BufferWidth = w, BufferHeight = h;
    int TitleHeight = t;
    int ButtonLeft, ButtonTop;
    int ButtonIndex;
    bool ButtonHovered = false;

    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;
    
    //std::cout << " X = " << CurrentX << "\n";
    //std::cout << " Y = " << CurrentY << "\n";

    context->DButtonRenderer->Text(DButtonTexts.front(), true);
    for (auto Text : DButtonTexts) {
        context->DButtonRenderer->Text(Text);
    }
    context->DButtonRenderer->Height(context->DButtonRenderer->Height());
    context->DButtonRenderer->Width(context->DButtonRenderer->Width());

    
    ButtonIndex = 0;

    DButtonLocations.resize(DButtonTexts.size());
    for (auto Text : DButtonTexts) {
        
        if(ButtonIndex < 3){
            ButtonLeft = DButtonLocations[ButtonIndex].DXPosition;
            ButtonTop =  DButtonLocations[ButtonIndex].DYPosition;
            ButtonLeft *= offsetW;
            ButtonTop *= offsetH;
        }
        else{
            ButtonLeft = DButtonLocations[ButtonIndex].DXPosition;
            ButtonTop =  DButtonLocations[ButtonIndex].DYPosition;   
        }

        
        if (Text.length()) {
            CButtonRenderer::EButtonState ButtonState = CButtonRenderer::EButtonState::None;
            
            if (DButtonLocations[ButtonIndex].PointInside(CurrentX, CurrentY)) {
                ButtonState = context->DLeftDown ? CButtonRenderer::EButtonState::Pressed : CButtonRenderer::EButtonState::Hover;
                ButtonHovered = true;
            }
            
            context->DButtonRenderer->Text(Text);
            context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface, ButtonLeft, ButtonTop, ButtonState);
            DButtonLocations[ButtonIndex] = SRectangle({ButtonLeft, ButtonTop, context->DButtonRenderer->Width(), context->DButtonRenderer->Height()});
        } else {
            //DButtonLocations[ButtonIndex] = SRectangle({0, 0, 0, 0});
        }
        ButtonIndex++;
    }
    if (!DButtonHovered && ButtonHovered) {
        context->DSoundLibraryMixer->PlayClip(context->DSoundLibraryMixer->FindClip("tick"), context->DSoundVolume, 0.0);
    }

    if (context->ModeIsChanging()) {
        context->DSoundLibraryMixer->PlayClip(context->DSoundLibraryMixer->FindClip("place"), context->DSoundVolume, 0.0);
    }
    DButtonHovered = ButtonHovered;
}

void CDecorator::AddButtons(std::string buttonName, SRectangle location, TCallbackFunction function)
{

    DButtonTexts.push_back(buttonName);
    DButtonFunctions.push_back(function);
    DButtonLocations.push_back(location);

}
