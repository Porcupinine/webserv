FROM ubuntu:22.04

WORKDIR /usr/src/app

RUN apt-get update && \
    apt-get install -y build-essential cmake curl telnet vim

# COPY . .
COPY CMakeLists.txt .

# RUN ls -la

RUN mkdir -p build
WORKDIR /usr/src/app/build
# RUN cmake ..
# RUN make

EXPOSE 8080

# CMD ["./webserver", "configs/test.conf"]
