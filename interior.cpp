#include "Interior.h"
#include <iostream>
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

Interior::Interior() : mComputerSprite(mComputerTexture) {}
void Interior::init() {
    if (!mFloorTexture.loadFromFile("textures/house/house_tile.png")) {
        Image img;
        img.resize({32, 32}, Color(139, 69, 19));
        if (!mFloorTexture.loadFromImage(img)) {
            cerr << "Failed to load fallback texture" << endl;
        }
    }
    if (!mComputerTexture.loadFromFile("textures/house/computer_table.png")) {
        throw runtime_error("Failed to load computer texture");
    }
    mComputerSprite.setTexture(mComputerTexture, true);
    FloatRect bounds = mComputerSprite.getLocalBounds();
    mComputerSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y}); 
    sf::Vector2f tilePos = IgridToIso(0, 5);
    mComputerSprite.setScale({0.375f, 0.375f});
    mComputerSprite.setPosition({tilePos.x + 15.f, tilePos.y + (TILE_HEIGHT / 2.f) + 10.f});

    generateWalls();

    mMesh.setPrimitiveType(PrimitiveType::Triangles);
    mMesh.resize(ROOM_WIDTH * ROOM_HEIGHT * 18);


    Vector2u texSize = mFloorTexture.getSize();

    float tsX = static_cast<float>(texSize.x);
    float tsY = static_cast<float>(texSize.y);

    for (int y = 0; y < ROOM_HEIGHT; y++) {
        for (int x = 0; x < ROOM_WIDTH; x ++) {
            float tileCapHeight = tsX / 2.f;

            Vector2f pos = IgridToIso(x, y);

            Vector2f uvTop = {tsX / 2.f , 0.f};
            Vector2f uvBottom = {tsX / 2.f, tileCapHeight};
            Vector2f uvRight = {tsX, tileCapHeight / 2.f};
            Vector2f uvLeft = {0, tileCapHeight / 2.f};

            Vector2f ptTop    = {pos.x, pos.y};
            Vector2f ptBottom = {pos.x, pos.y + TILE_HEIGHT };
            Vector2f ptRight  = {pos.x + TILE_WIDTH / 2.f, pos.y + TILE_HEIGHT / 2.f};
            Vector2f ptLeft   = {pos.x - TILE_WIDTH / 2.f, pos.y + TILE_HEIGHT / 2.f};
            
            Vector2f ptRightDown  = ptRight + Vector2f(0, TILE_DEPTH);
            Vector2f ptBottomDown = ptBottom + Vector2f(0, TILE_DEPTH);
            Vector2f ptLeftDown   = ptLeft + Vector2f(0, TILE_DEPTH);

            Vector2f uvBaseCenter = {tsX / 2.f, tsY};
            Vector2f uvBaseRight = {tsX, tsY - (tileCapHeight / 2.f)};
            Vector2f uvBaseLeft = {0.f, tsY - (tileCapHeight / 2.f)};

            int idx = (x + y * ROOM_WIDTH) * 18;

            mMesh[idx+0] = {ptTop, Color::White, uvTop};
            mMesh[idx+1] = {ptRight, Color::White, uvRight};
            mMesh[idx+2] = {ptLeft, Color::White, uvLeft};

            mMesh[idx+3] = {ptRight, Color::White, uvBottom};
            mMesh[idx+4] = {ptBottom, Color::White, uvLeft};
            mMesh[idx+5] = {ptLeft, Color::White, uvRight};

            Color rightShade(180, 180, 180);

            mMesh[idx+6] =  {ptRight,      rightShade, uvRight};
            mMesh[idx+7] =  {ptRightDown,  rightShade, uvBaseRight};
            mMesh[idx+8] =  {ptBottom,     rightShade, uvBottom};

            mMesh[idx+9] =  {ptBottom,     rightShade, uvBottom};
            mMesh[idx+10] =  {ptRightDown,  rightShade, uvBaseRight};
            mMesh[idx+11] =  {ptBottomDown, rightShade, uvBaseCenter};

            Color leftShade(130, 130, 130);

            mMesh[idx+12] =  {ptLeft,       leftShade, uvLeft};
            mMesh[idx+13] =  {ptBottom,     leftShade, uvBottom};
            mMesh[idx+14] =  {ptLeftDown,   leftShade, uvBaseLeft};

            mMesh[idx+15] =  {ptBottom,     leftShade, uvBottom};
            mMesh[idx+16] =  {ptBottomDown, leftShade, uvBaseCenter};
            mMesh[idx+17] =  {ptLeftDown,   leftShade, uvBaseLeft};
        }
    }
}

