#include "TreeVisualizationManager.h"
#include "../Core/TreeNode.h"
#include "../Visual/TreeBuilder.h"
#include "../Visual/TreeLayout.h"
#include "../Visual/TreeRenderer.h"
#include "../Visual/CanvasView.h"
#include "../Visual/DisplayManager.h"
#include "../Visual/BoardRenderer.h"
#include "../Interaction/InteractionManager.h"
#include "../Interaction/CommandRegistrar.h"
#include <cmath>
#include <iostream>

/**
 * @brief 构造函数
 * @param window SFML渲染窗口引用
 * @param windowSize 窗口大小
 */
TreeVisualizationManager::TreeVisualizationManager(sf::RenderWindow& window, const sf::Vector2f& windowSize)
    : window_(window)
    , windowSize_(windowSize)
    , initialized_(false)
    , lastNodePosition_(0, 0)
    , shouldCenterOnNodeChange_(true) {
}

/**
 * @brief 析构函数
 */
TreeVisualizationManager::~TreeVisualizationManager() {
}

/**
 * @brief 运行树可视化主循环
 * @param solution 解决方案数据
 */
void TreeVisualizationManager::run(const ISolution& solution) {
    initialize(solution);
    
    sf::Clock clock;
    bool shouldReturnToEditor = false;
    
    // 主循环
    while (window_.isOpen() && !shouldReturnToEditor) {
        sf::Time deltaTime = clock.restart();
        
        // 处理事件
        sf::Event event;
        while (window_.pollEvent(event)) {
            if (handleEvent(event)) {
                shouldReturnToEditor = true;
                break;
            }
        }
        
        if (shouldReturnToEditor) {
            break;
        }
        
        // 更新和绘制
        update(deltaTime);
        
        window_.clear(sf::Color::Black);
        draw();
        window_.display();
    }
    
    // 不关闭窗口，让ProblemEditor继续使用
    std::cout << "Returning to Problem Editor..." << std::endl;
    
    // 重置窗口视图，确保ProblemEditor能正确显示
    window_.setView(window_.getDefaultView());
}

/**
 * @brief 处理事件
 * @param event SFML事件
 * @return 如果需要返回编辑器返回true，否则返回false
 */
bool TreeVisualizationManager::handleEvent(const sf::Event& event) {
    if (!initialized_) return false;
    
    // 处理窗口关闭事件
    if (event.type == sf::Event::Closed) {
        window_.close();
        return false;
    }
    
    // 处理B键返回ProblemEditor
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
        std::cout << "Returning to Problem Editor..." << std::endl;
        return true; // 返回true表示需要返回ProblemEditor
    }
    
    // 处理CanvasView事件（缩放和平移）
    canvasView_->handleEvent(event);
    
    // 如果正在拖拽，则不要自动居中
    if (canvasView_->isDragging()) {
        shouldCenterOnNodeChange_ = false;
    }
    
    // 处理交互命令
    interactionManager_->handleEvent(event);
    
    return false;
}

/**
 * @brief 更新状态
 * @param deltaTime 时间增量
 */
void TreeVisualizationManager::update(sf::Time deltaTime) {
    if (!initialized_) return;
    
    // 更新显示管理器（用于自动播放）
    displayManager_->updateAutoPlay(deltaTime);
    
    // 更新CanvasView
    canvasView_->update();
    
    // 获取当前节点位置
    auto currentNodePos = treeRenderer_->getCurrentNodePosition();
    
    // 只在节点位置发生变化时才居中，并且没有在拖拽时才居中
    if (!canvasView_->isDragging()) {
        if (currentNodePos != lastNodePosition_) {
            // 节点位置发生变化，居中显示
            canvasView_->setCenter(currentNodePos);
            lastNodePosition_ = currentNodePos;
            shouldCenterOnNodeChange_ = true;
        }
    }
}

/**
 * @brief 绘制树结构
 */
void TreeVisualizationManager::draw() {
    if (!initialized_) return;
    
    // 应用CanvasView到窗口
    canvasView_->applyToWindow(window_);
    
    // 绘制整个树结构
    treeRenderer_->draw(window_);
    
    // 绘制拖拽指示器（在树结构之上）
    canvasView_->drawDragIndicator(window_);
}

/**
 * @brief 初始化管理器
 * @param solution 解决方案数据
 */
void TreeVisualizationManager::initialize(const ISolution& solution) {
    if (initialized_) return;
    
    setupTreeRendering(solution);
    setupInteraction();
    
    initialized_ = true;
    std::cout << "Tree visualization manager initialized\n";
}

/**
 * @brief 设置树渲染
 * @param solution 解决方案数据
 */
void TreeVisualizationManager::setupTreeRendering(const ISolution& solution) {
    std::cout << "Entering tree visualization mode\n";
    
    // 构建树结构
    treeBuilder_ = std::make_unique<TreeBuilder>();
    tree_ = std::move(treeBuilder_->buildTree(solution));
    
    std::cout << treeBuilder_->getBuildStats() << std::endl;
    
    // 创建BoardRenderer
    boardRenderer_ = std::make_unique<BoardRenderer>();
    const auto& firstState = solution.getState(0);
    int boardSize = static_cast<int>(std::sqrt(firstState.size()));
    boardRenderer_->setBoardSize(boardSize, 80.0f);
    // 在TreeView中显示panel
    boardRenderer_->setShowValuePanel(true);
    
    // 计算布局
    treeLayout_ = std::make_unique<TreeLayout>();
    treeLayout_->setTree(tree_.get());
    
    // 根据BoardRenderer的实际大小设置布局参数
    auto boardTotalSize = boardRenderer_->getTotalSize();
    treeLayout_->setLayoutParameters(250.0f, 200.0f, boardTotalSize.x, boardTotalSize.y);
    treeLayout_->setPosition(100, 50);
    treeLayout_->calculateLayout();
    
    std::cout << "Layout calculation completed, overall size: "
              << treeLayout_->getTotalSize().x << " x "
              << treeLayout_->getTotalSize().y << std::endl;
    
    // 创建渲染器
    treeRenderer_ = std::make_unique<TreeRenderer>();
    treeRenderer_->setTreeAndLayout(tree_.get(), treeLayout_.get());
    treeRenderer_->setBoardRenderer(std::move(boardRenderer_));
    treeRenderer_->setConnectionColor(sf::Color(200, 200, 200));
    
    // 创建CanvasView
    canvasView_ = std::make_unique<CanvasView>(windowSize_);
    
    // 设置初始视图中心为树的中心
    auto treeSize = treeLayout_->getTotalSize();
    canvasView_->setCenter(sf::Vector2f(treeSize.x / 2, treeSize.y / 2));
    
    // 创建显示管理器
    displayManager_ = std::make_unique<DisplayManager>();
    displayManager_->setTotalNodes(tree_->size());
    displayManager_->setPlayInterval(sf::milliseconds(100)); // 100ms for 10 nodes per second
    
    // 将显示管理器设置到树渲染器
    treeRenderer_->setDisplayManager(displayManager_.get());
}

/**
 * @brief 设置交互功能
 */
void TreeVisualizationManager::setupInteraction() {
    // 创建交互管理器
    interactionManager_ = std::make_unique<InteractionManager>();
    
    // 注册交互命令
    CommandRegistrar::registerDefaultCommands(
        *interactionManager_,
        *canvasView_,
        *treeRenderer_,
        tree_,
        displayManager_.get());
}
