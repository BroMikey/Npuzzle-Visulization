#pragma once

#include "../Core/PuzzleState.h"

// 文件格式 size start_state goal_state

struct ProblemData
{
    std::vector<int> start;
    std::vector<int> goal;
};

ProblemData loadProblem(const std::string &filename);