# Build dtranslatebot
FROM alpine:3.23 AS build
RUN apk add --no-cache \
    clang \
    cmake \
    curl-dev \
    git \
    libc++-dev \
    llvm-libunwind-dev \
    ninja-build \
    openssl-dev \
    zlib-dev
WORKDIR /build
COPY CMakeLists.txt dtranslatebot/
COPY cmake/ dtranslatebot/cmake/
COPY src/ dtranslatebot/src/
ENV CC=clang CXX=clang++
ENV DPP_VERSION=10.1.4
RUN git clone https://github.com/brainboxdotcc/DPP.git --branch "v$DPP_VERSION" --depth=1
RUN cmake \
    -DAVX_TYPE=AVX0 \
    -DBUILD_VOICE_SUPPORT=OFF \
    -DDPP_NO_CORO=ON \
    -DDPP_NO_VCPKG=ON \
    -DRUN_LDCONFIG=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS=-stdlib=libc++ \
    -DCMAKE_INSTALL_PREFIX=/opt/dtranslatebot \
    -DCMAKE_MAKE_PROGRAM=/usr/lib/ninja-build/bin/ninja \
    -G Ninja \
    -B DPP-build \
    DPP
RUN cmake --build DPP-build
RUN cmake --install DPP-build --strip
RUN cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS=-stdlib=libc++ \
    -DCMAKE_INSTALL_PREFIX=/opt/dtranslatebot \
    -DCMAKE_INSTALL_RPATH=/opt/dtranslatebot/lib \
    -DCMAKE_MAKE_PROGRAM=/usr/lib/ninja-build/bin/ninja \
    -G Ninja \
    -B dtranslatebot-build \
    dtranslatebot
RUN cmake --build dtranslatebot-build
RUN cmake --install dtranslatebot-build --strip

# Create the dtranslatebot Container
FROM alpine:3.23
RUN apk add --no-cache \
    libc++ \
    libcrypto3 \
    libcurl \
    libssl3 \
    llvm-libunwind \
    zlib
COPY --from=build /opt/dtranslatebot/bin/dtranslatebot /opt/dtranslatebot/bin/
COPY --from=build /opt/dtranslatebot/lib/*.so /opt/dtranslatebot/lib/
COPY --from=build /opt/dtranslatebot/lib/*.so.* /opt/dtranslatebot/lib/
RUN adduser --disabled-password dtranslatebot
WORKDIR /home/dtranslatebot
USER dtranslatebot
ENTRYPOINT ["/opt/dtranslatebot/bin/dtranslatebot", "--wait-for-translator", "/home/dtranslatebot/dtranslatebot.json"]
