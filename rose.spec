Summary: Foobar
Name: @NAME@
Version: @VER@
Release: 0
Copyright: BSD
Group: Productivity/Networking
Source0: https://outreach.scidac.gov/frs/download.php/45/rose-0.9.3a-source-with-EDG-binary-x86-64-gcc422-1303.tar.gz
Source1: https://outreach.scidac.gov/frs/download.php/46/rose-0.9.3a-source-with-EDG-binary-i386-gcc422-1303.tar.gz
URL: http://www.roseCompiler.org/
BuildRoot: %{_tmppath}/%{name}-root
Prefix: %{_prefix}

%description
ROSE is an open source compiler infrastructure to build source-to-source
program transformation and analysis tools for large-scale Fortran 77/95/2003,
C, C++, OpenMP, and UPC applications. The intended users of ROSE could be
either experienced compiler researchers or library and tool developers who may
have minimal compiler experience. ROSE is particularly well suited for building
custom tools for static analysis, program optimization, arbitrary program
transformation, domain-specific optimizations, complex loop optimizations,
performance analysis, and cyber-security.

%prep
%setup

%build
%configure
make

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

%doc COPYRIGHT README

%changelog
