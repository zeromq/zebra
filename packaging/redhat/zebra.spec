#
#    zebra - Translates REST HTTP-Request into the XRAP protocol.
#
#    Copyright (c) the Contributors as noted in the AUTHORS file.       
#    This file is part of ZEBRA.                                        
#                                                                       
#    This Source Code Form is subject to the terms of the Mozilla Public
#    License, v. 2.0. If a copy of the MPL was not distributed with this
#    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
#

Name:           zebra
Version:        0.2.0
Release:        1
Summary:        translates rest http-request into the xrap protocol.
License:        MIT
URL:            https://github.com/zeromq/zebra
Source0:        %{name}-%{version}.tar.gz
Group:          System/Libraries
BuildRequires:  asciidoc
BuildRequires:  automake
BuildRequires:  autoconf
BuildRequires:  libtool
BuildRequires:  pkg-config
BuildRequires:  systemd-devel
BuildRequires:  xmlto
BuildRequires:  zeromq-devel
BuildRequires:  czmq-devel
BuildRequires:  libmicrohttpd-devel
BuildRequires:  libcurl-devel
BuildRequires:  libsodium-devel
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
zebra translates rest http-request into the xrap protocol..

%package -n libzebra0
Group:          System/Libraries
Summary:        translates rest http-request into the xrap protocol.

%description -n libzebra0
zebra translates rest http-request into the xrap protocol..
This package contains shared library.

%post -n libzebra0 -p /sbin/ldconfig
%postun -n libzebra0 -p /sbin/ldconfig

%files -n libzebra0
%defattr(-,root,root)
%{_libdir}/libzebra.so.*

%package devel
Summary:        translates rest http-request into the xrap protocol.
Group:          System/Libraries
Requires:       libzebra0 = %{version}
Requires:       zeromq-devel
Requires:       czmq-devel
Requires:       libmicrohttpd-devel
Requires:       libcurl-devel
Requires:       libsodium-devel

%description devel
zebra translates rest http-request into the xrap protocol..
This package contains development files.

%files devel
%defattr(-,root,root)
%{_includedir}/*
%{_libdir}/libzebra.so
%{_libdir}/pkgconfig/libzebra.pc
%{_mandir}/man3/*
%{_mandir}/man7/*

%prep
%setup -q

%build
sh autogen.sh
%{configure} --with-systemd-units
make %{_smp_mflags}

%install
make install DESTDIR=%{buildroot} %{?_smp_mflags}

# remove static libraries
find %{buildroot} -name '*.a' | xargs rm -f
find %{buildroot} -name '*.la' | xargs rm -f


%changelog
