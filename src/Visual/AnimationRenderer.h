#pragma once

#include "../Core/PuzzleState.h"
#include "../Parser/Solution.h"
#include "BoardRenderer.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

/**
 * @brief 动画可视化搜索渲染器
 */
class AnimationRenderer
{
public:
    /**
     * @brief 构造函数
     */
    AnimationRenderer();

    /**
     * @brief 设置解决方案
     * @param solution 解决方案
     */
    void setSolution(const ISolution& solution);

    /**
     * @brief 设置棋盘渲染器
     * @param boardRenderer 棋盘渲染器
     */
    void setBoardRenderer(std::unique_ptr<BoardRenderer> boardRenderer);

    /**
     * @brief 下一步：显示下一个状态
     */
    void nextStep();

    /**
     * @brief 上一步：显示上一个状态
     */
    void previousStep();

    /**
     * @brief 重置到初始状态
     */
    void reset();

    /**
     * @brief 获取当前状态索引
     * @return 当前状态索引
     */
    int getCurrentStep() const;

    /**
     * @brief 获取总步数
     * @return 总步数
     */
    int getTotalSteps() const;

    /**
     * @brief 绘制动画
     * @param target 渲染目标
     */
    void draw(sf::RenderTarget& target) const;

    /**
     * @brief 获取当前节点的位置（用于居中显示）
     * @return 当前节点位置
     */
    sf::Vector2f getCurrentNodePosition() const;

    /**
     * @brief 设置是否显示连接线
     * @param show 是否显示连接线
     */
    void setShowConnectionLine(bool show);

    /**
     * @brief 获取是否显示连接线
     * @return 是否显示连接线
     */
    bool getShowConnectionLine() const;

private:
    /**
     * @brief 绘制连接线
     * @param target 渲染目标
     */
    void drawConnectionLine(sf::RenderTarget& target) const;

    /**
     * @brief 获取移动方向
     * @param prevState 前一个状态
     * @param currentState 当前状态
     * @return 移动方向字符串
     */
    std::string getMoveDirection(const PuzzleState& prevState, const PuzzleState& currentState) const;

    /**
     * @brief 绘制移动方向指示器
     * @param target 渲染目标
     */
    void drawMoveDirection(sf::RenderTarget& target) const;

    /**
     * @brief 绘制步数计数器
     * @param target 渲染目标
     */
    void drawStepCounter(sf::RenderTarget& target) const;

private:
    const ISolution* m_solution;                    // 解决方案指针
    std::unique_ptr<BoardRenderer> m_boardRenderer; // 棋盘渲染器
    int m_currentStep;                              // 当前步数
    sf::Vector2f m_currentPosition;                 // 当前节点位置
    sf::Vector2f m_previousPosition;                // 前一个节点位置
    sf::Font m_font;                                // 字体
    bool m_fontLoaded;                              // 字体是否加载成功
    bool m_showConnectionLine;                      // 是否显示连接线
};
