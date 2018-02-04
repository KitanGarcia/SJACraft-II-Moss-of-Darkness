/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.

    All source material (source code, images, sounds, etc.) have been provided to
    University of California, Davis students of course ECS 160 for educational
    purposes. It may not be distributed beyond those enrolled in the course without
    prior permission from the copyright holder.

    All sound files, sound fonts, midi files, and images that have been included
    that were extracted from original Warcraft II by Blizzard Entertainment
    were found freely available via internet sources and have been labeld as
    abandonware. They have been included in this distribution for educational
    purposes only and this copyright notice does not attempt to claim any 
    ownership of this material.
*/
/**
*
* @class CViewportRenderer
*
* @brief This class is responsible for rendering the 3 main Viewports. Specifically, the main game window,
*        the minimap, the action descriptions, and the Unit action Icons. It has several utility functions
*        that are responsible for setting the tile sizes and making sure everything is within bounds.
*
* @author Alex
*
* @version 9.0
*
* @date 12/3/17
*
* Contact: awhelan@ucdavis.edu
*
*/

#include "ViewportRenderer.h"

/**
 * @brief ViewPort Constructor.
 * Initializes Class variables.
 * @param[in] MapRender pointer
 * @param[in] AssetRender pointer
 * @param[in] FogRender pointer
 */
CViewportRenderer::CViewportRenderer(std::shared_ptr< CMapRenderer > maprender, std::shared_ptr< CAssetRenderer > assetrender, std::shared_ptr< CFogRenderer > fogrender){
    DMapRenderer = maprender;
    DAssetRenderer = assetrender;
    DFogRenderer = fogrender;
    DViewportX = 0;
    DViewportY = 0;
    DLastViewportWidth = maprender->DetailedMapWidth();
    DLastViewportHeight = maprender->DetailedMapHeight();
}

/**
 * @brief Class Destructor
 * empty default destructor
 */
CViewportRenderer::~CViewportRenderer(){

}

/**
 * @brief ViewportX: bounds check
 * Checks if x coordinate is within screen space
 * @param[in] x input coordinate
 * @return DViewportX
 */
int CViewportRenderer::ViewportX(int x){
    DViewportX = x;
    if(DViewportX + DLastViewportWidth >= DMapRenderer->DetailedMapWidth()){
        DViewportX = DMapRenderer->DetailedMapWidth() - DLastViewportWidth;
    }
    if(0 > DViewportX){
        DViewportX = 0;
    }
    return DViewportX;
}

/**
 * @brief ViewportY: bounds check
 * Checks if y coordinate is within screen space
 * @param[in] y input coordinate
 * @return DViewportY
 */
int CViewportRenderer::ViewportY(int y){
    DViewportY = y;
    if(DViewportY + DLastViewportHeight >= DMapRenderer->DetailedMapHeight()){
        DViewportY = DMapRenderer->DetailedMapHeight() - DLastViewportHeight;
    }
    if(0 > DViewportY){
        DViewportY = 0;
    }
    return DViewportY;
}

/**
 * @brief Centers the Camera
 * Used with other UI components (i.e MiniMap) to display camera in the center. Also when clicking icon in description Viewport
 * @param[in] pos input pixel position
 */
void CViewportRenderer::CenterViewport(const CPixelPosition &pos){
    ViewportX(pos.X() - DLastViewportWidth/2);
    ViewportY(pos.Y() - DLastViewportHeight/2);
}

/**
 * @brief Moves the Camera in the North Direction
 * @param[in] pan speed along y axis
 */
void CViewportRenderer::PanNorth(int pan){
    DViewportY -= pan;
    if(0 > DViewportY){
        DViewportY = 0;
    }
}

/**
 * @brief Moves the Camera in the East Direction
 * @param[in] pan speed along x axis
 */
void CViewportRenderer::PanEast(int pan){
    ViewportX(DViewportX + pan);
}

/**
 * @brief Moves the Camera in the South Direction
 * @param[in] pan speed along -y axis
 */
void CViewportRenderer::PanSouth(int pan){
    ViewportY(DViewportY + pan);
}

