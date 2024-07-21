#!/bin/bash

export GABP_LOG_NAME=/opt/test/log
export GABP_SRC_NAME=/opt/config
export GABP_OUT_NAME=/opt/test/out

cd /opt/bin

mpirun -np 4 ./bp

