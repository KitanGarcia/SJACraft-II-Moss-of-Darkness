#ifndef DECORATOR_H
#define DECORATOR_H
#include "ApplicationMode.h"
#include "ApplicationData.h"
#include "BattleMode.h"   
#include "Debug.h"
#include "Rectangle.h"

using TCallbackFunction = void (*)(std::shared_ptr< CApplicationData >);

class CDecorator : public CApplicationMode {
    
    protected:
        
    public:
        std::string DTitle;
        std::vector< std::string > DButtonTexts;
        std::vector< SRectangle > DButtonLocations;
        std::vector< TCallbackFunction > DButtonFunctions;
        bool DButtonHovered;
        CDecorator();
        virtual ~CDecorator(){};
        
        virtual void Input(std::shared_ptr< CApplicationData > context);
        virtual void Calculate(std::shared_ptr< CApplicationData > context);
        virtual void Render(std::shared_ptr< CApplicationData > context);
        
        void CRender(std::shared_ptr< CApplicationData > context, int t, int w, int h, bool resize, int offsetW, int offsetH);
        void BRender(std::shared_ptr< CApplicationData > context, int t, int w, int h, bool resize, float offsetW, float offsetH);
        
        virtual void AddButtons(std::string buttonName, SRectangle location, TCallbackFunction function);

        void Clear(){
            DButtonTexts.clear();
            DButtonLocations.clear();
            DButtonFunctions.clear();
        }
};

#endif
