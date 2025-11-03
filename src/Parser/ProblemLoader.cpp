#include "ProblemLoader.h"
#include <fstream>
#include <sstream>

ProblemData loadProblem(const std::string &filepath)
{
    std::ifstream fin(filepath);
    int n;
    fin >> n;
    std::vector<int> start(n * n, 0);
    std::vector<int> goal(n * n, 0);
    for (int i = 0; i < n * n; ++i)
        fin >> start[i];
    for (int i = 0; i < n * n; ++i)
        fin >> goal[i];
    return ProblemData{start, goal};
}
