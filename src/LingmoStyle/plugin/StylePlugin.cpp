#include "StylePlugin.h"

#include <LingmoStyle/Style.h>

namespace Lingmo {

QStyle *StylePlugin::create(const QString &key)
{
    if (key.compare("lingmo", Qt::CaseInsensitive) == 0) {
        return new Style;
    }
    return nullptr;
}

} // namespace Lingmo
