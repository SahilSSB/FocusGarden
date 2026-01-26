#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <algorithm>
#include <functional>
#include "World.h"
#include "Player.h"
using namespace std;
using namespace sf;

struct RenderItem {
    float y;
    function<void(RenderTarget&)> drawCall;
    bool operator<(const RenderItem& other) const {
        return y < other.y;
    }
};

World::World() : mHouseSprite(mHouseTexture) {
}

FloatRect World::getBounds() {
return mTerrainMesh.getBounds();
}

void World::init() {
    if(!mTileTexture.loadFromFile("textures/tiles/Grass_tile.png")) {
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
    
   mPlayer.init("textures/Small-8-Direction-Characters_by_AxulArt.png");
   Vector2f spawnPos = gridToIso(10, 10);
   mPlayer.setPosition(spawnPos);

   if (!mRockTexture1.loadFromFile("textures/rocks/rock.png")) {
        throw runtime_error("Failed to load rock texture. Is it in the correct folder?");
   }
   if (!mRockTexture2.loadFromFile("textures/rocks/boulder_1.png")) {
        throw runtime_error("Failed to load rock texture. Is it in the correct folder?");
   }
   if (!mRockTexture3.loadFromFile("textures/rocks/Cluster_1.png")) {
        throw runtime_error("Failed to load rock texture. Is it in the correct folder?");
   }
   if (!mRockTexture4.loadFromFile("textures/rocks/rock_large_1.png")) {
        throw runtime_error("Failed to load rock texture. Is it in the correct folder?");
   }
   if (!mRockTexture5.loadFromFile("textures/rocks/rock_large_2.png")) {
        throw runtime_error("Failed to load rock texture. Is it in the correct folder?");
   }

   if (!mWaterTexture.loadFromFile("textures/tiles/water_tile.png")) {
        throw runtime_error("Failed to load water texture. Is it in the correct folder?");
   }

   if (!mFenceTexture.loadFromFile("textures/house/fence1.png")) {
       throw runtime_error("Failed to load fens texture. Is it in the correct folder?");
    }

   if (!mFenceTexture1.loadFromFile("textures/house/fence2.png")) {
       throw runtime_error("Failed to load fens texture. Is it in the correct folder?");
    }

    if (!mHouseTexture.loadFromFile("textures/house/house.png")) {
        throw runtime_error("Failed to load house texture. Is it in the correct folder?");
    }
    mHouseSprite.setTexture(mHouseTexture);

    if (mBgMusic.openFromFile("sounds/bgm/A cup of tea.wav")) {
        mBgMusic.setLooping(true);
        mBgMusic.setVolume(25.f);

        mBgMusic.play();
    }
    else {
        cout << "Error: Could not open music file" << endl;
    }

    mHoverShape.setPointCount(4);
    mHoverShape.setPoint(0, {0.f, 0.f});
    mHoverShape.setPoint(1, {TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});
    mHoverShape.setPoint(2, {0.f, TILE_HEIGHT});
    mHoverShape.setPoint(3, {-TILE_WIDTH /2.f, TILE_HEIGHT / 2.f});

    mHoverShape.setFillColor(Color(48, 48, 48,50));
    mHoverShape.setOutlineColor(Color(48, 48, 48,100));
    mHoverShape.setOutlineThickness(1.f);

    Vector2u hSize = mHouseTexture.getSize();
    cout<< "House texture loaded: " << hSize.x << "x" << hSize.y << endl;
    mHouseSprite.setOrigin({hSize.x / 2.f, static_cast<float>(hSize.y)});
    float desiredWidth = TILE_WIDTH * 8.f;
    float scaledValue = desiredWidth / static_cast<float>(hSize.x);
    mHouseSprite.setScale({scaledValue, scaledValue});
    cout << "House Scale: " << scaledValue << endl;

    mHouseAnchor = {HOUSE_X, HOUSE_Y};

    mGrid.clear();
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Tile tile;
            tile.x = x;
            tile.y = y;
            tile.isSolid = false;

            Vector2f pos = gridToIso(x, y);

            if (x >= HOUSE_X && x < HOUSE_X + HOUSE_W &&
                 y >= HOUSE_Y && y < HOUSE_Y + HOUSE_H) {
                    tile.hasHouse = true;
            }
            else {
                tile.hasHouse = false;
            }
            if (x == 13 && ( y !=0 && y <=6)) {
                tile.hasFence = true;
                tile.fenceVariant = 0;
            }
            else if (x == 5 && ( y !=0 && y <=6)) {
                tile.hasFence = true;
                tile.fenceVariant = 0;
            }
            else if ((x >= 6 && x <= 7) && ( y == 7)) {
                tile.hasFence = true;
                tile.fenceVariant = 1;
            }
            else if ((x >= 10 && x <=12) && (y == 7)) {
                tile.hasFence = true;
                tile.fenceVariant = 1;
            }
            else if ((x >= 10 && x <= 12) && (y == 0)) {
                tile.hasFence = true;
                tile.fenceVariant = 1;
            }

            tile.hasTree = false;
            if (((x == 17 || x == 18 ) && (y == 0) )) {
                tile.hasTree = true;
                tile.growthState = 1.f;
            }
            if ((x == 11 && y == 5 )) {
                tile.hasTree = true;
                tile.growthState = 1.f;
            }
            tile.growthState = tile.hasTree ? 1.f : 0.f;
            mGrid.push_back(tile);
        }
    }

    addRock(14, 19, 0);
    addRock(0, 0, 3);
    addRock(3 , 0, 2);
    addRock(14, 0, 1);

    for (int y = 15; y < MAP_HEIGHT; y++) {
        for (int x = 15; x < MAP_WIDTH; x++) {
            int idx = x + y * MAP_WIDTH;
            mGrid[idx].isWater = true;
            mGrid[idx].hasTree = false;
            mGrid[idx].hasRock = false;
        }
    }
    /*int lakeX = rand() % (MAP_WIDTH - 6) + 3;
    int lakeY = rand() % (MAP_HEIGHT - 6) + 3;
    int lakeSize = 15;

    for (int i = 0; i < lakeSize; i++) {
        int idx = lakeX + lakeY * MAP_WIDTH;
        if (!mGrid[idx].hasHouse && !mGrid[idx].hasFence) {
            mGrid[idx].isWater = true;
            mGrid[idx].hasTree = false;
        }

        int dir = rand() % 4;
        switch(dir) {
            case 0: lakeX++; break;
            case 1: lakeX--; break;
            case 2: lakeY++; break;
            case 3: lakeY--; break;
        }
        lakeX = max(0, min(lakeX, MAP_WIDTH - 1));
        lakeY = max(0, min(lakeY, MAP_HEIGHT -1));
    }*/

    mTerrainMesh.clear();
    mWaterMesh.clear();

    //Vertex array 
    mTerrainMesh.setPrimitiveType(PrimitiveType::Triangles);
    mTerrainMesh.resize(MAP_WIDTH * MAP_HEIGHT * 18);

    mWaterMesh.setPrimitiveType(PrimitiveType::Triangles);
    Vector2u waterSize = mWaterTexture.getSize();

    Vector2u texSize = mTileTexture.getSize();
    float tsX = static_cast<float>(texSize.x);
    float tsY = static_cast<float>(texSize.y);

    float capHeight = tsX / 1.7f;

    //Texture Mapping
    Vector2f uvTop = {tsX/2, 0};      
    Vector2f uvBottom = {tsX/2, capHeight}; 
    Vector2f uvRight = {tsX, capHeight/2};  
    Vector2f uvLeft = {0, capHeight/2}; 

    Vector2f uvBaseCenter = {tsX / 2.f, tsY};
    Vector2f uvBaseRight = {tsX, tsY - (capHeight / 2.f)};
    Vector2f uvBaseLeft = {0.f, tsY - (capHeight / 2.f)};
    
    //populate array 
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            float isoX = (x - y) * (TILE_WIDTH / 2.f);
            float isoY = (x + y) * (TILE_HEIGHT / 2.f);
            
            Tile& tile = mGrid[x + y * MAP_WIDTH];
            
            Vector2f ptTop    = {isoX, isoY};
            Vector2f ptBottom = {isoX, isoY + TILE_HEIGHT };
            Vector2f ptRight  = {isoX + TILE_WIDTH / 2.f, isoY + TILE_HEIGHT / 2.f};
            Vector2f ptLeft   = {isoX - TILE_WIDTH / 2.f, isoY + TILE_HEIGHT / 2.f};
            
            Vector2f ptRightDown  = ptRight + Vector2f(0, TILE_DEPTH);
            Vector2f ptBottomDown = ptBottom + Vector2f(0, TILE_DEPTH);
            Vector2f ptLeftDown   = ptLeft + Vector2f(0, TILE_DEPTH);
            
            VertexArray* targetMesh;
            if (tile.isWater) {
                targetMesh = &mWaterMesh;
                
                Vector2u waterTexSize = mWaterTexture.getSize();
                float wX = static_cast<float>(waterTexSize.x);
                float wY = static_cast<float>(waterTexSize.y);
                
                float waterCapHeight = wX / 1.7f;
                
                Vector2f wUvTop = {wX/2, 0};      
                Vector2f wUvBottom = {wX/2, waterCapHeight}; 
                Vector2f wUvRight = {wX, waterCapHeight/2};  
                Vector2f wUvLeft = {0, waterCapHeight/2}; 
                
                Vector2f wUVBaseCenter = {wX / 2.f, wY};
                Vector2f wUVBaseRight = {wX, wY - (waterCapHeight / 2.f)};
                Vector2f wUVBaseLeft = {0.f, wY - (waterCapHeight / 2.f)};

                targetMesh->append(Vertex{ptTop, Color(255, 255, 255, 150), wUvTop});
                targetMesh->append(Vertex{ptRight, Color(255, 255, 255, 150), wUvRight});
                targetMesh->append(Vertex{ptLeft, Color(255, 255, 255, 150 ), wUvLeft});
                
                targetMesh->append(Vertex{ptBottom, Color(255, 255, 255, 150 ), wUvBottom}); 
                targetMesh->append(Vertex{ptLeft, Color(255, 255, 255, 150 ), wUvLeft});
                targetMesh->append(Vertex{ptRight, Color(255, 255, 255, 150), wUvRight});
                
                bool showRightWall = true;
                if (x + 1 < MAP_WIDTH) {
                    if (mGrid[(x + 1) + y * MAP_WIDTH].isWater) {
                        showRightWall = false; // Neighbor is water, hide the wall!
                    }
                }
                bool showLeftWall = true;
                if (y + 1 < MAP_HEIGHT) {
                    if (mGrid[x + (y + 1) * MAP_WIDTH].isWater) {
                        showLeftWall = false; // Neighbor is water, hide the wall!
                    }
                }
                if (showRightWall) {
                Color rightShade(255, 255, 255, 150); 
                
                targetMesh->append(Vertex{ptRight,      rightShade, wUvRight});
                targetMesh->append(Vertex{ptRightDown,  rightShade, wUVBaseRight});
                targetMesh->append(Vertex{ptBottom,     rightShade, wUvBottom});

                targetMesh->append(Vertex{ptBottom,     rightShade, wUvBottom});
                targetMesh->append(Vertex{ptRightDown,  rightShade, wUVBaseRight});
                targetMesh->append(Vertex{ptBottomDown, rightShade, wUVBaseCenter});
                }
                if (showLeftWall) {
                Color leftShade(255, 255, 255, 150);

                targetMesh->append(Vertex{ptLeft,       leftShade, wUvLeft});
                targetMesh->append(Vertex{ptBottom,     leftShade, wUvBottom});
                targetMesh->append(Vertex{ptLeftDown,   leftShade, wUVBaseLeft});

                targetMesh->append(Vertex{ptBottom,     leftShade, wUvBottom});
                targetMesh->append(Vertex{ptBottomDown, leftShade, wUVBaseCenter});
                targetMesh->append(Vertex{ptLeftDown,   leftShade, wUVBaseLeft});
                }
            }
            else {
                targetMesh = &mTerrainMesh;

                targetMesh->append(Vertex{ptTop,    Color::White, uvTop});
                targetMesh->append(Vertex{ptRight,  Color::White, uvRight});
                targetMesh->append(Vertex{ptLeft,   Color::White, uvLeft});

                targetMesh->append(Vertex{ptBottom, Color::White, uvBottom});
                targetMesh->append(Vertex{ptLeft,   Color::White, uvLeft});
                targetMesh->append(Vertex{ptRight,  Color::White, uvRight});

                Color rightShade(180, 180, 180);
                
                targetMesh->append(Vertex{ptRight,      rightShade, uvRight});
                targetMesh->append(Vertex{ptRightDown,  rightShade, uvBaseRight});
                targetMesh->append(Vertex{ptBottom,     rightShade, uvBottom});

                targetMesh->append(Vertex{ptBottom,     rightShade, uvBottom});
                targetMesh->append(Vertex{ptRightDown,  rightShade, uvBaseRight});
                targetMesh->append(Vertex{ptBottomDown, rightShade, uvBaseCenter});

                Color leftShade(130, 130, 130);

                targetMesh->append(Vertex{ptLeft,       leftShade, uvLeft});
                targetMesh->append(Vertex{ptBottom,     leftShade, uvBottom});
                targetMesh->append(Vertex{ptLeftDown,   leftShade, uvBaseLeft});

                targetMesh->append(Vertex{ptBottom,     leftShade, uvBottom});
                targetMesh->append(Vertex{ptBottomDown, leftShade, uvBaseCenter});
                targetMesh->append(Vertex{ptLeftDown,   leftShade, uvBaseLeft});
            }
        }
    }
    initEnvironment();
    initInterior();
}

