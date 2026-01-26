#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "tile.h"
#include <map>
#include <memory>
#include <string>
#include <fstream>
#include <functional>
#include <algorithm>
using namespace std;

class Interior {
    public:
        Interior();
        void init();
        void draw(sf::RenderTarget& target, sf::Vector2f playerWorldPos, function<void()> drawPlayerFunc, bool isEditing);
        bool isExit(sf::Vector2f playerPos);
        bool isBlocked(sf::Vector2f playerPos);
        bool isPositionBlocked(sf::Vector2f worldPos);
        bool isComputer(sf::Vector2f playerPos);
        sf::Vector2f IgridToIso(int x, int y);
        sf::Vector2f IisoToGrid(float x, float y);
        sf::FloatRect getBounds();
        void generateWalls();
        void loadTextures();
        
        void updateEditor(sf::RenderWindow& window, sf::View& view);
        bool handleEditorInput(sf::RenderWindow& window, sf::View& view, const sf::Event& event);
        void saveFurnitureLayout();
        void defineFurniture();
        void addObject(const string& name, int x, int y);
        void rebuildCollisions();
        void loadFurnitureLayout();
        void deselectAndRefresh();

    
    private:
        const int ROOM_WIDTH = 15;
        const int ROOM_HEIGHT = 15;
        const int TILE_WIDTH = 32.f;
        const int TILE_HEIGHT = 16.f;
        const int TILE_DEPTH = 5.f;
        const float WALL_HEIGHT = 100.f;
        const float WALL_THICKNESS = 10.f;
        
        const float START_X = 400.f;
        const float START_Y = 120.f;
        
        sf::Texture mFloorTexture;
        sf::Texture mComputerTexture;
        sf::Sprite mComputerSprite; 
        sf::VertexArray mMesh;
        sf::VertexArray mWallMesh;
        map<string, sf::Texture> mInteriorTexture;
        vector<sf::Vector2i> mDebugBlockedTiles;

        map<string, Furniture> mFurnitureDef;
        vector<PlacedFurniture> mPlacedObjects;
        PlacedFurniture* mSelectedObject = nullptr;
        vector<sf::Vector2i> mBlockedTiles;
};
