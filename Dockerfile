FROM ubuntu:latest AS buildenv
MAINTAINER Federico Fuga <fuga@studiofuga.com>
ENV LANG en_US.utf8
ENV DEBIAN_FRONTEND noninteractive
RUN apt -qqy update && apt install -qqy libboost-dev libboost-filesystem-dev libsqlite3-dev libgtest-dev
