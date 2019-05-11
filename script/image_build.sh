#!/bin/sh

set -ex

IMG=$1

docker build --pull --build-arg COMPILER="${IMG}" -t "${DOCKER_IMG}/${IMG}" .
docker push "${DOCKER_IMG}/${IMG}"

