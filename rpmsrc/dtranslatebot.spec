%global         _lto_cflags %{?_lto_cflags} -ffat-lto-objects

%if 0%{?rhel} && 0%{?rhel} < 8
%global         cmake %{?cmake3}
%global         cmake_build %{?cmake3_build}
%global         cmake_install %{?cmake3_install}
%endif

Name:           dtranslatebot
Version:        0.2.0
Release:        1%{?dist}
Summary:        Discord Translation Bot
License:        BSD-2-Clause
URL:            https://github.com/Syping/%{name}
Source0:        %{name}-%{version}.tar.gz
Source1:        %{name}.json
Source2:        %{name}.service
Source3:        %{name}.sysusersd

BuildRequires:  make
BuildRequires:  perl
BuildRequires:  perl-IPC-Cmd
%if 0%{?rhel} && 0%{?rhel} < 8
BuildRequires:  cmake3 >= 3.16
BuildRequires:  devtoolset-9-annobin
BuildRequires:  devtoolset-9-gcc-c++
%else
%if 0%{?rhel} && 0%{?rhel} == 8
BuildRequires:  cmake >= 3.16
BuildRequires:  gcc-toolset-9-annobin
BuildRequires:  gcc-toolset-9-gcc-c++
%else
BuildRequires:  cmake >= 3.16
BuildRequires:  annobin
BuildRequires:  gcc-c++
%endif
%endif
%if 0%{?rhel} && 0%{?rhel} < 9
BuildRequires:  epel-rpm-macros-systemd
%endif
BuildRequires:  systemd-rpm-macros
%{?systemd_requires}
%{?sysusers_requires_compat}

%description
dtranslatebot is a Discord Bot which translate incoming Discord messages to Discord webhooks.

%prep
%setup -q

%build
%if 0%{?rhel} && 0%{?rhel} < 8
source /opt/rh/devtoolset-9/enable
%else
%if 0%{?rhel} && 0%{?rhel} == 8
source /opt/rh/gcc-toolset-9/enable
%endif
%endif
%cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DWITH_DPP_STATIC_BUNDLE=TRUE \
  -DWITH_SYSTEMD=TRUE
%cmake_build

%install
%cmake_install
mkdir -p %{buildroot}%{_localstatedir}/lib/%{name}
install -p -D -m 0644 %{SOURCE1} %{buildroot}%{_sysconfdir}/%{name}.json
install -p -D -m 0644 %{SOURCE2} %{buildroot}%{_unitdir}/%{name}.service
install -p -D -m 0644 %{SOURCE3} %{buildroot}%{_sysusersdir}/%{name}.conf

%pre
%sysusers_create_compat %{SOURCE3}

%post
%systemd_post %{name}.service

%preun
%systemd_preun %{name}.service

%postun
%systemd_postun_with_restart %{name}.service

%files
%{_bindir}/%{name}
%{_unitdir}/%{name}.service
%{_sysusersdir}/%{name}.conf
%config(noreplace) %attr(0640,root,%{name}) %{_sysconfdir}/%{name}.json
%dir %attr(0750,%{name},%{name}) %{_localstatedir}/lib/%{name}
