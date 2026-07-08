# ── Generate export header ────────────────────────────────
configure_file(
    "${CMAKE_SOURCE_DIR}/include/LingmoStyle/LingmoStyleExport.h.in"
    "${CMAKE_BINARY_DIR}/include/LingmoStyle/LingmoStyleExport.h"
)
configure_file(
    "${CMAKE_SOURCE_DIR}/include/LingmoStyle/LingmoStyleVersion.h.in"
    "${CMAKE_BINARY_DIR}/include/LingmoStyle/LingmoStyleVersion.h"
)

# ── Shared library ────────────────────────────────────────
add_library(LingmoStyle SHARED
    # public headers
    "${CMAKE_SOURCE_DIR}/include/LingmoStyle/Style.h"
    # generated headers
    "${CMAKE_BINARY_DIR}/include/LingmoStyle/LingmoStyleExport.h"
    "${CMAKE_BINARY_DIR}/include/LingmoStyle/LingmoStyleVersion.h"
    # implementation
    Style.cpp
    private/Style_p.h
    detail/color_table.h
    detail/metrics_table.h
)

target_include_directories(LingmoStyle
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

target_link_libraries(LingmoStyle
    PUBLIC
        Qt6::Widgets
        Lingmo::Theme
)

target_compile_definitions(LingmoStyle PRIVATE LINGMOSTYLE_LIBRARY)

set_target_properties(LingmoStyle PROPERTIES
    AUTOMOC ON
    AUTOMOC_MOC_NAMESPACE Lingmo
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    EXPORT_NAME LingmoStyle
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/LingmoStyle/Style.h"
)

lingmo_pch(LingmoStyle
    PUBLIC
        <LingmoStyle/Style.h>
        <LingmoTheme/Theme.h>
    PRIVATE
        <QStyleOption>
        <QPainter>
)

# ── Install rules ─────────────────────────────────────────
install(TARGETS LingmoStyle
    EXPORT LingmoStyleTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LingmoStyle COMPONENT devel
)

install(FILES
    "${CMAKE_BINARY_DIR}/include/LingmoStyle/LingmoStyleExport.h"
    "${CMAKE_BINARY_DIR}/include/LingmoStyle/LingmoStyleVersion.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LingmoStyle
    COMPONENT devel
)

install(EXPORT LingmoStyleTargets
    FILE LingmoStyleTargets.cmake
    NAMESPACE Lingmo::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LingmoStyle
    COMPONENT devel
)

# ── Config file ───────────────────────────────────────────
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/LingmoStyleConfig.cmake.in"
    "${CMAKE_BINARY_DIR}/LingmoStyleConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LingmoStyle
)
write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/LingmoStyleConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_BINARY_DIR}/LingmoStyleConfig.cmake"
    "${CMAKE_BINARY_DIR}/LingmoStyleConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LingmoStyle
    COMPONENT devel
)

# ── Qt style plugin ───────────────────────────────────────
# NOTE: Plugin target must NOT set AUTOMOC_MOC_NAMESPACE
# because Q_PLUGIN_METADATA generates a static object that
# must reside in the global namespace for Qt's plugin loader.
add_library(lingmo_style MODULE
    plugin/StylePlugin.h
    plugin/StylePlugin.cpp
    plugin/lingmo-style.json
)

target_link_libraries(lingmo_style PRIVATE LingmoStyle Qt6::Widgets)

set_target_properties(lingmo_style PROPERTIES
    AUTOMOC ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

install(TARGETS lingmo_style
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/plugins/styles
    COMPONENT runtime
)

install(FILES plugin/lingmo-style.json
    DESTINATION ${CMAKE_INSTALL_DATADIR}/lingmo/style
    COMPONENT runtime
)
