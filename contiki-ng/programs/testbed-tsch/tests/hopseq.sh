#!/bin/bash

PROJ_DIR="/home/jonatas/contiki-ng/examples/rpl-udp-ifpb"
MIN_CH=11
MAX_CH=26

seqlen=${1}

seqstr=""
for (( i=0; i < ${seqlen}; i++ ));
do
    ch=$(( ( ${RANDOM} % ( ${MAX_CH} - ${MIN_CH} + 1 ) ) + ${MIN_CH} ))
    seqstr+="${ch}"
    if [ ${i} -lt $(( ${seqlen} - 1 )) ];
    then
        seqstr+=","
    fi
done

echo "Canais: ${seqstr}"

#${PROJ_DIR}/build.sh -h "${seq}"