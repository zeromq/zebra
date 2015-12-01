#
#    zwebrap - Translates REST HTTP-Request into the XRAP protocol.
#
#    Copyright (c) the Contributors as noted in the AUTHORS file.       
#    This file is part of ZWEBRAP.                                      
#                                                                       
#    This Source Code Form is subject to the terms of the Mozilla Public
#    License, v. 2.0. If a copy of the MPL was not distributed with this
#    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
#

Name:           zwebrap
Version:        0.1.0
Release:        1
Summary:        translates rest http-request into the xrap protocol.
License:        MIT
URL:            http://example.com/
Source0:        %{name}-%{version}.tar.gz
Group:          System/Libraries
BuildRequires:  automake
BuildRequires:  autoconf
BuildRequires:  libtool
BuildRequires:  pkg-config
BuildRequires:  zeromq-devel
BuildRequires:  czmq-devel
BuildRequires:  libmicrohttpd-devel
BuildRequires:  libcurl-devel
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
zwebrap translates rest http-request into the xrap protocol..

%package -n libzwebrap0
Group:          System/Libraries
Summary:        translates rest http-request into the xrap protocol.

%description -n libzwebrap0
zwebrap translates rest http-request into the xrap protocol..
This package contains shared library.

%post -n libzwebrap0 -p /sbin/ldconfig
%postun -n libzwebrap0 -p /sbin/ldconfig

%files -n libzwebrap0
%defattr(-,root,root)
%doc COPYING
%{_libdir}/libzwebrap.so.*

%package devel
Summary:        translates rest http-request into the xrap protocol.
Group:          System/Libraries
Requires:       libzwebrap0 = %{version}
Requires:       zeromq-devel
Requires:       czmq-devel
Requires:       libmicrohttpd-devel
Requires:       libcurl-devel

%description devel
zwebrap translates rest http-request into the xrap protocol..
This package contains development files.

%files devel
%defattr(-,root,root)
%{_includedir}/*
%{_libdir}/libzwebrap.so
%{_libdir}/pkgconfig/zwebrap.pc

%prep
%setup -q

%build
sh autogen.sh
%{configure}
make %{_smp_mflags}

%install
make install DESTDIR=%{buildroot} %{?_smp_mflags}

# remove static libraries
find %{buildroot} -name '*.a' | xargs rm -f
find %{buildroot} -name '*.la' | xargs rm -f


%changelog
