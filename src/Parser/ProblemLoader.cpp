#include "ProblemLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

ProblemData loadProblem(const std::string &filepath)
{
    std::ifstream fin(filepath);
    if (!fin.is_open()) {
        throw std::runtime_error("Cannot open problem file: " + filepath);
    }
    
    int n;
    if (!(fin >> n)) {
        throw std::runtime_error("Invalid problem file format: cannot read board size");
    }
    
    if (n <= 0 || n > 10) {
        throw std::runtime_error("Invalid board size: " + std::to_string(n) + ". Must be between 1 and 10");
    }
    
    std::vector<int> start(n * n, 0);
    std::vector<int> goal(n * n, 0);
    
    // Read start state
    for (int i = 0; i < n * n; ++i) {
        if (!(fin >> start[i])) {
            throw std::runtime_error("Invalid problem file format: cannot read start state element at position " + std::to_string(i));
        }
    }
    
    // Read goal state
    for (int i = 0; i < n * n; ++i) {
        if (!(fin >> goal[i])) {
            throw std::runtime_error("Invalid problem file format: cannot read goal state element at position " + std::to_string(i));
        }
    }
    
    // Validate that both states contain all numbers from 0 to n*n-1
    std::vector<bool> startCheck(n * n, false);
    std::vector<bool> goalCheck(n * n, false);
    
    for (int i = 0; i < n * n; ++i) {
        if (start[i] < 0 || start[i] >= n * n) {
            throw std::runtime_error("Invalid start state: value " + std::to_string(start[i]) + " at position " + std::to_string(i) + " is out of range");
        }
        if (goal[i] < 0 || goal[i] >= n * n) {
            throw std::runtime_error("Invalid goal state: value " + std::to_string(goal[i]) + " at position " + std::to_string(i) + " is out of range");
        }
        startCheck[start[i]] = true;
        goalCheck[goal[i]] = true;
    }
    
    for (int i = 0; i < n * n; ++i) {
        if (!startCheck[i]) {
            throw std::runtime_error("Invalid start state: missing value " + std::to_string(i));
        }
        if (!goalCheck[i]) {
            throw std::runtime_error("Invalid goal state: missing value " + std::to_string(i));
        }
    }
    
    std::cout << "Successfully loaded problem from " << filepath << ", board size: " << n << "x" << n << std::endl;
    
    return ProblemData{start, goal};
}
