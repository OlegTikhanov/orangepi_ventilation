Управление вентиляцией через orangepi по mqtt для Home Assistant
=====================================

## Установка зависимостей
    # apt-get update
    # apt install libmosquitto-dev
    # apt-get install -y git

## Установка wiringOP
    # git clone https://github.com/orangepi-xunlong/wiringOP.git
    # ./build clean
    # ./build -lwiringPi

## Компиляция
    $ make

## Установка демона
    # make install

## Удаление демона
    # make uninstall

