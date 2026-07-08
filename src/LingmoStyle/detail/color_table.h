#pragma once

#include <LingmoTheme/ColorPalette.h>

#include <QPalette>

namespace Lingmo::Detail {

inline QPalette paletteFromColorSet(const ColorPalette &palette)
{
    QPalette p;

    p.setColor(QPalette::Window, palette.windowBackground);
    p.setColor(QPalette::WindowText, palette.windowForeground);
    p.setColor(QPalette::Button, palette.buttonBackground);
    p.setColor(QPalette::ButtonText, palette.buttonForeground);
    p.setColor(QPalette::Base, palette.fieldBackground);
    p.setColor(QPalette::Text, palette.fieldForeground);
    p.setColor(QPalette::Highlight, palette.accent);
    p.setColor(QPalette::HighlightedText, palette.accentText);
    p.setColor(QPalette::Link, palette.link);
    p.setColor(QPalette::LinkVisited, palette.linkVisited);

    // Disabled state: derive from window background
    QColor disabled = palette.windowBackground;
    disabled.setAlpha(128);
    p.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
    p.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
    p.setColor(QPalette::Disabled, QPalette::Text, disabled);

    return p;
}

} // namespace Lingmo::Detail
