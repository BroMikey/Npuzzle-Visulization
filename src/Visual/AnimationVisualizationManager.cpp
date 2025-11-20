#include "AnimationVisualizationManager.h"
#include "../Visual/AnimationRenderer.h"
#include "../Visual/CanvasView.h"
#include "../Visual/BoardRenderer.h"
#include "../Interaction/InteractionManager.h"
#include <cmath>
#include <iostream>

AnimationVisualizationManager::AnimationVisualizationManager(sf::RenderWindow& window, const sf::Vector2f& windowSize)
    : window_(window)
    , windowSize_(windowSize)
    , initialized_(false)
    , autoPlayEnabled_(false)
    , autoPlayInterval_(sf::milliseconds(500))
    , autoPlayAccumulator_(sf::Time::Zero) {
}

AnimationVisualizationManager::~AnimationVisualizationManager() {
}

void AnimationVisualizationManager::run(const ISolution& solution) {
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

bool AnimationVisualizationManager::handleEvent(const sf::Event& event) {
    if (!initialized_) return false;
    
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

void AnimationVisualizationManager::draw() {
    if (!initialized_) return;
    
    // 应用CanvasView到窗口
    canvasView_->applyToWindow(window_);
    
    // 绘制动画
    animationRenderer_->draw(window_);
}

void AnimationVisualizationManager::initialize(const ISolution& solution) {
    if (initialized_) return;
    
    setupAnimationRendering(solution);
    setupInteraction();
    
    initialized_ = true;
    std::cout << "Animation visualization manager initialized\n";
}

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

void AnimationVisualizationManager::toggleAutoPlay() {
    autoPlayEnabled_ = !autoPlayEnabled_;
    autoPlayAccumulator_ = sf::Time::Zero;
    std::cout << "Auto play: " << (autoPlayEnabled_ ? "ON" : "OFF") << std::endl;
}

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
