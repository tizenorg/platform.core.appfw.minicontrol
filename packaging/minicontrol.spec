Name:       minicontrol
Summary:    minicontrol library
Version:    0.0.3
Release:    1
Group:      TBD
License:    Flora Software License
Source0:    %{name}-%{version}.tar.gz
BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(dbus-glib-1)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(ecore-evas)
BuildRequires: pkgconfig(dlog)
BuildRequires: cmake
Requires(post): /sbin/ldconfig
requires(postun): /sbin/ldconfig
%description
Minicontrol library.

%prep
%setup -q

%package devel
Summary:    Minicontrol library (devel)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Minicontrol library (devel).

%build
export LDFLAGS+="-Wl,--rpath=%{_prefix}/lib -Wl,--as-needed"
LDFLAGS="$LDFLAGS" cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}
make %{?jobs:-j%jobs}

%install
%make_install

mkdir -p %{buildroot}/usr/share/license
cp -f LICENSE.Flora %{buildroot}/usr/share/license/%{name}


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

