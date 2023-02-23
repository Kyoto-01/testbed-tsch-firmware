# Testbed TSCH Firmware Build Tool

A ferramenta de build do testbed TSCH automatiza e simplifica a tarefa de gravar firmwares cliente/servidor Contiki-NG em motes da plataforma OpenMote-B que fazem parte do testbed. Com um único comando seguido de alguns argumentos opcionais, todos os motes podem ser gravados de uma só vez.

## Utilização

```
testbed-build.sh [-f firmware_type] [-p tx_power] [-s server_usb] [-l hop_sequence_len] [-h hop_sequence]
```

* -f client | server | all : 
Tipo do firmware para o qual a gravação será feita. Para **client** a gravação será feita em todos os dispositivos não conectados na porta especificada para o servidor, se nenhuma porta for especificada para o servidor, a gravação será feita em todos os dispositivos conectados. Para **server** a gravação será feita apenas no dispositivo conectado na porta especificada para o servidor, se nenhuma porta for especificada, a gravação será feita no dispositivo conectado na porta ```/dev/ttyUSB1```. Para **all** o servidor será gravado no dispositivo conectado na porta especificada para o servidor (```/dev/ttyUSB1``` caso nenhuma porta seja especificada) e o cliente será gravado nos dispositivos conectados nas demais portas. O valor padrão do parâmetro -f é **all**.

* -p &lt;tx_power&gt;: Inteiro que define a potência de transmissão que será configurada no(s) dispositivo(s). O valor padrão do parâmetro -p é **3**.

* -s &lt;server_usb&gt;: Porta onde está conectado o dispositivo que irá funcionar como servidor. O valor padrão do parâmetro -s é ```/dev/ttyUSB1```.

* -l &lt;hop_sequence_len&gt;: Inteiro que define o tamanho da sequência de saltos TSCH que será gerada pela ferramenta e configurada no(s) dispositivo(s). Se o parâmetro -l não for passado, será utilizada a sequência definida pelo parâmetro -h ou uma sequência padrão.

* -h &lt;hop_sequence&gt;: Lista de valores inteiros separados por vírgula que definem a sequência de saltos TSCH que será configurada no(s) dispositivo(s). Se o parâmetro -h não for passado, será utilizada a sequência gerada pelo parâmetro -l ou uma sequência padrão.