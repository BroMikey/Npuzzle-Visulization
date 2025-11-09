/**
 * @file TreeBuilder.h
 * @author your name (you@domain.com)
 * @brief 树构建器，负责从Solution构建树结构
 * @version 0.1
 * @date 2025-11-08
 * 根据solution构建一个TreeNode的树结构
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "../Core/TreeNode.h"
#include "../Parser/SolutionLoader.h"
#include <memory>

/**
 * @brief 树构建器，负责从Solution构建树结构
 */
class TreeBuilder
{
public:
    /**
     * @brief 构造函数
     */
    TreeBuilder();

    /**
     * @brief 从Solution构建树结构
     * @param solution 解决方案数据
     * @return 构建的树结构
     */
    std::unique_ptr<Tree> buildTree(const Solution &solution);

    /**
     * @brief 获取构建统计信息
     * @return 统计信息字符串
     */
    std::string getBuildStats() const;

private:
    int m_totalNodes; // 总节点数
    int m_maxDepth;   // 最大深度
    int m_leafNodes;  // 叶子节点数

    /**
     * @brief 递归构建子树
     * @param nodeIndex 当前节点索引
     * @param solution 解决方案数据
     * @param parent 父节点指针
     * @param nodeMap 节点映射表
     * @return 构建的节点
     */
    TreeNode *buildSubtree(int nodeIndex, const Solution &solution,
                           TreeNode *parent, std::vector<TreeNode *> &nodeMap);

    /**
     * @brief 收集统计信息
     * @param tree 树结构
     */
    void collectStats(const Tree &tree);

    /**
     * @brief 递归收集统计信息
     * @param node 当前节点
     * @param depth 当前深度
     */
    void collectStatsRecursive(TreeNode *node, int depth);
};
