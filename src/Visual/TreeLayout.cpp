#include "TreeLayout.h"
#include <algorithm>
#include <cmath>
#include <limits>

TreeLayout::TreeLayout()
    : m_tree(nullptr), m_horizontalSpacing(200.0f), m_verticalSpacing(150.0f),
      m_nodeWidth(80.0f), m_nodeHeight(80.0f), m_position(0, 0)
{
}

void TreeLayout::setTree(Tree *tree)
{
    m_tree = tree;
    if (m_tree)
    {
        // 初始化节点位置列表和算法数据结构
        int nodeCount = m_tree->size();
        m_nodePositions.resize(nodeCount);
        m_mod.resize(nodeCount, 0.0f);
        m_prelim.resize(nodeCount, 0.0f);
        m_shift.resize(nodeCount, 0.0f);
        m_change.resize(nodeCount, 0.0f);
        m_thread.resize(nodeCount, -1);
        m_ancestor.resize(nodeCount, -1);
    }
}

void TreeLayout::setLayoutParameters(float horizontalSpacing, float verticalSpacing,
                                     float nodeWidth, float nodeHeight)
{
    m_horizontalSpacing = horizontalSpacing;
    m_verticalSpacing = verticalSpacing;
    m_nodeWidth = nodeWidth;
    m_nodeHeight = nodeHeight;
}

void TreeLayout::calculateLayout()
{
    if (!m_tree || !m_tree->getRoot())
    {
        return;
    }

    // 重置节点位置
    std::fill(m_nodePositions.begin(), m_nodePositions.end(), sf::Vector2f(0, 0));
    std::fill(m_mod.begin(), m_mod.end(), 0.0f);
    std::fill(m_prelim.begin(), m_prelim.end(), 0.0f);
    std::fill(m_shift.begin(), m_shift.end(), 0.0f);
    std::fill(m_change.begin(), m_change.end(), 0.0f);
    std::fill(m_thread.begin(), m_thread.end(), -1);
    std::fill(m_ancestor.begin(), m_ancestor.end(), -1);

    // 执行Reingold-Tilford算法
    executeReingoldTilford();
}

sf::Vector2f TreeLayout::getNodePosition(int nodeIndex) const
{
    if (nodeIndex >= 0 && nodeIndex < static_cast<int>(m_nodePositions.size()))
    {
        return m_nodePositions[nodeIndex];
    }
    return sf::Vector2f(0, 0);
}

const std::vector<sf::Vector2f> &TreeLayout::getAllNodePositions() const
{
    return m_nodePositions;
}

sf::Vector2f TreeLayout::getTotalSize() const
{
    if (m_nodePositions.empty())
    {
        return sf::Vector2f(0, 0);
    }

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (const auto &pos : m_nodePositions)
    {
        if (pos.x < minX)
            minX = pos.x;
        if (pos.x > maxX)
            maxX = pos.x;
        if (pos.y > maxY)
            maxY = pos.y;
    }

    return sf::Vector2f(maxX - minX + m_nodeWidth, maxY + m_nodeHeight);
}

void TreeLayout::setPosition(float x, float y)
{
    m_position = sf::Vector2f(x, y);
}

void TreeLayout::executeReingoldTilford()
{
    if (!m_tree || !m_tree->getRoot())
        return;

    // 第一次遍历：计算初步位置
    firstWalk(m_tree->getRoot(), 0);

    // 第二次遍历：计算最终位置
    secondWalk(m_tree->getRoot(), 0);
}

void TreeLayout::firstWalk(TreeNode *node, int level)
{
    if (!node)
        return;

    TreeNode *leftSibling = getLeftSibling(node);

    if (node->children.empty())
    {
        // 叶子节点
        if (leftSibling)
        {
            m_prelim[node->index] = m_prelim[leftSibling->index] + m_horizontalSpacing + m_nodeWidth;
        }
        else
        {
            m_prelim[node->index] = 0;
        }
    }
    else
    {
        // 内部节点
        TreeNode *leftmostChild = getLeftmostChild(node);
        TreeNode *rightmostChild = getRightmostChild(node);

        // 递归处理子节点
        for (TreeNode *child : node->children)
        {
            firstWalk(child, level + 1);
        }

        float midPoint = (m_prelim[leftmostChild->index] + m_prelim[rightmostChild->index]) / 2.0f;

        if (leftSibling)
        {
            m_prelim[node->index] = m_prelim[leftSibling->index] + m_horizontalSpacing + m_nodeWidth;
            m_mod[node->index] = m_prelim[node->index] - midPoint;

            // 检查子树冲突
            TreeNode *leftContour = leftSibling;
            TreeNode *rightContour = node;
            int currentLevel = level + 1;

            while (leftContour && rightContour)
            {
                float leftPos = getRightContour(leftContour, m_mod[leftContour->index], currentLevel);
                float rightPos = getLeftContour(rightContour, m_mod[rightContour->index], currentLevel);

                if (rightPos - leftPos < m_horizontalSpacing + m_nodeWidth)
                {
                    float shift = (m_horizontalSpacing + m_nodeWidth) - (rightPos - leftPos);
                    m_prelim[node->index] += shift;
                    m_mod[node->index] += shift;
                }

                leftContour = nextRight(leftContour);
                rightContour = nextLeft(rightContour);
                currentLevel++;
            }
        }
        else
        {
            m_prelim[node->index] = midPoint;
        }
    }
}

