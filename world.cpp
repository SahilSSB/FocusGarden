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
   mPlayer.setCollissionCallback([this](Vector2f pos) {
        return this->isPositionBlocked(pos);
   });

   if (!mRockTexture.loadFromFile("textures/rock.png")) {
        throw runtime_error("Failed to load rock texture. Is it in the correct folder?");
   }

   if (!mWaterTexture.loadFromFile("textures/tiles/water_tile.png")) {
        throw runtime_error("Failed to load water texture. Is it in the correct folder?");
   }

   if (!mFenceTexture.loadFromFile("textures/house /fence1.png")) {
       throw runtime_error("Failed to load fens texture. Is it in the correct folder?");
    }

   if (!mFenceTexture1.loadFromFile("textures/house /fence2.png")) {
       throw runtime_error("Failed to load fens texture. Is it in the correct folder?");
    }

    if (!mHouseTexture.loadFromFile("textures/house /house.png")) {
        throw runtime_error("Failed to load house texture. Is it in the correct folder?");
    }
    mHouseSprite.setTexture(mHouseTexture);

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
            if ((x == 11 && y == 5 )) {
                tile.hasTree = true;
                tile.growthState = 1.f;
            }
            tile.growthState = tile.hasTree ? 1.f : 0.f;
            mGrid.push_back(tile);
        }
    }

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

    /*for (auto& tile : mGrid) {
        if (!tile.isWater && !tile.hasHouse && !tile.hasFence && !tile.hasTree) {
            if (rand() % 100 < 5) {
                tile.hasRock = true;
            }
        }
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

                targetMesh->append(Vertex{ptTop, Color(255, 255, 255), wUvTop});
                targetMesh->append(Vertex{ptRight, Color(255, 255, 255), wUvRight});
                targetMesh->append(Vertex{ptLeft, Color(255, 255, 255), wUvLeft});
                
                targetMesh->append(Vertex{ptBottom, Color(255, 255, 255), wUvBottom}); 
                targetMesh->append(Vertex{ptLeft, Color(255, 255, 255), wUvLeft});
                targetMesh->append(Vertex{ptRight, Color(255, 255, 255), wUvRight});

                Color rightShade(180, 180, 180); 
                
                targetMesh->append(Vertex{ptRight,      rightShade, wUvRight});
                targetMesh->append(Vertex{ptRightDown,  rightShade, wUVBaseRight});
                targetMesh->append(Vertex{ptBottom,     rightShade, wUvBottom});

                targetMesh->append(Vertex{ptBottom,     rightShade, wUvBottom});
                targetMesh->append(Vertex{ptRightDown,  rightShade, wUVBaseRight});
                targetMesh->append(Vertex{ptBottomDown, rightShade, wUVBaseCenter});

                Color leftShade(130, 130, 130);

                targetMesh->append(Vertex{ptLeft,       leftShade, wUvLeft});
                targetMesh->append(Vertex{ptBottom,     leftShade, wUvBottom});
                targetMesh->append(Vertex{ptLeftDown,   leftShade, wUVBaseLeft});

                targetMesh->append(Vertex{ptBottom,     leftShade, wUvBottom});
                targetMesh->append(Vertex{ptBottomDown, leftShade, wUVBaseCenter});
                targetMesh->append(Vertex{ptLeftDown,   leftShade, wUVBaseLeft});
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

void World::update(Time dt, bool isFocussing) {
    GameState currentState = isFocussing ? GameState::FOCUSSING : GameState ::ROAMING;
    mPlayer.update(dt, currentState);
    updateEnvironment(dt);
    if (!isFocussing) {
        Vector2i facingTile = getFacingTile();
        setHoveredTile(facingTile);
        return;
    } 
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
        houseSprite.move({10.f, 50.f});

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
            Sprite rockSprite(mRockTexture);
            rockSprite.setScale({0.2f, 0.2f});
            FloatRect bounds = rockSprite.getLocalBounds();
            rockSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
            Vector2f pos = gridToIso(tile.x, tile.y);
            rockSprite.setPosition({pos.x, pos.y + TILE_HEIGHT / 2.f});
            
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
        if (mGrid[index].hasTree && mGrid[index].growthState >= 1.f) {
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

Vector2f World::gridToIso(int x, int y) {
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

void World::finishSession() {
    mActiveSapling = {-1, -1};
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

bool World::isPositionBlocked(Vector2f worldPos) {
    Vector2i gridPos = isoToGrid(worldPos.x, worldPos.y);

    if (!mCheckCollision) return false;

    float halfW = TILE_WIDTH / 2.f;
    float halfH = TILE_HEIGHT / 2.f;

    float visualOffsetY = 5.f;

    float adjustedY = worldPos.y - visualOffsetY;

    float gridFloatX = (adjustedY / halfH + worldPos.x / halfW) / 2.f;
    float gridFloatY = (adjustedY / halfH - worldPos.x / halfW) / 2.f;
    float topEdgeBuffer = 0.9f;

    if (gridFloatX < topEdgeBuffer || 
        gridFloatX >= static_cast<float>(MAP_WIDTH) ||
        gridFloatY < topEdgeBuffer || 
        gridFloatY >= static_cast<float>(MAP_HEIGHT)) {
            return true;
    }

    float playerSizeBuffer = 0.3f;
    auto checkTile = [&](int x, int y) -> bool {
        if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return true;
        if ((x == 8 || x == 9) && (y == 7)) return false;
        int idx = x + y * MAP_WIDTH;
        if (mGrid[idx].hasHouse) return true;
        if (mGrid[idx].hasFence) return true;
        if (mGrid[idx].hasRock) return true;
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
    if(!mCloudTexture.loadFromFile("textures/cloud.png")) {
        throw runtime_error("Wanring: cloud textures not found");
    }
    if (!mBirdTexture.loadFromFile("textures/cloud.png")) {
        throw runtime_error("Warning: bird textures not found");
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
}