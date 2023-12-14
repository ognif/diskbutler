FROM ubuntu:20.04
# we need at least glibc 2.2.9!
MAINTAINER Dan Bryant (daniel.bryant@linux.com)

ENV TZ=Europe/London
ENV DEBIAN_FRONTEND=noninteractive 

# install basic dependencies for Linux build
RUN apt-get update
RUN apt-get install -y nano software-properties-common apt-transport-https build-essential g++-multilib libc6-dev \
  gcc git patch python3 python3-pip curl wget pkg-config mesa-common-dev freeglut3-dev libasound2 libfontconfig1 \
  libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-render-util0 libxcb-shape0 libxcb-xinerama0 libxcb-xkb1 \
  libxkbcommon-x11-0 libegl1 libxdamage1 libxrender1 libxkbfile1 libxrandr2

# common dependencies
RUN mkdir -p /usr/local/src
RUN mkdir -p /opt/output

# Qt5
RUN python3 -m pip install --upgrade pip setuptools wheel
RUN pip3 install py7zr aqtinstall pathlib
RUN cd /usr/local/bin && wget -q https://github.com/justdan96/7zip_static/releases/download/23.01/7zz && chmod +x 7zz
RUN cd /usr/local && aqt install-qt linux desktop 5.15.2 --external 7zz
RUN mv /usr/local/5.15.2/gcc_64 /usr/local/qt
ENV PATH="/usr/local/qt/bin:$PATH"
ENV CPATH=/usr/local/qt/include
ENV CMAKE_PREFIX_PATH=/usr/local/qt/lib/cmake
ENV Qt5_DIR=/usr/local/qt/lib/cmake/Qt5
RUN echo /usr/local/qt/lib >> /etc/ld.so.conf
RUN apt-get update && apt-get upgrade -y

# install linuxdeploy and the Qt plugin
RUN curl -sLo /usr/local/bin/linuxdeploy-x86_64.AppImage "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
RUN curl -sLo /usr/local/bin/linuxdeploy-plugin-qt-x86_64.AppImage "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
RUN chmod +x /usr/local/bin/linuxdeploy-x86_64.AppImage
RUN chmod +x /usr/local/bin/linuxdeploy-plugin-qt-x86_64.AppImage

RUN curl --proto '=https' --tlsv1.2 -sSf https://just.systems/install.sh | bash -s -- --to /usr/local/bin

# fix for issue of linuxdeploy in Docker containers
RUN dd if=/dev/zero of=/usr/local/bin/linuxdeploy-x86_64.AppImage conv=notrunc bs=1 count=3 seek=8
RUN dd if=/dev/zero of=/usr/local/bin/linuxdeploy-plugin-qt-x86_64.AppImage conv=notrunc bs=1 count=3 seek=8
