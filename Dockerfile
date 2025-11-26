FROM gcc:latest

WORKDIR /app

# Install python3 for benchmarking script
RUN apt-get update && apt-get install -y python3 python3-pip python3-venv
RUN python3 -m venv .venv
RUN .venv/bin/pip install matplotlib

# Copy project files
COPY . .

# Compile the C program
RUN make

# Default command (can be overridden)
CMD [".venv/bin/python", "scripts/benchmark.py"]
