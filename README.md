# Visulization

## 安装SFML库

1. 下载2.6.1的sfml库，解压到任意目录。
2. 下载对应的编译器mingw，解压到任意目录。
3. 将mingw/bin目录添加到环境变量中。
4. 将CPLUS_INCLUDE_PATH添加到环境变量中，值为sfml/include目录。
5. 将LIBRARY_PATH添加到环境变量中，值为sfml/lib目录。
6. 配置C++属性, 将sfml/lib目录添加到链接器的库目录中, 注意选择对应的编译器版本。
7. 配置task.json, 将-lsfml-graphics -lsfml-window -lsfml-system添加到链接器的输入中。

## 可视化需要的步骤

### 绘制搜索过程

1. 窗口和视图管理:
使用 sf::RenderWindow 作为主窗口\
使用 sf::View 实现平移/缩放(类似虚拟滚动)

2. 节点绘制:
根据`solution.txt`中的搜索过程, 绘制对应的树结构。\
需要先使用\
用 `sf::RectangleShape` 绘制节点边框\
用 `sf::Text + sf::Font` 渲染数字(替代位图字体) PuzzleStateRenderer\
用 `sf::VertexArray 或 sf::RectangleShape` 绘制分隔线
3. 树布局:
实现`Reingold-Tilford algorithm`计算节点坐标 `TreeLayout`\
使用递归遍历树结构
4. 连接线:
用 `sf::VertexArray(sf::Lines)` 绘制父子连接线

#### 绘制树节点

1. 使用`Reingold-Tilford algorithm`来计算节点坐标, 并根据坐标绘制节点边框和数字。
2. 用 `sf::VertexArray(sf::Lines)` 绘制父子连接线, 使用节点坐标计算连接点。
3. 实现按步绘制, 每次只绘制当前步的节点和连接线。
4. 实现节点点击事件, 点击节点时高亮显示, 并显示节点信息。

### 交互功能
1. 点击节点: 点击节点时, 高亮显示该节点, 并显示节点信息(状态、代价函数值)。
2. 平移/缩放: 使用鼠标滚轮或拖动来平移/缩放视图。
3. 重置视图: 点击键盘上的R键, 重置视图到初始状态。
4. 步前/步后: 点击键盘上的方向键, 分别实现一步前/一步后的绘制。


## 功能特性

- 可视化搜索树结构
- 支持平移和缩放视图
- 节点点击交互
- 步进式查看搜索过程

## 项目结构

```
src/
├── Core/           # 核心数据结构
├── Visual/         # 可视化组件
├── Interaction/    # 交互管理
└── Parser/         # 数据解析
```

## 构建说明

项目使用 g++ 构建系统，确保已安装 SFML 库和 MinGW 编译器。

## 使用说明

1. 将问题数据放入 `data/problem.txt`
2. 将解决方案数据放入 `data/solution.txt`
3. 运行/out/Visulization.exe程序查看可视化结果
