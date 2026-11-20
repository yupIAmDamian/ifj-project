# syntax=docker/dockerfile:1
# Copy this file into the root of an extracted IFJ26 submission, then run:
#   docker build -t ifj26-project .
#   docker run --rm -i ifj26-project < program.ifj > program.ifjcode
# The common toolchain can also be built with --target build-environment.

ARG DEBIAN_IMAGE=debian:trixie-slim@sha256:a99cfc517144bc59b1978475ec53b46ecabec7e43635402ee5b77cc54cd1b20a

# Stage 1: shared environment; no submission files affect these layers.
FROM ${DEBIAN_IMAGE} AS build-environment

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        clang-19 \
        clang-format \
        clang-tidy \
        cmake \
        cppcheck \
        curl \
        git \
        libclang-rt-19-dev \
        libgmp10 \
        libmpc3 \
        libmpfr6 \
        libzstd1 \
        zlib1g \
    && rm -rf /var/lib/apt/lists/*

# LLVM provides Clang 23 packages built for Trixie. Scope trust to this repo.
RUN curl --fail --show-error --silent --location --retry 3 \
        https://apt.llvm.org/llvm-snapshot.gpg.key \
        --output /usr/share/keyrings/llvm.asc \
    && printf '%s\n' \
        'deb [signed-by=/usr/share/keyrings/llvm.asc] https://apt.llvm.org/trixie/ llvm-toolchain-trixie-23 main' \
        > /etc/apt/sources.list.d/llvm.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends clang-23 libclang-rt-23-dev \
    && rm -rf /var/lib/apt/lists/*

# Import the official Trixie GCC installations, keeping versions isolated.
# External COPY sources do not add build stages. The digests pin the images.
COPY --from=gcc:14.4.0-trixie@sha256:9188ac751ca24431dc43dbd142a223c98ea74f01d2858e84d30ba342a0d67844 /usr/local/ /opt/gcc-14.4/
COPY --from=gcc:15.3.0-trixie@sha256:ead103e6d03b69232962d467f3520c3f70b6718c69ff71efcc08efe9011fadb6 /usr/local/ /opt/gcc-15.3/
COPY --from=gcc:16.2.0-trixie@sha256:ef558a40d1f13115293feee01526dbdb9aaad7c9c5a00da05f471ce042e855c1 /usr/local/ /opt/gcc-16.2/

# Merlin paths observed on 2026-10-05. /usr/bin/gcc remains Debian GCC 14;
# PATH selects /usr/local/bin/gcc (15.3) and /usr/bin/clang (19).
RUN set -eu; \
    for version in 14.4 15.3 16.2; do \
        for tool in gcc g++ cpp gcc-ar gcc-nm gcc-ranlib gcov gcov-dump gcov-tool; do \
            ln -s "/opt/gcc-${version}/bin/${tool}" "/usr/local/bin/${tool}-${version}"; \
        done; \
        ln -s "gcc-${version}" "/usr/local/bin/x86_64-linux-gcc-${version}"; \
        ln -s "gcc-${version}" "/usr/local/bin/x86_64-linux-gcc-${version}.0"; \
    done; \
    for tool in gcc g++ cpp gcc-ar gcc-nm gcc-ranlib gcov gcov-dump gcov-tool; do \
        ln -s "${tool}-15.3" "/usr/local/bin/${tool}"; \
    done; \
    ln -s gcc /usr/local/bin/cc; \
    ln -s /usr/lib/llvm-19 /usr/local/share/Clang-19.0; \
    ln -s /usr/lib/llvm-23 /usr/local/share/Clang-23.0; \
    ln -s Clang-23.0 /usr/local/share/Clang; \
    ln -s ../lib/llvm-19/bin/clang /usr/bin/clang; \
    ln -s ../lib/llvm-19/bin/clang++ /usr/bin/clang++; \
    printf '%s\n' /opt/gcc-16.2/lib64 /opt/gcc-15.3/lib64 /opt/gcc-14.4/lib64 \
        > /etc/ld.so.conf.d/000-gcc.conf; \
    ldconfig; \
    install -d -o 1000 -g 1000 /build /out /out/lib

ENV PATH="/usr/local/bin:/usr/bin:/bin:/usr/local/share/Clang/bin"
WORKDIR /build

# Stage 2: build the submitted Makefile's default target as an ordinary user.
FROM build-environment AS build
COPY --chown=1000:1000 . .
USER 1000:1000

# The executable may have any name. Find ELF executables (including PIE),
# excluding optional tests; executable scripts and object files do not count.
# Leave make's compiler, flags, target and job count to the submitted Makefile.
# Use line continuations: the legacy builder may silently discard a heredoc.
# Collect shared libraries for dynamic executables; static ones need none.
RUN set -eu; \
    make; \
    find . -path ./tests -prune -o -type f -perm /111 -print > /out/candidates; \
    count=0; \
    binary=; \
    while IFS= read -r candidate; do \
        if readelf -h "$candidate" 2>/dev/null | grep -Eq 'Type:.*(EXEC|DYN)' \
            && readelf -h "$candidate" 2>/dev/null | grep -Eq 'Entry point address:.*0x[1-9a-fA-F]'; then \
            count=$((count + 1)); \
            binary=$candidate; \
        fi; \
    done < /out/candidates; \
    if [ "$count" -ne 1 ]; then \
        printf 'Expected exactly one built executable outside tests/, found %s\n' "$count" >&2; \
        exit 1; \
    fi; \
    install -m 0755 "$binary" /out/compiler; \
    if readelf -l /out/compiler | grep -q INTERP; then \
        ldd /out/compiler > /out/dependencies; \
        if grep -q 'not found' /out/dependencies; then \
            cat /out/dependencies >&2; \
            exit 1; \
        fi; \
        awk '/=> \// { print $3 }' /out/dependencies > /out/libraries; \
        while IFS= read -r library; do \
            cp -L "$library" /out/lib/; \
        done < /out/libraries; \
    fi

# Stage 3: Debian runtime with the compiler and its libraries, without tools.
FROM ${DEBIAN_IMAGE} AS runtime
WORKDIR /app
COPY --from=build /out/compiler /app/compiler
COPY --from=build /out/lib/ /app/lib/
ENV LD_LIBRARY_PATH=/app/lib
USER 65532:65532
ENTRYPOINT ["/app/compiler"]
