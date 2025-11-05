#pragma once

#include <SFML/Graphics.hpp>

/**
 * @brief 画布视图，负责处理缩放和平移功能
 */
class CanvasView
{
public:
    /**
     * @brief 构造函数
     * @param windowSize 窗口大小
     */
    CanvasView(const sf::Vector2f &windowSize);

    /**
     * @brief 处理事件
     * @param event SFML事件
     */
    void handleEvent(const sf::Event &event);

    /**
     * @brief 更新视图
     */
    void update();

    /**
     * @brief 应用视图到窗口
     * @param window 渲染窗口
     */
    void applyToWindow(sf::RenderWindow &window);

    /**
     * @brief 绘制拖拽指示器
     * @param window 渲染窗口
     */
    void drawDragIndicator(sf::RenderWindow &window);

    /**
     * @brief 重置视图
     */
    void reset();

    /**
     * @brief 是否正在拖拽
     * @return 拖拽状态
     */
    bool isDragging() const { return m_isDragging; }

    /**
     * @brief 设置视图中心
     * @param center 中心位置
     */
    void setCenter(const sf::Vector2f &center);

    /**
     * @brief 设置缩放级别
     * @param zoom 缩放级别
     */
    void setZoom(float zoom);

    /**
     * @brief 获取当前缩放级别
     * @return 缩放级别
     */
    float getZoom() const;

    /**
     * @brief 获取视图中心
     * @return 视图中心
     */
    sf::Vector2f getCenter() const;

    /**
     * @brief 将窗口坐标转换为世界坐标
     * @param windowPos 窗口坐标
     * @return 世界坐标
     */
    sf::Vector2f windowToWorld(const sf::Vector2f &windowPos) const;

    /**
     * @brief 将世界坐标转换为窗口坐标
     * @param worldPos 世界坐标
     * @return 窗口坐标
     */
    sf::Vector2f worldToWindow(const sf::Vector2f &worldPos) const;

private:
    sf::View m_view;           // SFML视图
    sf::Vector2f m_windowSize; // 窗口大小
    sf::Vector2f m_center;     // 视图中心
    float m_zoom;              // 当前缩放级别

    // 鼠标状态
    bool m_isDragging;           // 是否正在拖拽
    sf::Vector2f m_lastMousePos; // 上一次鼠标位置
    sf::Vector2f m_dragStartPos; // 拖拽起始位置

    // 缩放限制
    static constexpr float MIN_ZOOM = 0.01f;  // 最小缩放（支持大尺寸树）
    static constexpr float MAX_ZOOM = 20.0f;  // 最大缩放
    static constexpr float ZOOM_SPEED = 0.1f; // 缩放速度

    // 拖拽速度控制
    static constexpr float DRAG_SPEED_FACTOR = 0.05f; // 拖拽速度因子（降低到原来的十分之一）
    static constexpr float MIN_DRAG_SPEED = 0.001f;   // 最小拖拽速度
    static constexpr float MAX_DRAG_SPEED = 0.01f;    // 最大拖拽速度

    /**
     * @brief 处理鼠标滚轮事件
     * @param delta 滚轮增量
     * @param mousePos 鼠标位置
     */
    void handleMouseWheel(int delta, const sf::Vector2f &mousePos);

    /**
     * @brief 处理鼠标按下事件
     * @param mousePos 鼠标位置
     */
    void handleMousePressed(const sf::Vector2f &mousePos);

    /**
     * @brief 处理鼠标移动事件
     * @param mousePos 鼠标位置
     */
    void handleMouseMoved(const sf::Vector2f &mousePos);

    /**
     * @brief 处理鼠标释放事件
     */
    void handleMouseReleased();

    /**
     * @brief 更新视图变换
     */
    void updateView();
};
