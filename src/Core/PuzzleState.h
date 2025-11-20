#pragma once

#include <vector>
#include <string>

class PuzzleState
{
private:
    std::vector<int> tiles;

public:
    PuzzleState() : tiles(9, 0) {} // 默认构造函数，3x3棋盘
    PuzzleState(const std::vector<int> &tiles) : tiles(tiles) {}
    PuzzleState(int n) : tiles(n * n, 0) {}
    ~PuzzleState() {}

    int size() const { return tiles.size(); }

    int operator[](int index) const { return tiles[index]; }
    
    // 获取所有瓦片值
    const std::vector<int>& getValues() const { return tiles; }
    
    // 设置瓦片值
    void setValue(int index, int value) { tiles[index] = value; }
};
