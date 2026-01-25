#pragma once 
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <fstream>
#include "Player.h"
#include "Interior.h"
#include "tile.h"
using namespace std;
using namespace sf;

struct Cloud {
    Sprite sprite;
    float speed = 0.f;

    Cloud(const Texture& tex) : sprite(tex) {}
};

struct Bird {
    Sprite sprite;
    Vector2f velocity;

    Bird(const Texture& tex) : sprite(tex) {}
};

class World {
    public:
        World();
        void init();
        void update(Time dt, bool isGrowing, float growthProgress);
        void draw(RenderTarget& target);
        void setHoveredTile(Vector2i gridPos);

        void save(const string& filename);
        void load(const string& filename);
        void finishSession();

        FloatRect getBounds();
        //math
        Vector2f gridToIso(float x, float y);
        Vector2i isoToGrid(float x, float y);

        //mechanics
        void destroyActiveSapling();
        void completeTreeGrowth();
        Vector2i getActiveSaplingPos() const { return mActiveSapling; }
        void plantTree(int x, int y);
        void burnTree(int x, int y);
        void toggleTree(int x, int y);
        
        void interact();
        Vector2i getFacingTile();

        bool isPositionBlocked(Vector2f worldPos, GameState state);
        bool checkDoorEntry(Vector2f playerPos);
        
        Player& getPlayer() { return mPlayer; }
        Vector2f getPlayerPosition() const { return mPlayer.getPosition(); }
        void setPlayerPosition(sf::Vector2f pos) { mPlayer.setPosition(pos); }
        void drawPlayer(sf::RenderTarget& target) { mPlayer.draw(target); }
        
        void disablePlayerCollision() { mCheckCollision = false; }
        void enablePlayerCollision() { mCheckCollision = true;}

        void initEnvironment();
        void updateEnvironment(Time dt);

        Interior& getInterior() { return mInterior; }
        void initInterior() { mInterior.init(); }
        
    private:
        const int MAP_WIDTH = 20;
        const int MAP_HEIGHT = 20;
        const float TILE_HEIGHT = 16.f;
        const float TILE_WIDTH = 32.f;
        const float TILE_DEPTH = 10;
        const int HOUSE_X = 4;
        const int HOUSE_Y = 0;
        const int HOUSE_H = 6;
        const int HOUSE_W = 8;
        ConvexShape mHoverShape;
        bool mCheckCollision = true;
        
        vector<Tile> mGrid;
        Vector2i mActiveSapling = {-1, -1};
        VertexArray mTerrainMesh;
        VertexArray mWaterMesh;
        Vector2i mHouseAnchor;
        vector<Cloud> mClouds;
        vector<Bird> mBirds;

        void addRock(int x, int y, int variant);

        Player mPlayer;
        Interior mInterior;

        //resources
        Texture mTileTexture;
        Texture mTreeSappling;
        Texture mTreeMedium;
        Texture mTreeMature;
        Texture mHouseTexture;
        Sprite mHouseSprite;
        Texture mCloudTexture;
        Texture mBirdTexture;
        Texture mFenceTexture;
        Texture mFenceTexture1;
        Texture mRockTexture1;
        Texture mRockTexture2;
        Texture mRockTexture3;
        Texture mRockTexture4;
        Texture mRockTexture5;
        Texture mWaterTexture;

};
