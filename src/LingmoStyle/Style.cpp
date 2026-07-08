#include <LingmoStyle/Style.h>

#include <private/Style_p.h>
#include <detail/color_table.h>
#include <detail/metrics_table.h>
#include <detail/drawing_helpers.h>

#include <QApplication>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionFrame>
#include <QStyleOptionMenuItem>
#include <QStyleOptionComboBox>
#include <QStyleOptionSlider>
#include <QStyleOptionSpinBox>
#include <QStyleOptionTab>
#include <QStyleOptionToolButton>
#include <QStyleOptionHeader>
#include <QStyleOptionProgressBar>
#include <QPainter>
#include <QPalette>
#include <QWidget>
#include <QFrame>
#include <QDialogButtonBox>
#include <QAbstractItemView>

namespace Lingmo {

StylePrivate::StylePrivate(Style *qq, Theme *theme)
    : q(qq)
    , theme(theme ? theme : Theme::instance())
{
    connectThemeSignals();
    rebuildPalette();
}

StylePrivate::~StylePrivate()
{
    if (themeDestroyConnection)
        QObject::disconnect(themeDestroyConnection);
    if (themeChangeConnection)
        QObject::disconnect(themeChangeConnection);
    if (paletteChangeConnection)
        QObject::disconnect(paletteChangeConnection);
}

void StylePrivate::connectThemeSignals()
{
    if (!theme)
        return;

    themeDestroyConnection = QObject::connect(theme, &QObject::destroyed, q, [this] {
        theme = nullptr;
        paletteValid = false;
    });

    themeChangeConnection = QObject::connect(theme, &Theme::themeChanged, q, [this] {
        rebuildPalette();
        emit q->themeChanged();
        if (auto *app = qobject_cast<QApplication *>(QCoreApplication::instance())) {
            q->polish(*const_cast<QPalette *>(&app->palette()));
            app->setPalette(palette);
        }
    });

    paletteChangeConnection = QObject::connect(theme, &Theme::paletteChanged, q, [this] {
        rebuildPalette();
        if (auto *app = qobject_cast<QApplication *>(QCoreApplication::instance())) {
            app->setPalette(palette);
        }
    });
}

void StylePrivate::rebuildPalette()
{
    if (theme) {
        palette = Detail::paletteFromColorSet(theme->palette(), theme->colorScheme());
        paletteValid = true;
    } else {
        paletteValid = false;
    }
}

// ── Style implementation ─────────────────────────────────

Style::Style(Theme *theme)
    : QProxyStyle(nullptr)
    , d(std::make_unique<StylePrivate>(this, theme))
{
    if (!d->theme) {
        d->theme = Theme::instance();
        d->connectThemeSignals();
        d->rebuildPalette();
    }
}

Style::~Style() = default;

Theme *Style::theme() const
{
    return d->theme;
}

void Style::setTheme(Theme *newTheme)
{
    if (d->theme == newTheme)
        return;

    d->theme = newTheme;
    d->connectThemeSignals();
    d->rebuildPalette();

    emit themeChanged();
}

// ── Primitives ──────────────────────────────────────────

void Style::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                          QPainter *painter, const QWidget *widget) const
{
    if (!option || !painter)
        return;

    auto cp = d->theme ? d->theme->palette() : ColorPalette{};
    auto cr = d->theme ? d->theme->cornerRadii(0) : CornerRadii{};
    auto crFrame = d->theme ? d->theme->cornerRadii(1) : CornerRadii{};

    switch (element) {
    case PE_PanelButtonCommand: {
        auto *btn = qstyleoption_cast<const QStyleOptionButton *>(option);
        bool isDefault = btn && (btn->features & QStyleOptionButton::DefaultButton);
        Detail::drawButtonBackground(painter, option->rect, option->state, cp, cr.topLeft, isDefault);
        return;
    }
    case PE_Frame:
    case PE_FrameDefaultButton:
        if (option->rect.isValid()) {
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(QPen(cp.border(), 1));
            painter->setBrush(Qt::NoBrush);
            painter->drawPath(Detail::roundedRect(option->rect, cr.topLeft));
        }
        return;
    case PE_FrameLineEdit: {
        auto *f = qstyleoption_cast<const QStyleOptionFrame *>(option);
        qreal r = f && f->lineWidth > 0 ? cr.topLeft : 0;
        painter->setRenderHint(QPainter::Antialiasing);
        painter->fillPath(Detail::roundedRect(option->rect, r), cp.surface());
        painter->setPen(QPen(cp.border(), 1));
        painter->drawPath(Detail::roundedRect(option->rect, r));
        return;
    }
    case PE_PanelLineEdit: {
        auto *f = qstyleoption_cast<const QStyleOptionFrame *>(option);
        qreal r = f && f->lineWidth > 0 ? cr.topLeft : 0;
        bool hasFocus = option->state & State_HasFocus;
        painter->setRenderHint(QPainter::Antialiasing);
        painter->fillPath(Detail::roundedRect(option->rect, r), cp.surface());
        if (hasFocus) {
            painter->setPen(QPen(cp.highlight(), 2));
        } else {
            painter->setPen(QPen(cp.border(), 1));
        }
        painter->drawPath(Detail::roundedRect(option->rect.adjusted(0.5, 0.5, -0.5, -0.5), r));
        return;
    }
    case PE_FrameMenu:
    case PE_PanelMenu:
        Detail::drawPopupBackground(painter, option->rect, cr.topLeft, cp);
        return;
    case PE_PanelTipLabel:
        Detail::drawPopupBackground(painter, option->rect, cr.topLeft, cp);
        return;
    case PE_PanelItemViewItem: {
        bool selected = option->state & State_Selected;
        bool active = option->state & State_Active;
        if (selected || (option->state & State_MouseOver)) {
            Detail::drawSelectionBackground(painter, option->rect, selected && active, cp);
        }
        return;
    }
    case PE_IndicatorCheckBox: {
        Detail::drawCheckBoxIndicator(painter, option->rect, option->state, cp, 3);
        return;
    }
    case PE_IndicatorRadioButton: {
        Detail::drawRadioButtonIndicator(painter, option->rect, option->state, cp);
        return;
    }
    case PE_FrameFocusRect:
        if (option->state & State_HasFocus) {
            Detail::drawFocusRing(painter, option->rect, cr.topLeft, cp.highlight());
        }
        return;
    case PE_IndicatorItemViewItemDrop: {
        painter->setPen(QPen(cp.highlight(), 2));
        if (!option->rect.isNull())
            painter->drawLine(option->rect.topLeft(), option->rect.topRight());
        return;
    }
    case PE_PanelStatusBar:
        painter->fillRect(option->rect, cp.surfaceVariant());
        return;
    case PE_FrameGroupBox: {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(cp.border(), 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(Detail::roundedRect(option->rect, cr.topLeft));
        return;
    }
    case PE_FrameTabWidget: {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(cp.border(), 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(Detail::roundedRect(option->rect, crFrame.topLeft));
        return;
    }
    case PE_IndicatorBranch: {
        int midX = option->rect.center().x();
        int midY = option->rect.center().y();
        int h = option->rect.height();

        painter->setPen(QPen(cp.border(), 1));

        // Vertical line (if has children)
        // Horizontal line to indicator
        painter->drawLine(midX, option->rect.top(), midX, option->rect.bottom());

        // Plus/minus expander
        if (option->state & State_Children) {
            int size = 8;
            QRect expRect(midX - size / 2, midY - size / 2, size, size);
            painter->setRenderHint(QPainter::Antialiasing);
            painter->fillRect(expRect, cp.surface());
            painter->drawRect(expRect);
            painter->drawLine(expRect.left() + 2, midY, expRect.right() - 2, midY);
            if (!(option->state & State_Open))
                painter->drawLine(midX, expRect.top() + 2, midX, expRect.bottom() - 2);
        }
        return;
    }
    default:
        QProxyStyle::drawPrimitive(element, option, painter, widget);
        break;
    }
}

// ── Controls ────────────────────────────────────────────

void Style::drawControl(ControlElement element, const QStyleOption *option,
                        QPainter *painter, const QWidget *widget) const
{
    if (!option || !painter)
        return;

    auto cp = d->theme ? d->theme->palette() : ColorPalette{};
    auto cr = d->theme ? d->theme->cornerRadii(0) : CornerRadii{};

    switch (element) {
    case CE_PushButton: {
        auto *btn = qstyleoption_cast<const QStyleOptionButton *>(option);
        if (btn) {
            bool isDefault = btn->features & QStyleOptionButton::DefaultButton;
            Detail::drawButtonBackground(painter, option->rect, option->state, cp, cr.topLeft, isDefault);
            drawControl(CE_PushButtonLabel, btn, painter, widget);
        }
        return;
    }
    case CE_PushButtonLabel: {
        auto *btn = qstyleoption_cast<const QStyleOptionButton *>(option);
        if (!btn) break;

        QStyleOptionButton sub = *btn;
        sub.rect = proxy()->subElementRect(SE_PushButtonContents, btn, widget);

        bool enabled = btn->state & State_Enabled;
        bool sunken = btn->state & State_Sunken;

        if (sunken)
            sub.rect.translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, btn, widget),
                               proxy()->pixelMetric(PM_ButtonShiftVertical, btn, widget));

        QColor textColor = enabled ? cp.foreground()
            : desaturate(blend(cp.foreground(), cp.background(), 0.5), 0.5);
        sub.palette.setColor(QPalette::ButtonText, textColor);

        proxy()->drawItemText(painter, sub.rect, Qt::AlignCenter | Qt::TextShowMnemonic,
                              sub.palette, enabled, btn->text, QPalette::ButtonText);
        if (btn->icon.isNull())
            break;
        QPixmap pix = btn->icon.pixmap(btn->iconSize, enabled ? QIcon::Normal : QIcon::Disabled);
        QRect iconRect = QRect(sub.rect.topLeft(), btn->iconSize);
        painter->drawPixmap(iconRect, pix);
        return;
    }
    case CE_CheckBox: {
        auto *btn = qstyleoption_cast<const QStyleOptionButton *>(option);
        if (!btn) break;
        drawPrimitive(PE_IndicatorCheckBox, btn, painter, widget);

        QStyleOptionButton sub = *btn;
        sub.rect = proxy()->subElementRect(SE_CheckBoxContents, btn, widget);
        QString text = btn->text;
        if (!text.isEmpty()) {
            QColor textColor = (btn->state & State_Enabled)
                ? cp.foreground() : desaturate(blend(cp.foreground(), cp.background(), 0.5), 0.5);
            sub.palette.setColor(QPalette::WindowText, textColor);
            proxy()->drawItemText(painter, sub.rect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic,
                                  sub.palette, btn->state & State_Enabled, text, QPalette::WindowText);
        }
        return;
    }
    case CE_RadioButton: {
        auto *btn = qstyleoption_cast<const QStyleOptionButton *>(option);
        if (!btn) break;
        drawPrimitive(PE_IndicatorRadioButton, btn, painter, widget);

        QStyleOptionButton sub = *btn;
        sub.rect = proxy()->subElementRect(SE_RadioButtonContents, btn, widget);
        QString text = btn->text;
        if (!text.isEmpty()) {
            QColor textColor = (btn->state & State_Enabled)
                ? cp.foreground() : desaturate(blend(cp.foreground(), cp.background(), 0.5), 0.5);
            sub.palette.setColor(QPalette::WindowText, textColor);
            proxy()->drawItemText(painter, sub.rect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic,
                                  sub.palette, btn->state & State_Enabled, text, QPalette::WindowText);
        }
        return;
    }
    case CE_ComboBoxLabel: {
        auto *cb = qstyleoption_cast<const QStyleOptionComboBox *>(option);
        if (!cb) break;

        QRect textRect = proxy()->subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField, widget);

        if (!cb->currentIcon.isNull()) {
            QPixmap pix = cb->currentIcon.pixmap(cb->iconSize);
            painter->drawPixmap(textRect.left(), textRect.center().y() - pix.height() / 2, pix);
            textRect.adjust(cb->iconSize.width() + 4, 0, 0, 0);
        }

        if (!cb->currentText.isEmpty()) {
            QColor textColor = (cb->state & State_Enabled)
                ? cp.foreground() : desaturate(blend(cp.foreground(), cp.background(), 0.5), 0.5);
            QPalette pal = cb->palette;
            pal.setColor(QPalette::Text, textColor);
            proxy()->drawItemText(painter, textRect, Qt::AlignLeft | Qt::AlignVCenter,
                                  pal, cb->state & State_Enabled, cb->currentText, QPalette::Text);
        }
        return;
    }
    case CE_ScrollBarSlider: {
        bool horizontal = option->state & State_Horizontal;
        Qt::Orientation orient = horizontal ? Qt::Horizontal : Qt::Vertical;

        int margin = 3;
        QRectF r = option->rect.adjusted(margin, margin, -margin, -margin);

        if (option->state & State_MouseOver || option->state & State_Sunken) {
            Detail::drawScrollBarHandle(painter, r, orient, cp);
        } else {
            // Subtle default
            Detail::drawScrollBarHandle(painter, r, orient, cp);
        }
        return;
    }
    case CE_ScrollBarAddPage:
    case CE_ScrollBarSubPage:
        // Transparent track
        return;
    case CE_ScrollBarAddLine:
    case CE_ScrollBarSubLine:
        return;
    case CE_MenuItem: {
        auto *mi = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
        if (!mi) break;

        Detail::drawMenuItemBackground(painter, option->rect, option->state, cp);

        // Text
        if (!mi->text.isEmpty()) {
            QRect textRect = mi->rect;
            QColor textColor = (mi->state & State_Enabled)
                ? cp.foreground() : desaturate(blend(cp.foreground(), cp.background(), 0.5), 0.5);
            painter->setPen(textColor);
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, mi->text);
        }

        // Checkmark
        if (mi->checked) {
            QRect checkRect(mi->rect.left() + 4, mi->rect.center().y() - 6, 12, 12);
            painter->fillRect(checkRect, cp.highlight());
        }
        return;
    }
    case CE_MenuBarItem: {
        bool selected = option->state & State_Selected;
        if (selected) {
            painter->fillRect(option->rect, cp.highlight());
            painter->setPen(cp.highlightForeground());
        } else {
            painter->setPen(cp.foreground());
        }
        painter->drawText(option->rect, Qt::AlignCenter, option->text);
        return;
    }
    case CE_TabBarTab: {
        auto *tab = qstyleoption_cast<const QStyleOptionTab *>(option);
        if (!tab) break;

        bool selected = tab->state & State_Selected;
        Detail::drawTabBackground(painter, option->rect, selected, cp, cr.topLeft);

        // Underline for selected tab
        if (selected) {
            painter->fillRect(option->rect.adjusted(4, option->rect.height() - 3, -4, 0), cp.highlight());
        }

        drawControl(CE_TabBarTabLabel, option, painter, widget);
        return;
    }
    case CE_TabBarTabLabel: {
        auto *tab = qstyleoption_cast<const QStyleOptionTab *>(option);
        if (!tab) break;
        QRect textRect = tab->rect;
        painter->setPen((tab->state & State_Selected) ? cp.foreground() : cp.border());
        painter->drawText(textRect, Qt::AlignCenter, tab->text);
        return;
    }
    case CE_HeaderSection: {
        Detail::drawHeaderSection(painter, option->rect, cp);

        // Sort indicator
        auto *header = qstyleoption_cast<const QStyleOptionHeader *>(option);
        if (header && header->sortIndicator != QStyleOptionHeader::None) {
            QRect ar(option->rect.right() - 16, option->rect.center().y() - 4, 12, 8);
            bool up = header->sortIndicator == QStyleOptionHeader::SortUp;
            if (up) {
                painter->setBrush(cp.foreground());
                painter->setPen(Qt::NoPen);
                painter->drawPolygon(QPolygonF()
                    << QPointF(ar.center().x(), ar.top())
                    << QPointF(ar.left(), ar.bottom())
                    << QPointF(ar.right(), ar.bottom()));
            } else {
                painter->setBrush(cp.foreground());
                painter->setPen(Qt::NoPen);
                painter->drawPolygon(QPolygonF()
                    << QPointF(ar.left(), ar.top())
                    << QPointF(ar.right(), ar.top())
                    << QPointF(ar.center().x(), ar.bottom()));
            }
        }
        return;
    }
    case CE_HeaderLabel: {
        auto *header = qstyleoption_cast<const QStyleOptionHeader *>(option);
        if (!header) break;
        painter->setPen(cp.foreground());
        painter->drawText(option->rect.adjusted(4, 0, -4, 0), Qt::AlignLeft | Qt::AlignVCenter, header->text);
        return;
    }
    case CE_ProgressBarGroove:
        Detail::drawProgressBarGroove(painter, option->rect, cp);
        return;
    case CE_ProgressBarContents: {
        auto *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(option);
        if (!pb) break;

        qreal progress = 0.0;
        if (pb->maximum > pb->minimum)
            progress = qreal(pb->progress - pb->minimum) / (pb->maximum - pb->minimum);

        int margin = 2;
        QRectF fillRect = option->rect.adjusted(margin, margin, -margin, -margin);
        Detail::drawProgressBarFill(painter, fillRect, progress, cp);
        return;
    }
    case CE_ProgressBarLabel: {
        auto *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(option);
        if (!pb || pb->text.isEmpty()) break;
        painter->setPen(cp.foreground());
        painter->drawText(option->rect, Qt::AlignCenter, pb->text);
        return;
    }
    case CE_ToolBar:
        painter->fillRect(option->rect, cp.surfaceVariant());
        return;
    case CE_ToolButtonLabel: {
        auto *tb = qstyleoption_cast<const QStyleOptionToolButton *>(option);
        if (!tb) break;

        bool enabled = tb->state & State_Enabled;
        QColor textColor = enabled ? cp.foreground()
            : desaturate(blend(cp.foreground(), cp.background(), 0.5), 0.5);
        painter->setPen(textColor);

        QRect textRect = tb->rect;
        if (!tb->icon.isNull()) {
            QPixmap pix = tb->icon.pixmap(tb->iconSize, enabled ? QIcon::Normal : QIcon::Disabled);
            QRect iconRect(QPoint(tb->rect.center().x() - tb->iconSize.width() / 2, tb->rect.top() + 4), tb->iconSize);
            painter->drawPixmap(iconRect, pix);
            textRect.setTop(iconRect.bottom() + 2);
        }

        if (!tb->text.isEmpty())
            painter->drawText(textRect, Qt::AlignCenter, tb->text);
        return;
    }
    case CE_Splitter: {
        int midX = option->rect.center().x();
        int midY = option->rect.center().y();
        painter->setPen(QPen(cp.border(), 1));
        painter->drawPoint(midX, midY);
        painter->drawPoint(midX - 2, midY);
        painter->drawPoint(midX + 2, midY);
        return;
    }
    default:
        QProxyStyle::drawControl(element, option, painter, widget);
        break;
    }
}

