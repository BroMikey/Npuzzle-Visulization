#include "TreeRenderer.h"
#include <algorithm>

TreeRenderer::TreeRenderer()
    : m_tree(nullptr), m_layout(nullptr), m_displayManager(nullptr), m_connectionColor(sf::Color::White), m_connectionWidth(2.0f)
{
}

void TreeRenderer::setTreeAndLayout(Tree *tree, TreeLayout *layout)
{
    m_tree = tree;
    m_layout = layout;
}

void TreeRenderer::setBoardRenderer(std::unique_ptr<BoardRenderer> renderer)
{
    m_boardRenderer = std::move(renderer);
}

void TreeRenderer::draw(sf::RenderWindow &window)
{
    if (!m_tree || !m_layout || !m_boardRenderer)
    {
        return;
    }

    // 绘制连接线
    drawConnections(window);

    // 绘制高亮路径
    drawHighlightPath(window);

    // 绘制节点
    drawNodes(window);
}

void TreeRenderer::setConnectionColor(const sf::Color &color)
{
    m_connectionColor = color;
}

void TreeRenderer::setConnectionWidth(float width)
{
    m_connectionWidth = width;
}

void TreeRenderer::setHighlightPath(const std::vector<int> &path)
{
    m_highlightPath = path;
}

void TreeRenderer::clearHighlightPath()
{
    m_highlightPath.clear();
}

void TreeRenderer::setDisplayManager(DisplayManager *displayManager)
{
    m_displayManager = displayManager;
}

DisplayManager *TreeRenderer::getDisplayManager() const
{
    return m_displayManager;
}

void TreeRenderer::drawConnections(sf::RenderWindow &window)
{
    if (!m_tree || !m_layout || !m_boardRenderer)
    {
        return;
    }

    // 创建连接线
    sf::VertexArray lines(sf::Lines, 0);

    const auto &allNodes = m_tree->getAllNodes();
    for (TreeNode *node : allNodes)
    {
        if (!node || node->isRoot())
        {
            continue;
        }

        // 检查父节点和当前节点是否都可见（如果有显示管理器）
        if (m_displayManager)
        {
            if (!m_displayManager->isNodeVisible(node->parent->index) ||
                !m_displayManager->isNodeVisible(node->index))
            {
                continue;
            }
        }

        // 获取父节点和当前节点的位置
        sf::Vector2f parentPos = m_layout->getNodePosition(node->parent->index);
        sf::Vector2f currentPos = m_layout->getNodePosition(node->index);

        // 获取BoardRenderer的实际大小
        sf::Vector2f boardSize = m_boardRenderer->getTotalSize();

        // 调整连接点位置，从父节点底部中点连接到子节点顶部中点
        parentPos.x += boardSize.x / 2; // 父节点底部中点
        parentPos.y += boardSize.y;     // 从父节点底部

        currentPos.x += boardSize.x / 2; // 子节点顶部中点
        currentPos.y -= 5;               // 从子节点顶部稍微向上偏移，避免重叠

        // 添加连接线
        lines.append(sf::Vertex(parentPos, m_connectionColor));
        lines.append(sf::Vertex(currentPos, m_connectionColor));
    }

    // 绘制所有连接线
    window.draw(lines);
}

void TreeRenderer::drawNodes(sf::RenderWindow &window)
{
    if (!m_tree || !m_layout || !m_boardRenderer)
    {
        return;
    }

    const auto &allNodes = m_tree->getAllNodes();
    for (TreeNode *node : allNodes)
    {
        if (!node)
        {
            continue;
        }

        // 检查节点是否可见（如果有显示管理器）
        if (m_displayManager && !m_displayManager->isNodeVisible(node->index))
        {
            continue;
        }

        // 获取节点位置
        sf::Vector2f position = m_layout->getNodePosition(node->index);

        // 设置BoardRenderer的位置
        m_boardRenderer->setPosition(position.x, position.y);

        // 绘制棋盘
        m_boardRenderer->draw(window, node->state);

        // 设置数值显示
        m_boardRenderer->setValues(node->g, node->h, node->f);
    }
}

