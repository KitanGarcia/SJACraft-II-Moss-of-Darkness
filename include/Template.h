/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Template.h
 * Author: Whelan
 *
 * Created on November 10, 2017, 1:14 PM
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <cstddef>
#include <map>
#include <string>
#include "ApplicationData.h"
#include "ButtonMenuMode.h"
#include "EditOptionsMode.h"
#include "ApplicationMode.h"
#include "MainMenuMode.h"
#include "Decorator.h"
#include "ChatDecorator.h"
#include "ListDecorator.h"

class CTemplate : public CDecorator {
    
    protected:
        struct SPrivateConstructorType{};
        static std::shared_ptr< CDecorator > DDecorator;
        static std::shared_ptr< CChatDecorator > DChatDecorator;
        static std::shared_ptr< CListDecorator > DListDecorator;
        static std::shared_ptr< CTemplate > DTemplatePointer;
        
    public:
        static std::string DTitle;
        CTemplate();
        CTemplate(const CTemplate& orig) = delete;
        virtual ~CTemplate();
        explicit CTemplate(const SPrivateConstructorType &key);
        
        virtual void Input(std::shared_ptr< CApplicationData > context);
        virtual void Calculate(std::shared_ptr< CApplicationData > context);
        virtual void Render(std::shared_ptr< CApplicationData > context);
        
        //Callbacks 
        static void BackButtonCallback(std::shared_ptr< CApplicationData > context);
        static void StartButtonCallback(std::shared_ptr< CApplicationData > context);
        static void ReadyButtonCallback(std::shared_ptr< CApplicationData > context);
        static std::shared_ptr< CApplicationMode > Instance();
    private:
        int DWidth, DHeight, DTitleHeight;
        bool resize;

};

#endif /* TEMPLATE_H */

