#include "ProblemEditor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include "../Parser/SolutionLoader.h"
#include "../Parser/ProblemLoader.h"
#include "../Visual/TreeVisualizationManager.h"
#include "../Visual/AnimationVisualizationManager.h"

ProblemEditor::ProblemEditor(const sf::Vector2f& windowSize)
    : m_windowSize(windowSize)
    , m_boardSize(3)
    , m_selectedCell(-1)
    , m_shouldExit(false)
    , m_nextMode(0)
{
    // 初始化字体
    if (!m_font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        std::cerr << "Cannot load font file C:/Windows/Fonts/arial.ttf" << std::endl;
        m_fontLoaded = false;
    }
    else
    {
        m_fontLoaded = true;
    }

    // 尝试从exe同级目录的problem.txt文件读取初始状态
    try
    {
        ProblemData problemData = loadProblem("problem.txt");
        m_boardSize = static_cast<int>(std::sqrt(problemData.start.size()));
        m_startState = PuzzleState(problemData.start);
        m_goalState = PuzzleState(problemData.goal);
        std::cout << "Loaded problem from problem.txt, board size: " << m_boardSize << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Cannot load problem.txt: " << e.what() << ", using default initial state" << std::endl;
        // 如果文件不存在或读取失败，使用默认初始状态
        m_startState = createInitialState(m_boardSize);
        m_goalState = createGoalState(m_boardSize);
    }
    
    // 禁用数值面板显示（对于ProblemEditor，不需要显示g、h、f值）
    m_startBoardRenderer.setShowValuePanel(false);
    m_goalBoardRenderer.setShowValuePanel(false);
    
    // 初始化交互命令
    initializeCommands();
    
    // 初始化UI元素
    float buttonWidth = 180;
    float buttonHeight = 60;
    float boardSize = 400;
    float boardSpacing = 100;
    
    // 计算棋盘位置（水平居中）
    float totalBoardsWidth = 2 * boardSize + boardSpacing;
    float boardStartX = (m_windowSize.x - totalBoardsWidth) / 2;
    float boardY = 180;  // 给标题和控制说明留出足够空间
    
    // 设置棋盘渲染器位置
    m_startBoardRenderer.setBoardSize(m_boardSize);
    m_goalBoardRenderer.setBoardSize(m_boardSize);
    m_startBoardRenderer.setPosition(boardStartX, boardY);
    m_goalBoardRenderer.setPosition(boardStartX + boardSize + boardSpacing, boardY);
    
    // 起始状态棋盘区域
    m_startBoard = createButton(sf::Vector2f(boardStartX, boardY), 
                               sf::Vector2f(boardSize, boardSize), 
                               "Start State", sf::Color(100, 100, 200, 128));
    
    // 目标状态棋盘区域（只显示，不可编辑）
    m_goalBoard = createButton(sf::Vector2f(boardStartX + boardSize + boardSpacing, boardY), 
                              sf::Vector2f(boardSize, boardSize), 
                              "Goal State (Fixed)", sf::Color(200, 100, 100, 128));
    
    // 计算按钮位置（水平居中）
    float buttonRow1Y = boardY + boardSize + 40;  // 棋盘下方留出空间
    float buttonRow2Y = buttonRow1Y + buttonHeight + 20;
    
    // 第一行按钮（4个按钮）
    float buttonRow1Spacing = (m_windowSize.x - 4 * buttonWidth) / 5;
    
    m_resetButton = createButton(sf::Vector2f(buttonRow1Spacing, buttonRow1Y), 
                                sf::Vector2f(buttonWidth, buttonHeight), 
                                "Reset Start", sf::Color::Red);
    m_saveButton = createButton(sf::Vector2f(buttonRow1Spacing * 2 + buttonWidth, buttonRow1Y), 
                               sf::Vector2f(buttonWidth, buttonHeight), 
                               "Save to Java", sf::Color::Green);
    m_sizeDownButton = createButton(sf::Vector2f(buttonRow1Spacing * 3 + buttonWidth * 2, buttonRow1Y), 
                                   sf::Vector2f(buttonWidth, buttonHeight), 
                                   "Decrease Board", sf::Color::Cyan);
    m_sizeUpButton = createButton(sf::Vector2f(buttonRow1Spacing * 4 + buttonWidth * 3, buttonRow1Y), 
                                 sf::Vector2f(buttonWidth, buttonHeight), 
                                 "Increase Board", sf::Color::Cyan);
    
    // 第二行按钮（2个按钮居中）
    float buttonRow2Spacing = (m_windowSize.x - 2 * buttonWidth) / 3;
    
    m_treeViewButton = createButton(sf::Vector2f(buttonRow2Spacing, buttonRow2Y), 
                                   sf::Vector2f(buttonWidth, buttonHeight), 
                                   "Tree View", sf::Color(70, 130, 180));
    m_animationViewButton = createButton(sf::Vector2f(buttonRow2Spacing * 2 + buttonWidth, buttonRow2Y), 
                                       sf::Vector2f(buttonWidth, buttonHeight), 
                                       "Animation View", sf::Color(60, 179, 113));
}

