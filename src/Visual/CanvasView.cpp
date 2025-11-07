#include "CanvasView.h"
#include <cmath>
#include <iostream>

CanvasView::CanvasView(const sf::Vector2f &windowSize)
    : m_windowSize(windowSize), m_center(0, 0), m_zoom(1.0f), m_isDragging(false)
{
    reset();
}

void CanvasView::handleEvent(const sf::Event &event)
{
    switch (event.type)
    {
    case sf::Event::MouseWheelScrolled:
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
        {
            handleMouseWheel(event.mouseWheelScroll.delta,
                             sf::Vector2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y));
        }
        break;

    case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Middle)
        {
            handleMousePressed(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
        }
        break;

    case sf::Event::MouseMoved:
        if (m_isDragging)
        {
            handleMouseMoved(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
        }
        break;

    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Middle)
        {
            handleMouseReleased();
        }
        break;

    case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::R)
        {
            // R键重置视图
            reset();
        }
        break;

    default:
        break;
    }
}

void CanvasView::update()
{
    updateView();
}

void CanvasView::applyToWindow(sf::RenderWindow &window)
{
    window.setView(m_view);
}

void CanvasView::drawDragIndicator(sf::RenderWindow &window)
{
    if (m_isDragging)
    {
        // 保存当前视图
        sf::View originalView = window.getView();

        // 切换到默认视图（窗口坐标）
        window.setView(window.getDefaultView());

        // 绘制拖拽指示器（从拖拽起始位置到当前位置的线）
        sf::VertexArray dragLine(sf::Lines, 2);
        dragLine[0].position = m_dragStartPos;
        dragLine[0].color = sf::Color::Yellow;
        dragLine[1].position = m_lastMousePos;
        dragLine[1].color = sf::Color::Yellow;

        // 绘制拖拽线
        window.draw(dragLine);

        // 绘制起始点
        sf::CircleShape startPoint(3);
        startPoint.setFillColor(sf::Color::Green);
        startPoint.setPosition(m_dragStartPos - sf::Vector2f(3, 3));
        window.draw(startPoint);

        // 绘制当前点
        sf::CircleShape currentPoint(3);
        currentPoint.setFillColor(sf::Color::Red);
        currentPoint.setPosition(m_lastMousePos - sf::Vector2f(3, 3));
        window.draw(currentPoint);

        // 恢复原始视图
        window.setView(originalView);
    }
}

void CanvasView::reset()
{
    m_center = sf::Vector2f(0, 0);
    m_zoom = 1.0f;
    updateView();
}

void CanvasView::setCenter(const sf::Vector2f &center)
{
    m_center = center;
    updateView();
}

void CanvasView::setZoom(float zoom)
{
    m_zoom = std::clamp(zoom, MIN_ZOOM, MAX_ZOOM);
    updateView();
}

float CanvasView::getZoom() const
{
    return m_zoom;
}

sf::Vector2f CanvasView::getCenter() const
{
    return m_center;
}

sf::Vector2f CanvasView::windowToWorld(const sf::Vector2f &windowPos) const
{
    return m_view.getInverseTransform().transformPoint(windowPos);
}

sf::Vector2f CanvasView::worldToWindow(const sf::Vector2f &worldPos) const
{
    return m_view.getTransform().transformPoint(worldPos);
}

void CanvasView::handleMouseWheel(int delta, const sf::Vector2f &mousePos)
{
    // 保存缩放前的鼠标世界坐标
    sf::Vector2f worldPosBeforeZoom = windowToWorld(mousePos);

    // 计算新的缩放级别
    float zoomFactor = 1.0f + (delta > 0 ? ZOOM_SPEED : -ZOOM_SPEED);
    float newZoom = m_zoom * zoomFactor;

    // 限制缩放范围
    newZoom = std::clamp(newZoom, MIN_ZOOM, MAX_ZOOM);

    if (newZoom != m_zoom)
    {
        m_zoom = newZoom;

        // 保存缩放后的鼠标世界坐标
        sf::Vector2f worldPosAfterZoom = windowToWorld(mousePos);

        // 调整中心点，使缩放围绕鼠标位置进行
        m_center += worldPosBeforeZoom - worldPosAfterZoom;

        updateView();
    }
}

void CanvasView::handleMousePressed(const sf::Vector2f &mousePos)
{
    m_isDragging = true;
    m_lastMousePos = mousePos;
    m_dragStartPos = mousePos;
}

void CanvasView::handleMouseMoved(const sf::Vector2f &mousePos)
{
    if (!m_isDragging)
        return;
    // 计算鼠标移动的增量（窗口坐标）
    sf::Vector2f delta = mousePos - m_lastMousePos;

    // 应用速度控制：基于缩放级别的速度调整
    float dragSpeed = std::clamp(DRAG_SPEED_FACTOR / m_zoom, MIN_DRAG_SPEED, MAX_DRAG_SPEED); // 精美的库函数使用
    delta *= dragSpeed;

    // 转换为世界坐标增量 - 修正方向问题
    // 对于树结构，拖拽应该像移动地图一样：
    // 鼠标向下移动时，视图向上移动
    // 鼠标向左移动时，视图向右移动
    // std::cout << "Delta: (" << delta.x << ", " << delta.y << ")\n";
    sf::Vector2f worldDelta = windowToWorld(sf::Vector2f(-delta.x, delta.y)) - windowToWorld(sf::Vector2f(0, 0));

    // 更新中心点（修正后的方向：鼠标移动方向与视图移动方向相反）
    m_center += worldDelta;
    m_lastMousePos = mousePos;

    updateView();
}

void CanvasView::handleMouseReleased()
{
    m_isDragging = false;
}

void CanvasView::updateView()
{
    // 设置视图大小（基于窗口大小和缩放级别）
    sf::Vector2f viewSize = m_windowSize / m_zoom;

    // 创建视图
    m_view.setSize(viewSize);
    m_view.setCenter(m_center);

    // 设置视图端口（整个窗口）
    m_view.setViewport(sf::FloatRect(0, 0, 1, 1));
}
