#pragma once

#include "../Core/TreeNode.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

/**
 * @brief 树布局器，负责计算树节点的位置
 */
class TreeLayout
{
public:
    /**
     * @brief 构造函数
     */
    TreeLayout();

    /**
     * @brief 设置树结构
     * @param tree 树结构
     */
    void setTree(Tree *tree);

    /**
     * @brief 设置布局参数
     * @param horizontalSpacing 水平间距
     * @param verticalSpacing 垂直间距
     * @param nodeWidth 节点宽度
     * @param nodeHeight 节点高度
     */
    void setLayoutParameters(float horizontalSpacing = 200.0f, float verticalSpacing = 150.0f,
                             float nodeWidth = 80.0f, float nodeHeight = 80.0f);

    /**
     * @brief 计算布局
     */
    void calculateLayout();

    /**
     * @brief 获取节点位置
     * @param nodeIndex 节点索引
     * @return 节点位置
     */
    sf::Vector2f getNodePosition(int nodeIndex) const;

    /**
     * @brief 获取所有节点位置
     * @return 节点位置列表
     */
    const std::vector<sf::Vector2f> &getAllNodePositions() const;

    /**
     * @brief 获取整体尺寸
     * @return 整体尺寸
     */
    sf::Vector2f getTotalSize() const;

    /**
     * @brief 设置位置偏移
     * @param x X偏移
     * @param y Y偏移
     */
    void setPosition(float x, float y);

private:
    Tree *m_tree;                              // 树结构
    std::vector<sf::Vector2f> m_nodePositions; // 节点位置列表

    float m_horizontalSpacing; // 水平间距
    float m_verticalSpacing;   // 垂直间距
    float m_nodeWidth;         // 节点宽度
    float m_nodeHeight;        // 节点高度
    sf::Vector2f m_position;   // 位置偏移

    // Reingold-Tilford 算法相关
    std::vector<float> m_mod;    // 节点修正值
    std::vector<float> m_prelim; // 节点初步X坐标
    std::vector<float> m_shift;  // 节点偏移量
    std::vector<float> m_change; // 节点变化量
    std::vector<int> m_thread;   // 线程指针
    std::vector<int> m_ancestor; // 祖先节点

    /**
     * @brief 执行Reingold-Tilford树布局算法
     */
    void executeReingoldTilford();

    /**
     * @brief 第一次遍历：计算初步位置
     * @param node 当前节点
     * @param level 当前层级
     */
    void firstWalk(TreeNode *node, int level);

    /**
     * @brief 第二次遍历：计算最终位置
     * @param node 当前节点
     * @param modSum 修正值总和
     */
    void secondWalk(TreeNode *node, float modSum);

    /**
     * @brief 获取节点的左兄弟
     * @param node 当前节点
     * @return 左兄弟节点，如果没有返回nullptr
     */
    TreeNode *getLeftSibling(TreeNode *node);

    /**
     * @brief 获取节点的最左子节点
     * @param node 当前节点
     * @return 最左子节点
     */
    TreeNode *getLeftmostChild(TreeNode *node);

    /**
     * @brief 获取节点的最右子节点
     * @param node 当前节点
     * @return 最右子节点
     */
    TreeNode *getRightmostChild(TreeNode *node);

    /**
     * @brief 获取节点的下一个左轮廓节点
     * @param node 当前节点
     * @return 下一个左轮廓节点
     */
    TreeNode *nextLeft(TreeNode *node);

    /**
     * @brief 获取节点的下一个右轮廓节点
     * @param node 当前节点
     * @return 下一个右轮廓节点
     */
    TreeNode *nextRight(TreeNode *node);

    /**
     * @brief 移动子树
     * @param node 当前节点
     * @param shift 移动距离
     */
    void moveSubtree(TreeNode *node, float shift);

    /**
     * @brief 执行移动
     * @param node 当前节点
     * @param shift 移动距离
     */
    void executeShifts(TreeNode *node);

    /**
     * @brief 计算子树分离
     * @param leftNode 左节点
     * @param rightNode 右节点
     * @return 需要的分离距离
     */
    float getSubtreeSeparation(TreeNode *leftNode, TreeNode *rightNode);

    /**
     * @brief 计算轮廓冲突
     * @param leftNode 左节点
     * @param rightNode 右节点
     * @param shift 当前偏移
     * @return 需要的额外偏移
     */
    float getContourConflict(TreeNode *leftNode, TreeNode *rightNode, float shift);

    /**
     * @brief 获取左轮廓
     * @param node 当前节点
     * @param maxMod 最大修正值
     * @param level 层级
     * @return 左轮廓位置
     */
    float getLeftContour(TreeNode *node, float &maxMod, int level);

    /**
     * @brief 获取右轮廓
     * @param node 当前节点
     * @param minMod 最小修正值
     * @param level 层级
     * @return 右轮廓位置
     */
    float getRightContour(TreeNode *node, float &minMod, int level);
};
