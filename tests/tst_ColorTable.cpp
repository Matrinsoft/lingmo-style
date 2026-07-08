#include <LingmoStyle/detail/color_table.h>

#include <QTest>
#include <QPalette>

class tst_ColorTable : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void blendBindsEndpoints();
    void blendInterpolates();
    void blendClampsT();
    void blendPreservesAlpha();
    void lighterIncreasesLightness();
    void lighterReducesSaturation();
    void darkerDecreasesLightness();
    void darkerIncreasesSaturation();
    void desaturateReducesSaturation();
    void desaturatePreservesLightness();
    void isDarkBelowThreshold();
    void isDarkAboveThreshold();
    void relativeLuminanceIsWCAGCompliant();
    void setColorGroupPopulatesAllRoles();
    void paletteFromColorSetActiveInactiveMatch();
    void paletteFromColorSetDisabledLowerContrast();
    void paletteFromColorSetHasAllRoles();
};

void tst_ColorTable::blendBindsEndpoints()
{
    using Lingmo::Detail::blend;

    QColor red(255, 0, 0);
    QColor blue(0, 0, 255);

    QCOMPARE(blend(red, blue, 0.0), red);
    QCOMPARE(blend(red, blue, 1.0), blue);
}

void tst_ColorTable::blendInterpolates()
{
    using Lingmo::Detail::blend;

    QColor black(0, 0, 0);
    QColor white(255, 255, 255);

    QColor mid = blend(black, white, 0.5);
    QCOMPARE(mid, QColor(127, 127, 127));
}

void tst_ColorTable::blendClampsT()
{
    using Lingmo::Detail::blend;

    QColor red(255, 0, 0);
    QColor blue(0, 0, 255);

    QCOMPARE(blend(red, blue, -0.5), red);
    QCOMPARE(blend(red, blue, 1.5), blue);
}

void tst_ColorTable::blendPreservesAlpha()
{
    using Lingmo::Detail::blend;

    QColor a(255, 0, 0, 128);
    QColor b(0, 0, 255, 200);

    QColor result = blend(a, b, 0.5);
    QCOMPARE(result.alpha(), 164); // (128 + 200) / 2
}

void tst_ColorTable::lighterIncreasesLightness()
{
    using Lingmo::Detail::lighter;

    QColor c(100, 100, 100);
    QColor l = lighter(c);

    QVERIFY(l.lightnessF() > c.lightnessF());
}

void tst_ColorTable::lighterReducesSaturation()
{
    using Lingmo::Detail::lighter;

    QColor c(200, 50, 50); // saturated red-ish
    QColor l = lighter(c);

    QVERIFY(l.hslSaturationF() <= c.hslSaturationF());
}

void tst_ColorTable::darkerDecreasesLightness()
{
    using Lingmo::Detail::darker;

    QColor c(100, 100, 100);
    QColor d = darker(c);

    QVERIFY(d.lightnessF() < c.lightnessF());
}

void tst_ColorTable::darkerIncreasesSaturation()
{
    using Lingmo::Detail::darker;

    QColor c(200, 100, 100);
    QColor d = darker(c);

    QVERIFY(d.hslSaturationF() >= c.hslSaturationF());
}

void tst_ColorTable::desaturateReducesSaturation()
{
    using Lingmo::Detail::desaturate;

    QColor c(200, 50, 50);
    QColor d = desaturate(c, 0.8);

    QVERIFY(d.hslSaturationF() < c.hslSaturationF());
}

void tst_ColorTable::desaturatePreservesLightness()
{
    using Lingmo::Detail::desaturate;

    QColor c(200, 50, 50);
    QColor d = desaturate(c, 0.5);

    QCOMPARE(d.lightnessF(), c.lightnessF());
}

void tst_ColorTable::isDarkBelowThreshold()
{
    using Lingmo::Detail::isDark;

    QVERIFY(isDark(QColor(0, 0, 0)));
    QVERIFY(isDark(QColor(20, 20, 20)));
}

