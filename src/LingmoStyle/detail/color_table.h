#pragma once

#include <LingmoTheme/ColorPalette.h>
#include <LingmoTheme/Types.h>

#include <QPalette>
#include <QColor>

#include <algorithm>

namespace Lingmo::Detail {

// ── Color utilities ─────────────────────────────────────

inline qreal srgbChannelToLinear(qreal c)
{
    return c <= 0.04045 ? c / 12.92 : std::pow((c + 0.055) / 1.055, 2.4);
}

inline qreal linearChannelToSrgb(qreal c)
{
    return c <= 0.0031308 ? 12.92 * c : 1.055 * std::pow(c, 1.0 / 2.4) - 0.055;
}

inline qreal relativeLuminance(const QColor &c)
{
    return 0.2126 * srgbChannelToLinear(c.redF())
         + 0.7152 * srgbChannelToLinear(c.greenF())
         + 0.0722 * srgbChannelToLinear(c.blueF());
}

inline QColor blend(const QColor &a, const QColor &b, qreal t)
{
    t = std::clamp(t, 0.0, 1.0);
    return QColor::fromRgbF(
        a.redF()   * (1 - t) + b.redF()   * t,
        a.greenF() * (1 - t) + b.greenF() * t,
        a.blueF()  * (1 - t) + b.blueF()  * t,
        a.alphaF() * (1 - t) + b.alphaF() * t
    );
}

inline QColor lighter(const QColor &c, qreal factor = 1.3)
{
    return QColor::fromHslF(c.hslHueF(),
        std::max(0.0, c.hslSaturationF() * 0.85),
        std::min(1.0, c.lightnessF() * factor),
        c.alphaF());
}

inline QColor darker(const QColor &c, qreal factor = 0.7)
{
    return QColor::fromHslF(c.hslHueF(),
        std::min(1.0, c.hslSaturationF() * 1.15),
        std::max(0.0, c.lightnessF() * factor),
        c.alphaF());
}

inline QColor desaturate(const QColor &c, qreal amount = 0.5)
{
    return QColor::fromHslF(c.hslHueF(),
        std::max(0.0, c.hslSaturationF() * (1.0 - amount)),
        c.lightnessF(),
        c.alphaF());
}

inline bool isDark(const QColor &c)
{
    return relativeLuminance(c) < 0.2;
}

// ── QPalette generation ─────────────────────────────────

inline void setColorGroup(QPalette &palette, QPalette::ColorGroup group,
                          const ColorPalette &cp, const QColor &bg,
                          const QColor &fg)
{
    auto set = [&](QPalette::ColorRole role, QColor color) {
        palette.setColor(group, role, color);
    };

    // Core roles
    set(QPalette::Window,       bg);
    set(QPalette::WindowText,   fg);
    set(QPalette::Button,       cp.surface());
    set(QPalette::ButtonText,   fg);
    set(QPalette::Base,         cp.surface());
    set(QPalette::Text,         fg);
    set(QPalette::AlternateBase, cp.surfaceVariant());
    set(QPalette::ToolTipBase,  cp.surface());
    set(QPalette::ToolTipText,  fg);

    // Highlight
    set(QPalette::Highlight,         cp.highlight());
    set(QPalette::HighlightedText,   cp.highlightForeground());

    // Links
    set(QPalette::Link,       cp.link());
    set(QPalette::LinkVisited, cp.link());

    // 3D bevel roles (derived from surface)
    set(QPalette::Light,     lighter(cp.surface(), 1.4));
    set(QPalette::Midlight,  lighter(cp.surface(), 1.15));
    set(QPalette::Dark,      darker(cp.surface(), 0.6));
    set(QPalette::Mid,       darker(cp.surface(), 0.75));
    set(QPalette::Shadow,    darker(cp.surface(), 0.3));

    set(QPalette::BrightText, isDark(bg) ? QColor(255, 255, 255) : QColor(0, 0, 0));
}

inline QPalette paletteFromColorSet(const ColorPalette &cp, ColorScheme scheme)
{
    QPalette palette;

    auto bg = cp.background();
    auto fg = cp.foreground();

    // Active
    setColorGroup(palette, QPalette::Active, cp, bg, fg);

    // Inactive (same as active)
    setColorGroup(palette, QPalette::Inactive, cp, bg, fg);

    // Disabled (reduced contrast)
    QColor disabledBg = blend(bg, cp.surface(), 0.5);
    QColor disabledFg = blend(fg, bg, 0.65);

    // For disabled, reduce saturation of highlight
    ColorPalette disabledCp = cp;
    // We can't modify the const ref, so just compute inline
    QColor disabledHighlight = desaturate(blend(cp.highlight(), bg, 0.5), 0.6);
    QColor disabledHighlightFg = blend(cp.highlightForeground(), bg, 0.5);

    palette.setColor(QPalette::Disabled, QPalette::Window,        disabledBg);
    palette.setColor(QPalette::Disabled, QPalette::WindowText,    disabledFg);
    palette.setColor(QPalette::Disabled, QPalette::Button,        disabledBg);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText,    disabledFg);
    palette.setColor(QPalette::Disabled, QPalette::Base,          disabledBg);
    palette.setColor(QPalette::Disabled, QPalette::Text,          disabledFg);
    palette.setColor(QPalette::Disabled, QPalette::AlternateBase, blend(cp.surfaceVariant(), bg, 0.5));
    palette.setColor(QPalette::Disabled, QPalette::ToolTipBase,   disabledBg);
    palette.setColor(QPalette::Disabled, QPalette::ToolTipText,   disabledFg);
    palette.setColor(QPalette::Disabled, QPalette::Highlight,     disabledHighlight);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledHighlightFg);
    palette.setColor(QPalette::Disabled, QPalette::Link,          disabledFg);
    palette.setColor(QPalette::Disabled, QPalette::LinkVisited,   disabledFg);
    palette.setColor(QPalette::Disabled, QPalette::Light,         disabledBg);
    palette.setColor(QPalette::Disabled, QPalette::Midlight,      disabledBg);
    palette.setColor(QPalette::Disabled, QPalette::Dark,          disabledBg);
    palette.setColor(QPalette::Disabled, QPalette::Mid,           disabledBg);
    palette.setColor(QPalette::Disabled, QPalette::Shadow,        disabledBg);
    palette.setColor(QPalette::Disabled, QPalette::BrightText,    disabledFg);

    return palette;
}

} // namespace Lingmo::Detail
