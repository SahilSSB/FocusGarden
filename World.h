#pragma once 
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <fstream>
#include "Player.h"
using namespace std;
using namespace sf;

struct Tile {
    int x, y;
    bool hasTree;
    bool hasHouse = false;
    float growthState = 0.0f;
};

class World {
    public:
        World();
        void init();
        void update(Time dt, bool isFocussing);
        void draw(RenderTarget& target);
        void setHoveredTile(Vector2i gridPos);

        void save(const string& filename);
        void load(const string& filename);
        void finishSession();

        FloatRect getBounds();
        //math
        Vector2f gridToIso(int x, int y);
        Vector2i isoToGrid(float x, float y);

        //mechanics
        void plantTree(int x, int y);
        void burnTree(int x, int y);
        void toggleTree(int x, int y);
        
        void interact();
        Vector2i getFacingTile();

        bool isPositionBlocked(Vector2f worldPos);
    
    private:
        const int MAP_WIDTH = 20;
        const int MAP_HEIGHT = 20;
        const float TILE_HEIGHT = 16.f;
        const float TILE_WIDTH = 32.f;
        const float TILE_DEPTH = 10.f;
        const int HOUSE_X = 4;
        const int HOUSE_Y = 0;
        const int HOUSE_H = 6;
        const int HOUSE_W = 8;
        ConvexShape mHoverShape;
        
        vector<Tile> mGrid;
        Vector2i mActiveSapling = {-1, -1};
        VertexArray mTerrainMesh;
        Vector2i mHouseAnchor;

        Player mPlayer;

        //resources
        Texture mTileTexture;
        Texture mTreeSappling;
        Texture mTreeMedium;
        Texture mTreeMature;
        Texture mHouseTexture;
        Sprite mHouseSprite;

};
