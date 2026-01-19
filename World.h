#pragma once 
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <fstream>
using namespace std;
using namespace sf;

struct Tile {
    int x, y;
    bool hasTree;

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
    
    private:
        const int MAP_WIDTH = 20;
        const int MAP_HEIGHT = 20;
        const float TILE_HEIGHT = 16.f;
        const float TILE_WIDTH = 32.f;
        const float TILE_DEPTH = 20.f;

        ConvexShape mHoverShape;
        
        vector<Tile> mGrid;
        Vector2i mActiveSapling = {-1, -1};
        VertexArray mTerrainMesh;

        //resources
        Texture mTileTexture;
        Texture mTreeSappling;
        Texture mTreeMedium;
        Texture mTreeMature;

};
