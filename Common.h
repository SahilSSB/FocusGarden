enum TileType {GRASS, SOIL, HOUSE_FLOOR, WALL};
enum PlantState {NONE, GROWING, FULL_GROWN, BURNT};

struct Tile {
    int gridX, gridY;
    TileType type;
    PlantState plant;
    bool isWalkable;
}
