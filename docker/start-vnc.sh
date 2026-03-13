#!/bin/bash
set -e

export DISPLAY=:99

# Kill any existing Xvfb/x11vnc on this display (e.g. on container restart)
pkill -f "Xvfb :99" >/dev/null 2>&1 || true
pkill -f "x11vnc.*:99" >/dev/null 2>&1 || true
sleep 1

# Start virtual framebuffer (1792x1120 = host resolution for VNC in Safari)
Xvfb :99 -screen 0 1792x992x24 &
sleep 2

# Wait for the X server to be ready
if command -v xdpyinfo >/dev/null 2>&1; then
  max_attempts=30
  attempt=0
  while [ $attempt -lt $max_attempts ]; do
    if DISPLAY=:99 xdpyinfo >/dev/null 2>&1; then
      echo "VNC display :99 is ready"
      break
    fi
    attempt=$((attempt + 1))
    sleep 1
  done
  if [ $attempt -eq $max_attempts ]; then
    echo "Warning: VNC display :99 may not be fully ready, continuing anyway..."
  fi
else
  echo "Waiting for X server (xdpyinfo not available)..."
  sleep 5
fi

# Start window manager (LANG=C avoids missing nls warning; redirect stderr to silence config defaults)
export LANG=C
fluxbox 2>/dev/null &
sleep 1

# Start VNC server (password-controlled, shared sessions)
# -noxdamage/-noxfixes avoid hangs with many VNC clients
x11vnc -display :99 -forever -shared -rfbport 5900 -listen 0.0.0.0 \
  -rfbauth ~/.vnc/passwd \
  -xkb -noxrecord -noxfixes -noxdamage &

# Start noVNC web proxy on :6080 pointing at local VNC (HTTPS if cert exists)
if [ -x /opt/novnc/utils/novnc_proxy ]; then
  NOVNC_OPTS="--vnc localhost:5900 --listen 0.0.0.0:6080"
  [ -f "${HOME}/novnc.pem" ] && NOVNC_OPTS="${NOVNC_OPTS} --cert ${HOME}/novnc.pem"
  /opt/novnc/utils/novnc_proxy $NOVNC_OPTS &
fi

# Run the passed command (e.g. bash or /startapp.sh)
exec "$@"
