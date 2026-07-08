#pragma once

#include <LingmoStyle/LingmoStyleExport.h>

#include <LingmoTheme/Theme.h>

#include <QProxyStyle>
#include <QPalette>
#include <QFont>
#include <QIcon>

#include <memory>

namespace Lingmo {

class StylePrivate;

class LINGMOSTYLE_EXPORT Style : public QProxyStyle
{
    Q_OBJECT

public:
    explicit Style(Theme *theme = nullptr);
    ~Style() override;

    Theme *theme() const;
    void setTheme(Theme *theme);

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget = nullptr) const override;

    void drawControl(ControlElement element, const QStyleOption *option,
                     QPainter *painter, const QWidget *widget = nullptr) const override;

    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                            QPainter *painter, const QWidget *widget = nullptr) const override;

    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &contentsSize, const QWidget *widget = nullptr) const override;

    QRect subElementRect(SubElement element, const QStyleOption *option,
                         const QWidget *widget = nullptr) const override;

    QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                         SubControl subControl, const QWidget *widget = nullptr) const override;

    SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                     const QPoint &position, const QWidget *widget = nullptr) const override;

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr,
                    const QWidget *widget = nullptr) const override;

    int styleHint(StyleHint hint, const QStyleOption *option = nullptr,
                  const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;

    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *option = nullptr,
                       const QWidget *widget = nullptr) const override;

    QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                const QStyleOption *option) const override;

    QPalette standardPalette() const override;

    void polish(QWidget *widget) override;
    void unpolish(QWidget *widget) override;
    void polish(QApplication *application) override;
    void unpolish(QApplication *application) override;
    void polish(QPalette &palette) override;

Q_SIGNALS:
    void themeChanged();

private:
    std::unique_ptr<StylePrivate> d;
};

} // namespace Lingmo
