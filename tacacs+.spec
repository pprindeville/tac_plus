Summary: TACACS+ Daemon
Name: tacacs+
Group: Networking/Servers
Version: F4.0.4.29
Release: 2%{?dist}
License: Cisco

Packager: Bruce Carleton <bruce.carleton@jasperwireless.com>
Vendor: Cisco

Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: gcc, bison, flex, m4, pam-devel, tcp_wrappers-devel
Requires: pam


%description

%prep
%setup

%{__cat} <<'EOF' >tac_plus.sysvinit
#!/bin/bash
#
# /etc/rc.d/init.d/tac_plus
#
# chkconfig: 2345 86 14
# description: TACACS+ Daemon

# Define variables
TACPLUS_PID=/var/run/tac_plus.pid
TACPLUS_EXE=/usr/sbin/tac_plus
TACPLUS_ARG=""
TACPLUS_CNF=/etc/tac_plus.conf

# Source function library.
. /etc/rc.d/init.d/functions

case "$1" in
start)
# Check to see if tac_plus is running.
if [[ -f ${TACPLUS_PID} || -f /var/lock/subsys/tac_plus ]]; then
	echo "tac_plus may already be running. Check for existing tac_plus processes."
	exit 1
fi
echo -n "Starting tac_plus:"
$TACPLUS_EXE $TACPLUS_ARG -C $TACPLUS_CNF && success || failure
echo
touch /var/lock/subsys/tac_plus
;;
stop)
if [[ -f ${TACPLUS_PID} && -f /var/lock/subsys/tac_plus ]]; then
	echo -n "Stopping tac_plus:"
	killproc -p ${TACPLUS_PID}
	echo
	rm -f /var/lock/subsys/tac_plus
	rm -f ${TACPLUS_PID}
else
	echo "tac_plus does not appear to be running."
fi
;;
status)
if [[ -f ${TACPLUS_PID} && -f /var/lock/subsys/tac_plus ]]; then
       echo "tac_plus pid is `cat ${TACPLUS_PID}`"
else
        echo "tac_plus does not appear to be running."
fi
;;
restart)
$0 stop; $0 start
;;
reload)
echo -n "Reloading tac_plus..."
if [[ -f ${TACPLUS_PID} && -f /var/lock/subsys/tac_plus ]]; then
	kill -HUP `cat ${TACPLUS_PID}`
	RETVAL=$?
fi
if [ $RETVAL -ne 0 ]; then
	failure
else
	success
fi
echo

;;
*)
echo "Usage: $0 {start|stop|status|reload|restart}"
exit 1
;;
esac
EOF

%build
%configure --enable-acls --enable-uenable
%{__make}

%install
%{__rm} -rf %{buildroot}
%makeinstall
%{__install} -Dp -m0755 tac_plus.sysvinit %{buildroot}%{_initrddir}/tac_plus
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
