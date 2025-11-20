#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../Core/PuzzleState.h"
#include "../Visual/BoardRenderer.h"
#include "../Interaction/InteractionManager.h"

/**
 * @brief 问题编辑器类
 */
class ProblemEditor
{
public:
    /**
     * @brief 构造函数
     * @param windowSize 窗口大小
     */
    ProblemEditor(const sf::Vector2f& windowSize);

    /**
     * @brief 处理事件
     * @param event SFML事件
     * @return 如果请求退出返回true
     */
    bool handleEvent(const sf::Event& event);

    /**
     * @brief 更新编辑器状态
     */
    void update();

    /**
     * @brief 绘制编辑器界面
     * @param target 渲染目标
     */
    void draw(sf::RenderTarget& target) const;

    /**
     * @brief 运行编辑器主循环
     * @param window SFML窗口
     */
    void run(sf::RenderWindow& window);

    /**
     * @brief 保存问题到Java项目文件
     */
    void saveProblemToJavaProject();

    /**
     * @brief 保存当前问题到data文件夹
     */
    void saveCurrentProblem();

private:
    /**
     * @brief 初始化交互命令
     */
    void initializeCommands();

    /**
     * @brief 创建按钮
     * @param position 按钮位置
     * @param size 按钮大小
     * @param text 按钮文本
     * @param color 按钮颜色
     * @return 按钮形状
     */
    sf::RectangleShape createButton(const sf::Vector2f& position, const sf::Vector2f& size, 
                                   const std::string& text, const sf::Color& color) const;

    /**
     * @brief 检查点是否在矩形内
     * @param point 点坐标
     * @param rect 矩形
     * @return 如果点在矩形内返回true
     */
    bool contains(const sf::Vector2f& point, const sf::RectangleShape& rect) const;

    /**
     * @brief 处理单元格点击
     * @param mousePos 鼠标位置
     */
    void handleCellClick(const sf::Vector2f& mousePos);

    /**
     * @brief 切换编辑模式（起始状态/目标状态）
     */
    void toggleEditMode();

    /**
     * @brief 重置当前编辑的状态
     */
    void resetCurrentState();

    /**
     * @brief 增加棋盘大小
     */
    void increaseBoardSize();

    /**
     * @brief 减少棋盘大小
     */
    void decreaseBoardSize();

    /**
     * @brief 创建升序排列的目标状态
     */
    PuzzleState createGoalState(int size);

    /**
     * @brief 创建有数字的初始状态
     */
    PuzzleState createInitialState(int size);

    /**
     * @brief 移动空白格
     * @param dx X方向移动（-1左，1右）
     * @param dy Y方向移动（-1上，1下）
     */
    void moveEmptyTile(int dx, int dy);

    /**
     * @brief 运行树形视图
     */
    void runTreeView(sf::RenderWindow& window);

    /**
     * @brief 运行动画视图
     */
    void runAnimationView(sf::RenderWindow& window);

private:
    sf::Vector2f m_windowSize;              // 窗口大小
    InteractionManager m_interactionManager; // 交互管理器
    
    // 棋盘状态
    int m_boardSize;                        // 当前棋盘大小
    PuzzleState m_startState;               // 起始状态
    PuzzleState m_goalState;                // 目标状态
    
    // 渲染组件
    BoardRenderer m_startBoardRenderer;     // 起始状态棋盘渲染器
    BoardRenderer m_goalBoardRenderer;      // 目标状态棋盘渲染器
    sf::RectangleShape m_startBoard;        // 起始状态棋盘区域
    sf::RectangleShape m_goalBoard;         // 目标状态棋盘区域
    sf::RectangleShape m_resetButton;       // 重置按钮
    sf::RectangleShape m_saveButton;        // 保存按钮
    sf::RectangleShape m_sizeUpButton;      // 增加大小按钮
    sf::RectangleShape m_sizeDownButton;    // 减少大小按钮
    sf::RectangleShape m_treeViewButton;    // 树形视图按钮
    sf::RectangleShape m_animationViewButton; // 动画视图按钮
    
    // 文本显示
    sf::Font m_font;
    bool m_fontLoaded;
    
    // 选中状态
    int m_selectedCell;                     // 当前选中的单元格索引
    bool m_shouldExit;                      // 是否应该退出编辑器
    int m_nextMode;                         // 下一个模式：0=编辑器，1=树形视图，2=动画视图
};
