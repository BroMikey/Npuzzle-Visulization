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
使用 sf::RenderWindow 作为主窗口
使用 sf::View 实现平移/缩放(类似虚拟滚动)

2. 节点绘制:
根据solution.txt中的搜索过程, 绘制对应的树结构。
需要先使用
用 sf::RectangleShape 绘制节点边框
用 sf::Text + sf::Font 渲染数字(替代位图字体) PuzzleStateRenderer
用 sf::VertexArray 或 sf::RectangleShape 绘制分隔线

1. 树布局:
实现Reingold-Tilford algorithm计算节点坐标 TreeLayout
使用递归遍历树结构

1. 连接线:
用 sf::VertexArray(sf::Lines) 绘制父子连接线

#### 绘制树节点

使用Reingold-Tilford algorithm来计算节点坐标, 并根据坐标绘制节点边框和数字。

#### 绘制连接线

用 sf::VertexArray(sf::Lines) 绘制父子连接线, 使用节点坐标计算连接点。

## 项目规划

1. 先实现core中文件, 后面文件都要基于该文件修改
2. 实现ProblemLoader
3. 实现BoardRenderer简单实现, 先绘制一个初始状态节点
4. 实现TreeLayout, 先绘制一个简单的树结构

### core

1. PuzzleState类

表示一个N-puzzle状态, 包含当前的tiles数组和空位置索引emptyIndex。

2. ProblemLoader类

加载problem.txt文件, 解析出初始状态和目标状态。

3. SolutionLoader类

加载solution.txt文件, 解析出搜索过程中的状态序列。 

难点是根据当前状态生成, **解析当前状态的父节点**
