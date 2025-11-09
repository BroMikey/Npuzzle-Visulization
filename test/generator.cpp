#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>

class PuzzleGenerator {
private:
    int puzzleSize;  // 3 或 5
    std::mt19937 rng;
    
public:
    PuzzleGenerator(int size) : puzzleSize(size), rng(std::random_device{}()) {
        if (size != 3 && size != 5) {
            throw std::invalid_argument("拼图大小必须为3或5");
        }
    }
    
private:
    // 生成随机排列的拼图状态
    std::vector<int> generateRandomState() {
        std::vector<int> state(puzzleSize * puzzleSize);
        for (int i = 0; i < puzzleSize * puzzleSize; i++) {
            state[i] = i;
        }
        std::shuffle(state.begin(), state.end(), rng);
        return state;
    }
    
    // 检查拼图是否可解
    bool isSolvable(const std::vector<int>& state) {
        int inversions = 0;
        int blankRow = 0;
        
        for (int i = 0; i < state.size(); i++) {
            if (state[i] == 0) {
                blankRow = i / puzzleSize;
                continue;
            }
            for (int j = i + 1; j < state.size(); j++) {
                if (state[j] != 0 && state[i] > state[j]) {
                    inversions++;
                }
            }
        }
        
        if (puzzleSize % 2 == 1) {
            return inversions % 2 == 0;
        } else {
            return (inversions + blankRow) % 2 == 1;
        }
    }
    
    // 生成可解的随机状态
    std::vector<int> generateSolvableState() {
        std::vector<int> state;
        do {
            state = generateRandomState();
        } while (!isSolvable(state));
        return state;
    }
    
    // 模拟A*搜索生成解决方案
    void generateSolution(const std::vector<int>& start, const std::vector<int>& goal, 
                         const std::string& solutionFile) {
        std::ofstream fout(solutionFile);
        if (!fout) {
            throw std::runtime_error("无法打开解决方案文件: " + solutionFile);
        }
        
        // 模拟搜索过程，生成一些中间状态
        int stepCount = 10 + std::uniform_int_distribution<int>(5, 20)(rng);
        
        // 生成根节点
        fout << puzzleSize << " " << -1 << " " << 0 << " " 
             << calculateHeuristic(start, goal) << " " 
             << calculateHeuristic(start, goal) << std::endl;
        for (int i = 0; i < puzzleSize * puzzleSize; i++) {
            fout << start[i] << " ";
        }
        fout << std::endl;
        
        // 生成一些中间状态
        std::vector<int> currentState = start;
        for (int step = 1; step < stepCount; step++) {
            // 模拟移动
            std::vector<int> nextState = simulateMove(currentState);
            int parent = step - 1;
            int g = step;
            int h = calculateHeuristic(nextState, goal);
            int f = g + h;
            
            fout << puzzleSize << " " << parent << " " << g << " " << h << " " << f << std::endl;
            for (int i = 0; i < puzzleSize * puzzleSize; i++) {
                fout << nextState[i] << " ";
            }
            fout << std::endl;
            
            currentState = nextState;
        }
        
        // 生成目标状态
        fout << puzzleSize << " " << stepCount - 1 << " " << stepCount << " " 
             << 0 << " " << stepCount << std::endl;
        for (int i = 0; i < puzzleSize * puzzleSize; i++) {
            fout << goal[i] << " ";
        }
        fout << std::endl;
        
        fout.close();
    }
    
    // 计算启发式值（错位方块数）
    int calculateHeuristic(const std::vector<int>& state, const std::vector<int>& goal) {
        int count = 0;
        for (int i = 0; i < state.size(); i++) {
            if (state[i] != 0 && state[i] != goal[i]) {
                count++;
            }
        }
        return count;
    }
    
    // 模拟一次移动
    std::vector<int> simulateMove(const std::vector<int>& state) {
        std::vector<int> newState = state;
        
        // 找到空白位置
        int blankPos = -1;
        for (int i = 0; i < state.size(); i++) {
            if (state[i] == 0) {
                blankPos = i;
                break;
            }
        }
        
        // 可能的移动方向
        std::vector<std::pair<int, int>> directions = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}  // 上、下、左、右
        };
        
        std::vector<int> validMoves;
        for (const auto& dir : directions) {
            int newRow = blankPos / puzzleSize + dir.first;
            int newCol = blankPos % puzzleSize + dir.second;
            
            if (newRow >= 0 && newRow < puzzleSize && newCol >= 0 && newCol < puzzleSize) {
                int newPos = newRow * puzzleSize + newCol;
                validMoves.push_back(newPos);
            }
        }
        
        if (!validMoves.empty()) {
            // 随机选择一个移动
            std::uniform_int_distribution<int> moveDist(0, validMoves.size() - 1);
            int swapPos = validMoves[moveDist(rng)];
            
            // 交换空白和选中的方块
            std::swap(newState[blankPos], newState[swapPos]);
        }
        
        return newState;
    }
    
public:
    void generatePuzzle(const std::string& problemFile, const std::string& solutionFile) {
        std::cout << "生成 " << puzzleSize << "x" << puzzleSize << " 拼图..." << std::endl;
        
        // 生成目标状态（已排序状态）
        std::vector<int> goal(puzzleSize * puzzleSize);
        for (int i = 0; i < puzzleSize * puzzleSize; i++) {
            goal[i] = i;
        }
        
        // 生成可解的初始状态
        std::vector<int> start = generateSolvableState();
        
        // 保存问题文件
        std::ofstream problemOut(problemFile);
        if (!problemOut) {
            throw std::runtime_error("无法打开问题文件: " + problemFile);
        }
        
        problemOut << puzzleSize << std::endl;
        for (int i = 0; i < puzzleSize * puzzleSize; i++) {
            problemOut << start[i] << " ";
        }
        problemOut << std::endl;
        for (int i = 0; i < puzzleSize * puzzleSize; i++) {
            problemOut << goal[i] << " ";
        }
        problemOut << std::endl;
        
        problemOut.close();
        
        std::cout << "问题文件已生成: " << problemFile << std::endl;
        
        // 生成解决方案文件
        generateSolution(start, goal, solutionFile);
        
        std::cout << "解决方案文件已生成: " << solutionFile << std::endl;
        
        // 显示一些统计信息
        std::cout << "初始状态启发式值: " << calculateHeuristic(start, goal) << std::endl;
        std::cout << "目标状态: 0 1 2 ... " << (puzzleSize * puzzleSize - 1) << std::endl;
    }
};

int main() {
    try {
        // 生成3x3拼图
        std::cout << "=== 生成3x3拼图 ===" << std::endl;
        PuzzleGenerator generator3x3(3);
        generator3x3.generatePuzzle("../data/problem.txt", "../data/solution.txt");
        
        std::cout << std::endl;
        
        // 生成5x5拼图
        std::cout << "=== 生成5x5拼图 ===" << std::endl;
        PuzzleGenerator generator5x5(5);
        generator5x5.generatePuzzle("../data/problem5x5.txt", "../data/solution5x5.txt");
        
        std::cout << std::endl << "所有拼图生成完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
