#!/bin/bash

#
# script para gravar firmwares Contiki-NG 6TiSCH cliente/servidor
#
# Uso:
# build.sh [-f fimware_type] [-p tx_power] [-s server_usb] [-l hop_sequence_len]
#           [-h hop_sequence]
#

# constantes
PROJ_DIR="/home/jonatas/contiki-ng/examples/rpl-udp-ifpb"
TARGET="openmote"
BOARD="openmote-b"
SERVER_FIRMWARE="udp-server"
CLIENT_FIRMWARE="udp-client"
MAIN_FIRMWARE="all"
MAIN_TX_POWER=3
MAIN_SERVER_USB="/dev/ttyUSB1"
MAIN_HOPSEQ="TSCH_HOPPING_SEQUENCE_4_4"
HOPSEQ_CONST_NAME="TSCH_CONF_DEFAULT_HOPPING_SEQUENCE"
MIN_CH=11
MAX_CH=26

# tipo do firmware -> server, client, all
firmware_type=""

# potência de transmissão em dBm
tx_power=""

# dispositivo USB do nó servidor
server_usb=""

# sequência de saltos TSCH
hopseq=""

# tamanho da sequenência de saltos TSCH (no caso de geração aleatória da sequência)
hopseq_len=""

# sequência de canais anterior à gravação
prev_hopseq=""

# grava o firmware do servidor
function build_server() {
    make distclean

    # grava firmware no dispositivo USB, se este existir
    if ls ${server_usb} &> /dev/null;
    then
        if make TARGET=${TARGET} BOARD=${BOARD} PORT=${server_usb} TX_POWER=${tx_power} ${SERVER_FIRMWARE}.upload;
        then
            echo "Servidor gravado em ${server_usb}."
        else
            echo "Erro ao gravar servidor em ${server_usb}."
        fi
    else
        echo "Dispositivo ${server_usb} não encontrado!"
    fi
}

# grava o firmware dos clientes
function build_clients() {
    make distclean

    usb_n=1
    usb_dev="/dev/ttyUSB${usb_n}"
    step=2

    # percorre dispositivos USB de step em step, gravando-os
    while ls ${usb_dev} &> /dev/null;
    do
        if [ "${usb_dev}" != "${server_usb}" ];
        then
            if make TARGET=${TARGET} BOARD=${BOARD} PORT=${usb_dev} TX_POWER=${tx_power} ${CLIENT_FIRMWARE}.upload;
            then
                echo "Cliente gravado em ${usb_dev}."
            else
                echo "Erro ao gravar cliente em ${usb_dev}."
            fi
        fi
        usb_n=$(( ${usb_n} + ${step} ))
        usb_dev="/dev/ttyUSB${usb_n}"
    done
}

# grava firmwares cliente e servidor
function build_all() {
    build_server
    build_clients
}

# gera sequência de canais TSCH aleatórios separados por vírgula
function generate_hopseq() {
    seqstr=""

    for (( i=0; i < ${hopseq_len}; i++ ));
    do
        ch=$(( ( ${RANDOM} % ( ${MAX_CH} - ${MIN_CH} + 1 ) ) + ${MIN_CH} ))
        seqstr+="${ch}"
        if [ ${i} -lt $(( ${hopseq_len} - 1 )) ];
        then
            seqstr+=","
        fi
    done

    echo "${seqstr}"
}

function define_hopseq() {
    # define uma nova sequência de canais no project-conf.h
    # $1 -> "#define FOO (uint8_t[]) {...}"
    sed -i -E "/${HOPSEQ_CONST_NAME}/c ${1}" ${PROJ_DIR}/project-conf.h
}

function trap_ctrlc() {
    define_hopseq "${prev_hopseq}"
    exit 2
}

function build() {
    # salva sequência de canais anterior
    prev_hopseq=$( grep "${HOPSEQ_CONST_NAME}" ${PROJ_DIR}/project-conf.h )

    trap "trap_ctrlc" 2

    # define uma nova sequência de canais
    define_hopseq "#define ${HOPSEQ_CONST_NAME} ${hopseq}"

    # grava firmware
    if [ "${firmware_type}" == "server" ];
    then
        build_server

    elif [ "${firmware_type}" == "client" ];
    then
        build_clients

    elif [ "${firmware_type}" == "all" ];
    then
        build_all

    else
        echo "Tipo de firmware '${firmware_type}' não aceito!"
        echo "Tente: server, client ou all"
    fi

    # redefine a sequência de canais para a anterior
    define_hopseq "${prev_hopseq}"
}

# processando argumentos
arg=""
for a in $@;
do
    case ${arg} in
        "-f") firmware_type=${a} ;;
        "-h") hopseq=${a} ;;
        "-l") hopseq_len=${a} ;;
        "-p") tx_power=${a} ;;
        "-s") server_usb=${a} ;;
    esac
    arg=${a}
done

# firmware padrão
if [ -z ${firmware_type} ];
then
    firmware_type=${MAIN_FIRMWARE}
fi

# potência de transmissão padrão
if [ -z ${tx_power} ];
then
    tx_power=${MAIN_TX_POWER}
fi

# dispositivo USB padrão
# quando não especificado em uma gravação do tipo "cliente", a gravação
# é feita em todos os dispositivos USB conectados, portanto não há nescessidade
# de definir um dispositivo USB padrão para o servidor
if [ -z ${server_usb} ] && [ "${firmware_type}" != "client" ];
then
    server_usb=${MAIN_SERVER_USB}
fi

# sequência de saltos TSCH
if [ ! -z ${hopseq_len} ];
then
    hopseq="$( generate_hopseq )"
    echo "sequência de saltos gerada: ${hopseq}"
fi

if [ -z ${hopseq} ];
then
    hopseq=${MAIN_HOPSEQ}
else
    hopseq="(uint8_t[]) { ${hopseq} }" 
fi

build