void World::update(Time dt, bool isGrowing, float growthProgress) {
    GameState currentState = isGrowing ? GameState::FOCUSSING : GameState ::ROAMING;
    mPlayer.update(dt, currentState);
    updateEnvironment(dt);
    if (!isGrowing) {
        Vector2i facingTile = getFacingTile();
        setHoveredTile(facingTile);
        return;
    }
    if (mActiveSapling.x != -1) {
    int index = mActiveSapling.x + mActiveSapling.y * MAP_WIDTH;
    if (mGrid[index].hasTree) {
        mGrid[index].growthState = growthProgress;
        if (mGrid[index].growthState > 1.f) {
             mGrid[index].growthState = 1.f;
            }
        }
    }
}

void World::destroyActiveSapling() {
    if (mActiveSapling.x != -1) {
        int index = mActiveSapling.x + mActiveSapling.y * MAP_WIDTH;
        mGrid[index].hasTree = false;
        mGrid[index].growthState = 0.f;
        mActiveSapling = {-1, -1};
        cout << "Tree destroyed due to cancelled session!" << endl;
    }
}

void World::completeTreeGrowth() {
    if (mActiveSapling.x != -1) {
        int index = mActiveSapling.x + mActiveSapling.y * MAP_WIDTH;
        mGrid[index].growthState = 1.f;
        cout << "Tree fully grown! Session complete!" << endl;
    }
}

