#include "Interior.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
using namespace sf;
using namespace std;

Interior::Interior() : mComputerSprite(mComputerTexture) {}

void Interior::defineFurniture() {

mFurnitureDef["sofaset"] = { 
        "sofaset", 
        { {-2,-2}, {-3,-2}, {-4,-2}, {-5,-2}, {0,-2}, {-1, -2} }, 
        {0.f, 0.f}, 
        true
    };

    mFurnitureDef["bed"] = { 
        "bed", 
        { {0,0}, {1,0}, {0,1}, {1,1}, {0,2}, {1,2} }, 
        {-5.f, 35.f}, 
        true
    };

    mFurnitureDef["diningtable"] = { 
        "diningtable", 
        { {-2,-2}, {-1,-2}, {-2,-1}, {-1,-1} }, 
        {0.f, -5.f}, 
        true
    };

    mFurnitureDef["fire"] =       { "fire",       { {0,0}, {0,1}, {0,2}, {0, -2}, {0, -1} }, {5.f, 17.f}, true};
    mFurnitureDef["bookshelf"] =  { "bookshelf",  { {0,0} },               {0.f, 10.f}, true};
    mFurnitureDef["sink"] =       { "sink",       { {0,0}, {-2,0}, {-1,0}, {-3,0} },               {-20.f, 10.f}, true};
    mFurnitureDef["dishwasher"] = { "dishwasher", { {0,0}, {-1,0}, {-2,0}, {-3,0} },               {-17.f, 3.5f}, true};
    
    mFurnitureDef["lamp"] =       { "lamp",       { {0,0} },               {2.f, 15.f}, true};

    mFurnitureDef["carpet"] =     { "carpet",     {}, {0.f, 0.f}, true};
    mFurnitureDef["shelf"] =      { "shelf",      {}, {0.f, -35.f}, true};
    mFurnitureDef["hanger"] =     { "hanger",     {}, {0.f, -25.f}, true};
    mFurnitureDef["photoframe"] = { "photoframe", {}, {0.f, -25.f}, false};
    mFurnitureDef["smallphoto"] = { "smallphoto", {}, {-20.f, -30.f}, false};
    mFurnitureDef["tablelamp"] =  { "tablelamp",  {}, {-5.f, 9.f}, true};
}    

void Interior::addObject(const string& name, int gridX, int gridY) {
    if (mInteriorTexture.count(name) == 0) return;

    Furniture def;
    if (mFurnitureDef.count(name)) def = mFurnitureDef[name];
    else def = { name, {}, {0.f, 0.f}, false};

    Sprite s(mInteriorTexture[name]);

    Vector2f isoPos = IgridToIso(gridX, gridY);
    isoPos.y += TILE_HEIGHT / 2.f;
    s.setPosition(isoPos);

    float scale = 0.375f;
    if (name == "lamp") scale += 0.375f;

    float scaleX = def.defaultFlip ? scale : -scale;
    s.setScale({scaleX, scale});

    FloatRect b = s.getLocalBounds();
    s.setOrigin({b.size.x / 2.f, b.size.y});
    Vector2f currentOrigin = s.getOrigin();
    currentOrigin.x -= (def.visualOffset.x / s.getScale().x);
    currentOrigin.y -= (def.visualOffset.y / s.getScale().y);
    s.setOrigin(currentOrigin);

    PlacedFurniture obj = {name, gridX, gridY, s, false};
    mPlacedObjects.push_back(obj);

}

void Interior::rebuildCollisions() {
    mBlockedTiles.clear();

    mBlockedTiles.push_back({0, 1});
    mBlockedTiles.push_back({1, 1});

    for (const auto& obj : mPlacedObjects) {
        if (obj.isBeingDragged) continue;

        if (mFurnitureDef.count(obj.name)) {
            const auto& def = mFurnitureDef[obj.name];
            
            for (const auto& offset : def.footprint) {
                mBlockedTiles.push_back({obj.gridX + offset.x, obj.gridY + offset.y});
            }
        }
    }
}

void Interior::updateEditor(RenderWindow& window, View& view) {
    if (mSelectedObject) {
        Vector2i pixelPos = Mouse::getPosition(window);
        Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);
        Vector2f gridPosF = IisoToGrid(worldPos.x, worldPos.y);

        int gx = static_cast<int>(round(gridPosF.x));
        int gy = static_cast<int>(round(gridPosF.y));

        mSelectedObject->gridX = gx;
        mSelectedObject->gridY = gy;
        Vector2f isoPos = IgridToIso(gx, gy);
        isoPos.y += TILE_HEIGHT / 2.f;

        if (mFurnitureDef.count(mSelectedObject->name)) {
            const auto& def = mFurnitureDef.at(mSelectedObject->name);
            Vector2f adjustedOrigin = mSelectedObject->sprite.getOrigin();
            adjustedOrigin.x = mSelectedObject->sprite.getLocalBounds().size.x / 2.f;
            adjustedOrigin.y = mSelectedObject->sprite.getLocalBounds().size.y;
            adjustedOrigin.x -= (def.visualOffset.x / mSelectedObject->sprite.getScale().x);
            adjustedOrigin.y -= (def.visualOffset.y / mSelectedObject->sprite.getScale().y);
            mSelectedObject->sprite.setOrigin(adjustedOrigin);
        }
        mSelectedObject->sprite.setPosition(isoPos);
    }
}

