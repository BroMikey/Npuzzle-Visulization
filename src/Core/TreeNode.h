#pragma once

#include "PuzzleState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

/**
 * @brief 树节点结构，表示搜索树中的一个状态节点
 */
struct TreeNode
{
    int index;                        // 在Solution中的索引
    PuzzleState state;                // 状态数据
    int g;                            // 实际代价
    int h;                            // 启发式代价
    int f;                            // 总代价 (f = g + h)
    TreeNode *parent;                 // 父节点指针
    std::vector<TreeNode *> children; // 子节点列表
    sf::Vector2f position;            // 布局位置

    /**
     * @brief 构造函数
     * @param idx 节点索引
     * @param st 状态数据
     * @param g_val 实际代价
     * @param h_val 启发式代价
     * @param parent_ptr 父节点指针
     */
    TreeNode(int idx, const PuzzleState &st, int g_val, int h_val, TreeNode *parent_ptr = nullptr);

    /**
     * @brief 析构函数，递归删除所有子节点
     */
    ~TreeNode();

    /**
     * @brief 添加子节点
     * @param child 要添加的子节点
     */
    void addChild(TreeNode *child);

    /**
     * @brief 判断是否为根节点
     * @return 如果是根节点返回true，否则返回false
     */
    bool isRoot() const;

    /**
     * @brief 判断是否为叶子节点
     * @return 如果是叶子节点返回true，否则返回false
     */
    bool isLeaf() const;

    /**
     * @brief 获取节点深度
     * @return 节点深度（根节点深度为0）
     */
    int getDepth() const;

    /**
     * @brief 获取从根节点到当前节点的路径
     * @return 路径上的节点索引序列
     */
    std::vector<int> getPathToRoot() const;
};

/**
 * @brief 树结构类，管理整个搜索树
 */
class Tree
{
private:
    TreeNode *root;                   // 根节点
    std::vector<TreeNode *> allNodes; // 所有节点的列表（用于快速访问）

public:
    /**
     * @brief 构造函数
     */
    Tree();

    /**
     * @brief 析构函数
     */
    ~Tree();

    /**
     * @brief 设置根节点
     * @param node 根节点
     */
    void setRoot(TreeNode *node);

    /**
     * @brief 获取根节点
     * @return 根节点指针
     */
    TreeNode *getRoot() const;

    /**
     * @brief 根据索引获取节点
     * @param index 节点索引
     * @return 节点指针，如果不存在返回nullptr
     */
    TreeNode *getNodeByIndex(int index) const;

    /**
     * @brief 获取所有节点
     * @return 所有节点的列表
     */
    const std::vector<TreeNode *> &getAllNodes() const;

    /**
     * @brief 获取树的大小（节点数量）
     * @return 节点数量
     */
    size_t size() const;

    /**
     * @brief 获取树的最大深度
     * @return 最大深度
     */
    int getMaxDepth() const;

private:
    /**
     * @brief 递归收集所有节点
     * @param node 当前节点
     */
    void collectAllNodes(TreeNode *node);

    /**
     * @brief 递归计算最大深度
     * @param node 当前节点
     * @return 子树的最大深度
     */
    int calculateMaxDepth(TreeNode *node) const;
};