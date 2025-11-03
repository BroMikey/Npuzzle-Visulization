#include "SolutionLoader.h"
#include <fstream>
#include <sstream>

std::vector<PuzzleState> loadSolution(const std::string &filepath)
{
    std::ifstream fin(filepath);
    std::vector<PuzzleState> solution;
    int size;
    while (fin >> size)
    {
        std::vector<int> state(size * size, 0);
        for (int i = 0; i < size * size; ++i)
            fin >> state[i];
        solution.push_back(PuzzleState(state));
    }
    return solution;
}