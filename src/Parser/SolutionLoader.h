#pragma once

#include "../Core/PuzzleState.h"
#include <vector>
#include <string>

// solution每一行是: size p g h f stateArray

class Solution
{
private:
    std::vector<PuzzleState> states;
    std::vector<int> parents;
    std::vector<int> g, h, f;

public:
    Solution(const std::string &filepath);
    ~Solution() {};

    const PuzzleState &getState(int index) const { return states[index]; }

    int getParent(int index) const { return parents[index]; }

    int getG(int index) const { return g[index]; }

    int getH(int index) const { return h[index]; }

    int getF(int index) const { return f[index]; }

    size_t size() const { return states.size(); }

    std::vector<int> getPathToRoot(int index) const;
};

Solution loadSolution(const std::string &path);
