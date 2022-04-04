#!/usr/bin/env bash
# USAGE: ./build-server.sh 1.9.0
docker build -t autoflowresearch/smartpeak-server:v$1 -f Dockerfile.smartpeak-server .