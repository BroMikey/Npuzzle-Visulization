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

TreeVisualizationManager::TreeVisualizationManager(sf::RenderWindow& window, const sf::Vector2f& windowSize)
    : window_(window)
    , windowSize_(windowSize)
    , initialized_(false)
    , lastNodePosition_(0, 0)
    , shouldCenterOnNodeChange_(true) {
}

TreeVisualizationManager::~TreeVisualizationManager() {
}

void TreeVisualizationManager::run(const ISolution& solution) {
    initialize(solution);
    
    sf::Clock clock;
    bool shouldReturnToEditor = false;
    
    while (window_.isOpen() && !shouldReturnToEditor) {
        sf::Time deltaTime = clock.restart();
        
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

bool TreeVisualizationManager::handleEvent(const sf::Event& event) {
    if (!initialized_) return false;
    
    if (event.type == sf::Event::Closed) {
        window_.close();
        return false;
    }
    
    // 处理B键返回ProblemEditor
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
        std::cout << "Returning to Problem Editor..." << std::endl;
        return true; // Return true to indicate need to return to ProblemEditor
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

void TreeVisualizationManager::draw() {
    if (!initialized_) return;
    
    // 应用CanvasView到窗口
    canvasView_->applyToWindow(window_);
    
    // 绘制整个树结构
    treeRenderer_->draw(window_);
    
    // 绘制拖拽指示器（在树结构之上）
    canvasView_->drawDragIndicator(window_);
}

void TreeVisualizationManager::initialize(const ISolution& solution) {
    if (initialized_) return;
    
    setupTreeRendering(solution);
    setupInteraction();
    
    initialized_ = true;
    std::cout << "Tree visualization manager initialized\n";
}

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