Vector2i World::getFacingTile() {
    Vector2f pPos = mPlayer.getPosition();
    int dir = mPlayer.getDirection();

    float adjustedY = pPos.y - (11.f * 1.8f);
    
    float halfW = TILE_WIDTH / 2.f;
    float halfH = TILE_HEIGHT / 2.f;
    
    // Round instead of truncate to get nearest tile
    int playerGridX = static_cast<int>(round((adjustedY / halfH + pPos.x / halfW) / 2.f));
    int playerGridY = static_cast<int>(round((adjustedY / halfH - pPos.x / halfW) / 2.f));

    int dx = 0;
    int dy = 0;

    switch(dir) {
        case 0: dx =  1; dy = -1; break; //NE
        case 1: dx =  1; dy =  0; break; //E
        case 2: dx =  1; dy =  1; break; //SE
        case 3: dx =  0; dy =  1; break; //S
        case 4: dx = -1; dy =  1; break; //SW
        case 5: dx = -1; dy =  0; break; //W
        case 6: dx = -1; dy = -1; break; //NW
        case 7: dx =  0; dy = -1; break; //N
        default: break;
    }
    Vector2i facingTile = Vector2i(playerGridX + dx, playerGridY + dy);
    
    return facingTile;
}

void World::interact() {
    Vector2i target = getFacingTile();
    toggleTree(target.x, target.y);
}

