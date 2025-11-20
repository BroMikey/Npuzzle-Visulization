#include "AnimationRenderer.h"
#include <iostream>
#include <cmath>

AnimationRenderer::AnimationRenderer()
    : m_solution(nullptr)
    , m_currentStep(0)
    , m_currentPosition(0.0f, 0.0f)  // 初始位置设为原点，由CanvasView居中
    , m_previousPosition(0.0f, 0.0f)
    , m_fontLoaded(false)
    , m_showConnectionLine(true)  // 默认显示连接线
{
    // 尝试加载字体
    if (!m_font.loadFromFile("C:/Windows/Fonts/msyh.ttc"))
    {
        std::cerr << "警告: 无法加载字体，移动方向指示器将不显示文本" << std::endl;
        m_fontLoaded = false;
    }
    else
    {
        m_fontLoaded = true;
    }
}

void AnimationRenderer::setSolution(const ISolution& solution)
{
    m_solution = &solution;
    reset();
}

void AnimationRenderer::setBoardRenderer(std::unique_ptr<BoardRenderer> boardRenderer)
{
    m_boardRenderer = std::move(boardRenderer);
}

void AnimationRenderer::nextStep()
{
    if (m_solution && m_currentStep < m_solution->size() - 1)
    {
        m_previousPosition = m_currentPosition;
        
        // 根据移动方向更新位置
        if (m_currentStep >= 0)
        {
            const auto& currentState = m_solution->getState(m_currentStep);
            const auto& nextState = m_solution->getState(m_currentStep + 1);
            std::string direction = getMoveDirection(currentState, nextState);
            
            // 根据方向移动位置
            float moveDistance = 150.0f; // 移动距离
            if (direction == "UP")
            {
                m_currentPosition.y -= moveDistance;
            }
            else if (direction == "DOWN")
            {
                m_currentPosition.y += moveDistance;
            }
            else if (direction == "LEFT")
            {
                m_currentPosition.x -= moveDistance;
            }
            else if (direction == "RIGHT")
            {
                m_currentPosition.x += moveDistance;
            }
        }
        
        m_currentStep++;
    }
}

void AnimationRenderer::previousStep()
{
    if (m_currentStep > 0)
    {
        m_currentStep--;
        
        // 根据移动方向反向更新位置
        if (m_currentStep >= 0)
        {
            const auto& currentState = m_solution->getState(m_currentStep);
            const auto& nextState = m_solution->getState(m_currentStep + 1);
            std::string direction = getMoveDirection(currentState, nextState);
            
            // 根据方向反向移动位置
            float moveDistance = 150.0f;
            if (direction == "UP")
            {
                m_currentPosition.y += moveDistance;
            }
            else if (direction == "DOWN")
            {
                m_currentPosition.y -= moveDistance;
            }
            else if (direction == "LEFT")
            {
                m_currentPosition.x += moveDistance;
            }
            else if (direction == "RIGHT")
            {
                m_currentPosition.x -= moveDistance;
            }
        }
        else
        {
            // 回到初始位置
            m_currentPosition = sf::Vector2f(0.0f, 0.0f);
        }
    }
}

void AnimationRenderer::reset()
{
    m_currentStep = 0;
    m_currentPosition = sf::Vector2f(0.0f, 0.0f);  // 重置到原点
    m_previousPosition = sf::Vector2f(0.0f, 0.0f);
}

int AnimationRenderer::getCurrentStep() const
{
    return m_currentStep;
}

int AnimationRenderer::getTotalSteps() const
{
    return m_solution ? m_solution->size() : 0;
}

void AnimationRenderer::draw(sf::RenderTarget& target) const
{
    if (!m_solution || m_solution->size() == 0 || !m_boardRenderer)
        return;

    // 绘制连接线（如果有前一步且启用了连接线显示）
    if (m_currentStep > 0 && m_showConnectionLine)
    {
        drawConnectionLine(target);
    }

    // 绘制当前状态
    const auto& currentState = m_solution->getState(m_currentStep);
    
    // 设置BoardRenderer的位置
    m_boardRenderer->setPosition(m_currentPosition.x, m_currentPosition.y);
    
    // 注意：不再设置g, h, f值，这些数据对于动画演示没有作用
    
    // 转换为RenderWindow进行绘制
    sf::RenderWindow* window = dynamic_cast<sf::RenderWindow*>(&target);
    if (window)
    {
        m_boardRenderer->draw(*window, currentState);
    }

    // 绘制移动方向指示器
    if (m_currentStep > 0)
    {
        drawMoveDirection(target);
    }
}

