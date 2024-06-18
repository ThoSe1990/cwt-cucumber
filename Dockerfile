# Use the official Ubuntu image from Docker Hub
FROM ubuntu:24.04

# Install necessary packages
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    git \
    python3 

RUN apt install -y pipx
RUN pipx ensurepath

RUN pipx install conan 
RUN pipx install cmake 

ENV PATH=/root/.local/bin:$PATH

# Install GCC compiler for C++20 support
RUN apt-get install -y \
    software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y \
    g++-13 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100

RUN apt-get install -y \
    clang-17 \
    llvm-17 \
    libclang-17-dev \
    && update-alternatives --install /usr/bin/cc cc /usr/bin/clang-17 100 \
    && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-17 100 

RUN mkdir -p /root/.conan2/profiles

RUN echo "[settings]" >> /root/.conan2/profiles/clang
RUN echo "arch=x86_64" >> /root/.conan2/profiles/clang
RUN echo "build_type=Release" >> /root/.conan2/profiles/clang
RUN echo "compiler=clang" >> /root/.conan2/profiles/clang
RUN echo "compiler.cppstd=gnu20" >> /root/.conan2/profiles/clang
RUN echo "compiler.libcxx=libstdc++11" >> /root/.conan2/profiles/clang
RUN echo "compiler.version=17" >> /root/.conan2/profiles/clang
RUN echo "os=Linux" >> /root/.conan2/profiles/clang

RUN echo "[settings]" >> /root/.conan2/profiles/gcc
RUN echo "arch=x86_64" >> /root/.conan2/profiles/gcc
RUN echo "build_type=Release" >> /root/.conan2/profiles/gcc
RUN echo "compiler=gcc" >> /root/.conan2/profiles/gcc
RUN echo "compiler.cppstd=gnu20" >> /root/.conan2/profiles/gcc
RUN echo "compiler.libcxx=libstdc++11" >> /root/.conan2/profiles/gcc
RUN echo "compiler.version=13.2" >> /root/.conan2/profiles/gcc
RUN echo "os=Linux" >> /root/.conan2/profiles/gcc

RUN cp /root/.conan2/profiles/gcc /root/.conan2/profiles/default

# RUN conan profile detect
# RUN sed -i -e 's/compiler.cppstd=gnu17/compiler.cppstd=gnu20/g' /root/.conan2/profiles/default


# RUN git clone https://github.com/ThoSe1990/cwt-cucumber-conan.git

# RUN conan create ./cwt-cucumber-conan/package --version 1.2.2 --user cwt --channel stable

# RUN conan install ./conan/with-cucumber.txt -of ./build_docker --build missing 
# RUN cmake -S . -B ./build_docker -DCMAKE_TOOLCHAIN_FILE="./build_docker/conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=Release
# RUN cmake --build ./build_docker

CMD ["bash"]