/**
 * @brief Moves the Camera in the North Direction
 * @param[in] pan speed along -x axis
 */
void CViewportRenderer::PanWest(int pan){
    DViewportX -= pan;
    if(0 > DViewportX){
        DViewportX = 0;
    }
}

/**
 * @brief Used in BattleModes Render() function
 * Checks that placement is within bounds of viewport
 * Renders various assets in the viewport using draw functions /
 * from AssetRender.cpp
 *
 * @param[in] surface source surface pointer used in Draw functions
 * @param[in] typesurface pointer used in Draw functions
 * @param[in] selectionmarkerlist list of assets selected
 * @param[in] selectrect the desired position to place an asset
 * @param[in] curcapability the assets that can be created based on gold, wood, etc
 */
void CViewportRenderer::DrawViewport(std::vector< CTilePosition >& newWallTiles, std::shared_ptr< CGraphicSurface > surface, std::shared_ptr< CGraphicSurface > typesurface, const std::list< std::weak_ptr< CPlayerAsset > > &selectionmarkerlist, const SRectangle &selectrect, EAssetCapabilityType curcapability){

    SRectangle TempRectangle;
    EAssetType PlaceType = EAssetType::None;
    std::shared_ptr< CPlayerAsset > Builder;

    DLastViewportWidth = surface->Width();
    DLastViewportHeight = surface->Height();

    if(DViewportX + DLastViewportWidth >= DMapRenderer->DetailedMapWidth()){
        DViewportX = DMapRenderer->DetailedMapWidth() - DLastViewportWidth;
    }
    if(DViewportY + DLastViewportHeight >= DMapRenderer->DetailedMapHeight()){
        DViewportY = DMapRenderer->DetailedMapHeight() - DLastViewportHeight;
    }

    TempRectangle.DXPosition = DViewportX;
    TempRectangle.DYPosition = DViewportY;
    TempRectangle.DWidth = DLastViewportWidth;
    TempRectangle.DHeight = DLastViewportHeight;

    switch(curcapability){
        case EAssetCapabilityType::BuildFarm:           PlaceType = EAssetType::Farm;
                                                        break;
        case EAssetCapabilityType::BuildTownHall:       PlaceType = EAssetType::TownHall;
                                                        break;
        case EAssetCapabilityType::BuildBarracks:       PlaceType = EAssetType::Barracks;
                                                        break;
        case EAssetCapabilityType::BuildLumberMill:     PlaceType = EAssetType::LumberMill;
                                                        break;
        case EAssetCapabilityType::BuildBlacksmith:     PlaceType = EAssetType::Blacksmith;
                                                        break;
        case EAssetCapabilityType::BuildScoutTower:     PlaceType = EAssetType::ScoutTower;
                                                        break;
        case EAssetCapabilityType::BuildWall:           PlaceType = EAssetType::Wall;
                                                        break;
        case EAssetCapabilityType::BuildGoldMine:       PlaceType = EAssetType::GoldMine;
                                                        break;
        default:                                        break;
    }
    DMapRenderer->DrawMap(surface, typesurface, TempRectangle, DAssetRenderer);
    DAssetRenderer->DrawSelections(surface, TempRectangle, selectionmarkerlist, selectrect, EAssetType::None != PlaceType);
    DAssetRenderer->DrawAssets(surface, typesurface, TempRectangle);
    DAssetRenderer->DrawOverlays(surface, TempRectangle);

    if(selectionmarkerlist.size()){
        Builder = selectionmarkerlist.front().lock();
    }
    for(std::vector< CTilePosition >::iterator it = newWallTiles.begin(); it != newWallTiles.end(); it++) {
        CPixelPosition tempPixel = CPixelPosition();
        tempPixel.SetFromTile(*it);
        DAssetRenderer->DrawPlacement(surface, TempRectangle, tempPixel, PlaceType, Builder);
    }
    //DAssetRenderer->DrawPlacement(surface, TempRectangle, CPixelPosition(selectrect.DXPosition, selectrect.DYPosition), PlaceType, Builder);
    DFogRenderer->DrawMap(surface, TempRectangle);
}
