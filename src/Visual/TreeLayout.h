#pragma once

#include <SFML/Graphics.hpp>
#include "../Parser/SolutionLoader.h"
#include "BoardRenderer.h"
#include <vector>
#include <memory>

class TreeLayout
{
public:
    TreeLayout();

    // 设置解决方案数据
    void setSolution(const Solution &solution);

    // 设置布局参数
    void setLayoutParameters(float horizontalSpacing = 200.0f, float verticalSpacing = 150.0f);

    // 绘制整个树结构
    void draw(sf::RenderWindow &window);

    // 获取整体尺寸
    sf::Vector2f getTotalSize() const;

    // 设置位置
    void setPosition(float x, float y);

private:
    const Solution *m_solution;
    std::vector<std::unique_ptr<BoardRenderer>> m_boardRenderers;
    std::vector<sf::Vector2f> m_nodePositions;

    float m_horizontalSpacing;
    float m_verticalSpacing;
    sf::Vector2f m_position;

    // 计算节点位置
    void calculateNodePositions();

    // 绘制连接线
    void drawConnections(sf::RenderWindow &window);

    // 计算树的深度
    int calculateTreeDepth(int nodeIndex) const;

    // 计算子树宽度
    int calculateSubtreeWidth(int nodeIndex) const;

    // 递归计算节点位置
    void calculateNodePosition(int nodeIndex, float x, float y, int depth);
};
