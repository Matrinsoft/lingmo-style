#include <StylePlugin.h>

#include <QTest>
#include <QStyle>

class tst_StylePlugin : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void createWithLingmoKey();
    void createWithLingmoVariant();
    void createWithUnknownKey();
};

void tst_StylePlugin::createWithLingmoKey()
{
    Lingmo::StylePlugin plugin;
    QStyle *style = plugin.create(QStringLiteral("lingmo"));
    QVERIFY(style != nullptr);
    QCOMPARE(style->objectName(), QStringLiteral("LingmoStyle"));
    delete style;
}

void tst_StylePlugin::createWithLingmoVariant()
{
    Lingmo::StylePlugin plugin;

    QStyle *light = plugin.create(QStringLiteral("lingmo-light"));
    QVERIFY(light != nullptr);
    delete light;

    QStyle *dark = plugin.create(QStringLiteral("lingmo-dark"));
    QVERIFY(dark != nullptr);
    delete dark;
}

void tst_StylePlugin::createWithUnknownKey()
{
    Lingmo::StylePlugin plugin;

    QVERIFY(plugin.create(QStringLiteral("fusion")) == nullptr);
    QVERIFY(plugin.create(QStringLiteral("windows")) == nullptr);
    QVERIFY(plugin.create(QStringLiteral("")) == nullptr);
}

QTEST_MAIN(tst_StylePlugin)

#include "tst_StylePlugin.moc"
