Name: stylom
Version: 1.00
Release: 1%{?dist}
Summary: Command line utility for stylometric text analysis
License: GPL3
URL: https://github.com/fuzzgun/stylom
Packager: Bob Mottram (4096 bits) <bob@robotics.uk.to>
Source0: http://yourdomainname.com/src/%{name}_%{version}.orig.tar.gz
Group: Utility/ConsoleOnly

%description
Tool to facilitate the stylometric analysis of texts. It could be used for
academic disambiguation of disputed authorship, and to help identify
plagiarists, astroturfers, sockpuppets and guerilla marketers. Another
possible use case is as an assistance to the anonymisation of writing
style.  Every author has their own unique writing style, and if enough
writing examples are available then it's possible to construct a
quantitative model of their style which can be compared against others.

%prep
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}
mkdir -p %{buildroot}/etc
mkdir -p %{buildroot}/etc/%{name}
mkdir -p %{buildroot}/usr
mkdir -p %{buildroot}/usr/bin
mkdir -p %{buildroot}/usr/share
mkdir -p %{buildroot}/usr/share/man
mkdir -p %{buildroot}/usr/share/man/man1
# Make install but to the RPM BUILDROOT directory
make install -B DESTDIR=%{buildroot}

%files
%doc README.md LICENSE
%defattr(-,root,root,-)
%{_bindir}/*
%{_mandir}/man1/*

%changelog
* Sun Jun 30 2013  Bob Mottram (4096 bits) <bob@robotics.uk.to>
- Spec file created
