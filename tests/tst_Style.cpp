#include <LingmoStyle/Style.h>
#include <LingmoTheme/Theme.h>
#include <LingmoTheme/ColorPalette.h>

#include <QPalette>
#include <QTest>
#include <QSignalSpy>

class tst_Style : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void styleIsCreated();
    void styleHasDefaultTheme();
    void paletteMapping();
    void paletteStates();
    void paletteDisabledIsDesaturated();
    void pixelMetricMapping();
    void pixelMetricUsesTheme();
    void styleHintReturnsModernDefaults();
    void sizeFromContents();
    void themeChanges();
    void customTheme();
    void setThemeEmitsSignal();
    void standardPaletteReflectsThemeChange();
};

void tst_Style::styleIsCreated()
{
    Lingmo::Style style;
    QVERIFY(style.theme() != nullptr);
}

void tst_Style::styleHasDefaultTheme()
{
    Lingmo::Style style;
    QVERIFY(style.theme() != nullptr);
    QCOMPARE(style.theme()->themeName(), QStringLiteral("lingmo-light"));
}

void tst_Style::paletteMapping()
{
    Lingmo::Style style;
    QPalette palette = style.standardPalette();

    QVERIFY(palette.color(QPalette::Window).isValid());
    QVERIFY(palette.color(QPalette::WindowText).isValid());
    QVERIFY(palette.color(QPalette::Button).isValid());
    QVERIFY(palette.color(QPalette::ButtonText).isValid());
    QVERIFY(palette.color(QPalette::Base).isValid());
    QVERIFY(palette.color(QPalette::Text).isValid());
    QVERIFY(palette.color(QPalette::Highlight).isValid());
    QVERIFY(palette.color(QPalette::HighlightedText).isValid());
    QVERIFY(palette.color(QPalette::Link).isValid());
}

void tst_Style::paletteStates()
{
    Lingmo::Style style;
    QPalette palette = style.standardPalette();

    // Active should always be valid
    QVERIFY(palette.color(QPalette::Active, QPalette::Window).isValid());
    QVERIFY(palette.color(QPalette::Active, QPalette::Highlight).isValid());

    // Inactive should match Active
    QCOMPARE(palette.color(QPalette::Active, QPalette::Window),
             palette.color(QPalette::Inactive, QPalette::Window));

    // Disabled should be different from Active (reduced contrast)
    QVERIFY(palette.color(QPalette::Disabled, QPalette::WindowText)
            != palette.color(QPalette::Active, QPalette::WindowText));
}

void tst_Style::paletteDisabledIsDesaturated()
{
    Lingmo::Style style;
    QPalette palette = style.standardPalette();

    auto activeHighlight = palette.color(QPalette::Active, QPalette::Highlight);
    auto disabledHighlight = palette.color(QPalette::Disabled, QPalette::Highlight);

    // Disabled highlight should be less saturated
    QVERIFY(activeHighlight.hslSaturationF() >= disabledHighlight.hslSaturationF());
}

void tst_Style::pixelMetricMapping()
{
    Lingmo::Style style;

    QVERIFY(style.pixelMetric(QStyle::PM_LayoutLeftMargin) >= 0);
    QVERIFY(style.pixelMetric(QStyle::PM_ButtonMargin) >= 0);
    QVERIFY(style.pixelMetric(QStyle::PM_DefaultFrameWidth) >= 0);
    QVERIFY(style.pixelMetric(QStyle::PM_ScrollBarExtent) >= 0);
    QVERIFY(style.pixelMetric(QStyle::PM_SliderThickness) >= 0);
    QVERIFY(style.pixelMetric(QStyle::PM_TabBarTabHSpace) >= 0);
    QVERIFY(style.pixelMetric(QStyle::PM_MenuBarItemSpacing) >= 0);
    QVERIFY(style.pixelMetric(QStyle::PM_ToolBarIconSize) >= 0);
}

void tst_Style::pixelMetricUsesTheme()
{
    Lingmo::Style style;

    int defaultMargin = style.pixelMetric(QStyle::PM_LayoutLeftMargin);

    // Create a custom theme with different spacing
    auto theme = std::make_unique<Lingmo::Theme>();
    Lingmo::Metrics customMetrics;
    customMetrics.setSpacing(0, 40.0);
    theme->setMetrics(customMetrics);
    style.setTheme(theme.get());

    int customMargin = style.pixelMetric(QStyle::PM_LayoutLeftMargin);
    QVERIFY(customMargin != defaultMargin || customMargin == 40);
}

void tst_Style::styleHintReturnsModernDefaults()
{
    Lingmo::Style style;

    QVERIFY(style.styleHint(QStyle::SH_Widget_Animate) != 0);
    QVERIFY(style.styleHint(QStyle::SH_Menu_Scrollable) != 0);
    QVERIFY(style.styleHint(QStyle::SH_ItemView_ScrollMode)
            == QAbstractItemView::ScrollPerPixel);
}

void tst_Style::sizeFromContents()
{
    Lingmo::Style style;

    QStyleOptionButton btnOpt;
    btnOpt.state = QStyle::State_Enabled;
    QSize defaultSize(80, 30);
    QSize result = style.sizeFromContents(QStyle::CT_PushButton, &btnOpt, defaultSize, nullptr);
    QVERIFY(result.height() >= defaultSize.height());
}

void tst_Style::themeChanges()
{
    Lingmo::Style style;
    auto *theme = style.theme();

    QSignalSpy spy(&style, &Lingmo::Style::themeChanged);
    style.setTheme(theme);
    QCOMPARE(spy.count(), 0);

    auto newTheme = std::make_unique<Lingmo::Theme>();
    style.setTheme(newTheme.get());
    QCOMPARE(spy.count(), 1);
}

void tst_Style::customTheme()
{
    auto theme = std::make_unique<Lingmo::Theme>();
    theme->setThemeName(QStringLiteral("test-custom"));

    Lingmo::Style style(theme.get());
    QCOMPARE(style.theme()->themeName(), QStringLiteral("test-custom"));
}

void tst_Style::setThemeEmitsSignal()
{
    Lingmo::Style style;
    auto theme = std::make_unique<Lingmo::Theme>();

    QSignalSpy spy(&style, &Lingmo::Style::themeChanged);
    style.setTheme(theme.get());
    QCOMPARE(spy.count(), 1);

    // Setting same theme should not emit
    style.setTheme(theme.get());
    QCOMPARE(spy.count(), 1);
}

void tst_Style::standardPaletteReflectsThemeChange()
{
    Lingmo::Style style;
    QPalette initialPalette = style.standardPalette();

    auto theme = std::make_unique<Lingmo::Theme>();
    Lingmo::ColorPalette customPalette;
    customPalette.setBackground(QColor(255, 0, 0)); // Red background for testing
    theme->setPalette(customPalette);
    style.setTheme(theme.get());

    QPalette updatedPalette = style.standardPalette();
    QCOMPARE(updatedPalette.color(QPalette::Window), QColor(255, 0, 0));
}

QTEST_MAIN(tst_Style)

#include "tst_Style.moc"
