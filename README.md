# Drift — RSS 阅读器

一个简洁、跨平台的桌面 RSS/Atom  feed 阅读器，使用 **Qt 6** 和 **C++20** 构建。

## 功能特性

- **RSS & Atom** — 解析 RSS 2.0 和 Atom 1.0 feed，支持完整内容提取
- **三栏布局** — 侧边栏（feed 列表）→ 文章列表 → 阅读区，支持可拖拽调整的分割视图
- **多种视图** — 按 *全部*、*未读*、*星标* 或 *今日* 筛选文章
- **离线阅读** — 内容缓存让你无需网络连接也能阅读文章
- **OPML 导入/导出** — 与其他 feed 阅读器之间迁移订阅源
- **Markdown 阅读** — 简洁、无干扰的阅读体验，支持 Markdown 渲染
- **搜索** — 对文章标题和摘要进行全文搜索
- **网站图标获取** — 自动获取并显示 feed 图标
- **主题支持** — 通过 Qt Material 支持浅色、深色和跟随系统主题
- **响应式设计** — 适配窄窗口（最低 500×400）
- **跨平台** — Windows（x86_64 & ARM64）、macOS、Android、iOS
- **应用内浏览器** — 可选的 Qt WebView 集成，用于打开原文链接

## 技术栈

| 层级 | 技术 |
|------|------|
| 语言 | C++20 |
| UI | Qt 6 QML（Quick Controls 2，Material 风格） |
| 构建 | CMake 3.25+ |
| 存储 | JSON 文件（无需数据库服务器） |
| 网络 | Qt Network + Qt Concurrent |
| 测试 | Qt Test |

## 项目结构

```
Drift/
├── src/
│   ├── core/                    # 共享库（libDriftCore）
│   │   ├── domain/              # 数据类型：Feed、Article、Category
│   │   ├── feed/                # Feed 获取、解析、图标
│   │   ├── storage/             # JSON 持久化、OPML、设置
│   │   ├── cache/               # 内容缓存、HTML 转换、离线
│   │   └── models/              # Qt 数据模型（FeedTreeModel、ArticleListModel）
│   ├── app/                     # 应用入口 + QML UI
│   │   ├── qml/                 # 按功能区域组织的 QML 视图
│   │   │   ├── Sidebar/         # Feed 树、视图筛选标签
│   │   │   ├── ArticleList/     # 文章列表、搜索、列表项
│   │   │   ├── Reader/          # 文章阅读器（Markdown + WebView）
│   │   │   ├── Dialogs/         # 添加 feed、OPML、设置、关于
│   │   │   └── components/      # 可复用组件
│   │   └── android/             # Android 清单文件
│   └── platform/                # 各平台辅助代码
└── tests/                       # 单元测试（feed 解析、OPML、存储、HTML）
```

## 下载安装

前往 [Releases](https://github.com/Linstone114514/Drift-the-RSS-Reader/releases) 页面，下载最新版的 `Drift-x.y.z-win64-setup.exe`（Windows 64 位安装包），双击运行即可安装。

应用名为 **Drift**，安装后可从开始菜单启动。安装包自带全部 Qt 运行库，无需另外安装 Qt。

## 构建

### 前置要求

- **Qt 6.5+** 及以下模块：`Core`、`Gui`、`Qml`、`Quick`、`QuickControls2`、`QuickLayouts`、`Network`、`Concurrent`
- **CMake 3.25+**
- 支持 C++20 的编译器（MSVC 2022+、GCC 13+、Clang 16+）

可选：
- `Qt WebView` — 启用应用内浏览器面板（缺失时回退到外部浏览器）

### 构建步骤

```bash
# 克隆仓库
git clone <repo-url>
cd "Drift the RSS Reader"

# 配置
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 构建
cmake --build build --config Release

# 运行
./build/drift        # Windows 上使用 build\drift.exe

# 运行测试（可选）
cmake -B build -DBUILD_TESTING=ON
cmake --build build
cd build && ctest
```

## 使用指南

1. **添加 feed** — 点击侧边栏的 `+` 按钮，粘贴 RSS/Atom 链接
2. **浏览文章** — 从侧边栏选择一个 feed（或 *全部未读*）查看其文章
3. **阅读** — 点击任意文章在阅读区打开；内容以 Markdown 格式渲染
4. **星标** — 点击星标图标收藏文章，方便稍后阅读
5. **标记已读/未读** — 使用工具栏或右键菜单管理阅读状态
6. **搜索** — 在搜索框中输入关键词筛选文章
7. **OPML** — 使用 *文件 → 导入/导出 OPML* 在不同阅读器之间迁移订阅

### 键盘快捷键

| 按键 | 操作 |
|------|------|
| `J` / `↓` | 下一篇文章 |
| `K` / `↑` | 上一篇文章 |
| `M` | 切换已读/未读 |
| `S` | 切换星标 |
| `V` | 在浏览器中打开原文 |
| `R` | 刷新当前 feed |
| `Shift+R` | 刷新全部 feed |

## 架构

Drift 在单进程内采用**面向服务**的架构：

```
┌──────────────────────────────────────────────┐
│                  QML UI 层                   │
│     MainWindow → Sidebar / ArticleList /     │
│     Reader / Dialogs                         │
└──────────────────┬───────────────────────────┘
                   │  DriftController（QML 外观层）
┌──────────────────▼───────────────────────────┐
│              服务层（C++）                    │
│  FeedFetcher  ArticleStore  ContentCache     │
│  FeedParser   SettingsStore  HtmlConverter   │
│  FaviconFetcher  SubscriptionStore           │
│  OpmlService    OfflineManager               │
└──────────────────┬───────────────────────────┘
                   │
┌──────────────────▼───────────────────────────┐
│              数据层                           │
│  Feed / Article / Category（磁盘 JSON 存储）  │
└──────────────────────────────────────────────┘
```

- **`DriftController`** 是唯一的 QML 外观层 —— 所有 UI 交互都通过它进行
- **数据模型**（`FeedTreeModel`、`ArticleListModel`）以响应式方式向 QML 视图暴露数据
- **服务**是纯 C++ 类，由控制器持有
- **存储**使用 `QStandardPaths::AppDataLocation` 下的 JSON 文件

## 配置

设置会自动持久化，包括：

- 窗口大小和位置
- 主题偏好（浅色 / 深色 / 跟随系统）
- Feed 刷新间隔
- 每篇文章的已读/未读状态
- 星标文章

所有数据存储在平台的标准应用数据目录中 —— 无需云端账号。

## 许可证

MIT

---

*Drift — read less, know more.*