bool Interior::handleEditorInput(RenderWindow& window, View& view, const Event& event) {
    if (event.is<Event::MouseButtonPressed>()) {
        const auto* mousePress = event.getIf<Event::MouseButtonPressed>();
        if (mousePress->button == Mouse::Button::Left) {
            Vector2i pixelPos = mousePress->position;
            Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);

            if (mSelectedObject) {
                deselectAndRefresh();
                return true;
            }
            for (int i = mPlacedObjects.size() - 1; i >= 0; i--) {
                if (mPlacedObjects[i].sprite.getGlobalBounds().contains(worldPos)) {
                    mSelectedObject = &mPlacedObjects[i];
                    mSelectedObject->isBeingDragged = true;
                    mSelectedObject->sprite.setColor(Color(255, 255, 255, 150));
                    return true;
                }
            }
        }
    }
    return false;
}

void Interior::saveFurnitureLayout() {
    ofstream file("furniture_layout.txt");
    if (file.is_open()) {
        for (const auto& obj : mPlacedObjects) {
            file << obj.name << " " << obj.gridX << " " << obj.gridY << "\n";
        }
        cout << "Layout Saved!" << endl;
    }
}

void Interior::loadFurnitureLayout() {
    ifstream file("furniture_layout.txt");
    mPlacedObjects.clear(); 
    
    if (file.is_open()) {
        string name;
        int x, y;
        while (file >> name >> x >> y) {
            addObject(name, x, y);
        }
        cout << "Layout Loaded!" << endl;
    } else {
        addObject("bed", 6, 0);
        addObject("sofaset", 6, 8);
        addObject("diningtable", 13, 5);
        addObject("shelf", 11, 0);
        addObject("bookshelf", 0, 10);
        addObject("lamp", 5, 0);
        addObject("fire", 0, 8);
        addObject("sink", 13, 0);
        addObject("dishwasher", 11, 0);
        addObject("carpet", 6, 11);
        addObject("smallphoto", -1, 0);
        addObject("photoframe", 7, 0);
        addObject("tablelamp", 9, 0);
        addObject("plant", 5, 1);
    }
    rebuildCollisions();
}

void Interior::init() {
    if (!mFloorTexture.loadFromFile("textures/house/house_tile.png")) {
        Image img;
        img.resize({32, 32}, Color(139, 69, 19));
        if (!mFloorTexture.loadFromImage(img)) {
            cerr << "Failed to load fallback texture" << endl;
        }
    }
    defineFurniture();
    loadFurnitureLayout();
    generateWalls();
    loadTextures();

    if (!mComputerTexture.loadFromFile("textures/house/computer_table.png")) {
        throw runtime_error("Failed to load computer texture");
    }
    mComputerSprite.setTexture(mComputerTexture, true);
    FloatRect bounds = mComputerSprite.getLocalBounds();
    mComputerSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y});
    sf::Vector2f tilePos = IgridToIso(0, 1);
    mComputerSprite.setScale({0.375f, 0.375f});
    mComputerSprite.setPosition({tilePos.x + 0.f, tilePos.y + (TILE_HEIGHT / 2.f) + 25.f});

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

void Interior::loadTextures() {
    auto load = [&](string name, string path) {
        Image img;
        if (img.loadFromFile(path)) {
            Texture tex;
            [[maybe_unused]] bool loaded = tex.loadFromImage(img);
            mInteriorTexture[name] = tex;
        }
        else {
            cerr << "Failed to load: " << name << endl;
        }
    };
    load("shelf", "textures/house/shelf.png");
    load("photoframe", "textures/house/photoframe.png");
    load("bookshelf", "textures/house/bookshelf.png");
    load("carpet", "textures/house/carpet.png");
    load("lamp", "textures/house/lamp.png");
    load("sofaset", "textures/house/sofaset.png");
    load("plant", "textures/house/plant.png");
    load("fire", "textures/house/fire.png");
    load("diningtable", "textures/house/diningtable.png");
    load("bed", "textures/house/bed.png");
    load("sink", "textures/house/sink.png");
    load("tablelamp", "textures/house/tablelamp.png");
    load("dishwasher", "textures/house/dishwasher.png");
    load("smallphoto", "textures/house/smallphoto.png");
    load("hanger", "textures/house/decor_1.png");
}

