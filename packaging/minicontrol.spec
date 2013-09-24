Name:           minicontrol
Version:        0.0.6
Release:        1
License:        Flora
Summary:        Minicontrol Library
Group:          Applications/Core Applications
Source0:        %{name}-%{version}.tar.gz
Source1001:     minicontrol.manifest
BuildRequires:  cmake
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(dbus-glib-1)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(ecore-evas)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(evas)
%description
Minicontrol library.

%prep
%setup -q
cp %{SOURCE1001} .

%package devel
Summary:        Minicontrol library (devel)
Requires:       %{name} = %{version}

%description devel
Minicontrol library (devel).

%build
CFLAGS="$CFLAGS -fPIC"
%cmake .
make %{?_smp_mflags}

%install
%make_install


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%license LICENSE.Flora
%defattr(-,root,root,-)
%{_libdir}/libminicontrol-provider.so*
%{_libdir}/libminicontrol-viewer.so*
%{_libdir}/libminicontrol-monitor.so*

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_includedir}/%{name}/*.h
%{_libdir}/pkgconfig/minicontrol-provider.pc
%{_libdir}/pkgconfig/minicontrol-monitor.pc
%{_libdir}/pkgconfig/minicontrol-viewer.pc