void World::draw(RenderTarget& target) {
    /*for (const auto& bird : mBirds) {
        target.draw(bird.sprite);
    }
    for (const auto& cloud : mClouds) {
        target.draw(cloud.sprite);
    }*/

    target.draw(mTerrainMesh, &mTileTexture);

    target.draw(mHoverShape);

    target.draw(mWaterMesh, &mWaterTexture);

    vector<RenderItem> renderQueue;

    int triggerX = HOUSE_X + (HOUSE_W / 2.f);
    int triggerY = HOUSE_Y + (HOUSE_H / 2.f);
    
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

            renderQueue.push_back({
                tree.getPosition().y,
                [tree](RenderTarget& t) mutable { t.draw(tree); }
            });
        }
    if (tile.hasFence) {

        Texture* Tex;
        if (tile.fenceVariant == 0) {
            Tex = &mFenceTexture;
        }
        else {
            Tex = &mFenceTexture1;
        }

        Sprite fence(*Tex);
        Vector2f pos = gridToIso(tile.x, tile.y);
        Vector2u fSize = mFenceTexture.getSize();

        fence.setOrigin({fSize.x / 2.f, static_cast<float>(fSize.y)});
        float targetWidth = TILE_WIDTH * 1.2f;
        float scaleFactor = targetWidth / static_cast<float>(fSize.x);

        fence.setScale({scaleFactor, scaleFactor});
        fence .setPosition({pos.x, pos.y + (TILE_HEIGHT / 2.f) + 8.f});

        renderQueue.push_back({
            fence.getPosition().y,
            [fence](RenderTarget& t) mutable { t.draw(fence); }
        });
    }

    Sprite houseSprite(mHouseTexture);
    if (tile.x == triggerX && tile.y == triggerY) {
        float centerGridX = HOUSE_X + (HOUSE_W / 2.f);
        float centerGridY = HOUSE_Y + (HOUSE_H / 2.f);

        Vector2f housePos = gridToIso(static_cast<int>(centerGridX), 
                                    static_cast<int>(centerGridY));
        Vector2u hSize = mHouseTexture.getSize();

        houseSprite.setOrigin({hSize.x / 2.f, static_cast<float>(hSize.y)});
        houseSprite.setPosition(housePos);

        float scaleValue = (TILE_WIDTH * 6.f) / static_cast<float>(hSize.x);
        houseSprite.setScale({scaleValue, scaleValue});
        houseSprite.move({10.f, 45.f});

        renderQueue.push_back({
            houseSprite.getPosition().y,
            [houseSprite](RenderTarget& t) mutable { t.draw(houseSprite);}
            });
        }
    }
    renderQueue.push_back({
        mPlayer.getPosition().y,
        [&](RenderTarget & t) {mPlayer.draw(t); }
    });

    for (const auto& tile : mGrid) {
        if (tile.hasRock) {
            Texture* tex;
            float scale = 1.f;
            float offsetX = 5.f;
            float offsetY = 20.f;
            float gridShiftX = 0.f;
            float gridShiftY = 0.f;
            switch (tile.rockVariant) {
                case 0: tex = &mRockTexture1;
                        scale = 0.7f;
                        offsetX = 5.f;
                        offsetY = 20.f;
                        break;
                case 1: tex = &mRockTexture2;
                        scale = 0.5f;
                        offsetX = 5.f;
                        offsetY = 30.f;
                        break;
                case 2: tex = &mRockTexture3;
                        scale = 0.5f;
                        offsetX = 10.f;
                        offsetY = 30.f;
                        gridShiftX = 1.f;
                        gridShiftY = 1.f;
                        break;
                case 3: tex = &mRockTexture4; 
                        scale = 0.5f;
                        offsetX = 5.f;
                        offsetY = 20.f;
                        gridShiftX = 1.f;
                        gridShiftY = 1.f;
                        break;
                case 4: tex = &mRockTexture5; 
                        scale = 0.5f;
                        offsetX = 5.f;
                        offsetY = 20.f;
                        break;

            }
            Sprite rockSprite(*tex);
            rockSprite.setScale({scale, scale});
            FloatRect bounds = rockSprite.getLocalBounds();
            rockSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
            Vector2f pos = gridToIso(tile.x + gridShiftX, tile.y + gridShiftY);
            if (tile.rockVariant != 0 && tile.rockVariant != 1) {
            float correctionX = 15.f;  
            float correctionY = 10.f;
            rockSprite.setPosition({pos.x + correctionX, pos.y + correctionY + TILE_HEIGHT / 2.f});
            }
            else {
                rockSprite.setPosition({pos.x + offsetX, pos.y + offsetY + TILE_HEIGHT / 2.f});
            }
            
            renderQueue.push_back({
                rockSprite.getPosition().y,
                [rockSprite](RenderTarget& t) mutable { t.draw(rockSprite); }
        });
        }
    }

    sort(renderQueue.begin(), renderQueue.end());

    for(const auto& item : renderQueue) {
        item.drawCall(target);
    }
}

