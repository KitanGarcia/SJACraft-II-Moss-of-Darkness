#ifndef CHATDECORATOR_H
#define CHATDECORATOR_H

#include "ApplicationMode.h"
#include "ApplicationData.h"
#include "BattleMode.h"   
#include "Debug.h"
#include "Rectangle.h"
#include <vector>
#include <string>
#include <list>


using TChatCallbackFunction = bool (*)(std::string, std::list<std::string> &);

class CChatDecorator : public CApplicationMode {
public:

    std::string DTitle;
    std::vector< std::string > DButtonTexts;
    std::vector< TChatCallbackFunction > DButtonFunctions;
    std::vector< SRectangle > DButtonLocations;
    bool DButtonHovered;

    int DEditSelected;
    int DEditSelectedCharacter;
    std::string DInput;
    std::list< std::string > DMessages;
    int start;

    std::vector< SRectangle > DEditLocations;
    std::vector< std::string > DEditTitles;
    std::vector< std::string > DEditText;
    std::vector< TChatCallbackFunction > DEditValidationFunctions;
    std::shared_ptr<CGraphicSurface> DSurface;

    int DChatWidth, DChatHeight;
    int DCX, DCY; //offsets for ChatBox
    void DrawChatBox(std::shared_ptr< CApplicationData > context, int BufferWidth, int BufferHeight);
    void DrawInputBox(std::shared_ptr< CApplicationData > context, int BufferWidth, int BufferHeight);
    void AddEditLocation();
    static bool SendMessageCallback(std::string str, std::list<std::string> &m);

    CChatDecorator();

    virtual ~CChatDecorator() {
    };

    virtual void Input(std::shared_ptr< CApplicationData > context);
    virtual void Calculate(std::shared_ptr< CApplicationData > context);
    virtual void Render(std::shared_ptr< CApplicationData > context);
protected:

private:


};

#endif /* CHATDECORATOR_H */

