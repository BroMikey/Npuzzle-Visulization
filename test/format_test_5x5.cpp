#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "src/Parser/ProblemLoader.h"
#include "src/Parser/SolutionLoader.h"

int main() {
    try {
        std::cout << "=== 测试5x5问题文件格式 ===" << std::endl;
        
        // 测试5x5问题文件加载
        ProblemData problem = loadProblem("../data/problem5x5.txt");
        std::cout << "5x5问题文件加载成功！" << std::endl;
        std::cout << "初始状态大小: " << problem.start.size() << std::endl;
        std::cout << "目标状态大小: " << problem.goal.size() << std::endl;
        
        std::cout << std::endl << "=== 测试5x5解决方案文件格式 ===" << std::endl;
        
        // 测试5x5解决方案文件加载
        Solution solution = loadSolution("../data/solution5x5.txt");
        std::cout << "5x5解决方案文件加载成功！" << std::endl;
        std::cout << "状态数量: " << solution.size() << std::endl;
        
        // 显示一些状态信息
        if (solution.size() > 0) {
            std::cout << "第一个状态大小: " << solution.getState(0).size() << std::endl;
            std::cout << "最后一个状态大小: " << solution.getState(solution.size() - 1).size() << std::endl;
            
            // 测试路径获取
            std::vector<int> path = solution.getPathToRoot(solution.size() - 1);
            std::cout << "从最后一个状态到根的路径长度: " << path.size() << std::endl;
            
            // 显示一些状态值
            std::cout << "第一个状态的父节点: " << solution.getParent(0) << std::endl;
            std::cout << "第一个状态的g值: " << solution.getG(0) << std::endl;
            std::cout << "第一个状态的h值: " << solution.getH(0) << std::endl;
            std::cout << "第一个状态的f值: " << solution.getF(0) << std::endl;
        }
        
        std::cout << std::endl << "=== 5x5所有测试通过！文件格式正确 ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
