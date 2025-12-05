FROM golang:1.23-bookworm

ENV DEBIAN_FRONTEND=noninteractive
ENV GOTOOLCHAIN=auto

RUN apt-get update && apt-get install -y \
    gcc \
    make \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY include/ ./include/
COPY src/ ./src/
COPY data/ ./data/
COPY examples/ ./examples/
COPY tui/ ./tui/
COPY Makefile ./

# Build C binaries and static lib
RUN make clean && make all && make obj/libmic1.a && make asm

# Build Go TUI (optional, requires TTY at runtime)
WORKDIR /app/tui
RUN go mod download
WORKDIR /app
ENV CGO_ENABLED=1
RUN make tui

# Set TUI as entrypoint
WORKDIR /app/tui
ENTRYPOINT ["./archsim-tui"]
CMD ["../examples/test_stack.asm"]
