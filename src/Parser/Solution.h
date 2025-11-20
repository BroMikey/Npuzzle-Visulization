#pragma once

#include "../Core/PuzzleState.h"
#include <vector>
#include <string>
#include <memory>

/**
 * @brief 解决方案接口基类
 * 
 * 定义了所有解决方案类必须实现的接口，支持树形可视化和动画可视化两种模式
 */
class ISolution {
public:
    virtual ~ISolution() = default;
    
    /**
     * @brief 获取指定索引的状态
     * @param index 状态索引
     * @return 对应的拼图状态
     */
    virtual const PuzzleState &getState(int index) const = 0;
    
    /**
     * @brief 获取指定索引的父节点索引
     * @param index 当前节点索引
     * @return 父节点索引，-1表示根节点
     */
    virtual int getParent(int index) const = 0;
    
    /**
     * @brief 获取从起始状态到当前状态的代价（g值）
     * @param index 状态索引
     * @return g值
     */
    virtual int getG(int index) const = 0;
    
    /**
     * @brief 获取当前状态到目标状态的启发式估计值（h值）
     * @param index 状态索引
     * @return h值
     */
    virtual int getH(int index) const = 0;
    
    /**
     * @brief 获取总评估值（f值 = g值 + h值）
     * @param index 状态索引
     * @return f值
     */
    virtual int getF(int index) const = 0;
    
    /**
     * @brief 获取解决方案中的状态总数
     * @return 状态数量
     */
    virtual size_t size() const = 0;
    
    /**
     * @brief 获取从指定节点到根节点的路径
     * @param index 起始节点索引
     * @return 路径索引列表，从根节点到指定节点
     */
    virtual std::vector<int> getPathToRoot(int index) const = 0;
};

/**
 * @brief 树形解决方案类
 * 
 * 用于树形可视化模式，包含完整的树结构信息，包括父子关系和启发式值
 */
class SolutionTree : public ISolution {
private:
    std::vector<PuzzleState> states;    ///< 所有状态集合
    std::vector<int> parents;           ///< 父节点索引集合
    std::vector<int> g, h, f;           ///< g值、h值、f值集合

public:
    /**
     * @brief 构造函数，从文件加载树形解决方案数据
     * @param filepath 解决方案文件路径
     */
    SolutionTree(const std::string &filepath);
    
    ~SolutionTree() {};

    const PuzzleState &getState(int index) const override { return states[index]; }
    int getParent(int index) const override { return parents[index]; }
    int getG(int index) const override { return g[index]; }
    int getH(int index) const override { return h[index]; }
    int getF(int index) const override { return f[index]; }
    size_t size() const override { return states.size(); }

    std::vector<int> getPathToRoot(int index) const override;
};

/**
 * @brief 动画解决方案类
 * 
 * 用于动画可视化模式，包含动作序列和生成的状态序列
 */
class SolutionAnimation : public ISolution {
private:
    std::vector<PuzzleState> states;    ///< 状态序列
    std::vector<std::string> actions;   ///< 动作序列

public:
    /**
     * @brief 构造函数，从文件加载动画解决方案数据
     * @param filepath 解决方案文件路径
     */
    SolutionAnimation(const std::string &filepath);
    
    ~SolutionAnimation() {};

    const PuzzleState &getState(int index) const override { return states[index]; }
    
    /**
     * @brief 获取父节点索引（动画模式中父节点总是前一个状态）
     * @param index 当前状态索引
     * @return 父节点索引（index-1），根节点返回-1
     */
    int getParent(int index) const override { return index > 0 ? index - 1 : -1; }
    
    /**
     * @brief 获取g值（动画模式中g值等于步数）
     * @param index 状态索引
     * @return g值（等于索引值）
     */
    int getG(int index) const override { return index; }
    
    /**
     * @brief 获取h值（动画模式中h值固定为0）
     * @param index 状态索引
     * @return h值（固定为0）
     */
    int getH(int index) const override { return 0; }
    
    /**
     * @brief 获取f值（动画模式中f值等于步数）
     * @param index 状态索引
     * @return f值（等于索引值）
     */
    int getF(int index) const override { return index; }
    
    size_t size() const override { return states.size(); }

    std::vector<int> getPathToRoot(int index) const override;
    
    /**
     * @brief 获取指定索引的动作
     * @param index 动作索引
     * @return 动作字符串（UP/DOWN/LEFT/RIGHT）
     */
    const std::string &getAction(int index) const { return actions[index]; }
};

/**
 * @brief 加载树形解决方案
 * @param path 解决方案文件路径
 * @return 指向ISolution的智能指针
 */
std::unique_ptr<ISolution> loadSolutionTree(const std::string &path);

/**
 * @brief 加载动画解决方案
 * @param path 解决方案文件路径
 * @return 指向ISolution的智能指针
 */
std::unique_ptr<ISolution> loadSolutionAnimation(const std::string &path);
