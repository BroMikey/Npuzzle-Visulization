#include "CommandRegistrar.h"
#include <iostream>

namespace CommandRegistrar
{
    void registerDefaultCommands(
        InteractionManager &manager,
        CanvasView &canvasView,
        TreeRenderer &treeRenderer,
        std::shared_ptr<Tree> tree)
    {
        static bool showStats = true;

        // 空格：显示/隐藏统计
        manager.registerKeyCommand(sf::Keyboard::Space,
                                   std::make_unique<FunctionCommand>([]() mutable
                                                                     {
                showStats = !showStats;
                std::cout << (showStats ? "显示统计信息" : "隐藏统计信息") << std::endl; }));

        // H：高亮路径
        manager.registerKeyCommand(sf::Keyboard::H,
                                   std::make_unique<FunctionCommand>([tree, &treeRenderer]()
                                                                     {
                if (tree && tree->size() > 0)
                {
                    auto path = tree->getNodeByIndex(tree->size() - 1)->getPathToRoot();
                    treeRenderer.setHighlightPath(path);
                    std::cout << "高亮路径: 从根节点到节点 " << (tree->size() - 1) << std::endl;
                } }));

        // C：清除高亮
        manager.registerKeyCommand(sf::Keyboard::C,
                                   std::make_unique<FunctionCommand>([&treeRenderer]()
                                                                     {
                treeRenderer.clearHighlightPath();
                std::cout << "清除高亮路径" << std::endl; }));

        // R：重置视图
        manager.registerKeyCommand(sf::Keyboard::R,
                                   std::make_unique<FunctionCommand>([&canvasView]()
                                                                     {
                canvasView.reset();
                std::cout << "重置视图" << std::endl; }));
    }
}
