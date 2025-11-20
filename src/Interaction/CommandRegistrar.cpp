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

        // S: Show/Hide statistics
        manager.registerKeyCommand(sf::Keyboard::S,
                                   std::make_unique<FunctionCommand>([]() mutable
                                                                     {
                showStats = !showStats;
                std::cout << (showStats ? "Show statistics" : "Hide statistics") << std::endl; }));

        // Space: Start/Pause Auto mode
        if (displayManager)
        {
            manager.registerKeyCommand(sf::Keyboard::Space,
                                       std::make_unique<FunctionCommand>([displayManager]()
                                                                         {
                auto currentMode = displayManager->getDisplayMode();
                auto currentState = displayManager->getPlayState();
                
                if (currentMode == DisplayMode::Manual) {
                    // If in manual mode, switch to auto mode and start playing
                    displayManager->setDisplayMode(DisplayMode::AutoPlay);
                    displayManager->setPlayState(PlayState::Playing);
                    std::cout << "Auto mode started - Playing" << std::endl;
                } else {
                    // If already in auto mode, toggle play/pause
                    displayManager->togglePlayPause();
                    auto newState = displayManager->getPlayState();
                    std::cout << (newState == PlayState::Playing ? "Resumed - Playing" : "Paused") << std::endl;
                } }));
        }

        // Right Arrow: Next step (Manual mode)
        if (displayManager)
        {
            manager.registerKeyCommand(sf::Keyboard::Right,
                                       std::make_unique<FunctionCommand>([displayManager]()
                                                                         {
                displayManager->nextStep();
                std::cout << "Next step - Showing " << displayManager->getVisibleCount() 
                          << "/" << displayManager->getTotalNodes() << " nodes" << std::endl; }));
        }

        // Left Arrow: Previous step (Manual mode)
        if (displayManager)
        {
            manager.registerKeyCommand(sf::Keyboard::Left,
                                       std::make_unique<FunctionCommand>([displayManager]()
                                                                         {
                displayManager->previousStep();
                std::cout << "Previous step - Showing " << displayManager->getVisibleCount() 
                          << "/" << displayManager->getTotalNodes() << " nodes" << std::endl; }));
        }

        // A: Toggle Manual/Auto mode
        if (displayManager)
        {
            manager.registerKeyCommand(sf::Keyboard::A,
                                       std::make_unique<FunctionCommand>([displayManager]()
                                                                         {
                displayManager->toggleDisplayMode();
                auto mode = displayManager->getDisplayMode();
                auto state = displayManager->getPlayState();
                std::cout << "Switched to " << (mode == DisplayMode::Manual ? "Manual" : "Auto")
                          << " mode (" << (state == PlayState::Playing ? "Playing" : "Paused") << ")" << std::endl; }));
        }

        // H: Highlight path
        manager.registerKeyCommand(sf::Keyboard::H,
                                   std::make_unique<FunctionCommand>([tree, &treeRenderer]()
                                                                     {
                if (tree && tree->size() > 0)
                {
                    auto path = tree->getNodeByIndex(tree->size() - 1)->getPathToRoot();
                    treeRenderer.setHighlightPath(path);
                    std::cout << "Highlight path: from root to node " << (tree->size() - 1) << std::endl;
                } }));

        // C: Clear highlight
        manager.registerKeyCommand(sf::Keyboard::C,
                                   std::make_unique<FunctionCommand>([&treeRenderer]()
                                                                     {
                treeRenderer.clearHighlightPath();
                std::cout << "Clear highlight path" << std::endl; }));

        // R: Reset view
        manager.registerKeyCommand(sf::Keyboard::R,
                                   std::make_unique<FunctionCommand>([&canvasView]()
                                                                     {
                canvasView.reset();
                std::cout << "Reset view" << std::endl; }));
    }
}
