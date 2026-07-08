#pragma once

#include <QStylePlugin>

namespace Lingmo {

class StylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QStyleFactoryInterface_iid FILE "lingmo-style.json")

public:
    QStyle *create(const QString &key) override;
};

} // namespace Lingmo
