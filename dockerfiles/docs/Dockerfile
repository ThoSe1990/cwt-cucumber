FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

RUN apt-get update && apt-get install -y \
    python3 python3-pip doxygen graphviz git locales \
 && locale-gen en_US.UTF-8 \
 && rm -rf /var/lib/apt/lists/*

RUN pip install --break-system-packages --no-cache-dir \
  pyyaml \
  sphinx \
  sphinx_rtd_theme \
  breathe \
  sphinx-js \
  linuxdoc


CMD ["bash"]
