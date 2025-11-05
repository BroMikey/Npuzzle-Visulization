#pragma once

#include "../Core/TreeNode.h"
#include "TreeLayout.h"
#include "BoardRenderer.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

/**
 * @brief 树渲染器，负责渲染树结构和节点
 */
class TreeRenderer
{
public:
    /**
     * @brief 构造函数
     */
    TreeRenderer();

    /**
     * @brief 设置树结构和布局
     * @param tree 树结构
     * @param layout 布局信息
     */
    void setTreeAndLayout(Tree *tree, TreeLayout *layout);

    /**
     * @brief 设置BoardRenderer
     * @param renderer BoardRenderer
     */
    void setBoardRenderer(std::unique_ptr<BoardRenderer> renderer);

    /**
     * @brief 绘制整个树结构
     * @param window 渲染窗口
     */
    void draw(sf::RenderWindow &window);

    /**
     * @brief 设置连接线颜色
     * @param color 连接线颜色
     */
    void setConnectionColor(const sf::Color &color);

    /**
     * @brief 设置连接线宽度
     * @param width 连接线宽度
     */
    void setConnectionWidth(float width);

    /**
     * @brief 设置高亮路径
     * @param path 要高亮的路径索引列表
     */
    void setHighlightPath(const std::vector<int> &path);

    /**
     * @brief 清除高亮路径
     */
    void clearHighlightPath();

private:
    Tree *m_tree;                                   // 树结构
    TreeLayout *m_layout;                           // 布局信息
    std::unique_ptr<BoardRenderer> m_boardRenderer; // BoardRenderer

    sf::Color m_connectionColor;      // 连接线颜色
    float m_connectionWidth;          // 连接线宽度
    std::vector<int> m_highlightPath; // 高亮路径

    /**
     * @brief 绘制连接线
     * @param window 渲染窗口
     */
    void drawConnections(sf::RenderWindow &window);

    /**
     * @brief 绘制节点
     * @param window 渲染窗口
     */
    void drawNodes(sf::RenderWindow &window);

    /**
     * @brief 绘制高亮路径
     * @param window 渲染窗口
     */
    void drawHighlightPath(sf::RenderWindow &window);

    /**
     * @brief 获取节点渲染颜色
     * @param node 节点
     * @return 渲染颜色
     */
    sf::Color getNodeColor(TreeNode *node) const;

    /**
     * @brief 判断节点是否在高亮路径中
     * @param nodeIndex 节点索引
     * @return 如果节点在高亮路径中返回true，否则返回false
     */
    bool isNodeInHighlightPath(int nodeIndex) const;
};
