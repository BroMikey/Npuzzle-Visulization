#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "../Core/TreeNode.h"
#include "../Parser/Solution.h"

// 前向声明
namespace sf {
    class RenderWindow;
    class Event;
}

class TreeBuilder;
class TreeLayout;
class TreeRenderer;
class CanvasView;
class DisplayManager;
class InteractionManager;
class BoardRenderer;

/**
 * @brief 树可视化管理器
 * 
 * 负责管理树结构的可视化过程，包括树的构建、布局、渲染和交互控制。
 * 提供步进式显示、自动播放、视图控制等功能。
 */
class TreeVisualizationManager {
public:
    /**
     * @brief 构造函数
     * @param window SFML渲染窗口引用
     * @param windowSize 窗口大小
     */
    TreeVisualizationManager(sf::RenderWindow& window, const sf::Vector2f& windowSize);
    
    /**
     * @brief 析构函数
     */
    ~TreeVisualizationManager();

    /**
     * @brief 运行树可视化
     * @param solution 解决方案数据
     */
    void run(const ISolution& solution);

    /**
     * @brief 处理事件
     * @param event SFML事件
     * @return 如果需要返回编辑器返回true，否则返回false
     */
    bool handleEvent(const sf::Event& event);

    /**
     * @brief 更新状态
     * @param deltaTime 时间增量
     */
    void update(sf::Time deltaTime);

    /**
     * @brief 绘制树结构
     */
    void draw();

private:
    /**
     * @brief 初始化管理器
     * @param solution 解决方案数据
     */
    void initialize(const ISolution& solution);

    /**
     * @brief 设置树渲染
     * @param solution 解决方案数据
     */
    void setupTreeRendering(const ISolution& solution);

    /**
     * @brief 设置交互功能
     */
    void setupInteraction();

    sf::RenderWindow& window_;              // SFML渲染窗口引用
    const sf::Vector2f windowSize_;         // 窗口大小
    
    std::unique_ptr<TreeBuilder> treeBuilder_;          // 树构建器
    std::unique_ptr<TreeRenderer> treeRenderer_;        // 树渲染器
    std::unique_ptr<CanvasView> canvasView_;            // 画布视图
    std::unique_ptr<DisplayManager> displayManager_;    // 显示管理器
    std::unique_ptr<InteractionManager> interactionManager_; // 交互管理器
    
    std::shared_ptr<Tree> tree_;                        // 树结构
    std::unique_ptr<TreeLayout> treeLayout_;            // 树布局
    std::unique_ptr<BoardRenderer> boardRenderer_;      // 棋盘渲染器
    
    bool initialized_;                                  // 是否已初始化
    sf::Vector2f lastNodePosition_;                     // 上一次的节点位置
    bool shouldCenterOnNodeChange_;                     // 是否应该在节点变化时居中
};