void TreeLayout::secondWalk(TreeNode *node, float modSum)
{
    if (!node)
        return;

    // 计算最终位置
    float x = m_prelim[node->index] + modSum;
    float y = node->getDepth() * (m_verticalSpacing + m_nodeHeight);

    m_nodePositions[node->index] = sf::Vector2f(
        m_position.x + x,
        m_position.y + y);

    // 递归处理子节点
    for (TreeNode *child : node->children)
    {
        secondWalk(child, modSum + m_mod[node->index]);
    }
}

TreeNode *TreeLayout::getLeftSibling(TreeNode *node)
{
    if (!node || !node->parent)
        return nullptr;

    auto &children = node->parent->children;
    auto it = std::find(children.begin(), children.end(), node);
    if (it != children.begin())
    {
        return *(it - 1);
    }
    return nullptr;
}

TreeNode *TreeLayout::getLeftmostChild(TreeNode *node)
{
    if (!node || node->children.empty())
        return nullptr;
    return node->children.front();
}

TreeNode *TreeLayout::getRightmostChild(TreeNode *node)
{
    if (!node || node->children.empty())
        return nullptr;
    return node->children.back();
}

TreeNode *TreeLayout::nextLeft(TreeNode *node)
{
    if (!node)
        return nullptr;

    if (!node->children.empty())
    {
        return node->children.front();
    }
    else if (m_thread[node->index] != -1)
    {
        return m_tree->getNodeByIndex(m_thread[node->index]);
    }
    return nullptr;
}

TreeNode *TreeLayout::nextRight(TreeNode *node)
{
    if (!node)
        return nullptr;

    if (!node->children.empty())
    {
        return node->children.back();
    }
    else if (m_thread[node->index] != -1)
    {
        return m_tree->getNodeByIndex(m_thread[node->index]);
    }
    return nullptr;
}

void TreeLayout::moveSubtree(TreeNode *node, float shift)
{
    if (!node)
        return;

    m_prelim[node->index] += shift;
    m_mod[node->index] += shift;
}

void TreeLayout::executeShifts(TreeNode *node)
{
    if (!node)
        return;

    float shift = 0;
    float change = 0;

    for (auto it = node->children.rbegin(); it != node->children.rend(); ++it)
    {
        TreeNode *child = *it;
        m_prelim[child->index] += shift;
        m_mod[child->index] += shift;
        change += m_change[child->index];
        shift += m_shift[child->index] + change;
    }
}

float TreeLayout::getSubtreeSeparation(TreeNode *leftNode, TreeNode *rightNode)
{
    return m_horizontalSpacing + m_nodeWidth;
}

float TreeLayout::getContourConflict(TreeNode *leftNode, TreeNode *rightNode, float shift)
{
    float distance = 0;
    TreeNode *leftContour = leftNode;
    TreeNode *rightContour = rightNode;
    int level = 0;

    while (leftContour && rightContour)
    {
        float leftPos = getRightContour(leftContour, m_mod[leftContour->index], level);
        float rightPos = getLeftContour(rightContour, m_mod[rightContour->index], level);

        distance = std::max(distance, leftPos - rightPos + shift);

        leftContour = nextRight(leftContour);
        rightContour = nextLeft(rightContour);
        level++;
    }

    return distance;
}

float TreeLayout::getLeftContour(TreeNode *node, float &minMod, int level)
{
    if (!node)
        return std::numeric_limits<float>::lowest();

    float currentMod = m_mod[node->index];
    minMod = std::min(minMod, currentMod);

    float position = m_prelim[node->index] + currentMod;

    TreeNode *next = nextLeft(node);
    if (next)
    {
        return std::min(position, getLeftContour(next, minMod, level + 1));
    }

    return position;
}

float TreeLayout::getRightContour(TreeNode *node, float &maxMod, int level)
{
    if (!node)
        return std::numeric_limits<float>::max();

    float currentMod = m_mod[node->index];
    maxMod = std::max(maxMod, currentMod);

    float position = m_prelim[node->index] + currentMod;

    TreeNode *next = nextRight(node);
    if (next)
    {
        return std::max(position, getRightContour(next, maxMod, level + 1));
    }

    return position;
}
