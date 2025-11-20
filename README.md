# Visulization

整合部分在最后

## 安装SFML库

1. 下载2.6.1的sfml库，解压到任意目录。
2. 下载对应的编译器mingw(如2.6.1对应的是mingw64-winlib-13.1.0)，解压到任意目录。
3. 将mingw64-winlib-13.1.0/bin目录添加到环境变量中。
4. 将CPLUS_INCLUDE_PATH添加到环境变量中，值为/path/to/sfml/include目录。
5. 将LIBRARY_PATH添加到环境变量中，值为/path/to/sfml/lib目录。
6. 配置C++属性, 将/path/to/sfml/lib目录添加到链接器的库目录中, 注意选择对应的编译器版本。
7. 配置task.json, 将-lsfml-graphics -lsfml-window -lsfml-system添加到链接器的输入中。

## 可视化

开屏, 可以编辑start state, 显示出默认的goal state. 最下方的两个view按钮分别对应以下两个可视化的功能:

1. 可视化为一棵树(可缩放)
2. 动画可视化搜索(不可缩放, 固定视角大小)

## 可视化为一棵树

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
5. 居中显示: 每次点击显示或取消显示一个节点时, 将该节点居中显示在视图中。


## 动画可视化搜索

只显示一个节点, 但是可以根据solution.txt中的搜索过程, 动态显示搜索过程。(比如已知这一步是DOWN,就显示当前空白节点向下移动)
根据solutionAction.txt中内容(UP/DOWN/LEFT/RIGHT), 一步步显示当前节点移动的方向, 同时有auto模式, 同时可以手动暂停,选择上一步下一步进行播放。

### 绘制搜索动画

1. 读取solutionAction.txt文件, 解析搜索过程。
2. 根据搜索过程, 动态更新当前显示的节点。
3. 实现按步播放, 每次只播放当前步的动画。
4. 手动点击下一步, 播放下一个动画, 自动播放时每播放一步, 暂停0.5秒。


## 功能特性

- 可视化搜索树结构
- 支持平移和缩放视图
- 节点点击交互
- 步进式查看搜索过程
- 动态显示当前节点移动方向
- 自动播放和手动控制
- 居中显示当前节点


## 整合注意事项

### 功能简述

先打开bin/Visualization.exe, 检查是否能正常运行。

功能:

打开页面一共两个Puzzle, 左边是start state(读取./problems.txt中的第一个问题), 右边是goal state(默认的, 不可修改)

按上下左右可以移动空白块, reset按钮可以重置当前start state.

点击save to java会将当前的start state**追加到**保存到resources/problems.txt

点击Decrease会降低当前Puzzle的大小, 但是不能低于3x3, 点击Increase会增加当前Puzzle的大小, 但是不能超过4x4.

点击TreeView会跳转到可视化搜索树结构的页面。跳转后使用**左右方向键**即可查看下一步, 上一步,使用**a键**可以自动播放(再次点击a键可以暂停自动播放).同时可以使用鼠标中键进行拖拽视图, 缩放视图。

点击AnimationView会跳转到动画可视化搜索的页面。跳转后使用**左右方向键**可以查看下一步, 上一步, 也可以点击**空格**按钮自动播放(不是a键)。

进入TreeView和AnimationView后, 暂停自动播放情况下, **按b键**返回到主页面。


### 整合需要修改部分

1. 问题文件:在bin(在三阶段根目录下)的problem.txt(这里不带复数), 保证每次运行时problem.txt中只有一个问题.(格式: `大小 start的状态字符串 goal的状态字符串`(一般都是默认的从1到n^2-1,0表示空白格), 使用空格隔开)
2. 输出:
   - 搜索树结构: 输出到solution.txt
      - 格式: 每个节点占一行, 格式为: `节点编号(首节点为-1) 状态字符串 p(父节点编号) g(到根节点的代价) h(启发式函数值) f(总代价) stateArray`(空格隔开), 也可也将g, h, f都默认为1(一开始的显示功能我给删了, 但是格式没改, 为了方便, 我就保留了g, h, f)
   - 搜索过程: 输出到solutionAction.txt(不过是基于problem.txt中的start state开始的)
      - 格式: 每个节点占一行, 格式为: `DOWN UP LEFT RIGHT`, 表示解决方案的四个移动方向(空格隔开)