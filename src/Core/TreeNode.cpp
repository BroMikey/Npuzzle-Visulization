// TreeNode.cpp
// 树节点和树结构的实现文件

#include "TreeNode.h"
#include <algorithm>

// TreeNode结构的实现
TreeNode::TreeNode(int idx, const PuzzleState &st, int g_val, int h_val, TreeNode *parent_ptr)
    : index(idx), state(st), g(g_val), h(h_val), f(g_val + h_val),
      parent(parent_ptr), position(0, 0) {}

TreeNode::~TreeNode()
{
    for (TreeNode *child : children)
    {
        delete child;
    }
}

void TreeNode::addChild(TreeNode *child)
{
    children.push_back(child);
    child->parent = this;
}

bool TreeNode::isRoot() const
{
    return parent == nullptr;
}

bool TreeNode::isLeaf() const
{
    return children.empty();
}

int TreeNode::getDepth() const
{
    int depth = 0;
    const TreeNode *current = this;
    while (current->parent != nullptr)
    {
        depth++;
        current = current->parent;
    }
    return depth;
}

std::vector<int> TreeNode::getPathToRoot() const
{
    std::vector<int> path;
    const TreeNode *current = this;
    while (current != nullptr)
    {
        path.push_back(current->index);
        current = current->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// Tree类的实现
Tree::Tree() : root(nullptr) {}

Tree::~Tree()
{
    if (root)
    {
        delete root;
    }
}

void Tree::setRoot(TreeNode *node)
{
    root = node;
    if (node)
    {
        collectAllNodes(node);
    }
}

TreeNode *Tree::getRoot() const
{
    return root;
}

TreeNode *Tree::getNodeByIndex(int index) const
{
    if (index >= 0 && index < static_cast<int>(allNodes.size()))
    {
        return allNodes[index];
    }
    return nullptr;
}

const std::vector<TreeNode *> &Tree::getAllNodes() const
{
    return allNodes;
}

size_t Tree::size() const
{
    return allNodes.size();
}

int Tree::getMaxDepth() const
{
    if (!root)
        return 0;
    return calculateMaxDepth(root);
}

void Tree::collectAllNodes(TreeNode *node)
{
    if (!node)
        return;

    // 确保allNodes有足够的空间
    if (node->index >= static_cast<int>(allNodes.size()))
    {
        allNodes.resize(node->index + 1, nullptr);
    }

    allNodes[node->index] = node;

    for (TreeNode *child : node->children)
    {
        collectAllNodes(child);
    }
}

int Tree::calculateMaxDepth(TreeNode *node) const
{
    if (node->children.empty())
    {
        return 1;
    }

    int maxDepth = 0;
    for (TreeNode *child : node->children)
    {
        int childDepth = calculateMaxDepth(child);
        if (childDepth > maxDepth)
        {
            maxDepth = childDepth;
        }
    }

    return maxDepth + 1;
}