// ── Complex controls ────────────────────────────────────

void Style::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                               QPainter *painter, const QWidget *widget) const
{
    if (!option || !painter)
        return;

    auto cp = d->theme ? d->theme->palette() : ColorPalette{};

    switch (control) {
    case CC_ScrollBar: {
        auto *sb = qstyleoption_cast<const QStyleOptionSlider *>(option);
        if (!sb) break;

        // Track background
        QRect trackRect = proxy()->subControlRect(CC_ScrollBar, sb, SC_ScrollBarGroove, widget);
        painter->fillRect(trackRect, cp.surfaceVariant());

        // Slider
        QRect sliderRect = proxy()->subControlRect(CC_ScrollBar, sb, SC_ScrollBarSlider, widget);
        if (sliderRect.isValid()) {
            QStyleOptionSlider sub = *sb;
            sub.rect = sliderRect;
            drawControl(CE_ScrollBarSlider, &sub, painter, widget);
        }

        // Arrows
        QRect addRect = proxy()->subControlRect(CC_ScrollBar, sb, SC_ScrollBarAddLine, widget);
        QRect subRect = proxy()->subControlRect(CC_ScrollBar, sb, SC_ScrollBarSubLine, widget);

        bool horiz = sb->orientation == Qt::Horizontal;
        auto drawArrow = [&](const QRect &r, bool forward) {
            if (!r.isValid() || r.isEmpty()) return;
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setBrush(cp.foreground());
            painter->setPen(Qt::NoPen);
            QPolygonF arrow;
            qreal cx = r.center().x();
            qreal cy = r.center().y();
            qreal s = 3.0;
            if (horiz) {
                if (forward) {
                    arrow << QPointF(cx + s, cy) << QPointF(cx - s, cy - s) << QPointF(cx - s, cy + s);
                } else {
                    arrow << QPointF(cx - s, cy) << QPointF(cx + s, cy - s) << QPointF(cx + s, cy + s);
                }
            } else {
                if (forward) {
                    arrow << QPointF(cx, cy + s) << QPointF(cx - s, cy - s) << QPointF(cx + s, cy - s);
                } else {
                    arrow << QPointF(cx, cy - s) << QPointF(cx - s, cy + s) << QPointF(cx + s, cy + s);
                }
            }
            painter->drawPolygon(arrow);
        };

        drawArrow(subRect, false);
        drawArrow(addRect, true);
        return;
    }
    case CC_Slider: {
        auto *sl = qstyleoption_cast<const QStyleOptionSlider *>(option);
        if (!sl) break;

        bool horiz = sl->orientation == Qt::Horizontal;

        // Groove
        QRect grooveRect = proxy()->subControlRect(CC_Slider, sl, SC_SliderGroove, widget);
        qreal grooveThickness = 4;
        QRectF groove;
        if (horiz) {
            groove = QRectF(grooveRect.left(), grooveRect.center().y() - grooveThickness / 2,
                            grooveRect.width(), grooveThickness);
        } else {
            groove = QRectF(grooveRect.center().x() - grooveThickness / 2, grooveRect.top(),
                            grooveThickness, grooveRect.height());
        }
        painter->setRenderHint(QPainter::Antialiasing);
        painter->fillRect(groove, cp.surfaceVariant());

        // Filled groove
        QRect sliderRect = proxy()->subControlRect(CC_Slider, sl, SC_SliderHandle, widget);
        QRectF filledGroove = groove;
        if (horiz) {
            filledGroove.setRight(sliderRect.center().x());
        } else {
            filledGroove.setBottom(sliderRect.center().y());
        }
        painter->fillRect(filledGroove, cp.highlight());

        // Handle
        qreal handleR = qMin(sliderRect.width(), sliderRect.height()) * 0.45;
        Detail::drawSliderHandle(painter, sliderRect.center(), handleR, option->state, cp);
        return;
    }
    case CC_SpinBox: {
        auto *sb = qstyleoption_cast<const QStyleOptionSpinBox *>(option);
        if (!sb) break;

        // Frame
        QRect frameRect = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxFrame, widget);
        bool hasFocus = option->state & State_HasFocus;
        painter->setRenderHint(QPainter::Antialiasing);
        painter->fillRect(frameRect, cp.surface());
        painter->setPen(QPen(hasFocus ? cp.highlight() : cp.border(), hasFocus ? 2 : 1));
        painter->drawRect(frameRect.adjusted(0, 0, -1, -1));

        // Up/down buttons
        QRect upRect = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxUp, widget);
        QRect downRect = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxDown, widget);

        auto drawSpinArrow = [&](const QRect &r, bool up) {
            if (!r.isValid()) return;
            painter->setRenderHint(QPainter::Antialiasing);
            painter->fillRect(r, cp.surfaceVariant());
            painter->setPen(QPen(cp.border(), 1));
            painter->drawRect(r.adjusted(0, 0, -1, -1));
            painter->setBrush(cp.foreground());
            painter->setPen(Qt::NoPen);
            QPolygonF arrow;
            qreal cx = r.center().x();
            qreal cy = r.center().y();
            qreal s = 3.0;
            if (up) {
                arrow << QPointF(cx, cy - s) << QPointF(cx - s, cy + s) << QPointF(cx + s, cy + s);
            } else {
                arrow << QPointF(cx, cy + s) << QPointF(cx - s, cy - s) << QPointF(cx + s, cy - s);
            }
            painter->drawPolygon(arrow);
        };

        drawSpinArrow(upRect, true);
        drawSpinArrow(downRect, false);
        return;
    }
    case CC_ComboBox: {
        auto *cb = qstyleoption_cast<const QStyleOptionComboBox *>(option);
        if (!cb) break;

        // Frame
        QStyleOptionComboBox frameOpt = *cb;
        frameOpt.rect = proxy()->subControlRect(CC_ComboBox, cb, SC_ComboBoxFrame, widget);
        bool hasFocus = option->state & State_HasFocus;

        painter->setRenderHint(QPainter::Antialiasing);
        auto cr = d->theme ? d->theme->cornerRadii(0) : CornerRadii{};
        QPainterPath framePath = Detail::roundedRect(frameOpt.rect, cr.topLeft);
        painter->fillPath(framePath, cp.surface());
        painter->setPen(QPen(hasFocus ? cp.highlight() : cp.border(), hasFocus ? 2 : 1));
        painter->drawPath(framePath);

        // Arrow button drop-down
        QRect arRect = proxy()->subControlRect(CC_ComboBox, cb, SC_ComboBoxArrow, widget);
        painter->fillRect(arRect, cp.surfaceVariant());
        painter->setBrush(cp.foreground());
        painter->setPen(Qt::NoPen);
        QPolygonF arrow;
        qreal cx = arRect.center().x();
        qreal cy = arRect.center().y();
        qreal s = 3.0;
        arrow << QPointF(cx - s, cy - s) << QPointF(cx + s, cy - s) << QPointF(cx, cy + s);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawPolygon(arrow);

        // Label
        drawControl(CE_ComboBoxLabel, cb, painter, widget);
        return;
    }
    case CC_ToolButton: {
        auto *tb = qstyleoption_cast<const QStyleOptionToolButton *>(option);
        if (!tb) break;

        if (tb->state & (State_Sunken | State_On | State_Raised)) {
            QColor bg;
            if (tb->state & State_Sunken || tb->state & State_On)
                bg = darker(cp.surface(), 0.9);
            else if (tb->state & State_MouseOver)
                bg = blend(cp.surface(), cp.highlight(), 0.06);
            else
                bg = cp.surface();

            painter->fillRect(tb->rect, bg);
        }

        drawControl(CE_ToolButtonLabel, tb, painter, widget);
        return;
    }
    default:
        QProxyStyle::drawComplexControl(control, option, painter, widget);
        break;
    }
}

