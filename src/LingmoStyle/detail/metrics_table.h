#pragma once

#include <LingmoTheme/Metrics.h>
#include <LingmoTheme/Theme.h>

#include <QStyle>

#include <unordered_map>

namespace Lingmo::Detail {

inline int spacingToPixelMetric(const Theme &theme, const char *metricName, int defaultValue)
{
    auto metrics = theme.metrics();
    return metrics.spacing.value_or(defaultValue);
}

inline const std::unordered_map<QStyle::PixelMetric, int(*)(const Theme&)> &metricsTable()
{
    static const std::unordered_map<QStyle::PixelMetric, int(*)(const Theme&)> table = {
        { QStyle::PM_ButtonMargin,          [](const Theme &t) { return t.metrics().spacing.value_or(4); } },
        { QStyle::PM_ButtonIconSize,         [](const Theme &t) { return t.metrics().iconSize.value_or(16); } },
        { QStyle::PM_DefaultFrameWidth,      [](const Theme &t) { return t.metrics().borderWidth.value_or(1); } },
        { QStyle::PM_LayoutLeftMargin,       [](const Theme &t) { return t.metrics().spacing.value_or(8); } },
        { QStyle::PM_LayoutTopMargin,        [](const Theme &t) { return t.metrics().spacing.value_or(8); } },
        { QStyle::PM_LayoutRightMargin,      [](const Theme &t) { return t.metrics().spacing.value_or(8); } },
        { QStyle::PM_LayoutBottomMargin,     [](const Theme &t) { return t.metrics().spacing.value_or(8); } },
        { QStyle::PM_LayoutHorizontalSpacing, [](const Theme &t) { return t.metrics().spacing.value_or(4); } },
        { QStyle::PM_LayoutVerticalSpacing,  [](const Theme &t) { return t.metrics().spacing.value_or(4); } },
        { QStyle::PM_ListViewIconSize,       [](const Theme &t) { return t.metrics().iconSize.value_or(32); } },
        { QStyle::PM_ToolBarIconSize,        [](const Theme &t) { return t.metrics().iconSize.value_or(24); } },
        { QStyle::PM_ScrollBarExtent,        [](const Theme &t) { return t.metrics().spacing.value_or(12); } },
        { QStyle::PM_TabBarTabOverlap,       [](const Theme &t) { return 0; } },
        { QStyle::PM_TabBarTabHSpace,        [](const Theme &t) { return t.metrics().spacing.value_or(12); } },
        { QStyle::PM_TabBarTabVSpace,        [](const Theme &t) { return t.metrics().spacing.value_or(6); } },
    };
    return table;
}

} // namespace Lingmo::Detail
