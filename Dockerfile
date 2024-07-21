FROM ubuntu:24.04

LABEL author="michal@owsiak.org"
LABEL description="Master's Thesis supplement - Usage of Parralel Islands Genetic Algorithm for Designing Neural Network Structure"

# we need some packages to be installed
RUN apt-get update
RUN apt-get install -y gcc
RUN apt-get install -y g++
RUN apt-get install -y mpich
RUN apt-get install -y build-essential

# Layout of /opt directories
# /opt
#  |-- bin                - main application
#  |-- src                - source code
#  |-- config             - configuration files
#  `-- test               - location of learning/test data
#      `-- out            - output of the execution

RUN mkdir -p /opt/bin

COPY src /opt/src
COPY config /opt/config
COPY test /opt/test

WORKDIR /opt/src
RUN make

CMD /opt/run.sh

ADD run.sh /opt/run.sh
RUN chmod +x /opt/run.sh

