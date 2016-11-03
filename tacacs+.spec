Summary: TACACS+ Daemon
Name: tacacs+
Group: Networking/Servers
Version: F4.0.4.29
Release: 2%{?dist}
License: Cisco

Packager: Bruce Carleton <bruce.carleton@jasperwireless.com>
Vendor: Cisco

%global name2 %(eval echo "%{name}" | %{__sed} -e 's/+$//')

Source: %{name2}-%{version}.tar.gz
Source2: tac_plus.sysvinit
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: gcc, bison, flex, m4, pam-devel, tcp_wrappers-devel
Requires: pam


%description

%prep -n %{name2}-%{version}
%setup -n %{name2}-%{version}

%build

# these 2 files aren't shipped with the tarball so we have
# to generate them
echo 'echo -n "%{name2}"' > aconf/package.sh
%{__chmod} +x aconf/package.sh
echo 'echo -n "%{version}"' > aconf/version.sh
%{__chmod} +x aconf/version.sh

%configure --enable-acls --enable-uenable
%{__make}

%install
%{__rm} -rf %{buildroot}
%makeinstall
%{__install} -Dp -m0755 %{SOURCE2} %{buildroot}%{_initrddir}/tac_plus
%{__mkdir} %{buildroot}%{_sbindir}
%{__mv} %{buildroot}%{_bindir}/tac_plus %{buildroot}%{_sbindir}
### Clean up buildroot
%{__rm} -f %{buildroot}%{_infodir}/dir

%post

%preun

%clean
%{__rm} -rf %{buildroot}

%files

%{_includedir}/tacacs.h
%{_bindir}/tac_pwd
%{_sbindir}/tac_plus
%{_datadir}/tacacs+/users_guide
%{_datadir}/tacacs+/tac_convert
%{_datadir}/tacacs+/do_auth.py
%{_datadir}/tacacs+/do_auth.pyc
%{_datadir}/tacacs+/do_auth.pyo
%{_mandir}/man5/tac_plus.conf.5.gz
%{_mandir}/man8/tac_pwd.8.gz
%{_mandir}/man8/tac_plus.8.gz
%{_libdir}/libtacacs.so.1.0.0
%{_libdir}/libtacacs.so.1
%{_libdir}/libtacacs.so
%{_libdir}/libtacacs.a
%{_libdir}/libtacacs.la
%attr(0755,root,root) %{_initrddir}/tac_plus

%changelog
