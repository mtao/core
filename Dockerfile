# Use an official Python runtime as a parent image
FROM ubuntu:groovy
ENV DEBIAN_FRONTEND=noninteractive

# Install any needed packages specified in requirements.txt
RUN apt-get update
RUN apt-get install -y git cmake g++ ninja-build
RUN apt-get install -y libgmp3-dev libtbb-dev libboost-thread-dev libmpfr-dev libmpfrc++-dev libeigen3-dev libblas-dev liblapack-dev libcgal-dev protobuf-compiler

# Set the working directory to /app
WORKDIR /app

RUN mkdir core
#COPY src/ include/ cmake/ examples/ poisson_2d/ wavesim_2d/ fluidsim_2d/ tests/ CMakeLists.txt /app/core/
COPY ./ /app/core/

# Download and compile TriWild
WORKDIR /app/core/build
RUN ls ..
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DMTAO_USE_OPENGL=OFF -DMTAO_USE_PYTHON=off -GNinja
RUN ninja

WORKDIR /data

