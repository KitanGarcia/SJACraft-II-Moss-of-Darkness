#ifndef LISTDECORATOR_H
#define LISTDECORATOR_H

#include "UnitDescriptionRenderer.h"
#include "ApplicationMode.h"
#include "ApplicationData.h"
#include "BattleMode.h"   
#include "Debug.h"
#include "Rectangle.h"
#include <vector>
#include <string>
#include <list>

class CListDecorator {
    
public:
    int percent;
    CListDecorator();
    virtual ~CListDecorator(){};
    void DrawPlayerBox(std::shared_ptr< CApplicationData > context, int BufferWidth, int BufferHeight);
    void DrawInfoSurface(std::shared_ptr< CApplicationData > context, int x, int y, int w, int h);
    void DrawGamesSurface(std::shared_ptr< CApplicationData > context, int x, int y, int w, int h);
    void DrawSurface(std::shared_ptr< CApplicationData > context, int x, int y, int w, int h);
    void DrawCompletionBar(std::shared_ptr< CApplicationData > context, std::shared_ptr< CGraphicSurface > surface, int percent);
    virtual void Render(std::shared_ptr< CApplicationData > context);
    virtual void Input(std::shared_ptr< CApplicationData > context);
protected:
    std::vector< SRectangle > DGamesLocations;

    std::shared_ptr<CGraphicSurface> DSurface;
    std::shared_ptr<CGraphicSurface> DSurfaceReady;
    uint32_t DConstructionRectangleFG;
    uint32_t DConstructionRectangleBG;
    uint32_t DConstructionRectangleCompletion;
    uint32_t DConstructionRectangleShadow;
    int DDisplayedWidth;
    int DDisplayedHeight;
    
private:
    int DItems, DXO, DYO;//Max Items and Offset (i.e Starting at XO, YO)
    int DBoxH, DBoxW; // Box dimensions
    std::string Title;

};

#endif /* LISTDECORATOR_H */

