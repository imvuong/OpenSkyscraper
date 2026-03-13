FROM ubuntu:20.04

ARG VNC_PASSWORD=0123456789
ENV VNC_PASSWORD=${VNC_PASSWORD}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates \
        build-essential \
        cmake \
        doxygen \
        graphviz \
        libsfml-dev \
        libfreetype-dev \
        libmspack-dev \
        git \
        vim \
        wget \
        curl \
        xvfb \
        x11vnc \
        fluxbox \
        feh \
        dbus-x11 \
        gnupg \
        apt-transport-https \
        software-properties-common \
        websockify \
        libxv1 \
        libglu1-mesa \
        xauth \
        x11-utils \
        xterm \
        xorg \
        tightvncserver \
        openssl && \
    update-ca-certificates && \
    rm -rf /var/lib/apt/lists/*

# TurboVNC repository
RUN wget -q -O- https://packagecloud.io/dcommander/turbovnc/gpgkey | gpg --dearmor >/etc/apt/trusted.gpg.d/TurboVNC.gpg && \
    wget -P /etc/apt/sources.list.d/ https://raw.githubusercontent.com/TurboVNC/repo/main/TurboVNC.list

# VirtualGL repository
RUN wget -q -O- https://packagecloud.io/dcommander/virtualgl/gpgkey | gpg --dearmor >/etc/apt/trusted.gpg.d/VirtualGL.gpg && \
    wget -P /etc/apt/sources.list.d/ https://raw.githubusercontent.com/VirtualGL/repo/main/VirtualGL.list

# Install VirtualGL and TurboVNC
RUN apt-get update && apt-get install -y --no-install-recommends \
    virtualgl \
    turbovnc \
    && rm -rf /var/lib/apt/lists/*

# Build and install libRocket as described in README
RUN git clone https://github.com/libRocket/libRocket.git /tmp/libRocket && \
    cd /tmp/libRocket/Build && \
    cmake . && \
    make && \
    make install && \
    rm -rf /tmp/libRocket

# noVNC + websockify (from source)
RUN git clone --depth 1 https://github.com/novnc/noVNC.git /opt/novnc && \
    git clone --depth 1 https://github.com/novnc/websockify.git /opt/novnc/utils/websockify && \
    chmod +x /opt/novnc/utils/novnc_proxy

# Use fluxbox as default session when something asks for gnome
RUN ln -sf /usr/share/xsessions/fluxbox.desktop /usr/share/xsessions/gnome.desktop 2>/dev/null || true

# VNC password + self-signed cert for noVNC (HTTPS)
RUN mkdir -p ~/.vnc && \
    echo "${VNC_PASSWORD}" | vncpasswd -f > ~/.vnc/passwd && \
    chmod 0600 ~/.vnc/passwd && \
    openssl req -x509 -nodes -newkey rsa:2048 \
      -keyout ~/novnc.pem -out ~/novnc.pem \
      -days 3650 \
      -subj "/C=US/ST=NY/L=NY/O=NY/OU=NY/CN=novnc"

WORKDIR /app

# VNC startup script
COPY docker/start-vnc.sh /usr/local/bin/start-vnc.sh
RUN chmod +x /usr/local/bin/start-vnc.sh

EXPOSE 5900
EXPOSE 6080

# Default command: start VNC stack, then exec the given process (shell by default)
CMD ["start-vnc.sh", "bash"]

