#include "TreeBuilder.h"
#include <sstream>
#include <algorithm>

TreeBuilder::TreeBuilder()
    : m_totalNodes(0), m_maxDepth(0), m_leafNodes(0)
{
}

std::unique_ptr<Tree> TreeBuilder::buildTree(const ISolution &solution)
{
    // 重置统计信息
    m_totalNodes = 0;
    m_maxDepth = 0;
    m_leafNodes = 0;

    if (solution.size() == 0)
    {
        return std::make_unique<Tree>();
    }

    // 创建节点映射表
    std::vector<TreeNode *> nodeMap(solution.size(), nullptr);

    // 找到根节点（parent为-1的节点）
    int rootIndex = -1;
    for (size_t i = 0; i < solution.size(); ++i)
    {
        if (solution.getParent(i) == -1)
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

    // 递归构建树
    TreeNode *root = buildSubtree(rootIndex, solution, nullptr, nodeMap);

    // 创建树结构
    auto tree = std::make_unique<Tree>();
    tree->setRoot(root);

    // 收集统计信息
    collectStats(*tree);

    return tree;
}

TreeNode *TreeBuilder::buildSubtree(int nodeIndex, const ISolution &solution,
                                    TreeNode *parent, std::vector<TreeNode *> &nodeMap)
{
    // 如果节点已经创建，直接返回
    if (nodeMap[nodeIndex] != nullptr)
    {
        return nodeMap[nodeIndex];
    }

    // 创建新节点
    const auto &state = solution.getState(nodeIndex);
    int g = solution.getG(nodeIndex);
    int h = solution.getH(nodeIndex);

    TreeNode *node = new TreeNode(nodeIndex, state, g, h, parent);
    nodeMap[nodeIndex] = node;

    // 找到所有子节点
    std::vector<int> children;
    for (size_t i = 0; i < solution.size(); ++i)
    {
        if (solution.getParent(i) == nodeIndex)
        {
            children.push_back(i);
        }
    }

    // 递归构建子树
    for (int childIndex : children)
    {
        TreeNode *child = buildSubtree(childIndex, solution, node, nodeMap);
        node->addChild(child);
    }

    return node;
}

void TreeBuilder::collectStats(const Tree &tree)
{
    m_totalNodes = static_cast<int>(tree.size());
    m_maxDepth = tree.getMaxDepth();
    m_leafNodes = 0;

    if (tree.getRoot())
    {
        collectStatsRecursive(tree.getRoot(), 0);
    }
}

void TreeBuilder::collectStatsRecursive(TreeNode *node, int depth)
{
    if (node == nullptr)
    {
        return;
    }

    // 更新最大深度
    if (depth > m_maxDepth)
    {
        m_maxDepth = depth;
    }

    // 统计叶子节点
    if (node->isLeaf())
    {
        m_leafNodes++;
    }

    // 递归处理子节点
    for (TreeNode *child : node->children)
    {
        collectStatsRecursive(child, depth + 1);
    }
}

std::string TreeBuilder::getBuildStats() const
{
    std::ostringstream oss;
    oss << "Tree Structure Statistics:\n"
        << "  Total Nodes: " << m_totalNodes << "\n"
        << "  Max Depth: " << m_maxDepth << "\n"
        << "  Leaf Nodes: " << m_leafNodes << "\n"
        << "  Internal Nodes: " << (m_totalNodes - m_leafNodes) << "\n"
        << "  Average Branching Factor: " << (m_totalNodes > 1 ? static_cast<float>(m_totalNodes - 1) / (m_totalNodes - m_leafNodes) : 0.0f);

    return oss.str();
}
