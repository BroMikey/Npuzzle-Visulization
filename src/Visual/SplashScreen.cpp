#include "SplashScreen.h"
#include <iostream>

SplashScreen::SplashScreen(const sf::Vector2f& windowSize)
    : m_windowSize(windowSize)
    , m_selectedMode(VisualizationMode::None)
    , m_fontLoaded(false)
{
    // 尝试加载字体
    if (!m_font.loadFromFile("C:/Windows/Fonts/msyh.ttc")) // 微软雅黑字体
    {
        std::cerr << "警告: 无法加载字体，按钮将不显示文本" << std::endl;
        m_fontLoaded = false;
    }
    else
    {
        m_fontLoaded = true;
    }

    // 计算按钮位置和大小
    float buttonWidth = 300.0f;
    float buttonHeight = 120.0f;
    float buttonSpacing = 50.0f;
    float totalWidth = buttonWidth * 3 + buttonSpacing * 2;
    float startX = (windowSize.x - totalWidth) / 2;
    float buttonY = windowSize.y / 2 - buttonHeight / 2;

    // 创建树形可视化按钮
    m_treeButton = createButton(
        sf::Vector2f(startX, buttonY),
        sf::Vector2f(buttonWidth, buttonHeight),
        "tree view",
        sf::Color(70, 130, 180)  // 钢蓝色
    );

    // 创建动画可视化按钮
    m_animationButton = createButton(
        sf::Vector2f(startX + buttonWidth + buttonSpacing, buttonY),
        sf::Vector2f(buttonWidth, buttonHeight),
        "animation view",
        sf::Color(60, 179, 113)  // 中海洋绿色
    );

    // 创建问题编辑器按钮
    m_editorButton = createButton(
        sf::Vector2f(startX + (buttonWidth + buttonSpacing) * 2, buttonY),
        sf::Vector2f(buttonWidth, buttonHeight),
        "problem editor",
        sf::Color(186, 85, 211)  // 中兰花紫色
    );
}

bool SplashScreen::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mousePos(
                static_cast<float>(event.mouseButton.x),
                static_cast<float>(event.mouseButton.y)
            );

            // 检查点击了哪个按钮
            if (contains(mousePos, m_treeButton))
            {
                m_selectedMode = VisualizationMode::TreeView;
                return true;
            }
            else if (contains(mousePos, m_animationButton))
            {
                m_selectedMode = VisualizationMode::AnimationView;
                return true;
            }
            else if (contains(mousePos, m_editorButton))
            {
                m_selectedMode = VisualizationMode::ProblemEditor;
                return true;
            }
        }
    }

    return false;
}

void SplashScreen::draw(sf::RenderTarget& target) const
{
    // 绘制黑色背景
    sf::RectangleShape background(m_windowSize);
    background.setFillColor(sf::Color::Black);
    target.draw(background);

    // 绘制按钮
    target.draw(m_treeButton);
    target.draw(m_animationButton);
    target.draw(m_editorButton);

    // 如果字体加载成功，绘制按钮文本
    if (m_fontLoaded)
    {
        // 树形按钮文本
        sf::Text treeText("tree view", m_font, 20);
        treeText.setFillColor(sf::Color::White);
        treeText.setStyle(sf::Text::Bold);
        
        sf::FloatRect treeBounds = treeText.getLocalBounds();
        treeText.setPosition(
            m_treeButton.getPosition().x + (m_treeButton.getSize().x - treeBounds.width) / 2,
            m_treeButton.getPosition().y + (m_treeButton.getSize().y - treeBounds.height) / 2 - 10
        );
        target.draw(treeText);

        // 动画按钮文本
        sf::Text animationText("animation view", m_font, 20);
        animationText.setFillColor(sf::Color::White);
        animationText.setStyle(sf::Text::Bold);
        
        sf::FloatRect animationBounds = animationText.getLocalBounds();
        animationText.setPosition(
            m_animationButton.getPosition().x + (m_animationButton.getSize().x - animationBounds.width) / 2,
            m_animationButton.getPosition().y + (m_animationButton.getSize().y - animationBounds.height) / 2 - 10
        );
        target.draw(animationText);

        // 问题编辑器按钮文本
        sf::Text editorText("problem editor", m_font, 20);
        editorText.setFillColor(sf::Color::White);
        editorText.setStyle(sf::Text::Bold);
        
        sf::FloatRect editorBounds = editorText.getLocalBounds();
        editorText.setPosition(
            m_editorButton.getPosition().x + (m_editorButton.getSize().x - editorBounds.width) / 2,
            m_editorButton.getPosition().y + (m_editorButton.getSize().y - editorBounds.height) / 2 - 10
        );
        target.draw(editorText);
    }
}

VisualizationMode SplashScreen::getSelectedMode() const
{
    return m_selectedMode;
}

void SplashScreen::reset()
{
    m_selectedMode = VisualizationMode::None;
}

sf::RectangleShape SplashScreen::createButton(const sf::Vector2f& position, const sf::Vector2f& size, 
                                             const std::string& text, const sf::Color& color)
{
    sf::RectangleShape button(size);
    button.setPosition(position);
    button.setFillColor(color);
    button.setOutlineColor(sf::Color::White);
    button.setOutlineThickness(2.0f);
    
    return button;
}

bool SplashScreen::contains(const sf::Vector2f& point, const sf::RectangleShape& rect) const
{
    sf::FloatRect bounds = rect.getGlobalBounds();
    return bounds.contains(point);
}
