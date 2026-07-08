#pragma once

#include <LingmoTheme/Theme.h>
#include <LingmoTheme/ColorPalette.h>
#include <LingmoTheme/Metrics.h>

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QWidget>

#include <algorithm>

namespace Lingmo::Detail {

// ── Color state resolution ──────────────────────────────

inline QColor buttonColor(QStyle::State state, const ColorPalette &cp)
{
    if (!(state & QStyle::State_Enabled))
        return desaturate(blend(cp.surface(), cp.background(), 0.4), 0.4);
    if (state & QStyle::State_Sunken)
        return darker(cp.surface(), 0.85);
    if (state & QStyle::State_MouseOver)
        return blend(cp.surface(), cp.highlight(), 0.06);
    return cp.surface();
}

inline QColor buttonBorderColor(QStyle::State state, const ColorPalette &cp)
{
    if (!(state & QStyle::State_Enabled))
        return desaturate(cp.border(), 0.5);
    if (state & QStyle::State_MouseOver)
        return cp.borderHover();
    if (state & QStyle::State_HasFocus)
        return cp.highlight();
    return cp.border();
}

inline QColor indicatorColor(QStyle::State state, const ColorPalette &cp)
{
    if (!(state & QStyle::State_Enabled))
        return desaturate(cp.border(), 0.6);
    if (state & QStyle::State_MouseOver)
        return cp.borderHover();
    if (state & QStyle::State_Sunken)
        return cp.highlight();
    return cp.border();
}

inline QColor fillColor(QStyle::State state, const ColorPalette &cp)
{
    if (!(state & QStyle::State_Enabled))
        return desaturate(blend(cp.highlight(), cp.background(), 0.6), 0.5);
    if (state & QStyle::State_MouseOver)
        return blend(cp.highlight(), cp.background(), 0.15);
    return cp.highlight();
}

// ── Drawing helpers ─────────────────────────────────────

inline QPainterPath roundedRect(const QRectF &r, qreal tl, qreal tr, qreal br, qreal bl)
{
    tl = std::max(0.0, tl); tr = std::max(0.0, tr);
    br = std::max(0.0, br); bl = std::max(0.0, bl);

    QPainterPath path;
    path.moveTo(r.left() + tl, r.top());
    path.lineTo(r.right() - tr, r.top());
    path.quadTo(r.right(), r.top(), r.right(), r.top() + tr);
    path.lineTo(r.right(), r.bottom() - br);
    path.quadTo(r.right(), r.bottom(), r.right() - br, r.bottom());
    path.lineTo(r.left() + bl, r.bottom());
    path.quadTo(r.left(), r.bottom(), r.left(), r.bottom() - bl);
    path.lineTo(r.left(), r.top() + tl);
    path.quadTo(r.left(), r.top(), r.left() + tl, r.top());
    path.closeSubpath();
    return path;
}

inline QPainterPath roundedRect(const QRectF &r, qreal radius)
{
    return roundedRect(r, radius, radius, radius, radius);
}

inline QPainterPath roundedRect(const QRect &r, qreal radius)
{
    return roundedRect(QRectF(r), radius);
}

// ── Button background ───────────────────────────────────

inline void drawButtonBackground(QPainter *p, const QRectF &rect,
                                  QStyle::State state, const ColorPalette &cp,
                                  qreal radius, bool isDefault = false)
{
    p->setRenderHint(QPainter::Antialiasing);

    auto bg = buttonColor(state, cp);
    auto border = buttonBorderColor(state, cp);

    QPainterPath path = roundedRect(rect, radius);
    p->fillPath(path, bg);

    if ((state & QStyle::State_HasFocus) && !(state & QStyle::State_Sunken)) {
        QPen focusPen(cp.highlight(), 2);
        p->setPen(focusPen);
        p->drawPath(roundedRect(rect.adjusted(1, 1, -1, -1), radius - 1));
    } else {
        p->setPen(QPen(border, 1));
        p->drawPath(path);
    }

    if (isDefault) {
        QPen defaultPen(cp.highlight(), 2);
        p->setPen(defaultPen);
        p->drawPath(roundedRect(rect.adjusted(1, 1, -1, -1), radius - 1));
    }
}

// ── Checkbox indicator ──────────────────────────────────

inline void drawCheckBoxIndicator(QPainter *p, const QRect &rect,
                                   QStyle::State state, const ColorPalette &cp, qreal radius)
{
    p->setRenderHint(QPainter::Antialiasing);

    QRectF r = QRectF(rect).adjusted(1.5, 1.5, -1.5, -1.5);
    QPainterPath path = roundedRect(r, radius);

    bool checked = state & QStyle::State_On;
    bool partial = state & QStyle::State_NoChange;
    bool hovered = state & QStyle::State_MouseOver;
    bool enabled = state & QStyle::State_Enabled;

    if (checked || partial) {
        QColor fill = fillColor(state, cp);
        p->fillPath(path, fill);
    } else {
        QColor bg = enabled ? cp.surface() : blend(cp.surface(), cp.background(), 0.3);
        p->fillPath(path, bg);

        QColor border = enabled
            ? (hovered ? cp.borderHover() : cp.border())
            : desaturate(cp.border(), 0.6);
        p->setPen(QPen(border, 1.5));
        p->drawPath(path);
    }

    if (checked) {
        p->setPen(QPen(cp.highlightForeground(), 2));
        qreal s = r.width() * 0.25;
        qreal cx = r.center().x();
        qreal cy = r.center().y();
        QPainterPath check;
        check.moveTo(cx - s * 0.6, cy);
        check.lineTo(cx - s * 0.15, cy + s * 0.5);
        check.lineTo(cx + s * 0.7, cy - s * 0.5);
        p->drawPath(check);
    } else if (partial) {
        qreal s = r.width() * 0.4;
        p->setPen(QPen(cp.highlightForeground(), 2));
        p->drawLine(QPointF(r.center().x() - s, r.center().y()),
                    QPointF(r.center().x() + s, r.center().y()));
    }
}

// ── Radio button indicator ───────────────────────────────

inline void drawRadioButtonIndicator(QPainter *p, const QRect &rect,
                                      QStyle::State state, const ColorPalette &cp)
{
    p->setRenderHint(QPainter::Antialiasing);

    QRectF r = QRectF(rect).adjusted(2, 2, -2, -2);
    QPointF center = r.center();
    qreal outerR = r.width() * 0.5;

    bool checked = state & QStyle::State_On;
    bool hovered = state & QStyle::State_MouseOver;
    bool enabled = state & QStyle::State_Enabled;

    if (checked) {
        QColor fill = fillColor(state, cp);
        p->setBrush(fill);
        p->setPen(Qt::NoPen);
        p->drawEllipse(center, outerR, outerR);

        p->setBrush(cp.highlightForeground());
        p->drawEllipse(center, outerR * 0.35, outerR * 0.35);
    } else {
        QColor bg = enabled ? cp.surface() : blend(cp.surface(), cp.background(), 0.3);
        p->setBrush(bg);
        QColor border = enabled
            ? (hovered ? cp.borderHover() : cp.border())
            : desaturate(cp.border(), 0.6);
        p->setPen(QPen(border, 1.5));
        p->drawEllipse(center, outerR, outerR);
    }
}

// ── Focus ring ───────────────────────────────────────────

inline void drawFocusRing(QPainter *p, const QRectF &rect, qreal radius,
                           const QColor &color)
{
    p->setRenderHint(QPainter::Antialiasing);
    QPen pen(color, 2);
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);
    p->drawPath(roundedRect(rect.adjusted(1, 1, -1, -1), radius - 1));
}

