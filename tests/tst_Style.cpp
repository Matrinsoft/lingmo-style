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
    void pixelMetricMapping();
    void themeChanges();
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
    QVERIFY(palette.color(QPalette::Highlight).isValid());
}

void tst_Style::pixelMetricMapping()
{
    Lingmo::Style style;
    int margin = style.pixelMetric(QStyle::PM_LayoutLeftMargin);
    QVERIFY(margin >= 0);
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

QTEST_MAIN(tst_Style)

#include "tst_Style.moc"
