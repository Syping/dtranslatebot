%global         _lto_cflags %{?_lto_cflags} -ffat-lto-objects

%if 0%{?rhel} && 0%{?rhel} < 8
%global         cmake %{?cmake3}
%global         cmake_build %{?cmake3_build}
%global         cmake_install %{?cmake3_install}
%global         cmake_suffix 3
%global         toolset_prefix devtoolset-9-
%endif
%if 0%{?rhel} && 0%{?rhel} == 8
%global         toolset_prefix gcc-toolset-9-
%endif
%if 0%{?suse_version} && 0%{?suse_version} < 1600
%global         toolset_version 9
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

%if 0%{?fedora} || 0%{?rhel}
BuildRequires:  %{?toolset_prefix}annobin
%if 0%{?rhel} && 0%{?rhel} < 9
BuildRequires:  epel-rpm-macros-systemd
%endif
%endif
BuildRequires:  cmake%{?cmake_suffix}
BuildRequires:  %{?toolset_prefix}gcc%{?toolset_version}-c++
BuildRequires:  make
BuildRequires:  perl(IPC::Cmd)
BuildRequires:  systemd-rpm-macros
%systemd_requires
%if 0%{?fedora} || 0%{?rhel}
%sysusers_requires_compat
%endif

%description
dtranslatebot is a Discord Bot which translate incoming Discord messages to Discord webhooks.

%prep
%setup -q

%build
%if 0%{?rhel} && 0%{?rhel} < 8
source /opt/rh/devtoolset-9/enable
%endif
%if 0%{?rhel} && 0%{?rhel} == 8
source /opt/rh/gcc-toolset-9/enable
%endif
%cmake \
  -DCMAKE_BUILD_TYPE=Release \
%if 0%{?toolset_version}
  -DCMAKE_C_COMPILER=gcc-%{toolset_version} \
  -DCMAKE_CXX_COMPILER=g++-%{toolset_version} \
%endif
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
%if 0%{?fedora} || 0%{?rhel}
%sysusers_create_compat %{SOURCE3}
%endif
%if 0%{?suse_version}
%sysusers_create_package %{name} %{SOURCE3}
%endif

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
