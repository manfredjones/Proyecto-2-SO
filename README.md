# BWFS - Black & White File System

## Descripción

Este proyecto implementa un sistema de archivos llamado **BWFS**, diseñado para funcionar en espacio de usuario bajo Linux, utilizando la biblioteca [FUSE](https://github.com/libfuse/libfuse). El sistema almacena su información en imágenes blanco y negro y ofrece soporte básico para operaciones de archivos.

## Estructura del Proyecto
Proyecto-2-SO/
├── bin/ # Binarios compilados
├── include/ # Archivos header (.h)
├── src/ # Código fuente (.c)
├── tests/ # Scripts de prueba
├── Makefile # Compilación
├── README.md # Este archivo
└── .gitignore # Archivos ignorados por Git


## Requisitos

- Sistema operativo: GNU/Linux
- Paquetes:
  ```bash
  sudo apt update
  sudo apt install build-essential libfuse-dev fuse git

## Compilación
Desde la raíz del proyecto:
  make
Para limpiar los binarios:
  make clean

## Uso
Crear un nuevo FS
  ./bin/mkfs.bwfs carpeta/
Verificar consistencia
  ./bin/fsck.bwfs carpeta/
Montar FS
  ./bin/mount.bwfs carpeta/ punto_de_montaje/

## Créditos
Desarrollado como parte del curso de Principios de Sistemas Operativos - Ingeniería en Computación, ITCR, 2025.