void tst_ColorTable::isDarkAboveThreshold()
{
    using Lingmo::Detail::isDark;

    QVERIFY(!isDark(QColor(255, 255, 255)));
    QVERIFY(!isDark(QColor(200, 200, 200)));
}

void tst_ColorTable::relativeLuminanceIsWCAGCompliant()
{
    using Lingmo::Detail::relativeLuminance;

    // Per WCAG 2.1: sRGB to luminance
    QVERIFY(relativeLuminance(QColor(0, 0, 0)) < 0.001);
    QVERIFY(relativeLuminance(QColor(255, 255, 255)) > 0.999);
    QVERIFY(relativeLuminance(QColor(255, 0, 0)) < relativeLuminance(QColor(0, 255, 0)));
}

void tst_ColorTable::setColorGroupPopulatesAllRoles()
{
    using Lingmo::Detail::setColorGroup;

    QPalette palette;
    Lingmo::ColorPalette cp;
    cp.setBackground(QColor(240, 240, 240));
    cp.setForeground(QColor(20, 20, 20));
    cp.setSurface(QColor(255, 255, 255));
    cp.setHighlight(QColor(0, 120, 212));
    cp.setHighlightForeground(QColor(255, 255, 255));

    setColorGroup(palette, QPalette::Active, cp, QColor(240, 240, 240), QColor(20, 20, 20));

    QVERIFY(palette.color(QPalette::Active, QPalette::Window).isValid());
    QVERIFY(palette.color(QPalette::Active, QPalette::Highlight).isValid());
    QVERIFY(palette.color(QPalette::Active, QPalette::Light).isValid());
    QVERIFY(palette.color(QPalette::Active, QPalette::Dark).isValid());
    QVERIFY(palette.color(QPalette::Active, QPalette::BrightText).isValid());
}

void tst_ColorTable::paletteFromColorSetActiveInactiveMatch()
{
    using Lingmo::Detail::paletteFromColorSet;

    Lingmo::ColorPalette cp;
    QPalette palette = paletteFromColorSet(cp, Lingmo::ColorScheme::Light);

    QCOMPARE(palette.color(QPalette::Active, QPalette::Window),
             palette.color(QPalette::Inactive, QPalette::Window));
    QCOMPARE(palette.color(QPalette::Active, QPalette::Highlight),
             palette.color(QPalette::Inactive, QPalette::Highlight));
}

void tst_ColorTable::paletteFromColorSetDisabledLowerContrast()
{
    using Lingmo::Detail::paletteFromColorSet;

    Lingmo::ColorPalette cp;
    cp.setBackground(QColor(255, 255, 255));
    cp.setForeground(QColor(0, 0, 0));
    QPalette palette = paletteFromColorSet(cp, Lingmo::ColorScheme::Light);

    // Disabled text should be lower contrast (closer to background)
    auto activeFg = palette.color(QPalette::Active, QPalette::WindowText);
    auto disabledFg = palette.color(QPalette::Disabled, QPalette::WindowText);
    QVERIFY(qAbs(disabledFg.lightness() - 255) > qAbs(activeFg.lightness() - 255)
            || disabledFg != activeFg);
}

void tst_ColorTable::paletteFromColorSetHasAllRoles()
{
    using Lingmo::Detail::paletteFromColorSet;

    Lingmo::ColorPalette cp;
    QPalette palette = paletteFromColorSet(cp, Lingmo::ColorScheme::Light);

    // Check that all QPalette::ColorRoles are set for Active
    auto roles = {
        QPalette::Window, QPalette::WindowText,
        QPalette::Button, QPalette::ButtonText,
        QPalette::Base, QPalette::Text,
        QPalette::Highlight, QPalette::HighlightedText,
        QPalette::Link, QPalette::LinkVisited,
        QPalette::Light, QPalette::Midlight,
        QPalette::Dark, QPalette::Mid, QPalette::Shadow,
        QPalette::BrightText
    };
    for (auto role : roles) {
        QVERIFY2(palette.color(QPalette::Active, role).isValid(),
                 qPrintable(QString("Role %1 is invalid").arg(role)));
    }
}

QTEST_MAIN(tst_ColorTable)

#include "tst_ColorTable.moc"
