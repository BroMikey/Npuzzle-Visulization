#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "../Parser/Solution.h"

// Forward declarations
namespace sf {
    class RenderWindow;
    class Event;
}

class AnimationRenderer;
class CanvasView;
class InteractionManager;

class AnimationVisualizationManager {
public:
    AnimationVisualizationManager(sf::RenderWindow& window, const sf::Vector2f& windowSize);
    ~AnimationVisualizationManager();

    void run(const ISolution& solution);
    bool handleEvent(const sf::Event& event);
    void update(sf::Time deltaTime);
    void draw();

private:
    void initialize(const ISolution& solution);
    void setupAnimationRendering(const ISolution& solution);
    void setupInteraction();
    void toggleAutoPlay();
    void updateAutoPlay(sf::Time deltaTime);

    sf::RenderWindow& window_;
    const sf::Vector2f windowSize_;
    
    std::unique_ptr<AnimationRenderer> animationRenderer_;
    std::unique_ptr<CanvasView> canvasView_;
    std::unique_ptr<InteractionManager> interactionManager_;
    
    bool initialized_;
    bool autoPlayEnabled_;           // 自动播放是否启用
    sf::Time autoPlayInterval_;      // 自动播放间隔
    sf::Time autoPlayAccumulator_;   // 自动播放累计时间
};
