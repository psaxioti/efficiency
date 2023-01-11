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
BuildRequires:	 pkgconfig
BuildRequires:  pkgconfig(gsl)
%if 0%{?sle_version} >= 150400
BuildRequires:  (gcc9-c++ or gcc10-c++ or gcc11-c++ or gcc12-c++ )
BuildRequires:	 qt6-widgets-devel
BuildRequires:	 qt6-printsupport-devel
%else
BuildRequires:	 pkgconfig(Qt5Widgets)
BuildRequires:	 pkgconfig(Qt5PrintSupport)
%endif

%description
This package contains the efficiency program that calculates the efficiency of an HPGe detector

%prep
%setup -q -n %{_sourcedir}/%{name}-%{version} -T -D
%if 0%{?sle_version} >= 150400
if ((`g++ -dumpversion` < 9)) ; then
   for file in `ls /usr/bin/g++-*` ; do
      if ((`${file} -dumpversion` >= 9)) ; then sed -i "s|/usr/bin/g++|${file}|" CMakeLists.txt ; break ; fi
   done
fi
%endif

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
