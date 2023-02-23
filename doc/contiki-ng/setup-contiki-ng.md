# Ambiente de desenvolvimento Contiki-NG

## Instalação e configuração de ferramentas de desenvolvimento

```
sudo apt update
```

```
sudo apt install build-essential doxygen git git-lfs curl wireshark python3-serial srecord rlwrap
```

### Adição de usuário ao grupo Wireshark

```bash
sudo usermod -a -G wireshark $USER
```

## Instalação do compilador ARM

```
mkdir ~/arm-compiler
```

```
cd ~/arm-compiler
```

```
wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
```

```
tar -xjf gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
This will create a directory named gcc-arm-none-eabi-9-2020-q2-update i
```

### Adição das ferramentas de compilação ARM ao PATH

Edite o arquivo ```~/.profile``` adicionando o seguite código ao fim do mesmo.

```bash
if [ -d "$HOME/arm-compiler/gcc-arm-none-eabi-9-2020-q2-update/bin" ] ; then
    PATH="$HOME/arm-compiler/gcc-arm-none-eabi-9-2020-q2-update/bin:$PATH"
fi
```

## Permissão para usuários acessarem portas USB sem sudo

```bash
sudo usermod -a -G plugdev $USER
```

```bash
sudo usermod -a -G dialout $USER
```

## Fornecendo estabilidade para o CC2538

```
echo 'ATTRS{idVendor}=="0451", ATTRS{idProduct}=="16c8", ENV{ID_MM_DEVICE_IGNORE}="1"' >> /lib/udev/rules.d/77-mm-usb-device-blacklist.rules
```
