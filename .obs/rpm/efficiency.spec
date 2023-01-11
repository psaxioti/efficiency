%define debug_package %{nil}

Name:           efficiency
Version:        1.4
Release:        0
Group:          Productivity/Scientific/Physics
License:        GPL-3.0
Summary:        Efficiency program made at Demokritos lab
Url:            https://github.com/psaxioti/efficiency

BuildRequires:  cmake >= 3.16
BuildRequires:  gcc-c++
BuildRequires:  gsl-devel
BuildRequires:	 pkgconfig
BuildRequires:	 (pkgconfig(Qt6Widgets) or pkgconfig(Qt5Widgets))
BuildRequires:	 (pkgconfig(Qt6PrintSupport) or pkgconfig(Qt5PrintSupport))

%description
This package contains the efficiency program that calculates the efficiency of an HPGe detector

%prep
%setup -q -n %{_sourcedir}/%{name}-%{version} -T -D

%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ../
make %{?_smp_mflags}

%install
cd build
make install DESTDIR=%{buildroot}

%files
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop