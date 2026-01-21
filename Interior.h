#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "tile.h"
using namespace std;
using namespace sf;

class Interior {
    public:
        void init();
        void draw(sf::RenderTarget& target);
        bool isExit(sf::Vector2f playerPos);
        bool isBlocked(sf::Vector2f playerPos);
        sf::Vector2f IgridToIso(int x, int y);
        sf::Vector2f IisoToGrid(float x, float y);
        sf::FloatRect getBounds();
    
    private:
        const int ROOM_WIDTH = 20;
        const int ROOM_HEIGHT = 20;
        const int TILE_WIDTH = 32.f;
        const int TILE_HEIGHT = 16.f;
        const int TILE_DEPTH = 5.f;
        
        const float START_X = 400.f;
        const float START_Y = 120.f;
        
        vector<Tile> mRoomGrid;
        sf::Texture mFloorTexture;
        
        sf::VertexArray mMesh;
};