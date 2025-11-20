#pragma once

#include <SFML/Graphics.hpp>
#include "../Core/PuzzleState.h"

class BoardRenderer
{
public:
    BoardRenderer();

    // 设置棋盘大小和位置
    void setBoardSize(int size, float cellSize = 80.0f);
    void setPosition(float x, float y);

    // 设置显示的数值
    void setValues(int g, int h, int f);

    // 设置是否显示数值面板
    void setShowValuePanel(bool show);

    // 绘制棋盘和数值面板
    void draw(sf::RenderWindow &window, const PuzzleState &state) const;

    // 获取整体尺寸信息
    sf::Vector2f getTotalSize() const;
    sf::Vector2f getPosition() const;

private:
    int m_boardSize;         // 棋盘大小 (n x n)
    float m_cellSize;        // 单元格大小
    sf::Vector2f m_position; // 棋盘位置

    int m_gValue;
    int m_hValue;
    int m_fValue;
    bool m_showValuePanel;   // 是否显示数值面板

    // 绘制单个单元格
    void drawCell(sf::RenderWindow &window, int value, int row, int col) const;

    // 绘制数字
    void drawNumber(sf::RenderWindow &window, int number, float x, float y) const;

    // 绘制数值面板
    void drawValuePanel(sf::RenderWindow &window) const;

    // 绘制文本
    void drawText(sf::RenderWindow &window, const std::string &text, float x, float y, sf::Color color = sf::Color::White) const;
};