void World::toggleTree(int x, int y) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        int index = x + y * MAP_WIDTH;
        if (mGrid[index].hasHouse) {
            cout << "Blocked\n";
                return;
            }
        if (mGrid[index].hasFence) {
            cout << "Blocked\n";
            return;
        }
        if (mGrid[index].isWater) {
            cout << "Blocked\n";
            return;
        }
        if (mGrid[index].hasTree && mGrid[index].growthState >= 0.6f) {
            return;
        }
        if (x == mActiveSapling.x && y == mActiveSapling.y) {
            mGrid[index].hasTree = false;
            mGrid[index].growthState = 0.f;
            mActiveSapling = {-1, -1};
        }
        else if (!mGrid[index].hasTree) {
            if (mActiveSapling.x != -1) {
                int oldIndex = mActiveSapling.x + mActiveSapling.y * MAP_WIDTH;
                mGrid[oldIndex].hasTree = false;
                mGrid[oldIndex].growthState = 0.f;
            }
            mGrid[index].hasTree = true;
            mGrid[index].growthState = 0.f;

            mActiveSapling = {x, y};
        }
    }
}

void World::setHoveredTile(Vector2i gridPos) {
    if (gridPos.x >= 0 && gridPos.x < MAP_WIDTH &&
        gridPos.y >= 0 && gridPos.y < MAP_HEIGHT) {

            Vector2f pos = gridToIso(gridPos.x, gridPos.y);
            mHoverShape.setPosition(pos);

            mHoverShape.setFillColor(Color(48, 48, 48,50));
            mHoverShape.setOutlineColor(Color(48, 48, 48,100));
        }
    else {
        mHoverShape.setFillColor(Color::Transparent);
        mHoverShape.setOutlineColor(Color::Transparent);
    }
}

