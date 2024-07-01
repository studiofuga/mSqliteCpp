FROM ubuntu:latest AS buildenv
MAINTAINER Federico Fuga <fuga@studiofuga.com>
ENV LANG en_US.utf8
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get -qqy update && apt-get install -qqy build-essential cmake libboost-dev libboost-filesystem-dev libsqlite3-dev libgtest-dev
