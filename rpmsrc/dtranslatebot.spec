%global         _lto_cflags %{?_lto_cflags} -ffat-lto-objects
%global         dtb_cmake_args -DCMAKE_BUILD_TYPE=Release -DWITH_DPP_STATIC_BUNDLE=TRUE -DWITH_SYSTEMD=TRUE

Name:           dtranslatebot
Version:        0.2.0
Release:        1%{?dist}
Summary:        Discord Translation Bot
License:        BSD-2-Clause
URL:            https://github.com/Syping/%{name}
Source0:        %{name}_%{version}.tar.gz
Source1:        %{name}.json
Source2:        %{name}.service
Source3:        %{name}.sysusersd

BuildRequires:  cmake >= 3.16
BuildRequires:  perl
%if 0%{?rhel} && 0%{?rhel} < 9
BuildRequires:  gcc-toolset-9-annobin
BuildRequires:  gcc-toolset-9-gcc-c++
%else
BuildRequires:  annobin
BuildRequires:  gcc-c++
%endif
%if 0%{?rhel}
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
%if 0%{?rhel} && 0%{?rhel} < 9
source /opt/rh/gcc-toolset-9/enable
%endif
%cmake %{?dtb_cmake_args}
%if 0%{?cmake_build:1}
%cmake_build
%else
%make_build
%endif

%install
%if 0%{?cmake_install:1}
%cmake_install
%else
%make_install
%endif
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