void ProblemEditor::initializeCommands()
{
    // 注册键盘命令
    m_interactionManager.registerKeyCommand(sf::Keyboard::R, 
        std::make_unique<FunctionCommand>([this]() { resetCurrentState(); }));
    
    // 棋盘大小调整：I键增加，D键减少
    m_interactionManager.registerKeyCommand(sf::Keyboard::I, 
        std::make_unique<FunctionCommand>([this]() { increaseBoardSize(); }));
    m_interactionManager.registerKeyCommand(sf::Keyboard::D, 
        std::make_unique<FunctionCommand>([this]() { decreaseBoardSize(); }));
    
    // 方向键控制空白格移动
    m_interactionManager.registerKeyCommand(sf::Keyboard::Up, 
        std::make_unique<FunctionCommand>([this]() { moveEmptyTile(0, -1); }));
    m_interactionManager.registerKeyCommand(sf::Keyboard::Down, 
        std::make_unique<FunctionCommand>([this]() { moveEmptyTile(0, 1); }));
    m_interactionManager.registerKeyCommand(sf::Keyboard::Left, 
        std::make_unique<FunctionCommand>([this]() { moveEmptyTile(-1, 0); }));
    m_interactionManager.registerKeyCommand(sf::Keyboard::Right, 
        std::make_unique<FunctionCommand>([this]() { moveEmptyTile(1, 0); }));
}

bool ProblemEditor::handleEvent(const sf::Event& event)
{
    m_interactionManager.handleEvent(event);
    
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), 
                                 static_cast<float>(event.mouseButton.y));
            
            // 检查按钮点击
            if (contains(mousePos, m_resetButton))
            {
                resetCurrentState();
            }
            else if (contains(mousePos, m_saveButton))
            {
                saveProblemToJavaProject();
            }
            else if (contains(mousePos, m_sizeDownButton))
            {
                decreaseBoardSize();
            }
            else if (contains(mousePos, m_sizeUpButton))
            {
                increaseBoardSize();
            }
            else if (contains(mousePos, m_treeViewButton))
            {
                std::cout << "Switching to Tree View" << std::endl;
                // 保存当前问题到data文件夹
                saveCurrentProblem();
                m_nextMode = 1; // 切换到树形视图
                return true;
            }
            else if (contains(mousePos, m_animationViewButton))
            {
                std::cout << "Switching to Animation View" << std::endl;
                // 保存当前问题到data文件夹
                saveCurrentProblem();
                m_nextMode = 2; // 切换到动画视图
                return true;
            }
            // 移除了棋盘点击处理，现在使用方向键控制
        }
    }
    
    return false;
}

void ProblemEditor::update()
{
    // 更新选中状态的高亮显示等
}