Vector2f World::gridToIso(float x, float y) {
    float isoX = (x - y) * (TILE_WIDTH / 2.f);
    float isoY = (x + y) * (TILE_HEIGHT / 2.f);
    return Vector2f(isoX, isoY);
}

Vector2i World::isoToGrid(float x, float y) {
    float adjX = x;
    float adjY = y;

    float halfW = TILE_WIDTH / 2.f;
    float halfH = TILE_HEIGHT / 2.f;

    int gridX = static_cast<int>(floor((adjY / halfH + adjX / halfW) / 2.f));
    int gridY = static_cast<int>(floor((adjY/ halfH - adjX / halfW) / 2.f));
    return Vector2i(gridX, gridY);
}

void World::save(const string& filename) {
    ofstream file(filename);

    if (file.is_open()) {
        for (const auto& tile : mGrid) {
            if (tile.hasTree) {
                file << tile.x << " " << tile.y << " " << tile.growthState << "\n";
            }
        }
        cout << "Game Saved to " << filename << endl;
    }
}

void World::load(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        int x, y;
        float growth;
        while (file >> x >> y >> growth) {
            if (x >= 0 && x < MAP_WIDTH && y >=0 && y < MAP_HEIGHT) {
                int index = x + y * MAP_WIDTH;
                mGrid[index].hasTree = true;
                mGrid[index].growthState = growth;
            }
        }
        cout << "Game Loaded from " << filename << endl;
    }
    else {
        cout << "No save file found (New Game)." << endl;
    }
}

bool World::isPositionBlocked(Vector2f worldPos, GameState state) {

    if (state == GameState::INSIDE_HOUSE) {
        return mInterior.isPositionBlocked(worldPos);
    }

    Vector2i gridPos = isoToGrid(worldPos.x, worldPos.y);

    if (!mCheckCollision) return false;

    float halfW = TILE_WIDTH / 2.f;
    float halfH = TILE_HEIGHT / 2.f;

    float visualOffsetY = 5.f;

    float adjustedY = worldPos.y - visualOffsetY;

    float gridFloatX = (adjustedY / halfH + worldPos.x / halfW) / 2.f;
    float gridFloatY = (adjustedY / halfH - worldPos.x / halfW) / 2.f;
    float topEdgeBuffer = 0.1f;

    if (gridFloatX < topEdgeBuffer || 
        gridFloatX >= static_cast<float>(MAP_WIDTH) ||
        gridFloatY < topEdgeBuffer || 
        gridFloatY >= static_cast<float>(MAP_HEIGHT)) {
            return true;
    }

    float playerSizeBuffer = 0.35f;
    auto checkTile = [&](int x, int y) -> bool {
        if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return true;
        if ((x == 8 || x == 9) && (y == 7)) return false;
        int idx = x + y * MAP_WIDTH;
        if (mGrid[idx].hasHouse) return true;
        if (mGrid[idx].hasFence) return true;
        if (mGrid[idx].isSolid) return true;
        if (mGrid[idx].isWater) return true;
        if (mGrid[idx].hasTree && mGrid[idx].growthState >= 0.66f) return true;
        return false;
    };

    int currentX = static_cast<int>(floor(gridFloatX));
    int currentY = static_cast<int>(floor(gridFloatY));
    
    if (checkTile(currentX, currentY)) return true;

    float fractX = gridFloatX - currentX;
    float fractY = gridFloatY - currentY;

    bool nearRight = fractX > (1.f - playerSizeBuffer);
    bool nearLeft  = fractX < playerSizeBuffer;
    bool nearDown  = fractY > (1.f - playerSizeBuffer);
    bool nearUp    = fractY < playerSizeBuffer;

    if (nearRight && checkTile(currentX + 1, currentY)) return true;
    if (nearLeft  && checkTile(currentX - 1, currentY)) return true;
    if (nearDown  && checkTile(currentX, currentY + 1)) return true;
    if (nearUp    && checkTile(currentX, currentY - 1)) return true;

    if (nearRight && nearDown && checkTile(currentX + 1, currentY + 1)) return true;
    if (nearRight && nearUp   && checkTile(currentX + 1, currentY - 1)) return true;
    if (nearLeft  && nearDown && checkTile(currentX - 1, currentY + 1)) return true;
    if (nearLeft  && nearUp   && checkTile(currentX - 1, currentY - 1)) return true;
    
    return false;

}

