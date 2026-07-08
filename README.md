# lingmo-style

Lingmo Desktop - Qt Widgets Style

[![Build](https://github.com/lingmo-desktop/lingmo-style/actions/workflows/build.yml/badge.svg)](https://github.com/lingmo-desktop/lingmo-style/actions/workflows/build.yml)

Lingmo Style provides a QProxyStyle-based Qt Widgets style implementation for the Lingmo Desktop environment. It reads design tokens from `lingmo-theme` and ensures that Qt Widgets applications share the same visual design as QML applications.

## Dependencies

### Build-time
- CMake >= 3.21
- C++20 compiler (GCC 12+, Clang 16+)
- Qt 6.5+ (Widgets)
- lingmo-theme (Lingmo::Theme)

### Runtime
- Qt 6 Widgets
- lingmo-theme

## Building

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --parallel
```

## Installing

```bash
cmake --install build --prefix /usr
```

## Testing

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Usage

### Via Qt style plugin (recommended)
```cpp
QApplication::setStyle("lingmo");
```

### Via programmatic instantiation
```cpp
#include <LingmoStyle/Style.h>
QApplication::setStyle(new Lingmo::Style);
```

## Contributing

See [Contributing Guide](https://github.com/lingmo-desktop/lingmo-specs/blob/main/CONTRIBUTING.md).

## License

GPL-3.0-or-later

## Links

- [Lingmo Desktop](https://lingmo.org)
- [lingmo-theme](https://github.com/lingmo-desktop/lingmo-theme)
- [Specifications](https://github.com/lingmo-desktop/lingmo-specs)
