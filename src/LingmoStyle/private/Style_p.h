#pragma once

#include <LingmoTheme/Theme.h>

namespace Lingmo {

class StylePrivate
{
public:
    explicit StylePrivate(Theme *theme);

    Theme *theme;
};

} // namespace Lingmo