bool World::checkDoorEntry(Vector2f playerPos) {
    Vector2i gridPos = isoToGrid(playerPos.x, playerPos.y);

    float halfW = TILE_WIDTH / 2.f;
    float halfH = TILE_HEIGHT / 2.f;

    float gridFloatX = (playerPos.y / halfH + playerPos.x / halfW) / 2.f;
    float gridFloatY = (playerPos.y / halfH - playerPos.x / halfW) / 2.f;

    int gridX = static_cast<int>(gridFloatX);
    int gridY = static_cast<int>(gridFloatY);

    if ((gridX == 8 || gridX == 9) && gridY == 7) {
        return true;
    }
    return false;
}

void World::initEnvironment() {
    if(!mCloudTexture.loadFromFile("textures/house/cloud.png")) {
        cerr << "Wanring: cloud textures not found" << endl;
    }
    if (!mBirdTexture.loadFromFile("textures/house/cloud.png")) {
        cerr << "Warning: bird textures not found" << endl;
    }
    for (int i = 0; i < 5; ++i) {
        Cloud c(mCloudTexture);

        float x = static_cast<float>(rand() % 1200 - 800);
        float y = static_cast<float>(rand() % 800 - 400);
        c.sprite.setPosition({x, y});

        float scale = (rand() % 10) / 10.f;
        c.sprite.setScale({scale, scale});

        c.sprite.setColor(sf::Color(255, 255, 255));

        c.speed = scale * 50.f + 5.f;

        mClouds.push_back(c);
    }
    for (int i = 0; i < 3; ++i) {
        Bird b(mBirdTexture);

        float x = static_cast<float>(rand() % 800);
        float y = static_cast<float>(rand() % 400);

        b.sprite.setPosition({x, y});
        b.sprite.setScale({0.5f, 0.5f});

        float velX = 30.f + (rand() % 20);
        float velY = 5.f - (rand() % 10);
        b.velocity = {velX, velY};

        mBirds.push_back(b);
    }
}

void World::updateEnvironment(Time dt) {
    float mapWidth = 1000.f;

    for (auto& cloud : mClouds) {
        cloud.sprite.move({cloud.speed * dt.asSeconds(), 0.f});

        if (cloud.sprite.getPosition().x > mapWidth) {
            float newY = static_cast<float>(rand() % 60);
            cloud.sprite.setPosition({-200.f, newY});
        }
    }

    for (auto& bird : mBirds) {
        bird.sprite.move(bird.velocity * dt.asSeconds());

        if (bird.sprite.getPosition().x > mapWidth) {
            float newY = static_cast<float>(rand() % 500);
            bird.sprite.setPosition({-50.f, newY});
            float driftY = 5.f - (rand() % 10);
            bird.velocity.y = driftY;
        }
    }
    updateDayNightCycle(dt);
    updateFireFlies(dt);
}

void World::addRock(int x, int y, int variant) {
    int index = x + y * MAP_WIDTH;
    if (index >= mGrid.size()) return;

    mGrid[index].hasRock = true;
    mGrid[index].rockVariant = variant;
    mGrid[index].isSolid = true;
    mGrid[index].isWater = false;

    int width = 2;
    int height = 2;

    for (int offsetY = 0; offsetY < height; offsetY++) {
        for (int offsetX = 0; offsetX < width; offsetX++) {
            
            int targetX = x + offsetX;
            int targetY = y + offsetY;

            if (targetX < MAP_WIDTH && targetY < MAP_HEIGHT) {
                int targetIdx = targetX + targetY * MAP_WIDTH;
                mGrid[targetIdx].isSolid = true;
            }
        }
    }
}