void ProblemEditor::draw(sf::RenderTarget& target) const
{
    // 绘制棋盘区域背景
    target.draw(m_startBoard);
    target.draw(m_goalBoard);
    
    // 绘制起始状态棋盘
    m_startBoardRenderer.draw(static_cast<sf::RenderWindow&>(target), m_startState);
    
    // 绘制目标状态棋盘
    m_goalBoardRenderer.draw(static_cast<sf::RenderWindow&>(target), m_goalState);
    
    // 绘制按钮
    target.draw(m_resetButton);
    target.draw(m_saveButton);
    target.draw(m_sizeDownButton);
    target.draw(m_sizeUpButton);
    target.draw(m_treeViewButton);
    target.draw(m_animationViewButton);
    
    // 绘制按钮文本
    if (m_fontLoaded)
    {
        auto drawButtonText = [&](const sf::RectangleShape& button, const std::string& text) {
            sf::Text buttonText(text, m_font, 18);
            buttonText.setFillColor(sf::Color::Black);
            
            // 计算文本边界并居中显示
            sf::FloatRect textBounds = buttonText.getLocalBounds();
            sf::FloatRect buttonBounds = button.getGlobalBounds();
            
            // 设置文本原点为中心点
            buttonText.setOrigin(textBounds.left + textBounds.width / 2.0f, 
                               textBounds.top + textBounds.height / 2.0f);
            
            // 设置文本位置为按钮中心
            buttonText.setPosition(buttonBounds.left + buttonBounds.width / 2.0f,
                                 buttonBounds.top + buttonBounds.height / 2.0f);
            
            target.draw(buttonText);
        };
        
        drawButtonText(m_resetButton, "Reset Start (R)");
        drawButtonText(m_saveButton, "Save to Java");
        drawButtonText(m_sizeDownButton, "Decrease Board (D)");
        drawButtonText(m_sizeUpButton, "Increase Board (I)");
        drawButtonText(m_treeViewButton, "Tree View");
        drawButtonText(m_animationViewButton, "Animation View");
        
        // 绘制标题 - 居中显示
        sf::Text title("N-Puzzle Problem Editor", m_font, 30);
        title.setFillColor(sf::Color::White);
        
        // 计算标题边界并居中
        sf::FloatRect titleBounds = title.getLocalBounds();
        title.setOrigin(titleBounds.left + titleBounds.width / 2.0f, 
                       titleBounds.top + titleBounds.height / 2.0f);
        title.setPosition(m_windowSize.x / 2.0f, 50);
        target.draw(title);
        
        // 绘制说明文本 - 居中显示
        sf::Text instruction("Edit Start State - Goal State is Fixed", m_font, 20);
        instruction.setFillColor(sf::Color::Yellow);
        
        // 计算说明文本边界并居中
        sf::FloatRect instructionBounds = instruction.getLocalBounds();
        instruction.setOrigin(instructionBounds.left + instructionBounds.width / 2.0f, 
                             instructionBounds.top + instructionBounds.height / 2.0f);
        instruction.setPosition(m_windowSize.x / 2.0f, 90);
        target.draw(instruction);
        
        // 绘制控制说明 - 居中显示
        sf::Text controls("Use Arrow Keys to move empty tile | R: Reset | I/D: Change board size", m_font, 16);
        controls.setFillColor(sf::Color::Cyan);
        
        // 计算控制说明边界并居中
        sf::FloatRect controlsBounds = controls.getLocalBounds();
        controls.setOrigin(controlsBounds.left + controlsBounds.width / 2.0f, 
                          controlsBounds.top + controlsBounds.height / 2.0f);
        controls.setPosition(m_windowSize.x / 2.0f, 120);
        target.draw(controls);
    }
}

