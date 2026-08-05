# Drift — the RSS Reader

A clean, cross-platform desktop RSS/Atom feed reader built with **Qt 6** and **C++20**.

## Features

- **RSS & Atom** — Parses RSS 2.0 and Atom 1.0 feeds with full content extraction
- **Three-pane layout** — Sidebar (feeds) → Article list → Reader, with resizable split views
- **Multiple views** — Filter articles by *All*, *Unread*, *Starred*, or *Today*
- **Offline reading** — Content caching lets you read articles without a network connection
- **OPML import/export** — Migrate your subscriptions to and from other feed readers
- **Markdown reader** — Clean, distraction-free reading experience with rendered Markdown
- **Search** — Full-text search across article titles and summaries
- **Favicon fetching** — Automatically retrieves and displays feed icons
- **Theme support** — Light, dark, and system-following themes via Qt Material
- **Responsive design** — Adapts to narrow windows (minimum 500×400)
- **Cross-platform** — Windows (x86_64 & ARM64), macOS, Android, iOS
- **In-app browser** — Optional Qt WebView integration for opening original articles

## Tech Stack

| Layer | Technology |
|-------|-----------|
| Language | C++20 |
| UI | Qt 6 QML (Quick Controls 2, Material style) |
| Build | CMake 3.25+ |
| Storage | JSON files (no database server needed) |
| Networking | Qt Network + Qt Concurrent |
| Testing | Qt Test |

## Project Structure

```
Drift/
├── src/
│   ├── core/                    # Shared library (libDriftCore)
│   │   ├── domain/              # Data types: Feed, Article, Category
│   │   ├── feed/                # Feed fetching, parsing, favicons
│   │   ├── storage/             # JSON persistence, OPML, settings
│   │   ├── cache/               # Content caching, HTML conversion, offline
│   │   └── models/              # Qt item models (FeedTreeModel, ArticleListModel)
│   ├── app/                     # Application entry point + QML UI
│   │   ├── qml/                 # QML views organized by area
│   │   │   ├── Sidebar/         # Feed tree, view chips
│   │   │   ├── ArticleList/     # Article list, search, delegate
│   │   │   ├── Reader/          # Article reader (Markdown + WebView)
│   │   │   ├── Dialogs/         # Add feed, OPML, settings, about
│   │   │   └── components/      # Reusable components
│   │   └── android/             # Android manifest
│   └── platform/                # Per-platform helpers
└── tests/                       # Unit tests (feed parser, OPML, storage, HTML)
```

## Building

### Prerequisites

- **Qt 6.5+** with modules: `Core`, `Gui`, `Qml`, `Quick`, `QuickControls2`, `QuickLayouts`, `Network`, `Concurrent`
- **CMake 3.25+**
- A C++20 compiler (MSVC 2022+, GCC 13+, Clang 16+)

Optional:
- `Qt WebView` — enables the in-app browser pane (falls back to external browser if absent)

### Build Steps

```bash
# Clone the repository
git clone <repo-url>
cd "Drift the RSS Reader"

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Run
./build/drift        # or build\drift.exe on Windows

# Run tests (optional)
cmake -B build -DBUILD_TESTING=ON
cmake --build build
cd build && ctest
```

## Usage

1. **Add a feed** — Click the `+` button in the sidebar and paste an RSS/Atom URL
2. **Browse articles** — Select a feed (or *All Unread*) from the sidebar to see its articles
3. **Read** — Click any article to open it in the reader pane; content renders as Markdown
4. **Star** — Click the star icon to bookmark articles for later
5. **Mark read/unread** — Use the toolbar or right-click to manage read status
6. **Search** — Type in the search field to filter articles by keyword
7. **OPML** — Use *File → Import/Export OPML* to move subscriptions between readers

### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `J` / `↓` | Next article |
| `K` / `↑` | Previous article |
| `M` | Toggle read/unread |
| `S` | Toggle star |
| `V` | Open original in browser |
| `R` | Refresh current feed |
| `Shift+R` | Refresh all feeds |

## Architecture

Drift follows a **service-oriented** architecture inside a single process:

```
┌──────────────────────────────────────────────┐
│                  QML UI Layer                 │
│     MainWindow → Sidebar / ArticleList /      │
│     Reader / Dialogs                          │
└──────────────────┬───────────────────────────┘
                   │  DriftController (QML facade)
┌──────────────────▼───────────────────────────┐
│              Service Layer (C++)              │
│  FeedFetcher  ArticleStore  ContentCache     │
│  FeedParser   SettingsStore  HtmlConverter   │
│  FaviconFetcher  SubscriptionStore           │
│  OpmlService    OfflineManager               │
└──────────────────┬───────────────────────────┘
                   │
┌──────────────────▼───────────────────────────┐
│              Data Layer                       │
│  Feed / Article / Category (JSON on disk)     │
└──────────────────────────────────────────────┘
```

- **`DriftController`** is the single QML-facing facade — all UI interactions go through it
- **Models** (`FeedTreeModel`, `ArticleListModel`) expose data to QML views reactively
- **Services** are plain C++ classes owned by the controller
- **Storage** uses JSON files under `QStandardPaths::AppDataLocation`

## Configuration

Settings are persisted automatically and include:

- Window size and position
- Theme preference (light / dark / system)
- Feed refresh interval
- Read/unread state per article
- Starred articles

All data lives in the platform's standard app data directory — no cloud account required.

## License

MIT

---

*Drift — read less, know more.*
