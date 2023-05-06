# Testbed TSCH Firmware Build Tool

A ferramenta de build do testbed TSCH automatiza e simplifica a tarefa de gravar firmwares Contiki-NG do tipo client, server ou stopped em motes da plataforma OpenMote B que fazem parte do testbed. Com um único comando seguido de alguns argumentos opcionais, todos os motes podem ser gravados de uma só vez.

## Utilização

```
testbed-build.sh [-f firmware_type] [-p tx_power] [-i send_interval] [-s server_usb] [-l hop_sequence_len] [-h hop_sequence]
```

* **-f**: 
Tipo do firmware para o qual a gravação será feita. Para **client** a gravação será feita nos dispositivos conectados nas portas informadas pelo parâmetro -u (ou todos os dispositivos conectados, caso -u não seja passado) e que não estão conectados na porta especificada pelo parâmetro -s, se -s não for passado, a gravação será feita em todos os dispositivos informados por -u (ou todos os dispositivos conectados, caso -u não seja passado). Para **server** a gravação será feita apenas no dispositivo conectado na porta especificada pelo parâmetro -s, se o parâmetro -s não for passado, a gravação será feita no dispositivo conectado na primeira porta da lista passada para o parâmetro -u (ou ```/dev/ttyUSB1```, caso -u não seja passado). Para **all**, ocorrerá uma gravação do tipo server e outra do tipo client de forma sequencial. Para **stopped**, a gravação de um firmware nulo será feita nos dispositivos conectados nas portas informadas pelo parâmetro -u (ou todos os dispositivos conectados, caso -u não seja passado), interrompendo o fluxo de dados do testbed. O valor padrão do parâmetro -f é **all**.

* **-p**: Potência de transmissão que será configurada no(s) dispositivo(s) em dBm. O valor padrão do parâmetro -p é **3**.

* **-i**: Intervalo entre transmissões que será configurado no(s) cliente(s) em segundos. O valor padrão do parâmetro -i é **5**.

* **-s**: Porta onde está conectado o dispositivo que irá funcionar como servidor. O valor padrão do parâmetro -s é a primeira porta da lista passada para o parâmetro -u. Se o parâmetro -u não for passado, o valor padrão do parâmetro -s será ```/dev/ttyUSB1```.

* **-l**: Tamanho da sequência de saltos TSCH que será gerada pela ferramenta e configurada no(s) dispositivo(s). Se o parâmetro -l não for passado, será utilizada a sequência definida pelo parâmetro -h ou uma sequência padrão.

* **-h**: Lista de valores inteiros separados por vírgula que definem a sequência de saltos TSCH que será configurada no(s) dispositivo(s). Se o parâmetro -h não for passado, será utilizada a sequência gerada pelo parâmetro -l ou uma sequência padrão.

* **-u**: Lista de portas onde estão conectados os dispositivos do testbed separadas por vírgula. Se este parâmetro não for passado, todos os dispositivos conectados nas portas USB serão tidos como parte do testbed.