// ── Scrollbar handle ─────────────────────────────────────

inline void drawScrollBarHandle(QPainter *p, const QRectF &rect,
                                 Qt::Orientation orientation, const ColorPalette &cp)
{
    p->setRenderHint(QPainter::Antialiasing);

    qreal thickness = std::min(rect.width(), rect.height());
    qreal handleSize = std::max(6.0, thickness * 0.4);

    QRectF handleRect;
    if (orientation == Qt::Horizontal) {
        qreal h = handleSize;
        qreal y = rect.center().y() - h * 0.5;
        handleRect = QRectF(rect.left(), y, rect.width(), h);
    } else {
        qreal w = handleSize;
        qreal x = rect.center().x() - w * 0.5;
        handleRect = QRectF(x, rect.top(), w, rect.height());
    }

    QPainterPath path = roundedRect(handleRect, handleSize * 0.5);
    p->fillPath(path, cp.border());
}

// ── Slider handle ────────────────────────────────────────

inline void drawSliderHandle(QPainter *p, const QPointF &center, qreal radius,
                              QStyle::State state, const ColorPalette &cp)
{
    p->setRenderHint(QPainter::Antialiasing);

    bool enabled = state & QStyle::State_Enabled;
    bool hovered = state & QStyle::State_MouseOver;

    QColor fill = enabled ? cp.highlight() : desaturate(cp.highlight(), 0.5);
    if (hovered && enabled)
        fill = lighter(fill, 1.1);

    p->setBrush(fill);
    p->setPen(QPen(darker(fill, 0.8), 1));
    p->drawEllipse(center, radius, radius);

    p->setBrush(cp.highlightForeground());
    p->setPen(Qt::NoPen);
    p->drawEllipse(center, radius * 0.3, radius * 0.3);
}

