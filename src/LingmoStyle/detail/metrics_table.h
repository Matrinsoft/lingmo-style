#pragma once

#include <LingmoTheme/Metrics.h>
#include <LingmoTheme/Theme.h>
#include <LingmoTheme/Types.h>

#include <QStyle>

#include <unordered_map>

namespace Lingmo::Detail {

inline const auto &metricsTable()
{
    static const std::unordered_map<QStyle::PixelMetric, int(*)(const Theme&)> table = {
        // ── Button metrics ────────────────────────────────
        { QStyle::PM_ButtonMargin,           [](const Theme &t) { return std::max(1, qRound(t.metrics().spacingSmall())); } },
        { QStyle::PM_ButtonIconSize,         [](const Theme &t) { return qRound(t.metrics().spacingLarge()) * 2; } },
        { QStyle::PM_ButtonShiftHorizontal,  [](const Theme &)  { return 0; } },
        { QStyle::PM_ButtonShiftVertical,    [](const Theme &)  { return 0; } },

        // ── Frame metrics ─────────────────────────────────
        { QStyle::PM_DefaultFrameWidth,      [](const Theme &)  { return 1; } },
        { QStyle::PM_ComboBoxFrameWidth,     [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },
        { QStyle::PM_SpinBoxFrameWidth,      [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },

        // ── Layout metrics ────────────────────────────────
        { QStyle::PM_LayoutLeftMargin,       [](const Theme &t) { return qRound(t.metrics().spacingMedium()); } },
        { QStyle::PM_LayoutTopMargin,        [](const Theme &t) { return qRound(t.metrics().spacingMedium()); } },
        { QStyle::PM_LayoutRightMargin,      [](const Theme &t) { return qRound(t.metrics().spacingMedium()); } },
        { QStyle::PM_LayoutBottomMargin,     [](const Theme &t) { return qRound(t.metrics().spacingMedium()); } },
        { QStyle::PM_LayoutHorizontalSpacing, [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },
        { QStyle::PM_LayoutVerticalSpacing,  [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },

        // ── Icon metrics ──────────────────────────────────
        { QStyle::PM_ListViewIconSize,       [](const Theme &t) { return qRound(t.metrics().spacingXLarge()) * 2; } },
        { QStyle::PM_IconViewIconSize,       [](const Theme &t) { return qRound(t.metrics().spacingXLarge()) * 3; } },
        { QStyle::PM_SmallIconSize,          [](const Theme &t) { return qRound(t.metrics().spacingLarge()); } },
        { QStyle::PM_ToolBarIconSize,        [](const Theme &t) { return qRound(t.metrics().spacingLarge()) + 2; } },
        { QStyle::PM_LargeIconSize,          [](const Theme &t) { return qRound(t.metrics().spacingXXLarge()) * 2; } },

        // ── Tab bar metrics ───────────────────────────────
        { QStyle::PM_TabBarTabOverlap,       [](const Theme &)  { return 0; } },
        { QStyle::PM_TabBarTabHSpace,        [](const Theme &t) { return qRound(t.metrics().spacingMedium()); } },
        { QStyle::PM_TabBarTabVSpace,        [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },
        { QStyle::PM_TabBarBaseHeight,       [](const Theme &)  { return 0; } },
        { QStyle::PM_TabBarBaseOverlap,      [](const Theme &)  { return 0; } },
        { QStyle::PM_TabBar_ScrollButtonOverlap, [](const Theme &) { return 0; } },
        { QStyle::PM_TabBarTabShiftHorizontal, [](const Theme &) { return 0; } },
        { QStyle::PM_TabBarTabShiftVertical,  [](const Theme &) { return 0; } },

        // ── Scroll bar metrics ────────────────────────────
        { QStyle::PM_ScrollBarExtent,        [](const Theme &t) { return qRound(t.metrics().spacingLarge()) + 4; } },
        { QStyle::PM_ScrollBarSliderMin,     [](const Theme &)  { return 24; } },

        // ── Slider metrics ────────────────────────────────
        { QStyle::PM_SliderThickness,        [](const Theme &t) { return qRound(t.metrics().spacingLarge()); } },
        { QStyle::PM_SliderLength,           [](const Theme &t) { return qRound(t.metrics().spacingLarge()) + 4; } },
        { QStyle::PM_SliderSpaceAvailable,   [](const Theme &)  { return 12; } },
        { QStyle::PM_SliderControlThickness, [](const Theme &t) { return qRound(t.metrics().spacingLarge()); } },

        // ── Title bar metrics ─────────────────────────────
        { QStyle::PM_TitleBarHeight,         [](const Theme &t) { return qRound(t.metrics().spacingXLarge()) + 12; } },
        { QStyle::PM_TitleBarButtonSize,     [](const Theme &t) { return qRound(t.metrics().spacingLarge()) + 4; } },
        { QStyle::PM_TitleBarButtonIconSize, [](const Theme &t) { return qRound(t.metrics().spacingLarge()); } },

        // ── Menu metrics ──────────────────────────────────
        { QStyle::PM_MenuDesktopFrameWidth,  [](const Theme &)  { return 0; } },
        { QStyle::PM_MenuPanelWidth,         [](const Theme &)  { return 1; } },
        { QStyle::PM_MenuHMargin,            [](const Theme &)  { return 0; } },
        { QStyle::PM_MenuVMargin,            [](const Theme &)  { return 0; } },
        { QStyle::PM_MenuButtonIndicator,    [](const Theme &t) { return qRound(t.metrics().spacingMedium()); } },
        { QStyle::PM_MenuBarItemSpacing,     [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },
        { QStyle::PM_MenuBarHMargin,         [](const Theme &)  { return 0; } },
        { QStyle::PM_MenuBarVMargin,         [](const Theme &)  { return 0; } },
        { QStyle::PM_MenuBarPanelWidth,      [](const Theme &)  { return 0; } },

        // ── Tool bar metrics ──────────────────────────────
        { QStyle::PM_ToolBarHandleExtent,    [](const Theme &t) { return qRound(t.metrics().spacingMedium()); } },
        { QStyle::PM_ToolBarFrameWidth,      [](const Theme &)  { return 1; } },
        { QStyle::PM_ToolBarItemMargin,      [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },
        { QStyle::PM_ToolBarItemSpacing,     [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },
        { QStyle::PM_ToolBarSeparatorExtent, [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },

        // ── Dock widget metrics ───────────────────────────
        { QStyle::PM_DockWidgetSeparatorExtent, [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },
        { QStyle::PM_DockWidgetHandleExtent,    [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },
        { QStyle::PM_DockWidgetFrameWidth,      [](const Theme &)  { return 1; } },
        { QStyle::PM_DockWidgetTitleMargin,     [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },

        // ── Splitter metrics ─────────────────────────────
        { QStyle::PM_SplitterWidth,          [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },

        // ── Header metrics ────────────────────────────────
        { QStyle::PM_HeaderMargin,           [](const Theme &t) { return qRound(t.metrics().spacingSmall()); } },
        { QStyle::PM_HeaderGripMargin,       [](const Theme &)  { return 0; } },

        // ── Progress bar metrics ──────────────────────────
        { QStyle::PM_ProgressBarChunkWidth,  [](const Theme &t) { return qRound(t.metrics().spacingMedium()); } },

        // ── Text metrics ──────────────────────────────────
        { QStyle::PM_TextCursorWidth,        [](const Theme &)  { return 2; } },

        // ── Tree / list view metrics ──────────────────────
        { QStyle::PM_TreeViewIndentation,    [](const Theme &t) { return qRound(t.metrics().spacingLarge()); } },

        // ── Focus frame ───────────────────────────────────
        { QStyle::PM_FocusFrameVMargin,      [](const Theme &)  { return 2; } },
        { QStyle::PM_FocusFrameHMargin,      [](const Theme &)  { return 2; } },

        // ── MDI ───────────────────────────────────────────
        { QStyle::PM_MdiSubWindowFrameWidth, [](const Theme &)  { return 4; } },
        { QStyle::PM_MdiSubWindowMinimizedWidth, [](const Theme &) { return 160; } },
    };
    return table;
}

} // namespace Lingmo::Detail
