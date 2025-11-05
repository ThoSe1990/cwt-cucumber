FROM ubuntu:latest

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    git \
    python3 

RUN apt install -y pipx && pipx install conan && pipx install cmake 

ENV PATH=/root/.local/bin:$PATH

RUN apt-get update && \
    apt-get install -y \
    software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y \
    g++-13 \
    clang-17 \
    llvm-17 \
    libclang-17-dev && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 && \
    update-alternatives --install /usr/bin/cc cc /usr/bin/clang-17 100 && \
    update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-17 100 

RUN mkdir -p /root/.conan2/profiles && \
    echo "[settings]" > /root/.conan2/profiles/clang && \
    echo "arch=x86_64" >> /root/.conan2/profiles/clang && \
    echo "build_type=Release" >> /root/.conan2/profiles/clang && \
    echo "compiler=clang" >> /root/.conan2/profiles/clang && \
    echo "compiler.cppstd=gnu20" >> /root/.conan2/profiles/clang && \
    echo "compiler.libcxx=libstdc++11" >> /root/.conan2/profiles/clang && \
    echo "compiler.version=17" >> /root/.conan2/profiles/clang && \
    echo "os=Linux" >> /root/.conan2/profiles/clang && \
    echo "[settings]" > /root/.conan2/profiles/gcc && \
    echo "arch=x86_64" >> /root/.conan2/profiles/gcc && \
    echo "build_type=Release" >> /root/.conan2/profiles/gcc && \
    echo "compiler=gcc" >> /root/.conan2/profiles/gcc && \
    echo "compiler.cppstd=gnu20" >> /root/.conan2/profiles/gcc && \
    echo "compiler.libcxx=libstdc++11" >> /root/.conan2/profiles/gcc && \
    echo "compiler.version=13.2" >> /root/.conan2/profiles/gcc && \
    echo "os=Linux" >> /root/.conan2/profiles/gcc && \
    cp /root/.conan2/profiles/gcc /root/.conan2/profiles/default

CMD ["/bin/bash"]
