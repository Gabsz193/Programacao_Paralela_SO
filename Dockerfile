FROM alpine:latest

WORKDIR /app

# Install python3 for benchmarking script
RUN apk add --no-cache python3 py3-pip python3-venv \
    build-base cmake ibus libdecor libthai fribidi \
    libgl libx11 libxcursor libxext libxfixes libxi \
    libxinerama libxkbcommon libxrandr libxrender libxss libxtst \
    mesa ninja vulkan-loader wayland


RUN python3 -m venv .venv
RUN .venv/bin/pip install matplotlib

# Copy project files
COPY . .

# Compile the C program
RUN cmake -S . -B out -G "Unix Makefiles" -DCMAKE_C_COMPILER=musl-gcc -DCMAKE_C_FLAGS=-static
RUN make -C out

# Default command (can be overridden)
CMD ["ls"]
