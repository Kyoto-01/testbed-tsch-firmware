# Testbed TSCH Firmware

Módulo do testbed TSCH referente ao firmware dos motes e suas ferramentas de automação.
O código-fonte dos firmwares cliente e servidor se encontram em ```testbed-tsch-firmware/contiki-ng/programs/testbed-tsch```.

Os firmwares foram escritos para executar em um ambiente Contiki-NG e o script de build foi desenvolvido para gravar os firmwares em motes da plataforma OpenMote-B. Se estiver utilizando uma outra plataforma será nescessário modificar o código da ferramenta de build. Se desejar usar um outro sistema operacional, estes firmwares não serão úteis.

## 1. Módulos

### 1.1. Firmware cliente

Coleta dados de transmissão e os envia para o servidor. Também envia os mesmos dados pela saída serial para que possam ser manipulados por algum outro software.

### 1.2. Firmware servidor

Aguarda por pacotes enviados pelos clientes. Ao receber algo, ecoa os dados como forma de confirmação e encaminha os dados recebidos para a saída serial para que possam ser manipulados por algum outro software.

## 2. Protocolo 

### 2.1. Comunicação cliente-servidor

O protocolo de comunicação entre o cliente e o servidor não é orientado a conexão e portanto utiliza o UDP como protocolo de transporte. 

#### 2.1.1. Confirmação
O cliente aguarda por uma confirmação quando envia os seus dados para o servidor, mas essa confirmação é feita pelo protocolo do testbed. Quando o servidor recebe dados de um cliente, ele ecoa esses dados para o mesmo, realizando assim a confirmação.

#### 2.1.2. Formato
O cliente envia dados para o servidor no formato de uma string de campos separados por vírgula.

Os campos do protocolo são os seguintes, nessa mesma ordem:
* **tx**: Quantidade de pacotes transmitidos.
* **rx**: Quantidade de pacotes confirmados.
* **txpwr**: Potência de transmissão.
* **ch**: Canal TSCH utilizado para a transmissão.

### 2.2. Comunicação serial

Todos os dados que são transmitidos por um cliente ou recebidos por um servidor são enviados para a saída serial com a adição de algumas informações.

#### 2.2.1. Formato
Tanto o cliente quanto o servidor enviam dados para a saída serial no formato de uma string de campos separados por vírgula.

O cliente envia os seguintes dados para a saída serial:
* **firmtype**: Tipo do firmware (client | server).
* **addrsend**: Endereço do transmissor (cliente).
* **addrrecv**: Endereço do receptor (servidor).
* **data**: Dados que foram enviados para o servidor.

O servidor envia os seguintes dados para a saída serial:
* **firmtype**: Tipo do firmware (client | server).
* **addrsend**: Endereço do transmissor (cliente).
* **addrrecv**: Endereço do receptor (servidor).
* **data**: Dados que foram recebidos de um cliente.
* **rssi**: Intensidade do sinal recebido.

## 3. Colocando o Testbed TSCH Firmware para rodar

### 3.1. Configuração da pasta do projeto 

Crie uma pasta chamada *testbed-tsch* no diretório *home* do usuário para colocar o repositório do projeto.

```
mkdir ~/testbed-tsch
```

Obs.: Caso prefira colocar o repositório em algum outro local, deverá editar o valor da constante ```PROJ_DIR``` no script de build do testbed localizado em ```testbed-tsch-firmware/tools/testbed-build/testbed-build.sh```.

### 3.2 Preparação de ambiente de desenvolvimento Contiki-NG

Siga os passos do tutorial disponível [aqui](doc/contiki-ng/setup-contiki-ng.md).

### 3.3. Clonagem do repositório 

Clone o repositório para a pasta criada.

```
cd ~/testbed-tsch
```

```
git clone https://github.com/Kyoto-01/testbed-tsch-firmware.git
```

### 3.4. Conexão dos dispositivos 

Conecte os motes em um hub USB já conectado em uma máquina Linux. De preferência, conecte o mote que será o servidor primeiro para que fique com a porta /dev/ttyUSB1, que é a porta padrão do servidor para a ferramenta de build do testbed.

### 3.5. Gravação dos firmwares

Execute a ferramenta de build.

```
cd testbed-tsch-firmware/tools/testbed-build
```
```
./testbed-build.sh
```
Para um build mais personalizado, veja a documentação da ferramenta de build disponível [aqui](tools/testbed-build/README.md).

### 3.6. (Bônus) Visualização de dados 

Você pode fazer o que achar nescessário com os dados que são enviados para a entrada serial do computador. Algo que pode ser nescessário é a visualização desses dados de forma bruta. Para isso, basta se deslocar até a pasta dos firmwares(```testbed-tsch-firmware/contiki-ng/programs/testbed-tsch```) e executar a seguinte linha de código:

```
make TARGET=openmote BOARD=openmote-b PORT=<serial_port> serialview
```
Onde *serial_port* é a porta onde estão chegando os dados que se quer visualizar.