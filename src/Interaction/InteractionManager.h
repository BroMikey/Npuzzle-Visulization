#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <unordered_map>
#include <memory>

/**
 * @brief 交互命令接口
 */
class InteractionCommand
{
public:
    virtual ~InteractionCommand() = default;
    virtual void execute() = 0;
};

/**
 * @brief 交互管理器，使用命令模式处理所有交互事件
 */
class InteractionManager
{
public:
    /**
     * @brief 构造函数
     */
    InteractionManager();

    /**
     * @brief 处理事件
     * @param event SFML事件
     */
    void handleEvent(const sf::Event &event);

    /**
     * @brief 注册键盘命令
     * @param key 键盘按键
     * @param command 命令对象
     */
    void registerKeyCommand(sf::Keyboard::Key key, std::unique_ptr<InteractionCommand> command);

    /**
     * @brief 注册鼠标命令
     * @param button 鼠标按钮
     * @param command 命令对象
     */
    void registerMouseCommand(sf::Mouse::Button button, std::unique_ptr<InteractionCommand> command);

    /**
     * @brief 清除所有命令
     */
    void clearCommands();

private:
    // 键盘命令映射
    std::unordered_map<sf::Keyboard::Key, std::unique_ptr<InteractionCommand>> m_keyCommands;

    // 鼠标命令映射
    std::unordered_map<sf::Mouse::Button, std::unique_ptr<InteractionCommand>> m_mouseCommands;
};

/**
 * @brief 函数命令包装器
 */
class FunctionCommand : public InteractionCommand
{
public:
    /**
     * @brief 构造函数
     * @param func 要执行的函数
     */
    FunctionCommand(std::function<void()> func) : m_function(func) {}

    /**
     * @brief 执行命令
     */
    void execute() override
    {
        if (m_function)
        {
            m_function();
        }
    }

private:
    std::function<void()> m_function;
};
