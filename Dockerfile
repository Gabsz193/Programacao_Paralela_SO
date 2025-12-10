FROM gcc:latest

WORKDIR /app

# Install python3 for benchmarking script
RUN apt-get update && apt-get install -y python3 python3-pip python3-venv cmake \
    pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
    libaudio-dev libfribidi-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
    libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
    libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
    libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libthai-dev \
    libpipewire-0.3-dev libwayland-dev libdecor-0-dev liburing-dev

RUN python3 -m venv .venv
RUN .venv/bin/pip install matplotlib

# Copy project files
COPY . .

# Compile the C program
RUN cmake -S . -B out -G "Unix Makefiles"
RUN make -C out

# Default command (can be overridden)
CMD ["ls"]
