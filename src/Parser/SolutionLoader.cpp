#include "Solution.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

// Helper function to apply an action to a puzzle state
std::vector<int> applyAction(const std::vector<int>& state, const std::string& action, int size)
{
    std::vector<int> newState = state;
    
    // Find the blank tile (0)
    int blankPos = -1;
    for (int i = 0; i < state.size(); ++i)
    {
        if (state[i] == 0)
        {
            blankPos = i;
            break;
        }
    }
    
    if (blankPos == -1)
        throw std::runtime_error("Invalid state: cannot find blank tile (0)");
    
    int blankRow = blankPos / size;
    int blankCol = blankPos % size;
    int swapPos = -1;
    
    // Determine the position to swap with based on the action
    if (action == "UP" && blankRow > 0)
    {
        swapPos = (blankRow - 1) * size + blankCol;
    }
    else if (action == "DOWN" && blankRow < size - 1)
    {
        swapPos = (blankRow + 1) * size + blankCol;
    }
    else if (action == "LEFT" && blankCol > 0)
    {
        swapPos = blankRow * size + (blankCol - 1);
    }
    else if (action == "RIGHT" && blankCol < size - 1)
    {
        swapPos = blankRow * size + (blankCol + 1);
    }
    else
    {
        throw std::runtime_error("Invalid action: " + action + " at position (" + 
                                std::to_string(blankRow) + ", " + 
                                std::to_string(blankCol) + ")");
    }
    
    // Swap the blank tile with the target tile
    std::swap(newState[blankPos], newState[swapPos]);
    
    return newState;
}

// SolutionTree implementation for structured tree data
SolutionTree::SolutionTree(const std::string &filepath)
{
    std::ifstream fin(filepath);
    if (!fin.is_open())
        throw std::runtime_error("Cannot open file: " + filepath);
    int size;
    while (fin >> size)
    {
        if (size < 3 || size > 10)
            throw std::runtime_error("Solution file format error: state size must be between 3 and 10");
        int p_, g_, h_, f_;
        fin >> p_ >> g_ >> h_ >> f_;
        parents.push_back(p_);
        g.push_back(g_);
        h.push_back(h_);
        f.push_back(f_);
        std::vector<int> state(size * size, 0);
        for (int i = 0; i < size * size; ++i)
            fin >> state[i];
        states.push_back(PuzzleState(state));
    }
    if (parents.size() != states.size())
        throw std::runtime_error("Solution file format error: number of parents does not match number of states");
}

std::vector<int> SolutionTree::getPathToRoot(int index) const
{
    std::vector<int> path;
    while (index != -1)
    {
        path.push_back(index);
        index = getParent(index);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// SolutionAnimation implementation for action-based animation data
SolutionAnimation::SolutionAnimation(const std::string &filepath)
{
    std::ifstream fin(filepath);
    if (!fin.is_open())
        throw std::runtime_error("Cannot open file: " + filepath);
    
    // 读取动作序列
    std::string action;
    while (fin >> action)
    {
        actions.push_back(action);
    }
    
    if (actions.empty())
        throw std::runtime_error("Animation solution file is empty");
    
    // 从 problem.txt 读取初始状态
    std::string problemPath = "problem.txt";
    std::ifstream problemFile(problemPath);
    if (!problemFile.is_open())
    {
        throw std::runtime_error("Cannot open problem file: problem.txt");
    }
    
    int size;
    problemFile >> size;
    
    // 读取初始状态（只读取第一个状态）
    std::vector<int> initialState(size * size);
    for (int i = 0; i < size * size; ++i)
    {
        problemFile >> initialState[i];
    }
    
    // 生成状态序列
    std::vector<int> currentState = initialState;
    states.push_back(PuzzleState(currentState)); // 初始状态
    
    // 根据动作序列生成后续状态
    for (size_t i = 0; i < actions.size(); ++i)
    {
        currentState = applyAction(currentState, actions[i], size);
        states.push_back(PuzzleState(currentState));
    }
}

std::vector<int> SolutionAnimation::getPathToRoot(int index) const
{
    std::vector<int> path;
    while (index != -1)
    {
        path.push_back(index);
        index = getParent(index);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// Factory functions
std::unique_ptr<ISolution> loadSolutionTree(const std::string &path)
{
    return std::make_unique<SolutionTree>(path);
}

std::unique_ptr<ISolution> loadSolutionAnimation(const std::string &path)
{
    return std::make_unique<SolutionAnimation>(path);
}
