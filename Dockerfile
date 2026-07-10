# 1. Usamos la imagen oficial de Alpine Linux por su ligereza absoluta
FROM alpine:latest

# 2. Instalamos el meta-paquete de desarrollo (gcc, g++, make, etc.) y musl-dev
#    + tzdata para poder configurar la zona horaria correctamente
RUN apk add --no-cache build-base musl-dev tzdata

# 3. Configuramos la zona horaria de Madrid, España
ENV TZ=Europe/Madrid
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# 4. Definimos el directorio de trabajo dentro del contenedor
WORKDIR /usr/src/app

# 5. Comando por defecto al arrancar el contenedor: abrir la shell de Alpine
CMD ["/bin/sh"]