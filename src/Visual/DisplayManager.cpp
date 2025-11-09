#include "DisplayManager.h"
#include <algorithm>

DisplayManager::DisplayManager()
    : m_displayMode(DisplayMode::Manual), m_playState(PlayState::Paused), m_totalNodes(0), m_playInterval(sf::milliseconds(500)) // 默认0.5秒间隔
      ,
      m_accumulatedTime(sf::Time::Zero), m_currentIndex(0)
{
}

void DisplayManager::setTotalNodes(size_t totalNodes)
{
    m_totalNodes = totalNodes;
    reset();
}

DisplayMode DisplayManager::getDisplayMode() const
{
    return m_displayMode;
}

void DisplayManager::setDisplayMode(DisplayMode mode)
{
    m_displayMode = mode;
    // 切换到手动模式时自动暂停
    if (mode == DisplayMode::Manual)
    {
        m_playState = PlayState::Paused;
    }
}

PlayState DisplayManager::getPlayState() const
{
    return m_playState;
}

void DisplayManager::setPlayState(PlayState state)
{
    m_playState = state;
}

void DisplayManager::togglePlayPause()
{
    if (m_displayMode == DisplayMode::AutoPlay)
    {
        m_playState = (m_playState == PlayState::Playing) ? PlayState::Paused : PlayState::Playing;
    }
}

void DisplayManager::toggleDisplayMode()
{
    m_displayMode = (m_displayMode == DisplayMode::Manual) ? DisplayMode::AutoPlay : DisplayMode::Manual;

    // 切换到自动模式时自动开始播放
    if (m_displayMode == DisplayMode::AutoPlay)
    {
        m_playState = PlayState::Playing;
    }
    else
    {
        m_playState = PlayState::Paused;
    }
}

void DisplayManager::nextStep()
{
    if (m_currentIndex < static_cast<int>(m_totalNodes))
    {
        m_visibleNodes.insert(m_currentIndex);
        m_currentIndex++;
    }
}

void DisplayManager::previousStep()
{
    if (m_currentIndex > 0)
    {
        m_currentIndex--;
        m_visibleNodes.erase(m_currentIndex);
    }
}

void DisplayManager::reset()
{
    m_visibleNodes.clear();
    m_currentIndex = 0;
    m_accumulatedTime = sf::Time::Zero;
}

void DisplayManager::updateAutoPlay(sf::Time deltaTime)
{
    if (m_displayMode == DisplayMode::AutoPlay && m_playState == PlayState::Playing)
    {
        m_accumulatedTime += deltaTime;

        while (m_accumulatedTime >= m_playInterval && m_currentIndex < static_cast<int>(m_totalNodes))
        {
            m_visibleNodes.insert(m_currentIndex);
            m_currentIndex++;
            m_accumulatedTime -= m_playInterval;
        }
    }
}

const std::unordered_set<int> &DisplayManager::getVisibleNodes() const
{
    return m_visibleNodes;
}

size_t DisplayManager::getVisibleCount() const
{
    return m_visibleNodes.size();
}

size_t DisplayManager::getTotalNodes() const
{
    return m_totalNodes;
}

bool DisplayManager::isNodeVisible(int nodeIndex) const
{
    return m_visibleNodes.find(nodeIndex) != m_visibleNodes.end();
}

void DisplayManager::setPlayInterval(sf::Time interval)
{
    m_playInterval = interval;
}

sf::Time DisplayManager::getPlayInterval() const
{
    return m_playInterval;
}