// ── Progress bar ─────────────────────────────────────────

inline void drawProgressBarGroove(QPainter *p, const QRectF &rect, const ColorPalette &cp)
{
    p->setRenderHint(QPainter::Antialiasing);
    QPainterPath path = roundedRect(rect, rect.height() * 0.5);
    p->fillPath(path, cp.surfaceVariant());
}

inline void drawProgressBarFill(QPainter *p, const QRectF &rect, qreal progress,
                                 const ColorPalette &cp)
{
    if (progress <= 0.0)
        return;

    p->setRenderHint(QPainter::Antialiasing);
    QRectF filled = rect.adjusted(0, 0, 0, 0);
    filled.setWidth(rect.width() * std::min(1.0, progress));

    QPainterPath path = roundedRect(filled, filled.height() * 0.5);
    p->fillPath(path, cp.highlight());
}

// ── Selection background ─────────────────────────────────

inline void drawSelectionBackground(QPainter *p, const QRectF &rect,
                                     bool active, const ColorPalette &cp)
{
    p->setRenderHint(QPainter::Antialiasing);
    QColor sel = active ? cp.highlight() : cp.surfaceVariant();
    p->fillPath(roundedRect(rect, 3), sel);
}

// ── Menu item background ─────────────────────────────────

inline void drawMenuItemBackground(QPainter *p, const QRectF &rect,
                                    QStyle::State state, const ColorPalette &cp)
{
    if (!(state & QStyle::State_Selected))
        return;

    p->setRenderHint(QPainter::Antialiasing);
    QColor bg = (state & QStyle::State_Enabled)
        ? cp.highlight() : cp.surfaceVariant();
    p->fillPath(roundedRect(rect, 3), bg);
}

// ── Header section ───────────────────────────────────────

inline void drawHeaderSection(QPainter *p, const QRectF &rect, const ColorPalette &cp)
{
    p->setRenderHint(QPainter::Antialiasing);
    p->fillRect(rect, cp.surfaceVariant());
    p->setPen(QPen(cp.border(), 1));
    p->drawLine(QPointF(rect.right(), rect.top()), QPointF(rect.right(), rect.bottom()));
}

// ── Tab bar tab ──────────────────────────────────────────

inline void drawTabBackground(QPainter *p, const QRectF &rect, bool selected,
                               const ColorPalette &cp, qreal radius)
{
    p->setRenderHint(QPainter::Antialiasing);
    if (selected) {
        p->fillPath(roundedRect(rect, radius), cp.surface());
        p->setPen(QPen(cp.border(), 1));
        p->drawPath(roundedRect(rect, radius));
    } else {
        QColor bg = blend(cp.surface(), cp.background(), 0.3);
        p->fillPath(roundedRect(rect, radius), bg);
    }
}

// ── ToolTip / Popup background ───────────────────────────

inline void drawPopupBackground(QPainter *p, const QRectF &rect, qreal radius,
                                 const ColorPalette &cp)
{
    p->setRenderHint(QPainter::Antialiasing);
    p->fillPath(roundedRect(rect, radius), cp.surface());
    p->setPen(QPen(cp.border(), 1));
    p->drawPath(roundedRect(rect, radius));
}

} // namespace Lingmo::Detail