// ── Layout ───────────────────────────────────────────────

QSize Style::sizeFromContents(ContentsType type, const QStyleOption *option,
                              const QSize &contentsSize, const QWidget *widget) const
{
    QSize size = QProxyStyle::sizeFromContents(type, option, contentsSize, widget);

    if (!d->theme)
        return size;

    auto m = d->theme->metrics();

    switch (type) {
    case CT_PushButton:
    case CT_ComboBox:
        size = size.expandedTo(QSize(0, qRound(m.spacingXLarge())));
        break;
    case CT_LineEdit:
        size = size.expandedTo(QSize(0, qRound(m.spacingXLarge()) - 4));
        break;
    case CT_SpinBox:
        size = size.expandedTo(QSize(0, qRound(m.spacingXLarge())));
        break;
    case CT_CheckBox:
    case CT_RadioButton:
        size = size.expandedTo(QSize(0, qRound(m.spacingXLarge())));
        break;
    case CT_MenuItem:
        size.rheight() = qRound(m.spacingLarge()) * 2 + 8;
        break;
    case CT_MenuBarItem:
        size = size.expandedTo(QSize(0, qRound(m.spacingLarge()) + 8));
        break;
    case CT_TabBarTab:
        size = size.expandedTo(QSize(0, qRound(m.spacingXLarge()) + 8));
        break;
    case CT_Slider:
        size = size.expandedTo(QSize(0, qRound(m.spacingLarge()) + 10));
        break;
    case CT_ScrollBar:
        size = size.expandedTo(QSize(qRound(m.spacingLarge()) + 8, qRound(m.spacingLarge()) + 8));
        break;
    case CT_ProgressBar:
        size = size.expandedTo(QSize(0, qRound(m.spacingLarge())));
        break;
    case CT_HeaderSection:
        size = size.expandedTo(QSize(0, qRound(m.spacingMedium()) + 8));
        break;
    default:
        break;
    }

    return size;
}

