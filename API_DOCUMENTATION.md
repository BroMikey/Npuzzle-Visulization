# NPuzzle可视化系统 API 文档

## 概述

NPuzzle可视化系统是一个基于SFML的C++应用程序，用于可视化NPuzzle问题的搜索过程和解决方案。系统提供两种可视化模式：树形结构可视化和动画移动可视化。

## 核心数据结构

### PuzzleState 类

**文件**: `src/Core/PuzzleState.h`

PuzzleState表示NPuzzle的状态，包含棋盘上所有格子的数值。

**构造函数**:
```cpp
PuzzleState()                          // 创建3x3空棋盘
PuzzleState(const std::vector<int> &tiles) // 从数组创建状态
PuzzleState(int n)                     // 创建n x n空棋盘
```

**主要方法**:
- `int size() const` - 获取棋盘格子总数
- `int operator[](int index) const` - 获取指定位置的格子值
- `const std::vector<int>& getValues() const` - 获取所有格子值
- `void setValue(int index, int value)` - 设置格子值

**使用示例**:
```cpp
// 创建3x3状态
PuzzleState state(3);
state.setValue(0, 1);
state.setValue(1, 2);
state.setValue(2, 3);
// 设置更多格子...

// 访问格子值
int value = state[0]; // 获取第一个格子的值
```

### TreeNode 结构体

**文件**: `src/Core/TreeNode.h`

TreeNode表示搜索树中的一个节点，包含状态信息和搜索代价。

**成员变量**:
- `int index` - 节点在解决方案中的索引
- `PuzzleState state` - 节点对应的拼图状态
- `int g, h, f` - 实际代价、启发式代价、总代价
- `TreeNode *parent` - 父节点指针
- `std::vector<TreeNode *> children` - 子节点列表
- `sf::Vector2f position` - 可视化布局位置

**主要方法**:
- `bool isRoot() const` - 判断是否为根节点
- `bool isLeaf() const` - 判断是否为叶子节点
- `int getDepth() const` - 获取节点深度
- `std::vector<int> getPathToRoot() const` - 获取到根节点的路径

### Tree 类

**文件**: `src/Core/TreeNode.h`

Tree管理整个搜索树结构。

**主要方法**:
- `void setRoot(TreeNode *node)` - 设置根节点
- `TreeNode *getRoot() const` - 获取根节点
- `TreeNode *getNodeByIndex(int index) const` - 根据索引获取节点
- `const std::vector<TreeNode *> &getAllNodes() const` - 获取所有节点
- `size_t size() const` - 获取节点总数
- `int getMaxDepth() const` - 获取树的最大深度

## 解析器模块

### ISolution 接口

**文件**: `src/Parser/Solution.h`

解决方案的抽象接口，定义了树形可视化和动画可视化共有的方法。

**主要方法**:
- `const PuzzleState &getState(int index) const` - 获取指定索引的状态
- `int getParent(int index) const` - 获取父节点索引
- `int getG(int index) const` - 获取实际代价
- `int getH(int index) const` - 获取启发式代价
- `int getF(int index) const` - 获取总代价
- `size_t size() const` - 获取状态总数
- `std::vector<int> getPathToRoot(int index) const` - 获取到根节点的路径

### SolutionTree 类

**文件**: `src/Parser/Solution.h`

用于树形可视化模式的解决方案类。

**构造函数**:
```cpp
SolutionTree(const std::string &filepath)  // 从文件加载树形解决方案
```

**数据格式**:
从`solution.txt`文件加载，格式为：
```
节点编号 父节点编号 g值 h值 f值 状态数组
```

### SolutionAnimation 类

**文件**: `src/Parser/Solution.h`

用于动画可视化模式的解决方案类。

**构造函数**:
```cpp
SolutionAnimation(const std::string &filepath)  // 从文件加载动画解决方案
```

**额外方法**:
- `const std::string &getAction(int index) const` - 获取指定索引的动作

**数据格式**:
从`solutionAnimation.txt`文件加载，包含移动方向序列：
```
UP DOWN LEFT RIGHT
```

## 可视化渲染组件

### BoardRenderer 类

**文件**: `src/Visual/BoardRenderer.h`

负责渲染Puzzle棋盘和代价信息。

**构造函数**:
```cpp
BoardRenderer()  // 创建棋盘渲染器
```

