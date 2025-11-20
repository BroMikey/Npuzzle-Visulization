#include "TreeLayout.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>
#include <queue>

TreeLayout::TreeLayout()
    : m_tree(nullptr), m_horizontalSpacing(200.0f), m_verticalSpacing(150.0f),
      m_nodeWidth(80.0f), m_nodeHeight(80.0f), m_position(0, 0), m_nextX(0.0f), m_inorderStep(0.0f)
{
}

void TreeLayout::setTree(Tree *tree)
{
    m_tree = tree;
    if (m_tree)
    {
        int nodeCount = m_tree->size();
        m_nodePositions.resize(nodeCount);
        m_mod.resize(nodeCount, 0.0f);
        m_prelim.resize(nodeCount, 0.0f);
        m_shift.resize(nodeCount, 0.0f);
        m_change.resize(nodeCount, 0.0f);
        m_thread.resize(nodeCount, -1);
        m_ancestor.resize(nodeCount, -1);

        // 根据树的大小动态调整节点尺寸和间距
        calculateAdaptiveParameters();
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

    calculateAdaptiveParameters();

    int nodeCount = m_tree->size();
    if (static_cast<int>(m_nodePositions.size()) != nodeCount)
    {
        m_nodePositions.resize(nodeCount);
        m_mod.assign(nodeCount, 0.0f);
        m_prelim.assign(nodeCount, 0.0f);
        m_shift.assign(nodeCount, 0.0f);
        m_change.assign(nodeCount, 0.0f);
        m_thread.assign(nodeCount, -1);
        m_ancestor.assign(nodeCount, -1);
    }
    else
    {
        std::fill(m_nodePositions.begin(), m_nodePositions.end(), sf::Vector2f(0, 0));
        std::fill(m_mod.begin(), m_mod.end(), 0.0f);
        std::fill(m_prelim.begin(), m_prelim.end(), 0.0f);
        std::fill(m_shift.begin(), m_shift.end(), 0.0f);
        std::fill(m_change.begin(), m_change.end(), 0.0f);
        std::fill(m_thread.begin(), m_thread.end(), -1);
        std::fill(m_ancestor.begin(), m_ancestor.end(), -1);
    }

    // 使用稳定方案：inorder 给叶子编号 -> 内部节点取子树中点 -> 层级轮廓冲突修正
    executeStableLayout();
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
        return sf::Vector2f(0, 0);

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

//////////////////////////////////////////////////////////////////////////
// 新的稳定实现：inorder + 轮廓冲突校正（不会产生 thread 相关死循环）
//////////////////////////////////////////////////////////////////////////

// 主流程
void TreeLayout::executeStableLayout()
{
    TreeNode *root = m_tree->getRoot();
    if (!root) return;

    // 给叶子一个连续的 x（inorder）
    m_nextX = 0.0f;
    float baseStep = m_nodeWidth + m_horizontalSpacing;
    m_inorderStep = baseStep;
    assignPrelimInorder(root);

    // 对每个内部节点，prelim 是第一与最后子节点的中点（在 assignPrelimInorder 中已处理）
    // 接着，针对每个节点的子树做兄弟间冲突修正（从上到下/后序均可）
    resolveConflictsRec(root);

    // secondWalk：计算最终 world 坐标
    secondWalk(root, 0.0f);

    // 归一化，使最小 x >= m_position.x
    normalizePositions();
}

// 中序遍历：为叶子分配 x；为内部节点设置初步 prelim（平均子节点）
void TreeLayout::assignPrelimInorder(TreeNode *node)
{
    if (!node) return;

    if (node->children.empty())
    {
        // 叶子：直接赋值并推进 nextX
        m_prelim[node->index] = m_nextX;
        m_nextX += m_inorderStep;
    }
    else
    {
        // 对每个子节点递归
        for (TreeNode *child : node->children)
        {
            assignPrelimInorder(child);
        }

        // 将当前节点放到最左子与最右子中点
        TreeNode *leftmost = getLeftmostChild(node);
        TreeNode *rightmost = getRightmostChild(node);
        if (leftmost && rightmost)
        {
            m_prelim[node->index] = (m_prelim[leftmost->index] + m_prelim[rightmost->index]) / 2.0f;
        }
        else
        {
            // 防御性处理（不应发生）
            m_prelim[node->index] = 0.0f;
        }
    }
}

// 计算并返回子树每层最小 x（leftContour）与最大 x（rightContour）
void TreeLayout::collectContours(TreeNode *node, const std::vector<float> &prelim, std::vector<float> &minAtDepth, std::vector<float> &maxAtDepth, int depth)
{
    if (!node) return;
    float x = prelim[node->index];
    if (static_cast<int>(minAtDepth.size()) <= depth)
    {
        minAtDepth.resize(depth + 1, std::numeric_limits<float>::infinity());
        maxAtDepth.resize(depth + 1, -std::numeric_limits<float>::infinity());
    }
    minAtDepth[depth] = std::min(minAtDepth[depth], x);
    maxAtDepth[depth] = std::max(maxAtDepth[depth], x);

    for (TreeNode *c : node->children)
    {
        collectContours(c, prelim, minAtDepth, maxAtDepth, depth + 1);
    }
}

// 将整个子树的 prelim（相对坐标）整体右移 shift（递归）
void TreeLayout::shiftSubtree(TreeNode *node, float shift)
{
    if (!node) return;
    std::queue<TreeNode*> q;
    q.push(node);
    while (!q.empty())
    {
        TreeNode *cur = q.front(); q.pop();
        m_prelim[cur->index] += shift;
        for (TreeNode *c : cur->children)
            q.push(c);
    }
}

// 对一个节点的所有子节点（从左到右）检测并解决兄弟子树冲突
void TreeLayout::resolveConflictsRec(TreeNode *node)
{
    if (!node) return;

    // 先处理子节点
    for (TreeNode *child : node->children)
        resolveConflictsRec(child);

    // 对兄弟子树逐对检查右边子树是否与左边子树重叠，若重叠则右移右子树
    for (size_t i = 1; i < node->children.size(); ++i)
    {
        TreeNode *leftSub = node->children[i - 1];
        TreeNode *rightSub = node->children[i];

        // 计算左右两个子树的轮廓（按深度）
        std::vector<float> leftMax, leftMin;
        std::vector<float> rightMax, rightMin;

        collectContours(leftSub, m_prelim, leftMin, leftMax, 0);
        collectContours(rightSub, m_prelim, rightMin, rightMax, 0);

        // 比较每一层，计算需要右移的最大值
        float needed = 0.0f;
        int levels = std::min(static_cast<int>(leftMax.size()), static_cast<int>(rightMin.size()));
        for (int d = 0; d < levels; ++d)
        {
            float requiredSeparation = m_nodeWidth + m_horizontalSpacing;
            float overlap = (leftMax[d] + requiredSeparation) - rightMin[d];
            if (overlap > needed) needed = overlap;
        }

        if (needed > 0.0f)
        {
            // 右移整个 rightSub 树
            shiftSubtree(rightSub, needed);

            // IMPORTANT: 当右子树被右移后，可能会与下一个兄弟再次冲突，
            // 因此需要重新比较这个新的右子树与其新的左兄弟（通常循环会继续并处理后续兄弟）
            // 为了保证最近的左兄弟不是仅上一项，我们应将 i 回退一步以重新比较
            // 但简单起见：把 i 回退到 max(1, i-1) 确保父级的最近兄弟链得到再检查
            if (i > 1) --i;
        }
    }
}

// secondWalk：将 prelim（相对 x）与 m_position 组合，计算最终坐标
void TreeLayout::secondWalk(TreeNode *node, float modSum)
{
    if (!node) return;

    float x = m_prelim[node->index] + modSum;
    float y = node->getDepth() * (m_verticalSpacing + m_nodeHeight);

    m_nodePositions[node->index] = sf::Vector2f(m_position.x + x, m_position.y + y);

    for (TreeNode *child : node->children)
    {
        secondWalk(child, modSum); // modSum is zero in this simple approach (we stored absolute x in prelim)
    }
}

// 将最左边移动到 m_position.x（如果需要）
void TreeLayout::normalizePositions()
{
    if (m_nodePositions.empty()) return;

    float minX = std::numeric_limits<float>::max();
    for (const auto &p : m_nodePositions)
        minX = std::min(minX, p.x);

    if (minX < m_position.x)
    {
        float shift = m_position.x - minX;
        for (auto &p : m_nodePositions)
            p.x += shift;
    }
}

//////////////////////////////////////////////////////////////////////////
// 辅助函数
//////////////////////////////////////////////////////////////////////////

void TreeLayout::calculateAdaptiveParameters()
{
    if (!m_tree)
        return;

    int nodeCount = m_tree->size();
    int maxDepth = m_tree->getMaxDepth();

    // 增加默认垂直间距，确保层高足够
    float baseVerticalSpacing = 200.0f; // 增加基础垂直间距

    // 根据节点数量动态调整参数
    if (nodeCount > 500)
    {
        m_nodeWidth = (std::max)(20.0f, 80.0f * (500.0f / nodeCount));
        m_nodeHeight = (std::max)(20.0f, 80.0f * (500.0f / nodeCount));
        m_horizontalSpacing = (std::max)(50.0f, 200.0f * (500.0f / nodeCount));
        m_verticalSpacing = (std::max)(80.0f, baseVerticalSpacing * (500.0f / nodeCount));
    }
    else if (nodeCount > 100)
    {
        float scale = 100.0f / nodeCount;
        m_nodeWidth = 60.0f * scale;
        m_nodeHeight = 60.0f * scale;
        m_horizontalSpacing = 150.0f * scale;
        m_verticalSpacing = 150.0f * scale; // 增加中型树的垂直间距
    }
    else
    {
        // 小型树使用更大的默认间距
        m_nodeWidth = 80.0f;
        m_nodeHeight = 80.0f;
        m_horizontalSpacing = 200.0f;
        m_verticalSpacing = baseVerticalSpacing; // 使用基础垂直间距
    }

    // 根据树的深度进一步调整垂直间距，确保深树有足够层高
    if (maxDepth > 10)
    {
        // 深树需要更大的垂直间距
        float depthFactor = std::max(1.0f, static_cast<float>(maxDepth) / 10.0f);
        m_verticalSpacing = std::max(100.0f, m_verticalSpacing * depthFactor * 0.8f);
    }
    else if (maxDepth > 5)
    {
        // 中等深度树适度增加间距
        m_verticalSpacing = std::max(120.0f, m_verticalSpacing * 1.2f);
    }

    // 确保垂直间距至少是节点高度的1.5倍，避免重叠
    m_verticalSpacing = std::max(m_verticalSpacing, m_nodeHeight * 1.5f);
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
    if (!node) return nullptr;
    if (!node->children.empty()) return node->children.front();
    if (m_thread[node->index] != -1) 
    {
        TreeNode* tnode = m_tree->getNodeByIndex(m_thread[node->index]);
        if (tnode && tnode->getDepth() > node->getDepth())
            return tnode;
    }
    return nullptr;
}

TreeNode *TreeLayout::nextRight(TreeNode *node)
{
    if (!node) return nullptr;
    if (!node->children.empty()) return node->children.back();
    if (m_thread[node->index] != -1) 
    {
        TreeNode* tnode = m_tree->getNodeByIndex(m_thread[node->index]);
        if (tnode && tnode->getDepth() > node->getDepth())
            return tnode;
    }
    return nullptr;
}

TreeNode *TreeLayout::getAncestor(TreeNode *node, int ancestorIndex)
{
    if (!node) return nullptr;
    // 保守实现：向上查找直到找到 ancestorIndex 或到 root
    TreeNode *cur = node;
    while (cur)
    {
        if (cur->index == ancestorIndex) return cur;
        cur = cur->parent;
    }
    return nullptr;
}

float TreeLayout::getSubtreeSeparation(TreeNode *leftNode, TreeNode *rightNode)
{
    (void)leftNode;
    (void)rightNode;
    return m_horizontalSpacing + m_nodeWidth;
}

float TreeLayout::getContourConflict(TreeNode *leftNode, TreeNode *rightNode, float shift)
{
    // 保留但不在主流程使用（仅作为备份/工具）
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
