#include "BoardRenderer.h"
#include <SFML/Graphics.hpp>
#include <iostream>

BoardRenderer::BoardRenderer()
    : m_boardSize(3), m_cellSize(80.0f), m_position(0, 0), m_gValue(0), m_hValue(0), m_fValue(0)
{
}

void BoardRenderer::setBoardSize(int size, float cellSize)
{
    m_boardSize = size;
    m_cellSize = cellSize;
    if (size == 5) // 5x5棋盘需要更小的单元格
    {
        m_cellSize = 60.0f;
    }
}

void BoardRenderer::setPosition(float x, float y)
{
    m_position = sf::Vector2f(x, y);
}

void BoardRenderer::setValues(int g, int h, int f)
{
    m_gValue = g;
    m_hValue = h;
    m_fValue = f;
}

void BoardRenderer::draw(sf::RenderWindow &window, const PuzzleState &state)
{
    // 绘制棋盘网格
    for (int i = 0; i < m_boardSize; ++i)
    {
        for (int j = 0; j < m_boardSize; ++j)
        {
            int index = i * m_boardSize + j;
            if (index < state.size())
            {
                drawCell(window, state[index], i, j);
            }
        }
    }

    // 绘制数值面板
    drawValuePanel(window);
}

void BoardRenderer::drawCell(sf::RenderWindow &window, int value, int row, int col)
{
    // 计算单元格位置
    float x = m_position.x + col * m_cellSize;
    float y = m_position.y + row * m_cellSize;

    // 绘制单元格背景
    sf::RectangleShape cell(sf::Vector2f(m_cellSize - 2, m_cellSize - 2));
    cell.setPosition(x + 1, y + 1);

    // 根据值设置颜色：0表示空格（灰色），其他数字为白色
    if (value == 0)
    {
        cell.setFillColor(sf::Color(200, 200, 200)); // 灰色
    }
    else
    {
        cell.setFillColor(sf::Color::White);
    }
    cell.setOutlineColor(sf::Color::Black);
    cell.setOutlineThickness(1.0f);

    window.draw(cell);

    // 如果不是空格，绘制数字
    if (value != 0)
    {
        drawNumber(window, value, x + m_cellSize / 2, y + m_cellSize / 2);
    }
}

void BoardRenderer::drawNumber(sf::RenderWindow &window, int number, float x, float y)
{
    static sf::Font font;
    static bool fontLoaded = false;

    // 尝试加载字体
    if (!fontLoaded)
    {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        {
            std::cerr << "无法加载字体，使用默认渲染" << std::endl;
        }
        fontLoaded = true;
    }

    sf::Text text;
    text.setFont(font);
    text.setString(std::to_string(number));
    text.setCharacterSize(static_cast<unsigned int>(m_cellSize * 0.4f));
    text.setFillColor(sf::Color::Black);

    // 居中文本
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
                   textRect.top + textRect.height / 2.0f);
    text.setPosition(x, y);

    window.draw(text);
}

void BoardRenderer::drawValuePanel(sf::RenderWindow &window)
{
    float boardWidth = m_boardSize * m_cellSize;
    float panelHeight = 40.0f;                    // 面板高度
    float panelY = m_position.y + boardWidth + 5; // 面板在棋盘下方

    // 绘制面板背景 - 宽度与棋盘一致
    sf::RectangleShape background(sf::Vector2f(boardWidth, panelHeight));
    background.setPosition(m_position.x, panelY);
    background.setFillColor(sf::Color(50, 50, 50, 200)); // 半透明深灰色
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(1.0f);
    window.draw(background);

    // 绘制标题
    // drawText(window, "Status", m_position.x + 10, panelY + 10, sf::Color::Yellow);

    // 绘制g值
    drawText(window, "g: " + std::to_string(m_gValue),
             m_position.x + 10, panelY + 10);

    // 绘制h值
    drawText(window, "h: " + std::to_string(m_hValue),
             m_position.x + boardWidth / 3, panelY + 10);

    // 绘制f值
    drawText(window, "f: " + std::to_string(m_fValue),
             m_position.x + 2 * boardWidth / 3, panelY + 10);
}

void BoardRenderer::drawText(sf::RenderWindow &window, const std::string &text, float x, float y, sf::Color color)
{
    static sf::Font font;
    static bool fontLoaded = false;

    // 尝试加载字体
    if (!fontLoaded)
    {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        {
            std::cerr << "无法加载字体" << std::endl;
        }
        fontLoaded = true;
    }

    sf::Text sfText;
    sfText.setFont(font);
    sfText.setString(text);
    sfText.setCharacterSize(18);
    sfText.setFillColor(color);
    sfText.setPosition(x, y);

    window.draw(sfText);
}

sf::Vector2f BoardRenderer::getTotalSize() const
{
    float boardWidth = m_boardSize * m_cellSize;
    float panelHeight = 40.0f; // 面板高度
    return sf::Vector2f(boardWidth, boardWidth + panelHeight + 10);
}

sf::Vector2f BoardRenderer::getPosition() const
{
    return m_position;
}
