#pragma once

#include "Core/PuzzleState.h"
#include <vector>
#include <string>

// solution每一行是: size state array

std::vector<PuzzleState> loadSolution(const std::string &filepath);