void Interior::draw(RenderTarget& target, float playerY, function<void()> drawPlayerFunc) {
    target.draw(mMesh, &mFloorTexture);
    target.draw(mWallMesh);

    vector<pair<float, function<void()>>> renderQueue;

    renderQueue.push_back({
        mComputerSprite.getPosition().y,
        [&]() { target.draw(mComputerSprite); }
    });
    renderQueue.push_back({
        playerY,
        drawPlayerFunc
    });

    sort(renderQueue.begin(), renderQueue.end(),
    [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    for (const auto& item : renderQueue) {
        item.second();
    }
    // DEBUG: Draw the Exit Tile
    sf::Vector2f pos = IgridToIso(9, 19);

    sf::ConvexShape debugExit;
    debugExit.setPointCount(4);
    
    // FIX: Add static_cast<float>(...) around TILE_HEIGHT
    debugExit.setPoint(0, {0.f, 0.f});
    debugExit.setPoint(1, {TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});
    debugExit.setPoint(2, {0.f, static_cast<float>(TILE_HEIGHT)});   // <--- Fixed line
    debugExit.setPoint(3, {-TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});

    debugExit.setPosition(pos);
    debugExit.setFillColor(sf::Color(0, 255, 0, 100)); 
    debugExit.setOutlineColor(sf::Color::Green);
    debugExit.setOutlineThickness(1.f);

    target.draw(debugExit);
    
    /*Vector2f computerPos = IgridToIso(0, 5); 

    ConvexShape debugComputer;
    debugComputer.setPointCount(4);
    
    debugComputer.setPoint(0, {0.f, 0.f});
    debugComputer.setPoint(1, {TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});
    debugComputer.setPoint(2, {0.f, static_cast<float>(TILE_HEIGHT)});
    debugComputer.setPoint(3, {-TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});

    debugComputer.setPosition(computerPos);
    debugComputer.setFillColor(sf::Color(0, 0, 255, 100)); 
    debugComputer.setOutlineColor(sf::Color::Blue);
    debugComputer.setOutlineThickness(1.f);

    target.draw(debugComputer);*/
}

Vector2f Interior::IgridToIso(int x, int y) {
    float isoX = (x - y) * (TILE_WIDTH / 2.f);
    float isoY = (x + y) * (TILE_HEIGHT / 2.f);
    return Vector2f(START_X + isoX, START_Y + isoY);
}

Vector2f Interior::IisoToGrid(float x, float y) {
    float adjX = x - START_X;
    float adjY = y - START_Y;
    
    float halfW = TILE_WIDTH / 2.f;
    float halfH = TILE_HEIGHT / 2.f;

    int gridX = static_cast<int>(floor((adjY / halfH + adjX / halfW) / 2.f));
    int gridY = static_cast<int>(floor((adjY / halfH - adjX / halfW) / 2.f));

    return Vector2f(gridX, gridY);
}

bool Interior::isExit(Vector2f playerPos) {
    Vector2f grid = IisoToGrid(playerPos.x, playerPos.y);

    if (grid.x == 9 && grid.y == 19) {
        return true;
    }
    return false;
}

bool Interior::isBlocked(Vector2f playerPos) {
    Vector2f grid = IisoToGrid(playerPos.x, playerPos.y);
    if (grid.x < 0 || grid.x >= ROOM_WIDTH || grid.y < 0 || grid.y >= ROOM_HEIGHT) {
        return true;
    }
    return false;
}

FloatRect Interior::getBounds() {
    FloatRect bounds = mMesh.getBounds();
    bounds.position.y -= 120.f; 
    bounds.size.y += 120.f;
    return bounds;
}

bool Interior::isPositionBlocked(Vector2f worldPos) {

    Vector2f gridPos = IisoToGrid(worldPos.x, worldPos.y);

    int gridX = static_cast<int>(gridPos.x);
    int gridY = static_cast<int>(gridPos.y);

    if (gridX < 0 || gridX >= ROOM_WIDTH || gridY < 0 || gridY >= ROOM_HEIGHT) {
        return true;
    }
    return false;
}

bool Interior::isComputer(Vector2f playerPos) {
    Vector2f grid = IisoToGrid(playerPos.x, playerPos.y);

    int gridX = static_cast<int>(grid.x);
    int gridY = static_cast<int>(grid.y);

     if (gridX == 1 && gridY == 5 ||
        gridX == 1 && gridY == 6 ||
        gridX == 1 && gridY == 4) {
            return true;
        }
    return false;
}

void Interior::generateWalls() {
    mWallMesh.setPrimitiveType(PrimitiveType::Triangles);
    mWallMesh.clear();

    Vector2f p0 = IgridToIso(0, 0);
    Vector2f p1 = IgridToIso(1, 0);
    Vector2f vecX = p1 - p0;
    Vector2f thickVecLeft = -vecX * 0.4f; 

    Vector2f pY1 = IgridToIso(0, 1);
    Vector2f vecY = pY1 - p0;
    Vector2f thickVecRight = -vecY * 0.4f;

    Color wallColorLeft(245, 222, 179);
    Color wallColorRight(220, 200, 160);     
    Color rimColorTop(101, 67, 33);
    Color rimColorSide(139, 90, 43);

    auto addQuad = [&](Vector2f p1, Vector2f p2, Vector2f p3, Vector2f p4, Color c) {
    mWallMesh.append(Vertex({p1, c}));
    mWallMesh.append(Vertex({p2, c}));
    mWallMesh.append(Vertex({p3, c}));
    mWallMesh.append(Vertex({p3, c}));
    mWallMesh.append(Vertex({p4, c}));
    mWallMesh.append(Vertex({p1, c}));
    };
    
    Vector2f backCorner = IgridToIso(0, 0);
    Vector2f leftEnd = IgridToIso(0, 20);
    Vector2f rightEnd = IgridToIso(20, 0);

    float yOffset = 0.f;
    backCorner.y += yOffset;
    leftEnd.y += yOffset;
    rightEnd.y += yOffset;

    addQuad(
        leftEnd, backCorner,
        {backCorner.x, backCorner.y - WALL_HEIGHT},
        {leftEnd.x, leftEnd.y - WALL_HEIGHT},
        wallColorLeft
    );

    addQuad(
        backCorner, rightEnd,
        {rightEnd.x, rightEnd.y - WALL_HEIGHT},
        {backCorner.x, backCorner.y - WALL_HEIGHT},
        wallColorRight
    );

    addQuad(
        {leftEnd.x, leftEnd.y - WALL_HEIGHT},
        {backCorner.x, backCorner.y - WALL_HEIGHT},
        {backCorner.x + thickVecLeft.x, backCorner.y - WALL_HEIGHT + thickVecLeft.y},
        {leftEnd.x + thickVecLeft.x, leftEnd.y - WALL_HEIGHT + thickVecLeft.y},
        rimColorTop
    );

    addQuad(
        {backCorner.x, backCorner.y - WALL_HEIGHT},
        {rightEnd.x, rightEnd.y - WALL_HEIGHT},
        {rightEnd.x + thickVecRight.x, rightEnd.y - WALL_HEIGHT + thickVecRight.y},
        {backCorner.x + thickVecRight.x, backCorner.y - WALL_HEIGHT + thickVecRight.y},
        rimColorTop
    );

    addQuad(
        {leftEnd.x + thickVecLeft.x, leftEnd.y + thickVecLeft.y},
        leftEnd,
        {leftEnd.x, leftEnd.y - WALL_HEIGHT},
        {leftEnd.x + thickVecLeft.x, leftEnd.y - WALL_HEIGHT + thickVecLeft.y},
        rimColorSide
    );

    addQuad(
        rightEnd,
        {rightEnd.x + thickVecRight.x, rightEnd.y + thickVecRight.y}, 
        {rightEnd.x + thickVecRight.x, rightEnd.y - WALL_HEIGHT + thickVecRight.y},
        {rightEnd.x, rightEnd.y - WALL_HEIGHT},
        rimColorSide
    );

    mWallMesh.append(Vertex{{backCorner.x, backCorner.y - WALL_HEIGHT}, rimColorTop});
    mWallMesh.append(Vertex{{backCorner.x + thickVecLeft.x, backCorner.y - WALL_HEIGHT + thickVecLeft.y}, rimColorTop});
    mWallMesh.append(Vertex{{backCorner.x + thickVecRight.x, backCorner.y - WALL_HEIGHT + thickVecRight.y}, rimColorTop});
    
    Vector2f backOuterCenter = {
        backCorner.x + thickVecLeft.x + thickVecRight.x,
        backCorner.y - WALL_HEIGHT + thickVecLeft.y + thickVecRight.y
    };
    
    addQuad(
        {backCorner.x, backCorner.y - WALL_HEIGHT},
        {backCorner.x + thickVecRight.x, backCorner.y - WALL_HEIGHT + thickVecRight.y},
        backOuterCenter,
        {backCorner.x + thickVecLeft.x, backCorner.y - WALL_HEIGHT + thickVecLeft.y},
        rimColorTop
    );
}
