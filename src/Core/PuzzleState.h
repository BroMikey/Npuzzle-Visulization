#pragma once

#include <vector>
#include <string>

class PuzzleState
{
public:
    PuzzleState(const std::vector<int> &tiles);
    PuzzleState(int n) : tiles(n * n, 0) {}

private:
    std::vector<int> tiles;
};