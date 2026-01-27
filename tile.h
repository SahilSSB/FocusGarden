#pragma once

struct Tile {
    int x, y;
    bool hasTree = false;
    bool hasHouse = false;
    bool hasFence = false;
    bool isWater = false;
    bool hasRock = false;
    int fenceVariant = 0;
    int rockVariant = 0; 
    bool isSolid = false;
    float growthState = 0.0f;

    int getTerrainID() {
        if (isWater) return 1;
        if (hasRock) return 2;
        return 0;
    }
};

struct Furniture {
    std::string name;
    std::vector<sf::Vector2i> footprint;
    sf::Vector2f visualOffset;
    bool defaultFlip;
};

struct PlacedFurniture {
    std::string name;
    int gridX;
    int gridY;
    sf::Sprite sprite;
    bool isBeingDragged = false;
};