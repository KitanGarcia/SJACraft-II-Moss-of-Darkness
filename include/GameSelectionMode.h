#ifndef GAMESELECTIONMODE_H
#define GAMESELECTIONMODE_H

#include "ApplicationData.h"
#include "ButtonMenuMode.h"
#include "EditOptionsMode.h"
#include "ApplicationMode.h"
#include "MainMenuMode.h"
#include "Decorator.h"
#include "ChatDecorator.h"
#include "ListDecorator.h"

#include <cstddef>
#include <map>
#include <string>
#include <iostream>

class CGameSelectionMode : public CApplicationMode {
    
    protected:
        struct SPrivateConstructorType{};
        static std::shared_ptr< CGameSelectionMode > DTemplatePointer;
        std::shared_ptr< CDecorator > DDecorator;
        std::shared_ptr< CListDecorator > DListDecorator;
        
    public:

        CGameSelectionMode();
        explicit CGameSelectionMode(const SPrivateConstructorType &key);
        
        virtual void Input(std::shared_ptr< CApplicationData > context);
        virtual void Calculate(std::shared_ptr< CApplicationData > context);
        virtual void Render(std::shared_ptr< CApplicationData > context);
        
        //Callbacks 
        static std::shared_ptr< CApplicationMode > Instance();
        static void LogoutCallback(std::shared_ptr< CApplicationData > context);
        static void BackButtonCallback(std::shared_ptr< CApplicationData > context);
        static void JoinPrivateCallback(std::shared_ptr< CApplicationData > context);
        static void HostCallback(std::shared_ptr< CApplicationData > context);
        
        static void HostSortCallback(std::shared_ptr< CApplicationData > context);
        static void ELOCallback(std::shared_ptr< CApplicationData > context);
        static void OtherCallback(std::shared_ptr< CApplicationData > context);
        static void StuffCallback(std::shared_ptr< CApplicationData > context);
        
    private:
        int DWidth, DHeight, DTitleHeight;
        bool resize;

};

#endif /* TEMPLATE2_H */

