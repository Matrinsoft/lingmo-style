#include <LingmoStyle/Style.h>

#include <private/Style_p.h>
#include <detail/color_table.h>
#include <detail/metrics_table.h>

#include <QApplication>
#include <QStyleOption>
#include <QPainter>
#include <QPalette>

namespace Lingmo {

StylePrivate::StylePrivate(Theme *theme)
    : theme(theme ? theme : Theme::instance())
{
}

Style::Style(Theme *theme)
    : QProxyStyle(nullptr)
    , d(std::make_unique<StylePrivate>(theme))
{
    if (d->theme) {
        polish(d->theme->palette());
    }
}

Style::~Style() = default;

Theme *Style::theme() const
{
    return d->theme;
}

void Style::setTheme(Theme *theme)
{
    if (d->theme == theme)
        return;
    d->theme = theme;
    if (d->theme) {
        polish(d->theme->palette());
    }
    emit themeChanged();
}

void Style::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                          QPainter *painter, const QWidget *widget) const
{
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void Style::drawControl(ControlElement element, const QStyleOption *option,
                        QPainter *painter, const QWidget *widget) const
{
    QProxyStyle::drawControl(element, option, painter, widget);
}

void Style::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                               QPainter *painter, const QWidget *widget) const
{
    QProxyStyle::drawComplexControl(control, option, painter, widget);
}

QSize Style::sizeFromContents(ContentsType type, const QStyleOption *option,
                              const QSize &contentsSize, const QWidget *widget) const
{
    return QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
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

int Style::pixelMetric(PixelMetric metric, const QStyleOption *option,
                       const QWidget *widget) const
{
    auto it = metricsTable.find(metric);
    if (it != metricsTable.end() && d->theme) {
        return it->second(*d->theme);
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

int Style::styleHint(StyleHint hint, const QStyleOption *option,
                     const QWidget *widget, QStyleHintReturn *returnData) const
{
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

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

QPalette Style::standardPalette() const
{
    if (!d->theme)
        return QProxyStyle::standardPalette();
    return paletteFromColorSet(d->theme->palette());
}

void Style::polish(QWidget *widget)
{
    QProxyStyle::polish(widget);
}

void Style::unpolish(QWidget *widget)
{
    QProxyStyle::unpolish(widget);
}

void Style::polish(QApplication *application)
{
    QProxyStyle::polish(application);
    if (d->theme) {
        application->setPalette(standardPalette());
        application->setFont(d->theme->typography().toQFont());
    }
}

void Style::unpolish(QApplication *application)
{
    QProxyStyle::unpolish(application);
}

void Style::polish(QPalette &palette)
{
    if (d->theme) {
        palette = paletteFromColorSet(d->theme->palette());
    }
}

} // namespace Lingmo
