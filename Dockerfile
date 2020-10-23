FROM ubuntu:20.04

ARG OPENCV_VERSION=4.4.0

ARG DEBIAN_FRONTEND=noninteractive
# Install updates & requirements:
#  * git, openssh-client, ca-certificates - clone & build
#  * locales, sudo - useful to set utf-8 locale & sudo usage
#  * curl - to download Qt bundle
#  * build-essential, pkg-config, libgl1-mesa-dev - basic Qt build requirements
#  * libsm6, libice6, libxext6, libxrender1, libfontconfig1, libdbus-1-3 - dependencies of the Qt bundle run-file
RUN apt-get update && apt-get full-upgrade -y && apt-get install -y --no-install-recommends \
    git \
    openssh-client \
    ca-certificates \
    locales \
    sudo \
    curl \
    build-essential \
    pkg-config \
    libgl1-mesa-dev \
    libsm6 \
    libice6 \
    libxext6 \
    libxrender1 \
    libxkbcommon-x11-0 \
    libfontconfig1 \
    libdbus-1-3 \
    && apt-get -qq clean \
    && rm -rf /var/lib/apt/lists/*
RUN apt-get update \
  && apt-get install -y -qq --no-install-recommends \
    libglvnd0 \
    libgl1 \
    libglx0 \
    libegl1 \
    libxext6 \
    libx11-6 \
    libx11-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    freeglut3-dev \
    mesa-common-dev \
    libxrandr-dev \
    libxi-dev \
    libglew-dev \
    libsdl2-dev \
    libsdl2-image-dev \
    libglm-dev \ 
    libfreetype6-dev \
    libglfw3 \
    libglfw3-dev \
    libgles2 \
  && rm -rf /var/lib/apt/lists/*
# start to install QT and QTcreator
RUN apt-get update && apt-get full-upgrade -y && apt-get install -y --no-install-recommends \
    mesa-common-dev \
    libglx-dev \
    libgl-dev \
    && rm -rf /var/lib/apt/lists/*
RUN apt-get update && apt-get install -y --no-install-recommends \
	libxau6 \
	libxdmcp6 \
	libxcb1 \
	libxext6 \
	libx11-6 \
    && rm -rf /var/lib/apt/lists/*
RUN apt-get update && apt-get full-upgrade -y && apt-get install -y --no-install-recommends \
    qtcreator \
    qt5-default \
    qt5-doc \
    qtbase5-examples \
    qtbase5-doc-html \
    && rm -rf /var/lib/apt/lists/*
RUN apt-get update && apt-get install -y\
    pkg-config \
    build-essential \
    cmake \
    vim \
    clang-format \
    ca-certificates \
    curl tar unzip \
    zip && \
    rm -rf /var/lib/apt/lists/*
RUN apt-get update && \
    apt-get install -y software-properties-common \
    libgtk2.0-dev  && \
    rm -rf /var/lib/apt/lists/*
RUN apt-get install libgtk2.0-dev
RUN set -ex \
    && apt-get -qq update \
    && apt-get -qq install -y --no-install-recommends software-properties-common \
    && add-apt-repository "deb http://security.ubuntu.com/ubuntu xenial-security main" \
    && apt-get -qq install -y --no-install-recommends \
        build-essential cmake \
        wget unzip \
        libhdf5-103 libhdf5-dev \
        libopenblas0 libopenblas-dev \
        libprotobuf17 libprotobuf-dev \
        libjpeg8 libjpeg8-dev \
        libpng16-16 libpng-dev \
        libtiff5 libtiff-dev \
        libwebp6 libwebp-dev \
        libjasper1 libjasper-dev \
        libtbb2 libtbb-dev \
        libeigen3-dev \
        tesseract-ocr tesseract-ocr-por libtesseract-dev \
        python3-numpy python3-dev \
    && wget -q https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip -O opencv.zip \
    && wget -q https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip -O opencv_contrib.zip \
    && unzip -qq opencv.zip -d /opt && rm -rf opencv.zip \
    && unzip -qq opencv_contrib.zip -d /opt && rm -rf opencv_contrib.zip \
    && cmake \
        -D CMAKE_BUILD_TYPE=RELEASE \
        -D CMAKE_INSTALL_PREFIX=/usr/local \
        -D OPENCV_EXTRA_MODULES_PATH=/opt/opencv_contrib-${OPENCV_VERSION}/modules \
        -D EIGEN_INCLUDE_PATH=/usr/include/eigen3 \
        -D OPENCV_ENABLE_NONFREE=ON \
        -D WITH_JPEG=ON \
        -D WITH_PNG=ON \
        -D WITH_TIFF=ON \
        -D WITH_WEBP=ON \
        -D WITH_JASPER=ON \
        -D WITH_EIGEN=ON \
        -D WITH_TBB=ON \
        -D WITH_LAPACK=ON \
        -D WITH_PROTOBUF=ON \
        -D WITH_V4L=OFF \
        -D WITH_GSTREAMER=OFF \
        -D WITH_GTK=ON \
        -D WITH_QT=OFF \
        -D WITH_CUDA=OFF \
        -D WITH_VTK=OFF \
        -D WITH_OPENEXR=OFF \
        -D WITH_FFMPEG=OFF \
        -D WITH_OPENCL=OFF \
        -D WITH_OPENNI=OFF \
        -D WITH_XINE=OFF \
        -D WITH_GDAL=OFF \
        -D WITH_IPP=OFF \
        -D BUILD_OPENCV_PYTHON3=ON \
        -D BUILD_OPENCV_PYTHON2=OFF \
        -D BUILD_OPENCV_JAVA=OFF \
        -D BUILD_TESTS=OFF \
        -D BUILD_IPP_IW=OFF \
        -D BUILD_PERF_TESTS=OFF \
        -D BUILD_EXAMPLES=OFF \
        -D BUILD_ANDROID_EXAMPLES=OFF \
        -D BUILD_DOCS=OFF \
        -D BUILD_ITT=OFF \
        -D INSTALL_PYTHON_EXAMPLES=OFF \
        -D INSTALL_C_EXAMPLES=OFF \
        -D INSTALL_TESTS=OFF \
        /opt/opencv-${OPENCV_VERSION} \
    && make -j$(nproc) \
    && make install \
    && rm -rf /opt/build/* \
    && rm -rf /opt/opencv-${OPENCV_VERSION} \
    && rm -rf /opt/opencv_contrib-${OPENCV_VERSION} \
    && apt-get -qq remove -y \
        software-properties-common \
        build-essential cmake \
        libhdf5-dev \
        libopenblas-dev \
        libprotobuf-dev \
        libjpeg8-dev \
        libpng-dev \
        libtiff-dev \
        libwebp-dev \
        libjasper-dev \
        libtbb-dev \
        libtesseract-dev \
        python3-dev \
    && apt-get -qq autoremove \
    && apt-get -qq clean
# for openCV to draw picture
RUN apt-get install -y --reinstall libgtk2.0-0
RUN apt-get install -y libcanberra-gtk-module libcanberra-gtk3-module
RUN apt-get install -y cmake
RUN apt-get install -y build-essential
# nvidia-container-runtime
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute

WORKDIR /home/project