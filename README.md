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

## 项目架构分析与改进建议

### 当前架构分析

**优点：**
- 模块化设计清晰（Core、Parser、Visual）
- 基础功能实现完整
- 代码结构相对清晰

**存在的问题：**
1. **TreeLayout职责过重**：同时负责布局计算、渲染、数据结构管理
2. **缺乏明确的树结构抽象**：直接操作Solution索引，代码复杂
3. **布局算法效率较低**：每次都需要重新计算子树信息
4. **缺乏视图管理**：没有缩放、平移功能

### 架构改进方案

#### 1. 引入TreeNode定义（高优先级）

**问题：** 当前TreeLayout直接操作Solution索引，导致：
- 每次计算都需要遍历整个Solution数组查找子节点
- 时间复杂度高，代码难以维护
- 缺乏明确的树结构抽象

**解决方案：** 创建 `src/Core/TreeNode.h/cpp`
```cpp
struct TreeNode {
    int index;           // 在Solution中的索引
    PuzzleState state;   // 状态数据
    int g, h, f;        // 代价函数值
    TreeNode* parent;    // 父节点指针
    std::vector<TreeNode*> children; // 子节点列表
    sf::Vector2f position; // 布局位置
};
```

#### 2. 重构项目结构

**推荐的新结构：**
```
src/
├── Core/
│   ├── PuzzleState.h/cpp
│   └── TreeNode.h/cpp          # 新增：树节点定义
├── Parser/
│   ├── ProblemLoader.h/cpp
│   └── SolutionLoader.h/cpp
├── Visual/
│   ├── TreeBuilder.h/cpp       # 新增：从Solution构建树结构
│   ├── TreeLayout.h/cpp        # 重构：专注于布局算法
│   ├── TreeRenderer.h/cpp      # 新增：负责渲染逻辑
│   ├── CanvasView.h/cpp        # 新增：视图管理（缩放、平移）
│   └── BoardRenderer.h/cpp
└── main.cpp
```

#### 3. 布局算法优化

**当前算法问题：**
- 简化的Reingold-Tilford算法，效率较低
- 每次布局都需要重新计算子树宽度
- 缺乏对大型树的优化

**改进方案：**

**方案A：优化现有算法**
- 预计算子树信息，避免重复计算
- 添加缓存机制
- 实现增量更新

**方案B：采用更高效的布局算法（推荐）**
- **Buchheim算法**：时间复杂度O(n)，适合大型树
- **Walker算法**：适合大型树的布局
- **分层布局**：对于搜索树特别有效

**推荐实现Buchheim算法：**
```cpp
class BuchheimTreeLayout {
    // 实现Buchheim的树布局算法
    // 时间复杂度O(n)，适合大型搜索树
};
```

## 项目规划

### 当前阶段
1. ✅ 实现Core模块（PuzzleState）
2. ✅ 实现ProblemLoader
3. ✅ 实现BoardRenderer
4. ✅ 实现TreeLayout基础版本

### 下一阶段改进计划

#### 高优先级
1. **创建TreeNode定义** - 建立明确的树结构抽象
2. **重构TreeBuilder** - 从Solution构建树结构
3. **重构TreeLayout** - 专注于布局算法
4. **创建TreeRenderer** - 分离渲染逻辑

#### 中优先级
5. **添加CanvasView** - 实现缩放平移功能
6. **优化布局算法** - 实现Buchheim算法

#### 低优先级
7. **性能优化** - 虚拟化渲染、渐进式布局
8. **交互功能** - 节点选择、路径高亮

### 具体实施步骤

#### 第一步：创建TreeNode定义
- 定义TreeNode结构体
- 实现树节点基本操作
- 更新SolutionLoader以支持树构建

#### 第二步：重构TreeBuilder
- 从Solution构建完整的树结构
- 提供树遍历接口
- 分离数据结构与布局逻辑

#### 第三步：重构TreeLayout
- 专注于布局算法实现
- 支持多种布局算法
- 添加布局参数配置

#### 第四步：创建TreeRenderer
- 负责所有渲染逻辑
- 支持节点样式定制
- 实现连接线渲染

#### 第五步：添加CanvasView
- 实现视图缩放功能
- 实现视图平移功能
- 添加视图状态管理

### 性能优化建议

对于大型搜索树（>1000节点），考虑：
- **虚拟化渲染**：只渲染可见区域
- **渐进式布局计算**：分步计算布局
- **多线程布局计算**：利用多核CPU
- **布局缓存**：避免重复计算

### 扩展功能建议

1. **交互功能**
   - 节点点击选择
   - 路径高亮显示
   - 搜索过程动画

2. **可视化增强**
   - 不同搜索算法的颜色编码
   - 节点大小根据启发值变化
   - 搜索过程回放

3. **导出功能**
   - 截图保存
   - 布局数据导出
   - 性能分析报告

## 技术要点

### 树布局算法对比

| 算法             | 时间复杂度 | 空间复杂度 | 适用场景   |
| ---------------- | ---------- | ---------- | ---------- |
| Reingold-Tilford | O(n²)      | O(n)       | 小型树     |
| Buchheim         | O(n)       | O(n)       | 大型树     |
| Walker           | O(n)       | O(n)       | 大型平衡树 |
| 分层布局         | O(n)       | O(n)       | 搜索树     |

### 数据结构设计原则

1. **单一职责**：每个类只负责一个明确的功能
2. **接口分离**：模块间通过清晰接口通信
3. **数据驱动**：布局与渲染分离
4. **性能优先**：针对搜索树特点优化

## 总结

通过引入TreeNode定义和重构模块职责，可以显著提高代码的可维护性和性能。建议优先实现TreeNode定义和TreeBuilder模块，为后续功能扩展奠定坚实基础。

TODO: 阅读代码，理解以及修改TreeLayout类的布局算法。同时修改拖拽实现逻辑