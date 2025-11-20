#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

/**
 * @brief 可视化模式枚举
 */
enum class VisualizationMode
{
    None,           // 未选择
    TreeView,       // 树形可视化
    AnimationView,  // 动画可视化
    ProblemEditor   // 问题编辑器
};

/**
 * @brief 开屏界面类
 */
class SplashScreen
{
public:
    /**
     * @brief 构造函数
     * @param windowSize 窗口大小
     */
    SplashScreen(const sf::Vector2f& windowSize);

    /**
     * @brief 处理事件
     * @param event SFML事件
     * @return 如果选择了模式返回true，否则返回false
     */
    bool handleEvent(const sf::Event& event);

    /**
     * @brief 绘制开屏界面
     * @param target 渲染目标
     */
    void draw(sf::RenderTarget& target) const;

    /**
     * @brief 获取选择的模式
     * @return 选择的模式
     */
    VisualizationMode getSelectedMode() const;

    /**
     * @brief 重置选择状态
     */
    void reset();

private:
    /**
     * @brief 创建按钮
     * @param position 按钮位置
     * @param size 按钮大小
     * @param text 按钮文本
     * @param color 按钮颜色
     * @return 按钮形状
     */
    sf::RectangleShape createButton(const sf::Vector2f& position, const sf::Vector2f& size, 
                                   const std::string& text, const sf::Color& color);

    /**
     * @brief 检查点是否在矩形内
     * @param point 点坐标
     * @param rect 矩形
     * @return 如果点在矩形内返回true
     */
    bool contains(const sf::Vector2f& point, const sf::RectangleShape& rect) const;

private:
    sf::Vector2f m_windowSize;              // 窗口大小
    VisualizationMode m_selectedMode;       // 选择的模式
    sf::RectangleShape m_treeButton;        // 树形可视化按钮
    sf::RectangleShape m_animationButton;   // 动画可视化按钮
    sf::RectangleShape m_editorButton;      // 问题编辑器按钮
    sf::Font m_font;                        // 字体
    bool m_fontLoaded;                      // 字体是否加载成功
};
