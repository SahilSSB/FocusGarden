#pragma once

struct Tile {
    int x, y;
    bool hasTree = false;
    bool hasHouse = false;
    bool hasFence = false; 
    int fenceVariant = 0; 
    float growthState = 0.0f;
};