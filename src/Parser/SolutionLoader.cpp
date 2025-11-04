#include "SolutionLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

Solution::Solution(const std::string &filepath)
{
    std::ifstream fin(filepath);
    if (!fin.is_open())
        throw std::runtime_error("无法打开文件: " + filepath);
    int size;
    while (fin >> size)
    {
        if (size != 3 && size != 5)
            throw std::runtime_error("解决方案文件格式错误: 状态大小必须为3或5");
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
        throw std::runtime_error("解决方案文件格式错误: 父节点数量与状态数量不匹配");
}

/**
 * @brief 获取从指定索引到根节点的路径
 *
 * @param index 目标索引
 * @return std::vector<int> 路径上的索引序列
 */
std::vector<int> Solution::getPathToRoot(int index) const
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

/**
 * @brief 从文件加载解决方案
 *
 * @param path 解决方案文件路径
 * @return Solution 加载的解决方案
 * @throws std::runtime_error 如果文件无法打开或格式错误
 */
Solution loadSolution(const std::string &path)
{
    return Solution(path);
}