void Interior::draw(RenderTarget& target, Vector2f playerWorldPos, function<void()> drawPlayerFunc, bool isEditing) {
    target.draw(mMesh, &mFloorTexture);
    target.draw(mWallMesh);

    for (const auto& prop : mPlacedObjects) {
        if (prop.name == "carpet") {
            target.draw(prop.sprite);
        }
    }

    vector<pair<float, function<void()>>> renderQueue;

    Vector2f playerGrid = IisoToGrid(playerWorldPos.x, playerWorldPos.y);
    int playerGridX = static_cast<int>(round(playerGrid.x));
    int playerGridY = static_cast<int>(round(playerGrid.y));

    for (const auto& prop : mPlacedObjects) {
        float sortingY = prop.sprite.getPosition().y;

        if (mFurnitureDef.count(prop.name)) {
            const auto& def = mFurnitureDef.at(prop.name);

            if (prop.name == "carpet") continue;

            if (!def.footprint.empty()) {
                float minDiff = 1000000.f;
                float bestY = sortingY;
                for (const auto& offset : def.footprint) {
                    int tileX = prop.gridX + offset.x;
                    int tileY = prop.gridY + offset.y;

                    int dx = abs(tileX - playerGridX);
                    int dy = abs(tileY - playerGridY);
                    float dist = dx + dy;
                    if (dist < minDiff) {
                        minDiff = dist;
                        Vector2f tilePos = IgridToIso(tileX, tileY);
                        bestY = tilePos.y + TILE_HEIGHT / 2.f;
                    }
                }
                sortingY = bestY;
            }
        }
        renderQueue.push_back({ 
            sortingY, 
            [&prop, &target]() { target.draw(prop.sprite); } 
        });
    }

    renderQueue.push_back({
        mComputerSprite.getPosition().y,
        [&]() { target.draw(mComputerSprite); }
    });
    renderQueue.push_back({
        playerWorldPos.y - 0.5f,
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

    sf::ConvexShape debugExit;
    debugExit.setPointCount(4);
    
    // FIX: Add static_cast<float>(...) around TILE_HEIGHT
    debugExit.setPoint(0, {0.f, 0.f});
    debugExit.setPoint(1, {TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});
    debugExit.setPoint(2, {0.f, static_cast<float>(TILE_HEIGHT)});   // <--- Fixed line
    debugExit.setPoint(3, {-TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});

    debugExit.setFillColor(sf::Color(0, 255, 0, 100)); 
    debugExit.setOutlineColor(sf::Color::Green);
    debugExit.setOutlineThickness(1.f);

    sf::Vector2f pos = IgridToIso(13, 14);
    debugExit.setPosition(pos);
    target.draw(debugExit);

    sf::Vector2f pos2 = IgridToIso(12,14);
    debugExit.setPosition(pos2);
    target.draw(debugExit);
    
    /*Vector2f computerPos = IgridToIso(5, 0); 

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

    sf::ConvexShape debugTile;
    debugTile.setPointCount(4);
    debugTile.setFillColor(sf::Color(255, 0, 0, 150));
    debugTile.setOutlineColor(sf::Color::Red);
    debugTile.setOutlineThickness(1.f);

    for (const auto& tile : mDebugBlockedTiles) {
        sf::Vector2f pos = IgridToIso(tile.x, tile.y);

        debugTile.setPoint(0, {0.f, 0.f});
        debugTile.setPoint(1, {TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});
        debugTile.setPoint(2, {0.f, static_cast<float>(TILE_HEIGHT)});
        debugTile.setPoint(3, {-TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});

        debugTile.setPosition(pos);
        target.draw(debugTile);
    }

    if (isEditing) {
        for (const auto& tile : mBlockedTiles) {
            sf::Vector2f pos = IgridToIso(tile.x, tile.y);
            debugTile.setPoint(0, {0.f, 0.f});
            debugTile.setPoint(1, {TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});
            debugTile.setPoint(2, {0.f, static_cast<float>(TILE_HEIGHT)});
            debugTile.setPoint(3, {-TILE_WIDTH / 2.f, TILE_HEIGHT / 2.f});
            debugTile.setPosition(pos);
            target.draw(debugTile);
        }
        if (mSelectedObject) {
        }
    }
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

    if ((grid.x == 12 || grid.x == 13) && grid.y == 14) {
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
    for (const auto& block : mBlockedTiles) {
        if (block.x == gridX && block.y == gridY) {
            return true; 
        }
    }
    return false;
}

bool Interior::isComputer(Vector2f playerPos) {
    Vector2f grid = IisoToGrid(playerPos.x, playerPos.y);

    int gridX = static_cast<int>(grid.x);
    int gridY = static_cast<int>(grid.y);

     if (gridX == 1 && gridY == 3 ||
        gridX == 1 && gridY == 2||
        gridX == 2 && gridY == 2) {
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
    Vector2f leftEnd = IgridToIso(0, 15);
    Vector2f rightEnd = IgridToIso(15, 0);

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
void Interior::deselectAndRefresh() {
    if (mSelectedObject) {
        mSelectedObject->isBeingDragged = false;
        mSelectedObject->sprite.setColor(Color::White);
        mSelectedObject = nullptr;
    }
    saveFurnitureLayout();
    rebuildCollisions();
}
