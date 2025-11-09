/**
 * @file CommandRegistrar.cpp
 * @author your name (you@domain.com)
 * @brief 注册默认命令
 * @version 0.1
 * @date 2025-11-08
 * @details 注册默认命令：
 * - S：显示/隐藏统计信息
 * - 空格：播放/暂停（自动播放模式）
 * - 右箭头：下一步（手动模式）
 * - 左箭头：上一步（手动模式）
 * - R：重置视图
 * - 方向键：步前/步后
 * @copyright Copyright (c) 2025
 * 
 */

#include "CommandRegistrar.h"
#include "../Visual/DisplayManager.h"
#include <iostream>

namespace CommandRegistrar
{
    void registerDefaultCommands(
        InteractionManager &manager,
        CanvasView &canvasView,
        TreeRenderer &treeRenderer,
        std::shared_ptr<Tree> tree,
        DisplayManager *displayManager)
    {
        static bool showStats = true;

        // S：显示/隐藏统计（原空格键功能移到S键）
        manager.registerKeyCommand(sf::Keyboard::S,
                                   std::make_unique<FunctionCommand>([]() mutable
                                                                     {
                showStats = !showStats;
                std::cout << (showStats ? "显示统计信息" : "隐藏统计信息") << std::endl; }));

        // 空格：播放/暂停（自动播放模式）
        if (displayManager)
        {
            manager.registerKeyCommand(sf::Keyboard::Space,
                                       std::make_unique<FunctionCommand>([displayManager]()
                                                                         {
                displayManager->togglePlayPause();
                auto state = displayManager->getPlayState();
                auto mode = displayManager->getDisplayMode();
                std::cout << (state == PlayState::Playing ? "播放" : "暂停") 
                          << " (" << (mode == DisplayMode::Manual ? "手动模式" : "自动模式") << ")" << std::endl; }));
        }

        // 右箭头：下一步（手动模式）
        if (displayManager)
        {
            manager.registerKeyCommand(sf::Keyboard::Right,
                                       std::make_unique<FunctionCommand>([displayManager]()
                                                                         {
                displayManager->nextStep();
                std::cout << "下一步 - 已显示 " << displayManager->getVisibleCount() 
                          << "/" << displayManager->getTotalNodes() << " 个节点" << std::endl; }));
        }

        // 左箭头：上一步（手动模式）
        if (displayManager)
        {
            manager.registerKeyCommand(sf::Keyboard::Left,
                                       std::make_unique<FunctionCommand>([displayManager]()
                                                                         {
                displayManager->previousStep();
                std::cout << "上一步 - 已显示 " << displayManager->getVisibleCount() 
                          << "/" << displayManager->getTotalNodes() << " 个节点" << std::endl; }));
        }

        // A：切换手动/自动模式
        if (displayManager)
        {
            manager.registerKeyCommand(sf::Keyboard::A,
                                       std::make_unique<FunctionCommand>([displayManager]()
                                                                         {
                displayManager->toggleDisplayMode();
                auto mode = displayManager->getDisplayMode();
                auto state = displayManager->getPlayState();
                std::cout << "切换到 " << (mode == DisplayMode::Manual ? "手动模式" : "自动模式")
                          << " (" << (state == PlayState::Playing ? "播放中" : "暂停") << ")" << std::endl; }));
        }

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
