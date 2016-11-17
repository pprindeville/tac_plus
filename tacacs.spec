Summary: Cisco authentication/authorization/accounting daemon
Name: tacacs
Group: System Environment/Daemons
Version: F4.0.4.29b
Release: 1%{?dist}
License: MIT


%if 0%{?el6}
%global __chkconfig /sbin/chkconfig
%global __ldconfig /sbin/ldconfig
%global __service /sbin/service
%else
%global __chkconfig /usr/sbin/chkconfig
%global __ldconfig /usr/sbin/ldconfig
%global __service /usr/sbin/service
%endif

%global _hardened_build 1

URL: https://github.com/pprindeville/tac_plus/tree/dev/29a
Source: %{name}-%{version}.tar.gz
Source2: tac_plus.sysvinit
Source3: tacacs.service
Source4: tacacs.logrotate

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: bison, flex, m4, pam-devel, tcp_wrappers-devel
BuildRequires: autoconf, automake, libtool, python2-devel, systemd

Requires: pam, perl
Requires(post): ldconfig
%if 0%{?el6}
Requires(post): chkconfig
Requires(preun): chkconfig, initscripts
%else
Requires(post): systemd-units
Requires(preun): systemd-units
Requires(postun): systemd-units
%endif
Requires(postun): ldconfig


%description
Terminal server AAA (authentication, authorization, and accounting)
server originally written by Lol Grant at Cisco.

%prep -n %{name}-%{version}
%setup -n %{name}-%{version} -q


%build

# these 2 files aren't shipped with the tarball so we have
# to generate them
echo 'echo -n "%{name}"' > aconf/package.sh
%{__chmod} +x aconf/package.sh
echo 'echo -n "%{version}"' > aconf/version.sh
%{__chmod} +x aconf/version.sh

autoreconf -f -i
%configure --enable-acls --enable-uenable --enable-warn \
	   --enable-static --disable-shared \
	   --enable-stderr-timestamps

%make_build


%install
# these should be created by the RPM build environment, but aren't...
%{__install} -d %{buildroot}%{_sysconfdir}/logrotate.d/
%if !0%{?el6}
%{__install} -d %{buildroot}%{_unitdir}
%endif

%make_install
%if 0%{?el6}
%{__install} -Dp -m0755 %{SOURCE2} %{buildroot}%{_initrddir}/tac_plus
%else
%{__install} -p -m0444 %{SOURCE3} %{buildroot}%{_unitdir}/
%endif
%{__install} -p -m0644 %{SOURCE4} %{buildroot}%{_sysconfdir}/logrotate.d/%{name}


%clean
%{__rm} -rf %{buildroot}


%post
%{__ldconfig} tacacs

%if 0%{?el6}
if [ "$1" -eq 2 ]; then
    %{__service} tac_plus condrestart >/dev/null 2>&1
fi
%{__chkconfig} --add tac_plus
%else
%systemd_post tacacs.service
%endif


%preun
%if 0%{?el6}
if [ "$1" -eq 0 ]; then
    %{__service} tac_plus stop >/dev/null 2>&1
fi
%{__chkconfig} --del tac_plus
%else
%systemd_preun tacacs.service
%endif


%postun
%{__ldconfig} tacacs

%if !0%{?el6}
%systemd_postun_with_restart tacacs.service
%endif

%files

%attr(0640,root,root) %config(missingok,noreplace) %verify(not md5 size mtime) %ghost %{_sysconfdir}/tac_plus.conf
%attr(0644,root,root) %config(noreplace) %{_sysconfdir}/logrotate.d/%{name}
%license COPYING
%doc users_guide
%if 0%{?el6}
%attr(0755,root,root) %{_initrddir}/tac_plus
%else
%attr(0444,root,root) %{_unitdir}/tacacs.service
%endif
%{_bindir}/tac_pwd
%{_sbindir}/tac_plus
%{_bindir}/tac_convert
%dir %{python_sitelib}/%{name}
%attr(0755,root,root) %{python_sitelib}/%{name}/do_auth.py
%{python_sitelib}/%{name}/do_auth.py[co]
%{_mandir}/man5/tac_plus.conf.5.gz
%{_mandir}/man8/tac_pwd.8.gz
%{_mandir}/man8/tac_plus.8.gz
%ghost %{_localstatedir}/log/tac_plus.log

%exclude %{_includedir}/tacacs.h
%exclude %{_libdir}/libtacacs.a
%exclude %{_libdir}/libtacacs.la

%changelog
* Sat Nov 05 2016 Philip Prindeville <philipp@fedoraproject.org> - F4.0.4.29b-1
- Initial version

