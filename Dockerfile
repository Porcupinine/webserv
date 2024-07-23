# Use an official Ubuntu as a parent image
FROM ubuntu:22.04

# Set the working directory in the container
WORKDIR /usr/src/app

# Install necessary packages
RUN apt-get update && \
    apt-get install -y g++ make cmake

# Copy the current directory contents into the container at /usr/src/app
COPY . .

# Build the application
RUN mkdir -p build
WORKDIR /usr/src/app/build
RUN cmake ..
RUN make

EXPOSE 8080

# Run the web server by default
CMD ["./webserver configs/test.conf"]