sf::Vector2f AnimationRenderer::getCurrentNodePosition() const
{
    if (!m_boardRenderer)
        return m_currentPosition;
    
    // 获取BoardRenderer的实际大小
    sf::Vector2f boardSize = m_boardRenderer->getTotalSize();
    
    // 返回Puzzle的中心位置，而不是左上角位置
    return sf::Vector2f(
        m_currentPosition.x + boardSize.x / 2,
        m_currentPosition.y + boardSize.y / 2
    );
}

void AnimationRenderer::setShowConnectionLine(bool show)
{
    m_showConnectionLine = show;
}

bool AnimationRenderer::getShowConnectionLine() const
{
    return m_showConnectionLine;
}

void AnimationRenderer::drawConnectionLine(sf::RenderTarget& target) const
{
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = m_previousPosition;
    line[0].color = sf::Color(200, 200, 200, 150); // 半透明灰色
    line[1].position = m_currentPosition;
    line[1].color = sf::Color(200, 200, 200, 150);
    
    target.draw(line);
}

std::string AnimationRenderer::getMoveDirection(const PuzzleState& prevState, const PuzzleState& currentState) const
{
    // 找到空白格子的位置变化
    int prevBlankPos = -1;
    int currentBlankPos = -1;
    
    // 找到前一个状态的空白格子位置
    for (int i = 0; i < prevState.size(); i++)
    {
        if (prevState[i] == 0)
        {
            prevBlankPos = i;
            break;
        }
    }
    
    // 找到当前状态的空白格子位置
    for (int i = 0; i < currentState.size(); i++)
    {
        if (currentState[i] == 0)
        {
            currentBlankPos = i;
            break;
        }
    }
    
    if (prevBlankPos == -1 || currentBlankPos == -1)
        return "UNKNOWN";
    
    int boardSize = static_cast<int>(std::sqrt(prevState.size()));
    
    // 计算行列位置
    int prevRow = prevBlankPos / boardSize;
    int prevCol = prevBlankPos % boardSize;
    int currentRow = currentBlankPos / boardSize;
    int currentCol = currentBlankPos % boardSize;
    
    // 判断移动方向
    if (currentRow == prevRow - 1 && currentCol == prevCol)
        return "UP";
    else if (currentRow == prevRow + 1 && currentCol == prevCol)
        return "DOWN";
    else if (currentRow == prevRow && currentCol == prevCol - 1)
        return "LEFT";
    else if (currentRow == prevRow && currentCol == prevCol + 1)
        return "RIGHT";
    else
        return "UNKNOWN";
}

void AnimationRenderer::drawMoveDirection(sf::RenderTarget& target) const
{
    if (!m_fontLoaded || m_currentStep < 1)
        return;
    
    const auto& prevState = m_solution->getState(m_currentStep - 1);
    const auto& currentState = m_solution->getState(m_currentStep);
    std::string direction = getMoveDirection(prevState, currentState);
    
    if (direction == "UNKNOWN")
        return;
    
    // 创建方向文本
    sf::Text directionText(direction, m_font, 24);
    directionText.setFillColor(sf::Color::Yellow);
    directionText.setStyle(sf::Text::Bold);
    
    // 固定显示在当前Puzzle的上方
    sf::Vector2f textPosition = m_currentPosition;
    
    // 根据BoardRenderer的大小调整位置，确保在Puzzle上方
    if (m_boardRenderer) {
        sf::Vector2f boardSize = m_boardRenderer->getTotalSize();
        // 将文本放在Puzzle上方，距离Puzzle顶部一定距离
        textPosition.y -= 40.0f; // 固定在上方40像素位置
        textPosition.x += boardSize.x / 2; // 水平居中
    }
    
    sf::FloatRect textBounds = directionText.getLocalBounds();
    directionText.setPosition(
        textPosition.x - textBounds.width / 2,
        textPosition.y - textBounds.height / 2
    );
    
    target.draw(directionText);
}
