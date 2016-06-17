Name:       minicontrol
Summary:    Library packaes for minicontrol
Version:    0.1.1
Release:    1
Group:      Application Framework/Other
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(gio-2.0)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(ecore-evas)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(bundle)
BuildRequires: cmake
Requires(post): /sbin/ldconfig
requires(postun): /sbin/ldconfig
%description
Minicontrol is a small control panel on notification panel or lock screen. This package supports to develop minicontrols.

%prep
%setup -q

%package devel
Summary:    Minicontrol library (devel)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Minicontrol library (devel).

%build
%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif
export CFLAGS+=" -fPIC "
export LDFLAGS+="-Wl,--rpath=%{_prefix}/lib -Wl,--as-needed"
LDFLAGS="$LDFLAGS"
%if %{with wayland}
%cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DHAVE_WAYLAND=On
%else
%cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DHAVE_X11=On
%endif
make %{?jobs:-j%jobs}

%install
%make_install

mkdir -p %{buildroot}/usr/share/license
cp -f LICENSE %{buildroot}/usr/share/license/%{name}


%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest minicontrol.manifest
%defattr(-,root,root,-)
%{_libdir}/libminicontrol-provider.so*
%{_libdir}/libminicontrol-viewer.so*
%{_libdir}/libminicontrol-monitor.so*
/usr/share/license/%{name}

%files devel
%defattr(-,root,root,-)
%{_includedir}/%{name}/*.h
%{_libdir}/pkgconfig/minicontrol-provider.pc
%{_libdir}/pkgconfig/minicontrol-monitor.pc
%{_libdir}/pkgconfig/minicontrol-viewer.pc

