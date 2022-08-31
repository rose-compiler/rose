from ubuntu:22.04

# sudo causes problems in Docker containers, so just run without it.
run echo '#!/bin/bash' >/bin/sudo; echo 'exec "$@"' >> /bin/sudo; chmod 755 /bin/sudo

# Install docker
run env DEBIAN_FRONTEND=noninteractive apt-get update
run env DEBIAN_FRONTEND=noninteractive apt-get -y install ca-certificates curl gnupg lsb-release
run mkdir -p /etc/apt/keyrings
run curl -fsSL https://download.docker.com/linux/ubuntu/gpg | gpg --dearmor -o /etc/apt/keyrings/docker.gpg
run echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" |\
    tee /etc/apt/sources.list.d/docker.list > /dev/null
run env DEBIAN_FRONTEND=noninteractive apt-get update
run env DEBIAN_FRONTEND=noninteractive apt-get -y install docker-ce docker-ce-cli containerd.io docker-compose-plugin

# Install top level portability testing scripts
# FIXME: Once debugging is finished, change the script URLs to something more stable
run env DEBIAN_FRONTEND=noninteractive apt-get -y install git wget sudo
run : serial 2022-08-16d
run cd; wget -O rose-matrix-slave-run     https://raw.githubusercontent.com/matzke1/rose/matzke/rose-dev/projects/MatrixTesting/scripts/rose-matrix-slave-run
run cd; wget -O rose-matrix-slave-babysit https://raw.githubusercontent.com/matzke1/rose/matzke/rose-dev/projects/MatrixTesting/scripts/rose-matrix-slave-babysit
run cd; wget -O rose-matrix-slave-clean   https://raw.githubusercontent.com/matzke1/rose/matzke/rose-dev/projects/MatrixTesting/scripts/rose-matrix-slave-clean
run cd; chmod 755 rose-matrix-slave-run rose-matrix-slave-babysit rose-matrix-slave-clean

# Set up the matrix testing
cmd export SPOCK_HOSTNAME=container; cd; ./rose-matrix-slave-run setup && ./rose-matrix-slave-babysit ./rose-matrix-slave-run

# Run it like
#  docker run -it --privileged \
#	-v /var/run/docker.sock:/var/run/docker.sock \
#       -v $(pwd)/var/rose-portability-testing:/var/rose-portability-testing \
#       -e ROSE_MATRIX_DATABASE=... \
#       -e MATRIX_HOST_ROOT=$(pwd)/ \
#       -e MATRIX_ROOT=/var/rose-portability-testing/slave-$$ \
#       matrix-testing:u2204
