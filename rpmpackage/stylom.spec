#
# rpm spec for stylom
#

%define        __spec_install_post %{nil}
%define          debug_package %{nil}
%define        __os_install_post %{_dbpath}/brp-compress

Summary: A command line utility for stylometric text analysis.
Name: stylom
Version: 1.00
Release: 1
License: GPL
Group: Utilities
SOURCE0 : %{name}-%{version}.tar.gz
URL: http://launchpad.net/stylom/
Packager: Bob Mottram <bob@robotics.uk.to>
Requires: gnuplot, libpng
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
This is a tool to facilitate the stylometric analysis of texts.  It could be used for academic disambiguation of disputed authorship, and to help identify plagiarists, astroturfers, sockpuppets and guerilla marketers.  Another possible use case is as an assistance to the anonymisation of writing style.

Every author has their own unique writing style, and if enough writing examples are available then it is possible to construct a quantitative model of their style which can be compared against others.

%prep
%setup -q

%build
 # Empty section.

%install
rm -rf %{buildroot}
mkdir -p  %{buildroot}

# in builddir
cp -a * %{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%config(noreplace) %{_sysconfdir}/%{name}/%{name}.conf
%{_bindir}/*
%attr(644,root,root) /usr/share/man/man1/stylom.1.gz

%changelog
* Tue Jan 15 2013  Bob Mottram <bob@robotics.uk.to> 0.1-1
- Spec file created

