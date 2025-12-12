FROM alpine:latest

WORKDIR /app

# Install python3 for benchmarking script
RUN apk add --no-cache python3 py3-matplotlib build-base cmake make sdl3-dev musl-dev linux-headers

# Copy project files
COPY . .

# Compile the C program
RUN cmake -S . -B out -G "Unix Makefiles" -DCMAKE_C_COMPILER=gcc
RUN make -C out

# Default command (can be overridden)
CMD ["out/simulation", "--size", "10"]
