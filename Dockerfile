FROM alpine:3.19

RUN apk add --no-cache gcc musl-dev make

WORKDIR /app

COPY include/ ./include/
COPY src/ ./src/
COPY tests/ ./tests/
COPY examples/ ./examples/
COPY Makefile ./

RUN make clean && make all

CMD ["make", "verify"]
