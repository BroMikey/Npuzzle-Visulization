#include "TreeLayout.h"
#include <algorithm>
#include <cmath>

TreeLayout::TreeLayout()
    : m_solution(nullptr), m_horizontalSpacing(200.0f), m_verticalSpacing(150.0f), m_position(0, 0)
{
}

void TreeLayout::setSolution(const Solution &solution)
{
    m_solution = &solution;

    // 为每个状态创建BoardRenderer
    m_boardRenderers.clear();
    for (size_t i = 0; i < solution.size(); ++i)
    {
        auto renderer = std::make_unique<BoardRenderer>();

        // 获取棋盘大小
        const auto &state = solution.getState(i);
        int boardSize = static_cast<int>(std::sqrt(state.size()));
        renderer->setBoardSize(boardSize, 80.0f);

        // 设置数值
        renderer->setValues(solution.getG(i), solution.getH(i), solution.getF(i));

        m_boardRenderers.push_back(std::move(renderer));
    }

    // 计算节点位置
    calculateNodePositions();
}

void TreeLayout::setLayoutParameters(float horizontalSpacing, float verticalSpacing)
{
    m_horizontalSpacing = horizontalSpacing;
    m_verticalSpacing = verticalSpacing;
    if (m_solution)
    {
        calculateNodePositions();
    }
}

void TreeLayout::setPosition(float x, float y)
{
    m_position = sf::Vector2f(x, y);
}

void TreeLayout::draw(sf::RenderWindow &window)
{
    if (!m_solution || m_solution->size() == 0)
        return;

    // 绘制连接线
    drawConnections(window);

    // 绘制所有节点
    for (size_t i = 0; i < m_solution->size(); ++i)
    {
        if (m_boardRenderers[i])
        {
            // 设置BoardRenderer的位置
            m_boardRenderers[i]->setPosition(m_nodePositions[i].x, m_nodePositions[i].y);

            // 绘制棋盘
            m_boardRenderers[i]->draw(window, m_solution->getState(i));
        }
    }
}

sf::Vector2f TreeLayout::getTotalSize() const
{
    if (m_nodePositions.empty())
        return sf::Vector2f(0, 0);

    float maxX = 0, maxY = 0;
    for (const auto &pos : m_nodePositions)
    {
        if (pos.x > maxX)
            maxX = pos.x;
        if (pos.y > maxY)
            maxY = pos.y;
    }

    // 加上BoardRenderer的大小
    if (!m_boardRenderers.empty() && m_boardRenderers[0])
    {
        auto boardSize = m_boardRenderers[0]->getTotalSize();
        maxX += boardSize.x;
        maxY += boardSize.y;
    }

    return sf::Vector2f(maxX, maxY);
}

void TreeLayout::calculateNodePositions()
{
    if (!m_solution || m_solution->size() == 0)
        return;

    m_nodePositions.resize(m_solution->size());

    // 找到根节点（parent为-1的节点）
    int rootIndex = -1;
    for (size_t i = 0; i < m_solution->size(); ++i)
    {
        if (m_solution->getParent(i) == -1)
        {
            rootIndex = i;
            break;
        }
    }

    if (rootIndex == -1)
    {
        // 如果没有找到根节点，使用第一个节点作为根
        rootIndex = 0;
    }

    // 从根节点开始递归计算位置
    calculateNodePosition(rootIndex, 0, 0, 0);
}

void TreeLayout::calculateNodePosition(int nodeIndex, float x, float y, int depth)
{
    // 设置当前节点的位置
    m_nodePositions[nodeIndex] = sf::Vector2f(
        m_position.x + x,
        m_position.y + y * m_verticalSpacing);

    // 找到所有子节点
    std::vector<int> children;
    for (size_t i = 0; i < m_solution->size(); ++i)
    {
        if (m_solution->getParent(i) == nodeIndex)
        {
            children.push_back(i);
        }
    }

    if (children.empty())
        return;

    // 计算子树的宽度
    float totalWidth = 0;
    std::vector<float> subtreeWidths;
    for (int child : children)
    {
        float width = calculateSubtreeWidth(child);
        subtreeWidths.push_back(width);
        totalWidth += width;
    }

    // 计算子节点的位置
    float currentX = x - totalWidth / 2.0f;
    for (size_t i = 0; i < children.size(); ++i)
    {
        int childIndex = children[i];
        float childWidth = subtreeWidths[i];
        float childX = currentX + childWidth / 2.0f;

        calculateNodePosition(childIndex, childX, y + 1, depth + 1);
        currentX += childWidth;
    }
}

int TreeLayout::calculateTreeDepth(int nodeIndex) const
{
    int maxDepth = 0;

    // 找到所有子节点
    for (size_t i = 0; i < m_solution->size(); ++i)
    {
        if (m_solution->getParent(i) == nodeIndex)
        {
            int childDepth = calculateTreeDepth(i);
            if (childDepth > maxDepth)
                maxDepth = childDepth;
        }
    }

    return maxDepth + 1;
}

int TreeLayout::calculateSubtreeWidth(int nodeIndex) const
{
    // 找到所有子节点
    std::vector<int> children;
    for (size_t i = 0; i < m_solution->size(); ++i)
    {
        if (m_solution->getParent(i) == nodeIndex)
        {
            children.push_back(i);
        }
    }

    if (children.empty())
        return 1; // 叶子节点宽度为1

    // 子树宽度为所有子节点子树宽度之和
    int totalWidth = 0;
    for (int child : children)
    {
        totalWidth += calculateSubtreeWidth(child);
    }

    return std::max(1, totalWidth);
}

void TreeLayout::drawConnections(sf::RenderWindow &window)
{
    if (!m_solution)
        return;

    // 创建连接线
    sf::VertexArray lines(sf::Lines, 0);

    for (size_t i = 0; i < m_solution->size(); ++i)
    {
        int parentIndex = m_solution->getParent(i);
        if (parentIndex >= 0 && parentIndex < static_cast<int>(m_solution->size()))
        {
            // 获取父节点和当前节点的位置
            sf::Vector2f parentPos = m_nodePositions[parentIndex];
            sf::Vector2f currentPos = m_nodePositions[i];

            // 获取BoardRenderer的大小来调整连接点
            if (m_boardRenderers[parentIndex] && m_boardRenderers[i])
            {
                auto parentSize = m_boardRenderers[parentIndex]->getTotalSize();
                auto currentSize = m_boardRenderers[i]->getTotalSize();

                // 调整连接点位置，从父节点底部连接到子节点顶部
                parentPos.y += parentSize.y;
                currentPos.y -= 10; // 稍微向上偏移，避免重叠
            }

            // 添加连接线
            lines.append(sf::Vertex(parentPos, sf::Color::White));
            lines.append(sf::Vertex(currentPos, sf::Color::White));
        }
    }

    // 绘制所有连接线
    window.draw(lines);
}
