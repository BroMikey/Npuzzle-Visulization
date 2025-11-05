#include "InteractionManager.h"

InteractionManager::InteractionManager()
{
}

void InteractionManager::handleEvent(const sf::Event &event)
{
    switch (event.type)
    {
    case sf::Event::KeyPressed:
    {
        auto it = m_keyCommands.find(event.key.code);
        if (it != m_keyCommands.end())
        {
            it->second->execute();
        }
        break;
    }

    case sf::Event::MouseButtonPressed:
    {
        auto it = m_mouseCommands.find(event.mouseButton.button);
        if (it != m_mouseCommands.end())
        {
            it->second->execute();
        }
        break;
    }

    default:
        break;
    }
}

void InteractionManager::registerKeyCommand(sf::Keyboard::Key key, std::unique_ptr<InteractionCommand> command)
{
    m_keyCommands[key] = std::move(command);
}

void InteractionManager::registerMouseCommand(sf::Mouse::Button button, std::unique_ptr<InteractionCommand> command)
{
    m_mouseCommands[button] = std::move(command);
}

void InteractionManager::clearCommands()
{
    m_keyCommands.clear();
    m_mouseCommands.clear();
}