void TreeRenderer::drawHighlightPath(sf::RenderWindow &window)
{
    if (m_highlightPath.empty() || !m_tree || !m_layout || !m_boardRenderer)
    {
        return;
    }

    // 创建高亮路径的连接线
    sf::VertexArray highlightLines(sf::Lines, 0);

    // 获取BoardRenderer的实际大小
    sf::Vector2f boardSize = m_boardRenderer->getTotalSize();

    for (size_t i = 0; i < m_highlightPath.size() - 1; ++i)
    {
        int currentIndex = m_highlightPath[i];
        int nextIndex = m_highlightPath[i + 1];

        TreeNode *currentNode = m_tree->getNodeByIndex(currentIndex);
        TreeNode *nextNode = m_tree->getNodeByIndex(nextIndex);

        if (!currentNode || !nextNode)
        {
            continue;
        }

        // 检查当前节点和下一个节点是否都可见（如果有显示管理器）
        if (m_displayManager)
        {
            if (!m_displayManager->isNodeVisible(currentIndex) ||
                !m_displayManager->isNodeVisible(nextIndex))
            {
                continue;
            }
        }

        // 获取节点位置
        sf::Vector2f currentPos = m_layout->getNodePosition(currentIndex);
        sf::Vector2f nextPos = m_layout->getNodePosition(nextIndex);

        // 调整连接点位置，从当前节点底部中点连接到下一个节点顶部中点
        currentPos.x += boardSize.x / 2; // 当前节点底部中点
        currentPos.y += boardSize.y;     // 从当前节点底部

        nextPos.x += boardSize.x / 2; // 下一个节点顶部中点
        nextPos.y -= 5;               // 从下一个节点顶部稍微向上偏移，避免重叠

        // 添加高亮连接线（使用黄色）
        highlightLines.append(sf::Vertex(currentPos, sf::Color::Yellow));
        highlightLines.append(sf::Vertex(nextPos, sf::Color::Yellow));
    }

    // 绘制高亮路径
    window.draw(highlightLines);
}

sf::Color TreeRenderer::getNodeColor(TreeNode *node) const
{
    if (!node)
    {
        return sf::Color::White;
    }

    // 根据节点是否在高亮路径中返回不同颜色
    if (isNodeInHighlightPath(node->index))
    {
        return sf::Color::Yellow; // 高亮路径中的节点使用黄色
    }

    // 根据节点深度返回不同颜色（可选功能）
    int depth = node->getDepth();
    if (depth == 0)
    {
        return sf::Color::Green; // 根节点使用绿色
    }
    else if (node->isLeaf())
    {
        return sf::Color::Red; // 叶子节点使用红色
    }
    else
    {
        return sf::Color::White; // 其他节点使用白色
    }
}

bool TreeRenderer::isNodeInHighlightPath(int nodeIndex) const
{
    return std::find(m_highlightPath.begin(), m_highlightPath.end(), nodeIndex) != m_highlightPath.end();
}

sf::Vector2f TreeRenderer::getCurrentNodePosition() const
{
    if (!m_tree || !m_layout || !m_displayManager)
    {
        return sf::Vector2f(0, 0);
    }

    // 获取当前显示的最后一个节点
    const auto& visibleNodes = m_displayManager->getVisibleNodes();
    if (visibleNodes.empty())
    {
        return sf::Vector2f(0, 0);
    }

    // 找到最大的索引（通常是最后显示的节点）
    int maxIndex = -1;
    for (int index : visibleNodes)
    {
        if (index > maxIndex)
        {
            maxIndex = index;
        }
    }

    if (maxIndex == -1)
    {
        return sf::Vector2f(0, 0);
    }

    // 获取该节点的位置
    sf::Vector2f position = m_layout->getNodePosition(maxIndex);
    
    // 调整位置到节点中心
    if (m_boardRenderer)
    {
        sf::Vector2f boardSize = m_boardRenderer->getTotalSize();
        position.x += boardSize.x / 2;
        position.y += boardSize.y / 2;
    }

    return position;
}
