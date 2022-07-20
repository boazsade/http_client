FROM ubuntu:latest

LABEL maintainer="boaz1sade@gmail.com"
LABEL version="0.1"
LABEL description="build image for building this code"

RUN apt update && \
    apt install -y build-essential && \
    apt install -y vim && apt install -y curl \
    apt install -y libboost-all-dev
RUN mkdir /builds
COPY Makefile  /builds
COPY client /builds/
COPY http_base /builds/
WORKDIR /builds
CMD [make all]