void ProblemEditor::run(sf::RenderWindow& window)
{
    bool exitRequested = false;
    
    while (window.isOpen() && !exitRequested)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return;
            }
            
            if (handleEvent(event))
            {
                // 如果handleEvent返回true，表示需要切换视图
                if (m_nextMode == 1)
                {
                    runTreeView(window);
                    m_nextMode = 0; // 重置模式
                }
                else if (m_nextMode == 2)
                {
                    runAnimationView(window);
                    m_nextMode = 0; // 重置模式
                }
            }
        }
        
        update();
        
        window.clear(sf::Color::Black);
        draw(window);
        window.display();
    }
}

void ProblemEditor::saveCurrentProblem()
{
    try
    {
        // 构建问题字符串
        std::stringstream problemStream;
        problemStream << m_boardSize << " ";
        
        // 添加起始状态
        auto startValues = m_startState.getValues();
        for (int value : startValues)
        {
            problemStream << value << " ";
        }
        
        // 添加目标状态
        auto goalValues = m_goalState.getValues();
        for (int value : goalValues)
        {
            problemStream << value << " ";
        }
        
        std::string problemLine = problemStream.str();
        
        // 保存到data/problem.txt文件
        std::ofstream problemFile("problem.txt");
        if (problemFile.is_open())
        {
            problemFile << problemLine << std::endl;
            problemFile.close();
            std::cout << "Current problem saved to problem.txt" << std::endl;
        }
        else
        {
            std::cerr << "Cannot open problem.txt" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error saving current problem: " << e.what() << std::endl;
    }
}

void ProblemEditor::saveProblemToJavaProject()
{
    try
    {
        // 构建问题字符串
        std::stringstream problemStream;
        problemStream << " "  << m_boardSize << " ";
        
        // 添加起始状态
        auto startValues = m_startState.getValues();
        for (int value : startValues)
        {
            problemStream << value << " ";
        }
        
        // 添加目标状态
        auto goalValues = m_goalState.getValues();
        for (int value : goalValues)
        {
            problemStream << value << " ";
        }
        
        std::string problemLine = problemStream.str();
        
        // 构建包含默认解的完整问题行
        std::string fullProblemLine = problemLine;
        
        // 追加到Java项目的problems.txt文件
        std::ofstream javaFile("../resources/problems.txt", std::ios::app);
        if (javaFile.is_open())
        {
            javaFile << fullProblemLine << std::endl;
            javaFile.close();
            std::cout << "Problem successfully saved to Java project file!" << std::endl;
            std::cout << "Problem includes start state, goal state, and default solution" << std::endl;
            
            // 同时保存到本地备份
            std::ofstream localFile("../data/custom_problem.txt", std::ios::app);
            if (localFile.is_open())
            {
                localFile << fullProblemLine << std::endl;
                localFile.close();
            }
        }
        else
        {
            std::cerr << "Cannot open Java project file: ../resources/problems.txt" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error saving file: " << e.what() << std::endl;
    }
}

sf::RectangleShape ProblemEditor::createButton(const sf::Vector2f& position, const sf::Vector2f& size, 
                                              const std::string& text, const sf::Color& color) const
{
    sf::RectangleShape button(size);
    button.setPosition(position);
    button.setFillColor(color);
    button.setOutlineColor(sf::Color::White);
    button.setOutlineThickness(2);
    return button;
}

bool ProblemEditor::contains(const sf::Vector2f& point, const sf::RectangleShape& rect) const
{
    return point.x >= rect.getPosition().x && 
           point.x <= rect.getPosition().x + rect.getSize().x &&
           point.y >= rect.getPosition().y && 
           point.y <= rect.getPosition().y + rect.getSize().y;
}


PuzzleState ProblemEditor::createInitialState(int size)
{
    std::vector<int> initialTiles;
    
    // 对于任意大小，生成数字1到(size*size-1)，然后0（空白格）
    for (int i = 1; i < size * size; i++)
    {
        initialTiles.push_back(i);
    }
    initialTiles.push_back(0); // 空白格在右下角
    
    return PuzzleState(initialTiles);
}

void ProblemEditor::resetCurrentState()
{
    m_startState = createInitialState(m_boardSize);
}

void ProblemEditor::increaseBoardSize()
{
    if (m_boardSize < 10) // 允许最大大小为10
    {
        m_boardSize++;
        m_startState = createInitialState(m_boardSize);
        m_goalState = createGoalState(m_boardSize);
        m_startBoardRenderer.setBoardSize(m_boardSize);
        m_goalBoardRenderer.setBoardSize(m_boardSize);
    }
}

void ProblemEditor::decreaseBoardSize()
{
    if (m_boardSize > 3) // 限制最小大小为3
    {
        m_boardSize--;
        m_startState = createInitialState(m_boardSize);
        m_goalState = createGoalState(m_boardSize);
        m_startBoardRenderer.setBoardSize(m_boardSize);
        m_goalBoardRenderer.setBoardSize(m_boardSize);
    }
}

void ProblemEditor::moveEmptyTile(int dx, int dy)
{
    auto values = m_startState.getValues();
    int emptyIndex = -1;
    
    // 找到空白格的位置（值为0的格子）
    for (int i = 0; i < values.size(); i++)
    {
        if (values[i] == 0)
        {
            emptyIndex = i;
            break;
        }
    }
    
    if (emptyIndex != -1)
    {
        int emptyRow = emptyIndex / m_boardSize;
        int emptyCol = emptyIndex % m_boardSize;
        
        // 计算目标位置
        int targetRow = emptyRow + dy;
        int targetCol = emptyCol + dx;
        
        // 检查目标位置是否在边界内
        if (targetRow >= 0 && targetRow < m_boardSize && targetCol >= 0 && targetCol < m_boardSize)
        {
            int targetIndex = targetRow * m_boardSize + targetCol;
            
            // 交换空白格和目标位置的数字
            std::vector<int> newValues = values;
            std::swap(newValues[emptyIndex], newValues[targetIndex]);
            m_startState = PuzzleState(newValues);
            
        std::cout << "Moved empty tile from (" << emptyRow << "," << emptyCol 
                      << ") to (" << targetRow << "," << targetCol << ")" << std::endl;
        }
        else
        {
            std::cout << "Cannot move empty tile outside board boundaries" << std::endl;
        }
    }
    else
    {
        std::cout << "No empty tile found" << std::endl;
    }
}

PuzzleState ProblemEditor::createGoalState(int size)
{
    std::vector<int> goalTiles;
    for (int i = 1; i < size * size; i++)
    {
        goalTiles.push_back(i);
    }
    goalTiles.push_back(0); // 0 represents the empty tile
    return PuzzleState(goalTiles);
}

void ProblemEditor::runTreeView(sf::RenderWindow& window)
{
    try
    {
        std::cout << "Loading tree solution from data/solution.txt" << std::endl;
        auto solution = loadSolutionTree("solution.txt");
        
        TreeVisualizationManager treeManager(window, sf::Vector2f(1600, 1000));
        treeManager.run(*solution);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading tree view: " << e.what() << std::endl;
        std::cout << "Press any key to return to editor..." << std::endl;
        sf::Event event;
        while (window.waitEvent(event))
        {
            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
            {
                break;
            }
        }
    }
}

void ProblemEditor::runAnimationView(sf::RenderWindow& window)
{
    try
    {
        std::cout << "Loading animation solution from ./solutionAnimation.txt" << std::endl;
        auto solution = loadSolutionAnimation("./solutionAnimation.txt");
        
        AnimationVisualizationManager animationManager(window, sf::Vector2f(1600, 1000));
        animationManager.run(*solution);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading animation view: " << e.what() << std::endl;
        std::cout << "Press any key to return to editor..." << std::endl;
        sf::Event event;
        while (window.waitEvent(event))
        {
            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
            {
                break;
            }
        }
    }
}
