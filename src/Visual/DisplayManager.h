#pragma once

#include <vector>
#include <unordered_set>
#include <SFML/System.hpp>

/**
 * @brief 显示模式枚举
 */
enum class DisplayMode
{
    Manual,  // 手动模式：通过按钮或快捷键逐步显示
    AutoPlay // 自动播放模式：自动按时间间隔显示节点
};

/**
 * @brief 播放状态枚举
 */
enum class PlayState
{
    Playing, // 播放中
    Paused   // 暂停
};

/**
 * @brief 显示管理器，控制树节点的显示进度和模式
 */
class DisplayManager
{
public:
    /**
     * @brief 构造函数
     */
    DisplayManager();

    /**
     * @brief 设置树节点总数
     * @param totalNodes 树节点总数
     */
    void setTotalNodes(size_t totalNodes);

    /**
     * @brief 获取当前显示模式
     * @return 当前显示模式
     */
    DisplayMode getDisplayMode() const;

    /**
     * @brief 设置显示模式
     * @param mode 显示模式
     */
    void setDisplayMode(DisplayMode mode);

    /**
     * @brief 获取播放状态
     * @return 播放状态
     */
    PlayState getPlayState() const;

    /**
     * @brief 设置播放状态
     * @param state 播放状态
     */
    void setPlayState(PlayState state);

    /**
     * @brief 切换播放/暂停状态
     */
    void togglePlayPause();

    /**
     * @brief 切换显示模式
     */
    void toggleDisplayMode();

    /**
     * @brief 下一步：显示下一个节点
     */
    void nextStep();

    /**
     * @brief 上一步：隐藏最后一个显示的节点
     */
    void previousStep();

    /**
     * @brief 重置显示进度
     */
    void reset();

    /**
     * @brief 更新自动播放
     * @param deltaTime 时间增量
     */
    void updateAutoPlay(sf::Time deltaTime);

    /**
     * @brief 获取当前已显示的节点索引集合
     * @return 已显示的节点索引集合
     */
    const std::unordered_set<int> &getVisibleNodes() const;

    /**
     * @brief 获取当前显示的节点数量
     * @return 显示的节点数量
     */
    size_t getVisibleCount() const;

    /**
     * @brief 获取总节点数量
     * @return 总节点数量
     */
    size_t getTotalNodes() const;

    /**
     * @brief 检查节点是否可见
     * @param nodeIndex 节点索引
     * @return 如果节点可见返回true，否则返回false
     */
    bool isNodeVisible(int nodeIndex) const;

    /**
     * @brief 设置播放间隔（毫秒）
     * @param intervalMs 间隔时间（毫秒）
     */
    void setPlayInterval(sf::Time interval);

    /**
     * @brief 获取播放间隔
     * @return 播放间隔
     */
    sf::Time getPlayInterval() const;

private:
    DisplayMode m_displayMode;              // 当前显示模式
    PlayState m_playState;                  // 当前播放状态
    std::unordered_set<int> m_visibleNodes; // 已显示的节点索引集合
    size_t m_totalNodes;                    // 总节点数量
    sf::Time m_playInterval;                // 播放间隔
    sf::Time m_accumulatedTime;             // 累积时间（用于自动播放）
    int m_currentIndex;                     // 当前显示的节点索引（用于顺序显示）
};
