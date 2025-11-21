#include "AnimationVisualizationManager.h"
#include "../Visual/AnimationRenderer.h"
#include "../Visual/CanvasView.h"
#include "../Visual/BoardRenderer.h"
#include "../Interaction/InteractionManager.h"
#include <cmath>
#include <iostream>

/**
 * @brief 构造函数
 * @param window SFML渲染窗口引用
 * @param windowSize 窗口大小
 */
AnimationVisualizationManager::AnimationVisualizationManager(sf::RenderWindow& window, const sf::Vector2f& windowSize)
    : window_(window)
    , windowSize_(windowSize)
    , initialized_(false)
    , autoPlayEnabled_(false)
    , autoPlayInterval_(sf::milliseconds(500))
    , autoPlayAccumulator_(sf::Time::Zero) {
}

/**
 * @brief 析构函数
 */
AnimationVisualizationManager::~AnimationVisualizationManager() {
}

/**
 * @brief 运行动画可视化主循环
 * @param solution 解决方案数据
 */
void AnimationVisualizationManager::run(const ISolution& solution) {
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
bool AnimationVisualizationManager::handleEvent(const sf::Event& event) {
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
    
    // 处理交互命令
    interactionManager_->handleEvent(event);
    
    return false;
}

/**
 * @brief 更新状态
 * @param deltaTime 时间增量
 */
void AnimationVisualizationManager::update(sf::Time deltaTime) {
    if (!initialized_) return;
    
    // 更新CanvasView
    canvasView_->update();
    
    // 更新自动播放
    updateAutoPlay(deltaTime);
    
    // 居中显示当前节点
    auto currentNodePos = animationRenderer_->getCurrentNodePosition();
    canvasView_->setCenter(currentNodePos);
}

/**
 * @brief 绘制动画
 */
void AnimationVisualizationManager::draw() {
    if (!initialized_) return;
    
    // 应用CanvasView到窗口
    canvasView_->applyToWindow(window_);
    
    // 绘制动画
    animationRenderer_->draw(window_);
}

/**
 * @brief 初始化管理器
 * @param solution 解决方案数据
 */
void AnimationVisualizationManager::initialize(const ISolution& solution) {
    if (initialized_) return;
    
    setupAnimationRendering(solution);
    setupInteraction();
    
    initialized_ = true;
    std::cout << "Animation visualization manager initialized\n";
}

/**
 * @brief 设置动画渲染
 * @param solution 解决方案数据
 */
void AnimationVisualizationManager::setupAnimationRendering(const ISolution& solution) {
    std::cout << "Entering animation visualization mode\n";
    
    // 创建BoardRenderer
    auto boardRenderer = std::make_unique<BoardRenderer>();
    const auto& firstState = solution.getState(0);
    int boardSize = static_cast<int>(std::sqrt(firstState.size()));
    boardRenderer->setBoardSize(boardSize, 80.0f);
    
    // 创建动画渲染器
    animationRenderer_ = std::make_unique<AnimationRenderer>();
    animationRenderer_->setSolution(solution);
    animationRenderer_->setBoardRenderer(std::move(boardRenderer));
    
    // 创建CanvasView
    canvasView_ = std::make_unique<CanvasView>(windowSize_);
}

/**
 * @brief 设置交互功能
 */
void AnimationVisualizationManager::setupInteraction() {
    // 创建交互管理器
    interactionManager_ = std::make_unique<InteractionManager>();
    
    // 注册动画模式的交互命令
    interactionManager_->registerKeyCommand(sf::Keyboard::Right, 
        std::make_unique<FunctionCommand>([this]() {
            animationRenderer_->nextStep();
        }));

    interactionManager_->registerKeyCommand(sf::Keyboard::Left, 
        std::make_unique<FunctionCommand>([this]() {
            animationRenderer_->previousStep();
        }));

    interactionManager_->registerKeyCommand(sf::Keyboard::R, 
        std::make_unique<FunctionCommand>([this]() {
            animationRenderer_->reset();
        }));

    // 注册连接线显示/隐藏命令
    interactionManager_->registerKeyCommand(sf::Keyboard::L, 
        std::make_unique<FunctionCommand>([this]() {
            bool currentState = animationRenderer_->getShowConnectionLine();
            animationRenderer_->setShowConnectionLine(!currentState);
            std::cout << "Connection line display: " << (currentState ? "OFF" : "ON") << std::endl;
        }));

    // 注册自动播放命令（空格键切换）
    interactionManager_->registerKeyCommand(sf::Keyboard::Space, 
        std::make_unique<FunctionCommand>([this]() {
            toggleAutoPlay();
        }));
}

/**
 * @brief 切换自动播放状态
 */
void AnimationVisualizationManager::toggleAutoPlay() {
    autoPlayEnabled_ = !autoPlayEnabled_;
    autoPlayAccumulator_ = sf::Time::Zero;
    std::cout << "Auto play: " << (autoPlayEnabled_ ? "ON" : "OFF") << std::endl;
}

/**
 * @brief 更新自动播放
 * @param deltaTime 时间增量
 */
void AnimationVisualizationManager::updateAutoPlay(sf::Time deltaTime) {
    if (!autoPlayEnabled_) return;
    
    autoPlayAccumulator_ += deltaTime;
    
    if (autoPlayAccumulator_ >= autoPlayInterval_) {
        // 执行下一步
        animationRenderer_->nextStep();
        autoPlayAccumulator_ = sf::Time::Zero;
        
        // 如果到达最后一步，停止自动播放
        if (animationRenderer_->getCurrentStep() >= animationRenderer_->getTotalSteps() - 1) {
            autoPlayEnabled_ = false;
            std::cout << "Auto play completed, stopped" << std::endl;
        }
    }
}
