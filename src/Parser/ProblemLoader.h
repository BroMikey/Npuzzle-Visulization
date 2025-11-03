#include "Core/PuzzleState.h"

struct ProblemData
{
    std::vector<int> start;
    std::vector<int> goal;
};

ProblemData loadProblem(const std::string &filename);