%global majver 1

Name:          lingmo-style
Version:       1.0.0
Release:       1%{?dist}
Summary:       Lingmo Desktop - Qt Widgets Style

License:       GPL-3.0-or-later
URL:           https://lingmo.org
Source0:       %{name}-%{version}.tar.gz

BuildRequires: cmake >= 3.21
BuildRequires: ninja-build
BuildRequires: qt6-qtbase-devel >= 6.5
BuildRequires: lingmo-theme-devel >= 1.0

%description
Lingmo Style provides a QProxyStyle-based Qt Widgets style implementation
for the Lingmo Desktop environment. It reads design tokens from lingmo-theme
and ensures that Qt Widgets applications share the same visual design as
QML applications.

%package libs
Summary: Shared library for %{name}
Requires: lingmo-theme-libs%{?_isa} >= 1.0

%description libs
Shared library providing the Lingmo Style widget style.

%package devel
Summary: Development files for %{name}
Requires: %{name}-libs%{?_isa} = %{version}-%{release}
Requires: qt6-qtbase-devel
Requires: lingmo-theme-devel%{?_isa} >= 1.0

%description devel
Development headers and CMake integration for Lingmo Style.

%prep
%setup -q

%build
%cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DBUILD_TESTING=OFF
%cmake_build

%install
%cmake_install

%check
# Tests are run in CI

%files libs
%license LICENSE
%{_libdir}/libLingmoStyle.so.%{majver}*
%{_libdir}/qt6/plugins/styles/liblingmo_style.so

%files devel
%{_includedir}/LingmoStyle/
%{_libdir}/libLingmoStyle.so
%{_libdir}/cmake/LingmoStyle/

%changelog
* Wed Jul 08 2026 Lingmo OS Team <team@lingmo.org> - 1.0.0-1
- Initial release
