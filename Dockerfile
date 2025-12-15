FROM alpine:latest

WORKDIR /app

# Install python3 for benchmarking script
RUN apk add --no-cache python3 py3-matplotlib build-base cmake make sdl3-dev musl-dev linux-headers

# Copy project files
COPY . .

# Compile the C program
RUN cmake -S . -B build -G "Unix Makefiles" -DCMAKE_C_COMPILER=gcc
RUN make -C build

# Default command (can be overridden)
CMD [ "scripts/init.sh" ]