# 1. Usamos la imagen oficial de Alpine Linux por su ligereza absoluta
FROM alpine:latest

# 2. Instalamos el meta-paquete de desarrollo (gcc, g++, make, etc.) y musl-dev
RUN apk add --no-cache build-base musl-dev

# 3. Definimos el directorio de trabajo dentro del contenedor
WORKDIR /usr/src/app

# 4. Comando por defecto al arrancar el contenedor: abrir la shell de Alpine
CMD ["/bin/sh"]