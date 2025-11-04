#include <cassert>
#include <cmath>

#include <SFML/Graphics.hpp>
#include "Core/PuzzleState.h"
#include "Parser/SolutionLoader.h"
#include "Parser/ProblemLoader.h"
#include "Visual/TreeLayout.h"
// 从solution.txt读取数据, 绘制搜索过程

int main()
{
    // 1. 先读取数据
    // auto problem = loadProblem("data/problem.txt");
    // auto solution = loadSolution("data/solution.txt");
    auto problem = loadProblem("../data/problem.txt");
    auto solution = loadSolution("../data/solution.txt");

    // 2. 初始化TreeLayout
    TreeLayout treeLayout;
    treeLayout.setSolution(solution);
    // FIXME: 树节点重合，需要调整布局参数，以及缩放
    treeLayout.setLayoutParameters(250.0f, 200.0f); // 设置水平和垂直间距
    treeLayout.setPosition(100, 50);                // 设置起始位置

    // 3. 初始化SFML窗口
    sf::RenderWindow window(sf::VideoMode(1600, 1000), "搜索树可视化");
    window.setFramerateLimit(60);

    // 4. 渲染循环
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        // 绘制整个树结构
        treeLayout.draw(window);

        window.display();
    }

    return 0;
}
