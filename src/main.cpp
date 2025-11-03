/**
 * @file Visulization.cpp
 * @author your name (you@domain.com)
 * @brief 可视化Npuzzle 启发式搜索过程
 * @version 0.1
 * @date 2025-11-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <SFML/Graphics.hpp>

// 从solution.txt读取数据, 绘制搜索过程

int main()
{
    // 1. 先读取数据
    // auto problem = loadProblem("data/problem.txt");
    // auto solution = loadSolution("data/solution.txt");

    // 2. 然后初始化sfml窗口和渲染循环
    // int n = problem.start.size();
    sf::RenderWindow window(sf::VideoMode(1600, 1200), "SFML window");
    window.setFramerateLimit(60);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::Black);
        // 读取problem.txt数据

        window.display();
    }

    return 0;
}