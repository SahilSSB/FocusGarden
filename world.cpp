#include <SFML/Graphics.hpp>
#include <optional>
#include "World.h"
using namespace std;
using namespace sf;

World::World() {

}

void World::init() {
    if(!mTileTexture.loadFromFile("textures/tiles/tile-1.png")) {
        throw runtime_error("Failed to load tile texture. Is it in the correct folder?");
    }
    bool s1 = mTreeSappling.loadFromFile("textures/plants/tree_small.png");
    bool s2 = mTreeMedium.loadFromFile("textures/plants/tree_medium.png");
    bool s3 = mTreeMature.loadFromFile("textures/plants/tree_large.png");

    if (!s1 || !s2 || !s3) {
        if(!mTreeSappling.loadFromFile("textures/plants/tree_small.png") ||
            mTreeMedium.loadFromFile("textures/plants/tree_medium.png") ||
            mTreeMature.loadFromFile("textures/plants/tree_large.png")) {
                throw runtime_error("Failed to load of the tree textures (small/medium/lage). Is it in the correct folder?");
            }
    }
    mGrid.clear();
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            Tile tile;
            tile.x = x;
            tile.y = y;

            tile.hasTree = false;
            tile.growthState = tile.hasTree ? 1.f : 0.f;
            mGrid.push_back(tile);
        }
    }

    //Vertex array 
    mTerrainMesh.setPrimitiveType(PrimitiveType::Triangles);
    mTerrainMesh.resize(MAP_WIDTH * MAP_HEIGHT * 18);

    //populate array 
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            float isoX = (x - y) * (TILE_WIDTH / 2.f) + 400.f;
            float isoY = (x + y) * (TILE_HEIGHT / 2.f);

            Vector2f ptTop    = {isoX, isoY};
            Vector2f ptBottom = {isoX, isoY + TILE_HEIGHT};
            Vector2f ptRight  = {isoX + TILE_WIDTH / 2.f, isoY + TILE_HEIGHT / 2.f};
            Vector2f ptLeft   = {isoX - TILE_WIDTH / 2.f, isoY + TILE_HEIGHT / 2.f};
            
            Vector2f ptRightDown  = ptRight + Vector2f(0, TILE_DEPTH);
            Vector2f ptBottomDown = ptBottom + Vector2f(0, TILE_DEPTH);
            Vector2f ptLeftDown   = ptLeft + Vector2f(0, TILE_DEPTH);

            Vector2u texSize = mTileTexture.getSize();
            float tsX = static_cast<float>(texSize.x);
            float tsY = static_cast<float>(texSize.y);

            //Texture Mapping
            Vector2f uvTop = {tsX/2, 0};      
            Vector2f uvBottom = {tsX/2, tsY}; 
            Vector2f uvRight = {tsX, tsY/2};  
            Vector2f uvLeft = {0, tsY/2};   
        
            Vertex* tri = &mTerrainMesh[(x + y * MAP_WIDTH) * 18];

            tri[0].position = ptTop;    tri[0].texCoords = uvTop;
            tri[1].position = ptRight;  tri[1].texCoords = uvRight;
            tri[2].position = ptLeft;   tri[2].texCoords = uvLeft;
            tri[3].position = ptBottom; tri[3].texCoords = uvBottom;
            tri[4].position = ptLeft;   tri[4].texCoords = uvLeft;
            tri[5].position = ptRight;  tri[5].texCoords = uvRight;
            
            tri[6].position = ptRight;       tri[6].texCoords = uvRight;
            tri[7].position = ptRightDown;   tri[7].texCoords = uvRight;
            tri[8].position = ptBottom;      tri[8].texCoords = uvBottom;
            tri[9].position  = ptBottom;     tri[9].texCoords = uvBottom;
            tri[10].position = ptRightDown;  tri[10].texCoords = uvRight;
            tri[11].position = ptBottomDown; tri[11].texCoords = uvBottom;
            
            tri[12].position = ptLeft;       tri[12].texCoords = uvLeft;
            tri[13].position = ptBottom;     tri[13].texCoords = uvBottom;
            tri[14].position = ptLeftDown;   tri[14].texCoords = uvLeft;
            tri[15].position = ptBottom;     tri[15].texCoords = uvBottom;
            tri[16].position = ptBottomDown; tri[16].texCoords = uvBottom;
            tri[17].position = ptLeftDown;   tri[17].texCoords = uvLeft;
            
            Color shadowColor(180, 180, 180);
            for(int i=6; i<12; i++) tri[i].color = shadowColor;
            Color darkShadowColor(130, 130, 130);
            for(int i=12; i<18; i++) tri[i].color = darkShadowColor;
        }
    }
}

void World::update(Time dt, bool isFocussing) {
    if (!isFocussing) return;

    float growthSpeed = 1.f / 10.f;
    for (auto& tile : mGrid) {
        if (tile.hasTree) {
            if (tile.growthState < 1.f) {
                tile.growthState += growthSpeed * dt.asSeconds();

                if (tile.growthState > 1.f) tile.growthState = 1.f;
            }
        }
    }
}

Vector2f World::gridToIso(int x, int y) {
    float isoX = (x - y) * (TILE_WIDTH / 2.f) + 400.f;
    float isoY = (x + y) * (TILE_HEIGHT / 2.f);
    return Vector2f(isoX, isoY);
}

Vector2i World::isoToGrid(float x, float y) {
    float adjX = 400.f - x;
    float adjY = y;

    float halfW = TILE_WIDTH / 2.f;
    float halfH = TILE_HEIGHT / 2.f;

    int gridX = static_cast<int>((adjY / halfH + adjX / halfW) / 2.f);
    int gridY = static_cast<int>((adjY/ halfH - adjX / halfW) / 2.f);
    return Vector2i(gridX, gridY);
}

void World::toggleTree(int x, int y) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        int index = x + y * MAP_WIDTH;
        if (index >=0 && index < mGrid.size()) {
            mGrid[index].hasTree = !mGrid[index].hasTree;
        
        if (mGrid[index].hasTree) {
            mGrid[index].growthState = 0.f;
            }
        }
    }
}


void World::draw(RenderTarget& target) {
    target.draw(mTerrainMesh, &mTileTexture);

    for (const auto& tile : mGrid) {
        if (tile.hasTree) {
            Texture* currentTexture;
            float verticaloffset = 0.f;
            float offset = 0.f;
            float treeHeight = TILE_HEIGHT * 2.f;
            if (tile.growthState < 0.33f) {
                currentTexture = &mTreeSappling;
                offset = +10.f;
                verticaloffset = TILE_HEIGHT / 2.f;
                treeHeight = treeHeight;
            }
            else if (tile.growthState < 0.66f) {
                currentTexture = &mTreeMedium;
                offset = +19.f;
                verticaloffset = TILE_HEIGHT / 2.f;
                treeHeight = treeHeight * 2.f;
            }
            else {
                currentTexture = &mTreeMature;
                offset = +4.5f;
                verticaloffset = TILE_HEIGHT;
                treeHeight = treeHeight * 3.f;
            }

            Sprite tree(*currentTexture);

            Vector2f pos = gridToIso(tile.x, tile.y);
            FloatRect bounds = tree.getLocalBounds();
            tree.setOrigin({bounds.size.x / 2.f , bounds.size.y});
            float centerX = pos.x;
            float centerY = pos.y;
            float scaleFactor = treeHeight / bounds.size.y;
            tree.setScale({scaleFactor, scaleFactor}); 
            tree.setPosition({pos.x, pos.y + verticaloffset + offset});
            target.draw(tree);
        }
    }
}