**主要方法**:
- `void setBoardSize(int size, float cellSize = 80.0f)` - 设置棋盘大小和格子尺寸
- `void setPosition(float x, float y)` - 设置渲染位置
- `void setValues(int g, int h, int f)` - 设置显示的代价数值
- `void setShowValuePanel(bool show)` - 设置是否显示代价面板
- `void setHighlightBorder(bool highlight)` - 设置是否显示高亮边框
- `void draw(sf::RenderWindow &window, const PuzzleState &state) const` - 绘制棋盘
- `sf::Vector2f getTotalSize() const` - 获取整体尺寸

**使用示例**:
```cpp
BoardRenderer renderer;
renderer.setBoardSize(3, 80.0f);  // 3x3棋盘，格子大小80像素
renderer.setPosition(100, 100);
renderer.setValues(5, 3, 8);      // 显示g=5, h=3, f=8
renderer.setShowValuePanel(true);
renderer.draw(window, puzzleState);
```

### DisplayManager 类

**文件**: `src/Visual/DisplayManager.h`

控制节点的显示进度和播放模式。

**构造函数**:
```cpp
DisplayManager()  // 创建显示管理器
```

**主要方法**:
- `void setTotalNodes(size_t totalNodes)` - 设置总节点数
- `void setDisplayMode(DisplayMode mode)` - 设置显示模式
- `void setPlayState(PlayState state)` - 设置播放状态
- `void nextStep()` - 显示下一个节点
- `void previousStep()` - 隐藏最后一个节点
- `void reset()` - 重置显示进度
- `void setPlayInterval(sf::Time interval)` - 设置播放间隔
- `bool isNodeVisible(int nodeIndex) const` - 检查节点是否可见

**枚举类型**:
- `DisplayMode`: Manual(手动), AutoPlay(自动播放)
- `PlayState`: Playing(播放中), Paused(暂停)

**使用示例**:
```cpp
DisplayManager manager;
manager.setTotalNodes(100);
manager.setDisplayMode(DisplayMode::AutoPlay);
manager.setPlayInterval(sf::milliseconds(200)); // 200ms间隔
manager.nextStep(); // 显示下一个节点
```

### CanvasView 类

**文件**: `src/Visual/CanvasView.h`

管理视图的缩放、平移和居中显示。

**主要方法**:
- `void handleEvent(const sf::Event& event)` - 处理视图事件
- `void update()` - 更新视图状态
- `void applyToWindow(sf::RenderWindow& window)` - 应用视图到窗口
- `void setCenter(const sf::Vector2f& center)` - 设置视图中心
- `bool isDragging() const` - 检查是否正在拖拽

### TreeBuilder 类

**文件**: `src/Visual/TreeBuilder.h`

从解决方案数据构建树结构。

**主要方法**:
- `std::unique_ptr<Tree> buildTree(const ISolution& solution)` - 构建树结构
- `std::string getBuildStats() const` - 获取构建统计信息

### TreeLayout 类

**文件**: `src/Visual/TreeLayout.h`

计算树节点的布局位置。

**主要方法**:
- `void setTree(Tree *tree)` - 设置树结构
- `void setLayoutParameters(float hSpacing, float vSpacing, float nodeWidth, float nodeHeight)` - 设置布局参数
- `void calculateLayout()` - 计算布局
- `sf::Vector2f getNodePosition(int index) const` - 获取节点位置
- `sf::Vector2f getTotalSize() const` - 获取整体尺寸

### TreeRenderer 类

**文件**: `src/Visual/TreeRenderer.h`

渲染树结构和节点连接线。

**主要方法**:
- `void setTreeAndLayout(Tree *tree, TreeLayout *layout)` - 设置树和布局
- `void setBoardRenderer(std::unique_ptr<BoardRenderer> renderer)` - 设置棋盘渲染器
- `void setConnectionColor(const sf::Color &color)` - 设置连接线颜色
- `void setHighlightPath(const std::vector<int> &path)` - 设置高亮路径
- `void setDisplayManager(DisplayManager *displayManager)` - 设置显示管理器
- `void draw(sf::RenderWindow &window)` - 绘制树结构

### AnimationRenderer 类

**文件**: `src/Visual/AnimationRenderer.h`

渲染动画移动过程和方向指示。

