#include "Interior.h"
#include <iostream>
#include <SFML/Graphics.hpp>

void Interior::init() {
    if (!mFloorTexture.loadFromFile("")) {
        Image img;
        img.resize({32, 32}, Color(139, 69, 19));
        if (!mFloorTexture.loadFromImage(img)) {
            cerr << "Failed to load fallback texture" << endl;
        }
    }
    mMesh.setPrimitiveType(PrimitiveType::Triangles);
    mMesh.resize(ROOM_WIDTH * ROOM_HEIGHT * 6);

    
    Vector2u texSize = mFloorTexture.getSize();

    float tsX = static_cast<float>(texSize.x);
    float tsY = static_cast<float>(texSize.y);

    Vector2f uv0 = {0.f, 0.f};
    Vector2f uv1 = {tsX, 0.f};
    Vector2f uv2 = {0.f, tsY};
    Vector2f uv3 = {tsX, tsY};

    for (int y = 0; y < ROOM_HEIGHT; y++) {
        for (int x = 0; x < ROOM_WIDTH; x ++) {
            Vector2f pos = IgridToIso(x, y);

            Vector2f ptTop = pos;
            Vector2f ptRight = {pos.x + TILE_WIDTH / 2.f, pos.y + TILE_HEIGHT / 2.f};
            Vector2f ptBottom = {pos.x, pos.y + TILE_HEIGHT};
            Vector2f ptLeft = {pos.x - TILE_WIDTH / 2.f, pos.y + TILE_HEIGHT / 2.f};

            int idx = (x + y * ROOM_WIDTH) * 6;

            mMesh[idx+0] = {ptTop, Color::White, uv0};
            mMesh[idx+1] = {ptRight, Color::White, uv1};
            mMesh[idx+2] = {ptLeft, Color::White, uv2};

            mMesh[idx+3] = {ptRight, Color::White, uv1};
            mMesh[idx+4] = {ptBottom, Color::White, uv3};
            mMesh[idx+5] = {ptLeft, Color::White, uv2};
        }
    }
}

void Interior::draw(RenderTarget& target) {
    target.draw(mMesh, &mFloorTexture);
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
    Vector2f grid = IgridToIso(playerPos.x, playerPos.y);
    if (grid.x < 0 || grid.x >= ROOM_WIDTH || grid.y < 0 || grid.y >= ROOM_HEIGHT) {
        return true;
    }
    return false;
}
