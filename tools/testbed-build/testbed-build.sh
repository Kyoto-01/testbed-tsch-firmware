#!/bin/bash

#
# script para gravar firmwares cliente/servidor Contiki-NG
# em motes da plataforma OpenMote-B.
#
# Uso:
# testbed-build.sh [-f client | server | all | stopped] [-i interval] 
#                   [-p tx_power] [-s server_usb] [-l hop_sequence_len] [-h hop_sequence]
#                   [-u usb_ports]

# constantes

# diretório onde se encontram os códigos dos firmwares
PROJ_DIR="${HOME}/testbed-tsch/testbed-tsch-firmware/contiki-ng/programs/testbed-tsch"
TARGET="openmote"
BOARD="openmote-b"
SERVER_FIRMWARE="testbed-server"
CLIENT_FIRMWARE="testbed-client"
STOPPED_FIRMWARE="testbed-stopped"
MAIN_FIRMWARE="all"
MAIN_TX_POWER=3  # dBm
MAIN_SEND_INTV=5 # segundos
MAIN_SERVER_USB="/dev/ttyUSB1"
MAIN_HOPSEQ="TSCH_HOPPING_SEQUENCE_4_4"
HOPSEQ_CONST_NAME="TSCH_CONF_DEFAULT_HOPPING_SEQUENCE"
MIN_CH=11
MAX_CH=26

# tipo do firmwar: server | client | all
firmware_type=""

# potência de transmissão em dBm
tx_power=""

# Intervalo de transmissão dos clientes em segundos
send_intv=""

# dispositivo USB do nó servidor
server_usb=""

# dispositivos USB do testbed
usb_ports=""

# sequência de saltos TSCH
hopseq=""

# tamanho da sequenência de saltos TSCH (no caso de geração 
# aleatória da sequência)
hopseq_len=""

# sequência de canais anterior à gravação
prev_hopseq=""

# grava o firmware do servidor
function build_server() {
    make distclean

    # grava firmware no dispositivo USB, se este existir
    if ls ${server_usb} &>/dev/null; then
        if make TARGET=${TARGET} BOARD=${BOARD} PORT=${server_usb} \
            TX_POWER=${tx_power} ${SERVER_FIRMWARE}.upload; then
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

    if [ ! -z "${usb_ports}" ]; then
        usb_n=0
        usb_dev=${usb_ports[${usb_n}]}
        step=1
    fi

    # percorre dispositivos USB de step em step, gravando-os
    while ls ${usb_dev} &>/dev/null; do

        if [ "${usb_dev}" != "${server_usb}" ]; then
            if make TARGET=${TARGET} BOARD=${BOARD} PORT=${usb_dev} \
                TX_POWER=${tx_power} SEND_INTV_SECS=${send_intv} \
                ${CLIENT_FIRMWARE}.upload; then
                echo "Client recorded at ${usb_dev}."
            else
                echo "Error when trying to write client at ${usb_dev}."
            fi
        fi

        usb_n=$((${usb_n} + ${step}))

        if [ ! -z "${usb_ports}" ]; then
            if [ ${usb_n} -ge ${#usb_ports[@]} ]; then
                break
            else
                usb_dev=${usb_ports[${usb_n}]}
            fi
        else
            usb_dev="/dev/ttyUSB${usb_n}"
        fi
    done
}

# grava firmwares cliente e servidor
function build_all() {
    build_server
    build_clients
}

# grava firmware nulo, para parar o testbed
function build_stopped() {
    make distclean

    usb_n=1
    usb_dev="/dev/ttyUSB${usb_n}"
    step=2

    if [ ! -z ${usb_ports} ]; then
        usb_n=0
        usb_dev=${usb_ports[${usb_n}]}
        step=1
    fi

    # percorre dispositivos USB de step em step, gravando-os
    while ls ${usb_dev} &>/dev/null; do

        if make TARGET=${TARGET} BOARD=${BOARD} PORT=${usb_dev} \
            ${STOPPED_FIRMWARE}.upload; then
            echo "Mote stopped at ${usb_dev}."
        else
            echo "Error when trying to stop mote at ${usb_dev}."
        fi

        usb_n=$((${usb_n} + ${step}))

        if [ ! -z ${usb_ports} ]; then
            if [ ${usb_n} -ge ${#usb_ports[@]} ]; then
                break
            else
                usb_dev=${usb_ports[${usb_n}]}
            fi
        else
            usb_dev="/dev/ttyUSB${usb_n}"
        fi
    done
}

# gera sequência de canais TSCH aleatórios separados por vírgula
function generate_hopseq() {
    seqstr=""

    for ((i = 0; i < ${hopseq_len}; i++)); do
        ch=$(((${RANDOM} % (${MAX_CH} - ${MIN_CH} + 1)) + ${MIN_CH}))
        seqstr+="${ch}"
        if [ ${i} -lt $((${hopseq_len} - 1)) ]; then
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
    cd ${PROJ_DIR}

    # salva sequência de canais anterior
    prev_hopseq=$(grep "${HOPSEQ_CONST_NAME}" ${PROJ_DIR}/project-conf.h)

    trap "trap_ctrlc" 2

    # define uma nova sequência de canais
    define_hopseq "#define ${HOPSEQ_CONST_NAME} ${hopseq}"

    # grava firmware
    if [ "${firmware_type}" == "server" ]; then
        build_server

    elif [ "${firmware_type}" == "client" ]; then
        build_clients

    elif [ "${firmware_type}" == "all" ]; then
        build_all

    elif [ "${firmware_type}" == "stopped" ]; then
        build_stopped

    else
        echo "Tipo de firmware '${firmware_type}' não aceito!"
        echo "Tente: server, client ou all"
    fi

    # redefine a sequência de canais para a anterior
    define_hopseq "${prev_hopseq}"
}

# processando argumentos
arg=""
for a in $@; do
    case ${arg} in
    "-f") firmware_type=${a} ;;
    "-h") hopseq=${a} ;;
    "-i") send_intv=${a} ;;
    "-l") hopseq_len=${a} ;;
    "-p") tx_power=${a} ;;
    "-s") server_usb=${a} ;;
    "-u") usb_ports=${a} ;;
    esac
    arg=${a}
done

# firmware padrão
if [ -z ${firmware_type} ]; then
    firmware_type=${MAIN_FIRMWARE}
fi

# potência de transmissão padrão
if [ -z ${tx_power} ]; then
    tx_power=${MAIN_TX_POWER}
fi

# intervalo entre transmissões dos clientes
if [ -z ${send_intv} ]; then
    send_intv=${MAIN_SEND_INTV}
fi

# dispositivos usb do testbed
if [ ! -z ${usb_ports} ]; then
    IFS=',' read -r -a arr <<< "${usb_ports}"
    usb_ports=(${arr[@]})
fi

# dispositivo USB padrão
# quando não especificado em uma gravação do tipo "cliente", a gravação
# é feita em todos os dispositivos USB conectados, portanto não há 
# nescessidade de definir um dispositivo USB padrão para o servidor
if [ -z ${server_usb} ] && [ "${firmware_type}" != "client" ]; then
    if [ ! -z "${usb_ports}" ]; then
        server_usb="${usb_ports[0]}"
    else
        server_usb=${MAIN_SERVER_USB}
    fi
fi

# sequência de saltos TSCH
if [ ! -z ${hopseq_len} ]; then
    hopseq="$(generate_hopseq)"
    echo "sequência de saltos gerada: ${hopseq}"
fi

if [ -z ${hopseq} ]; then
    hopseq=${MAIN_HOPSEQ}
else
    hopseq="(uint8_t[]) { ${hopseq} }"
fi

build