QRect Style::subElementRect(SubElement element, const QStyleOption *option,
                            const QWidget *widget) const
{
    return QProxyStyle::subElementRect(element, option, widget);
}

QRect Style::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                            SubControl subControl, const QWidget *widget) const
{
    return QProxyStyle::subControlRect(control, option, subControl, widget);
}

QStyle::SubControl Style::hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                                 const QPoint &position, const QWidget *widget) const
{
    return QProxyStyle::hitTestComplexControl(control, option, position, widget);
}

// ── Metrics ──────────────────────────────────────────────

int Style::pixelMetric(PixelMetric metric, const QStyleOption *option,
                       const QWidget *widget) const
{
    if (d->theme) {
        auto &table = Detail::metricsTable();
        auto it = table.find(metric);
        if (it != table.end())
            return it->second(*d->theme);
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

// ── Style hints ──────────────────────────────────────────

int Style::styleHint(StyleHint hint, const QStyleOption *option,
                     const QWidget *widget, QStyleHintReturn *returnData) const
{
    switch (hint) {
    case SH_DialogButtonLayout:
        return QDialogButtonBox::WinLayout;
    case SH_ItemView_ShowDecorationSelected:
        return 1;
    case SH_TableWidget_ItemView_SelectActiveRow:
        return 1;
    case SH_ToolBar_Movable:
        return 0;
    case SH_Menu_SubMenuPopupDelay:
        return 200;
    case SH_Widget_Animate:
        return 1;
    case SH_ComboBox_Popup:
        return 0;
    case SH_SpinBox_AnimateButton:
        return 1;
    case SH_GroupBox_TextLabelColor:
        return d->theme ? d->theme->palette().foreground().rgba() : 0;
    case SH_RequestMenuButtonAccelerator:
        return 0;
    case SH_Menu_Scrollable:
        return 1;
    case SH_ProgressDialog_CenterCancelButton:
        return 1;
    case SH_MenuBar_AltKeyNavigation:
        return 0;
    case SH_PrintDialog_RightAlignButtons:
        return 1;
    case SH_Slider_SnapToValue:
        return 1;
    case SH_TitleBar_NoBorder:
        return 1;
    case SH_ItemView_ArrowKeysNavigateIntoChildren:
        return 0;
    case SH_ItemView_MovementWithoutUpdatingSelection:
        return 1;
    case SH_ComboBox_ListMouseTracking:
        return 1;
    case SH_Menu_MouseTracking:
        return 1;
    case SH_MenuBar_MouseTracking:
        return 1;
    case SH_ItemView_ScrollMode:
        return QAbstractItemView::ScrollPerPixel;
    default:
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
}

// ── Icons ────────────────────────────────────────────────

QIcon Style::standardIcon(StandardPixmap standardIcon, const QStyleOption *option,
                          const QWidget *widget) const
{
    return QProxyStyle::standardIcon(standardIcon, option, widget);
}

QPixmap Style::generatedIconPixmap(IconMode iconMode, const QPixmap &pixmap,
                                   const QStyleOption *option) const
{
    return QProxyStyle::generatedIconPixmap(iconMode, pixmap, option);
}

// ── Palette ──────────────────────────────────────────────

QPalette Style::standardPalette() const
{
    if (d->paletteValid)
        return d->palette;
    if (d->theme) {
        const_cast<StylePrivate *>(d.get())->rebuildPalette();
        return d->palette;
    }
    return QProxyStyle::standardPalette();
}

// ── Polish / unpolish ────────────────────────────────────

void Style::polish(QWidget *widget)
{
    QProxyStyle::polish(widget);

    // Set auto-fill background for containers
    if (qobject_cast<QFrame *>(widget) || qobject_cast<QAbstractButton *>(widget)) {
        widget->setAttribute(Qt::WA_Hover);
    }
}

void Style::unpolish(QWidget *widget)
{
    QProxyStyle::unpolish(widget);
}

void Style::polish(QApplication *application)
{
    QProxyStyle::polish(application);

    if (!d->theme)
        return;

    d->rebuildPalette();
    application->setPalette(d->palette);

    auto type = d->theme->typography();
    auto fd = type.font(FontRole::UI);
    QFont font(fd.family, qRound(fd.pointSize * 0.75), fd.weight);
    font.setPointSizeF(fd.pointSize);
    application->setFont(font);
}

void Style::unpolish(QApplication *application)
{
    QProxyStyle::unpolish(application);
}

void Style::polish(QPalette &palette)
{
    if (d->theme) {
        d->rebuildPalette();
        palette = d->palette;
    }
}

} // namespace Lingmo
