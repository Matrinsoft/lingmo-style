#include "StylePlugin.h"

#include <LingmoStyle/Style.h>

namespace Lingmo {

QStyle *StylePlugin::create(const QString &key)
{
    if (key.compare(QStringLiteral("lingmo"), Qt::CaseInsensitive) == 0)
        return new Style;

    // Support "lingmo-*" variant keys gracefully
    if (key.startsWith(QStringLiteral("lingmo"), Qt::CaseInsensitive))
        return new Style;

    return nullptr;
}

} // namespace Lingmo
