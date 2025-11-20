#ifndef TREE_VISUALIZATION_MANAGER_H
#define TREE_VISUALIZATION_MANAGER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "../Core/TreeNode.h"
#include "../Parser/Solution.h"

// Forward declarations
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

class TreeVisualizationManager {
public:
    TreeVisualizationManager(sf::RenderWindow& window, const sf::Vector2f& windowSize);
    ~TreeVisualizationManager();

    void run(const ISolution& solution);
    bool handleEvent(const sf::Event& event);
    void update(sf::Time deltaTime);
    void draw();

private:
    void initialize(const ISolution& solution);
    void setupTreeRendering(const ISolution& solution);
    void setupInteraction();

    sf::RenderWindow& window_;
    const sf::Vector2f windowSize_;
    
    std::unique_ptr<TreeBuilder> treeBuilder_;
    std::unique_ptr<TreeRenderer> treeRenderer_;
    std::unique_ptr<CanvasView> canvasView_;
    std::unique_ptr<DisplayManager> displayManager_;
    std::unique_ptr<InteractionManager> interactionManager_;
    
    std::shared_ptr<Tree> tree_;
    std::unique_ptr<TreeLayout> treeLayout_;
    std::unique_ptr<BoardRenderer> boardRenderer_;
    
    bool initialized_;
    sf::Vector2f lastNodePosition_;     // 上一次的节点位置
    bool shouldCenterOnNodeChange_;     // 是否应该在节点变化时居中
};

#endif // TREE_VISUALIZATION_MANAGER_H
