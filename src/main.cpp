#include <cassert>
#include <cmath>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "Core/PuzzleState.h"
#include "Parser/Solution.h"
#include "Parser/ProblemLoader.h"
#include "Visual/TreeVisualizationManager.h"
#include "Visual/AnimationVisualizationManager.h"
#include "Visual/ProblemEditor.h"

int main()
{
    try
    {
        // 初始化SFML窗口
        sf::RenderWindow window(sf::VideoMode(1600, 1000), "N-Puzzle Visualization");
        window.setFramerateLimit(60);

        // 直接进入问题编辑器模式
        ProblemEditor editor(sf::Vector2f(1600, 1000));
        editor.run(window);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
