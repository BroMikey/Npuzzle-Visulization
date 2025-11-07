#include <cassert>
#include <cmath>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "Core/PuzzleState.h"
#include "Parser/SolutionLoader.h"
#include "Parser/ProblemLoader.h"
#include "Visual/TreeBuilder.h"
#include "Visual/TreeLayout.h"
#include "Visual/TreeRenderer.h"
#include "Visual/CanvasView.h"
#include "Interaction/InteractionManager.h"
#include "Interaction/CommandRegistrar.h"

int main()
{
    try
    {
        // 1. 加载数据
        auto problem = loadProblem("data/problem.txt");
        auto solution = loadSolution("data/solution.txt");

        std::cout << "成功加载解决方案，包含 " << solution.size() << " 个状态\n";

        // 2. 构建树结构
        TreeBuilder treeBuilder;
        auto tree = treeBuilder.buildTree(solution);

        std::cout << treeBuilder.getBuildStats() << std::endl;

        // 3. 创建BoardRenderer
        auto boardRenderer = std::make_unique<BoardRenderer>();
        const auto &firstState = solution.getState(0);
        int boardSize = static_cast<int>(std::sqrt(firstState.size()));
        boardRenderer->setBoardSize(boardSize, 80.0f);

        // 4. 计算布局
        TreeLayout treeLayout;
        treeLayout.setTree(tree.get());

        // 根据BoardRenderer的实际大小设置布局参数
        auto boardTotalSize = boardRenderer->getTotalSize();
        treeLayout.setLayoutParameters(250.0f, 200.0f, boardTotalSize.x, boardTotalSize.y);
        treeLayout.setPosition(100, 50); // 设置起始位置
        treeLayout.calculateLayout();

        std::cout << "布局计算完成，整体尺寸: "
                  << treeLayout.getTotalSize().x << " x "
                  << treeLayout.getTotalSize().y << std::endl;

        // 5. 创建渲染器
        TreeRenderer treeRenderer;
        treeRenderer.setTreeAndLayout(tree.get(), &treeLayout);
        treeRenderer.setBoardRenderer(std::move(boardRenderer));
        treeRenderer.setConnectionColor(sf::Color(200, 200, 200)); // 浅灰色连接线

        // 5. 初始化SFML窗口和CanvasView
        sf::RenderWindow window(sf::VideoMode(1600, 1000), "Search Tree Visualization");
        window.setFramerateLimit(60);

        // 创建CanvasView
        CanvasView canvasView(sf::Vector2f(1600, 1000));

        // 设置初始视图中心为树的中心
        auto treeSize = treeLayout.getTotalSize();
        canvasView.setCenter(sf::Vector2f(treeSize.x / 2, treeSize.y / 2));

        // 创建交互管理器
        InteractionManager interactionManager;

        // 注册交互命令
        CommandRegistrar::registerDefaultCommands(
            interactionManager,
            canvasView,
            treeRenderer,
            std::move(tree));

        // 6. 渲染循环
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();

                // 处理CanvasView事件（缩放和平移）
                canvasView.handleEvent(event);

                // 处理交互命令
                interactionManager.handleEvent(event);
            }

            // 更新CanvasView
            canvasView.update();

            window.clear(sf::Color::Black);

            // 应用CanvasView到窗口
            canvasView.applyToWindow(window);

            // 绘制整个树结构
            treeRenderer.draw(window);

            // 绘制拖拽指示器（在树结构之上）
            canvasView.drawDragIndicator(window);

            window.display();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
