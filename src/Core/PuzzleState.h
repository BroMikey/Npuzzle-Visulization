#pragma once

#include <vector>
#include <string>

class PuzzleState
{
private:
    std::vector<int> tiles;

public:
    PuzzleState(const std::vector<int> &tiles) : tiles(tiles) {}
    PuzzleState(int n) : tiles(n * n, 0) {}
    ~PuzzleState() {}

    int size() const { return tiles.size(); }

    int operator[](int index) const { return tiles[index]; }
};