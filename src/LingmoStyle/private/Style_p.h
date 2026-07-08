#pragma once

#include <LingmoTheme/Theme.h>

#include <QPalette>
#include <QPointer>
#include <QMetaObject>

namespace Lingmo {

class StylePrivate
{
public:
    explicit StylePrivate(Style *qq, Theme *theme);
    ~StylePrivate();

    void updateThemeConnection();
    void rebuildPalette();
    void connectThemeSignals();

    Style *q;

    QPointer<Theme> theme;
    QPalette palette;
    bool paletteValid = false;

    QMetaObject::Connection themeDestroyConnection;
    QMetaObject::Connection themeChangeConnection;
    QMetaObject::Connection paletteChangeConnection;
};

} // namespace Lingmo
