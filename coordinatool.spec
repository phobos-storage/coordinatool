Name: coordinatool
Version: 0.2
Release: 1%{?dist}
Summary: lustre userspace coordinator implemented as a copytool
License: LGPLv3+

Source0: %{name}-%{version}.tar.xz

BuildRequires: meson
BuildRequires: gcc
BuildRequires: pkgconfig(jansson)
BuildRequires: pkgconfig(liburcu)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: hiredis

%description
coordinatool is a lustre copytool that takes all requests off lustre's
cooridnator, then hand the requests back to normal copytools through a
LD_PRELOAD lib

%package lib
Summary: coordinatool LD_PRELOAD lib

%description lib
LD_PRELOAD library for coordinatool

%package client
Summary: coordinatool standalone client

%description client
standalone coordinatool client to interact with the server

%prep
%autosetup

%build
%meson -Dwerror=false
%meson_build

%install
%meson_install

%check
%meson_test

%files
%{_bindir}/lhsmd_coordinatool
%{_unitdir}/coordinatool@.service
%config(noreplace) /etc/sysconfig/coordinatool

%files client
%{_bindir}/coordinatool-client

%files lib
%{_libdir}/libcoordinatool_client.so

%post lib -p /sbin/ldconfig
%postun lib -p /sbin/ldconfig

%changelog
* Tue Oct 25 2022 Guillaume Courrier <guillaume.courrier@cea.fr> - 0.2-1
- Clients will try to reconnect the server when down
- Use a REDIS database to store requests and recover from crash
- Add compile time switch to use Phobos features

* Sun Jun 27 2021 Dominique Martinet <dominique.martinet@codewreck.org> - 0.1-1
- initial version

