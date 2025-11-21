#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "../Parser/Solution.h"

// 前向声明
namespace sf {
    class RenderWindow;
    class Event;
}

class AnimationRenderer;
class CanvasView;
class InteractionManager;

/**
 * @brief 动画可视化管理器
 * 
 * 负责管理Puzzle移动动画的可视化过程，包括动画渲染、自动播放和交互控制。
 * 提供步进式动画展示、移动方向指示、连接线显示等功能。
 */
class AnimationVisualizationManager {
public:
    /**
     * @brief 构造函数
     * @param window SFML渲染窗口引用
     * @param windowSize 窗口大小
     */
    AnimationVisualizationManager(sf::RenderWindow& window, const sf::Vector2f& windowSize);
    
    /**
     * @brief 析构函数
     */
    ~AnimationVisualizationManager();

    /**
     * @brief 运行动画可视化
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
     * @brief 绘制动画
     */
    void draw();

private:
    /**
     * @brief 初始化管理器
     * @param solution 解决方案数据
     */
    void initialize(const ISolution& solution);

    /**
     * @brief 设置动画渲染
     * @param solution 解决方案数据
     */
    void setupAnimationRendering(const ISolution& solution);

    /**
     * @brief 设置交互功能
     */
    void setupInteraction();

    /**
     * @brief 切换自动播放状态
     */
    void toggleAutoPlay();

    /**
     * @brief 更新自动播放
     * @param deltaTime 时间增量
     */
    void updateAutoPlay(sf::Time deltaTime);

    sf::RenderWindow& window_;                      // SFML渲染窗口引用
    const sf::Vector2f windowSize_;                 // 窗口大小
    
    std::unique_ptr<AnimationRenderer> animationRenderer_;  // 动画渲染器
    std::unique_ptr<CanvasView> canvasView_;                // 画布视图
    std::unique_ptr<InteractionManager> interactionManager_; // 交互管理器
    
    bool initialized_;                              // 是否已初始化
    bool autoPlayEnabled_;                          // 自动播放是否启用
    sf::Time autoPlayInterval_;                     // 自动播放间隔
    sf::Time autoPlayAccumulator_;                  // 自动播放累计时间
};