void World::updateDayNightCycle(Time dt) {
    mTimeOfDay += dt.asSeconds() * TIME_SPEED;
    if (mTimeOfDay >= 24.f) mTimeOfDay -= 24.f;

    float sunAngle = (mTimeOfDay / 24.f) * 2.f * 3.14159f;
    float sunIntensity = (sin((mTimeOfDay / 24.f) * 6.28f - 1.57f) + 1.f) / 2.f;

    uint8_t r = static_cast<uint8_t>(30 + (225 * sunIntensity));
    uint8_t g = static_cast<uint8_t>(30 + (225 * sunIntensity));
    uint8_t b = static_cast<uint8_t>(80 + (175 * sunIntensity));

    mAmbientLight = Color(r, g, b);
}

void World::updateFireFlies(Time dt) {
    bool isNight = (mAmbientLight.r < 150);

    if (isNight && mFireflies.size() < 30) {
        if (rand() % 50 == 0) {
            Particle p;
            p.position = gridToIso(rand() % MAP_WIDTH, rand() % MAP_HEIGHT);
            p.position.y -= (rand() % 50 + 20);
            p.velocity = { (rand() % 10 - 5.f) * 2.f,
                            (rand() % 10 - 5.f) * 2.f};
            p.maxLife = (rand() % 3) + 2.f;
            p.life = p.maxLife;
            mFireflies.push_back(p);
        }
    }

    for (auto it = mFireflies.begin(); it != mFireflies.end();) {
        it->position += it->velocity * dt.asSeconds();
        it->life -= dt.asSeconds();

        if (it->life <= 0.f) {
            it = mFireflies.erase(it);
        }
        else ++it;
    }
}

Color World::getAmbientLight() const {
    return mAmbientLight;
}

void World::drawParticle(RenderTarget& target) {
    if (mAmbientLight.r < 100) {
        VertexArray glowMesh(sf::PrimitiveType::Triangles);
        
        float centerGridX = HOUSE_X + (HOUSE_W / 2.f);
        float centerGridY = HOUSE_Y + (HOUSE_H / 2.f);
    
        Vector2f houseBase = gridToIso(centerGridX, centerGridY);
        Color warmLight(255, 200, 100, 100);

        if (mAmbientLight.r < 100) {
        glowMesh.clear();
        Color haloColor(255, 210, 120, 160); 
        Color puddleColor(255, 180, 50, 100);
        Vector2f w1Pos = {houseBase.x + (-58.1f), houseBase.y + (-22.075f)};
        addGradientBlob(glowMesh, w1Pos, 15.f, 20.f, haloColor);
        Vector2f w2Pos = {houseBase.x + (-38.525f), houseBase.y + (-11.2f)};
        addGradientBlob(glowMesh, w2Pos, 15.f, 20.f, haloColor);
        addGradientBlob(glowMesh, {w2Pos.x - 15.f, w2Pos.y + 35.f}, 35.f, 20.f, puddleColor);
        Vector2f w3Pos = {houseBase.x + (77.475f), houseBase.y + (6.925f)};
        addGradientBlob(glowMesh, w3Pos, 18.f, 24.f, haloColor);        
        addGradientBlob(glowMesh, {w3Pos.x + 15.f, w3Pos.y + 45.f}, 45.f, 25.f, puddleColor);

        target.draw(glowMesh, sf::BlendAdd);
    }
    }
    if (mAmbientLight.r < 100) {
        CircleShape glow(3.f);
        glow.setFillColor(Color(255, 255, 150));

        for (const auto& p : mFireflies) {
            float fade = p.life / p.maxLife;
            Color c = glow.getFillColor();
            c.a = static_cast<uint8_t>(fade * 255);
            glow.setFillColor(c);
            glow.setPosition(p.position);
            target.draw(glow);
        }
    }
}


void World::addGradientBlob(VertexArray& va, Vector2f center, float radiusX, float radiusY, Color color) {
    int segments = 20;
    float angleStep = 6.28318f / segments;
    
    Color edgeColor = color;
    edgeColor.a = 0;

    for (int i = 0; i < segments; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        Vector2f p1 = {center.x + cos(angle1) * radiusX, center.y + sin(angle1) * radiusY};
        Vector2f p2 = {center.x + cos(angle2) * radiusX, center.y + sin(angle2) * radiusY};

        va.append(Vertex({center, color}));
        va.append(Vertex({p1, edgeColor}));
        va.append(Vertex({p2, edgeColor}));
    }
}