**主要方法**:
- `void setSolution(const ISolution& solution)` - 设置解决方案
- `void setBoardRenderer(std::unique_ptr<BoardRenderer> boardRenderer)` - 设置棋盘渲染器
- `void nextStep()` - 移动到下一步
- `void previousStep()` - 移动到上一步
- `void reset()` - 重置到初始状态
- `void setShowConnectionLine(bool show)` - 设置是否显示连接线
- `void draw(sf::RenderTarget& target) const` - 绘制动画

## 可视化管理器

### TreeVisualizationManager 类

**文件**: `src/Visual/TreeVisualizationManager.h`

管理树形可视化模式的完整流程。

**构造函数**:
```cpp
TreeVisualizationManager(sf::RenderWindow& window, const sf::Vector2f& windowSize)
```

**主要方法**:
- `void run(const ISolution& solution)` - 运行树可视化
- `bool handleEvent(const sf::Event& event)` - 处理事件
- `void update(sf::Time deltaTime)` - 更新状态
- `void draw()` - 绘制树结构

**交互功能**:
- **B键**: 返回主编辑器
- **H键**: 高亮路径
- **R键**: 重置视图
- **方向键**: 步进/步退
- **空格键**: 切换自动播放
- **鼠标拖拽**: 平移视图
- **鼠标滚轮**: 缩放视图

### AnimationVisualizationManager 类

**文件**: `src/Visual/AnimationVisualizationManager.h`

管理动画可视化模式的完整流程。

**构造函数**:
```cpp
AnimationVisualizationManager(sf::RenderWindow& window, const sf::Vector2f& windowSize)
```

**主要方法**:
- `void run(const ISolution& solution)` - 运行动画可视化
- `bool handleEvent(const sf::Event& event)` - 处理事件
- `void update(sf::Time deltaTime)` - 更新状态
- `void draw()` - 绘制动画

**交互功能**:
- **B键**: 返回主编辑器
- **L键**: 切换连接线显示
- **R键**: 重置动画
- **方向键**: 步进/步退
- **空格键**: 切换自动播放

## 交互管理

### InteractionManager 类

**文件**: `src/Interaction/InteractionManager.h`

管理键盘和鼠标交互命令。

**主要方法**:
- `void handleEvent(const sf::Event& event)` - 处理交互事件
- `void registerKeyCommand(sf::Keyboard::Key key, std::unique_ptr<Command> command)` - 注册键盘命令

## 使用流程示例

### 树形可视化模式

```cpp
// 创建窗口
sf::RenderWindow window(sf::VideoMode(1200, 800), "NPuzzle Tree Visualization");

// 加载解决方案
auto solution = loadSolutionTree("solution.txt");

// 创建可视化管理器
TreeVisualizationManager manager(window, sf::Vector2f(1200, 800));

// 运行可视化
manager.run(*solution);
```

### 动画可视化模式

```cpp
// 创建窗口
sf::RenderWindow window(sf::VideoMode(800, 600), "NPuzzle Animation Visualization");

// 加载解决方案
auto solution = loadSolutionAnimation("solutionAnimation.txt");

// 创建可视化管理器
AnimationVisualizationManager manager(window, sf::Vector2f(800, 600));

// 运行动画
manager.run(*solution);
```

## 配置参数

### 树布局参数

通过`TreeLayout::setLayoutParameters()`设置：
- 水平间距: 250像素（默认）
- 垂直间距: 200像素（默认）
- 节点宽度: 根据BoardRenderer动态调整
- 节点高度: 根据BoardRenderer动态调整

### 动画参数

通过`AnimationVisualizationManager`构造函数设置：
- 自动播放间隔: 500毫秒（默认）
- 移动距离: 150像素（在AnimationRenderer中硬编码）

### 显示参数

通过`DisplayManager::setPlayInterval()`设置：
- 自动播放间隔: 100毫秒（树模式默认），500毫秒（动画模式默认）

## 文件格式说明

### 问题文件 (problem.txt)
```
大小 start状态 goal状态
```
例如: `3 1 2 3 4 5 6 7 8 0 1 2 3 4 5 6 7 8 0`

### 树解决方案 (solution.txt)
```
节点编号 父节点编号 g值 h值 f值 状态数组
```
例如: `-1 -1 0 9 9 1 2 3 4 5 6 7 8 0`

### 动画解决方案 (solutionAnimation.txt)
```
移动方向序列
```
例如: `UP DOWN LEFT RIGHT`
