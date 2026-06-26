#!/bin/sh

set -e

EMAIL="tu_email@github.com"
KEY_PATH="$HOME/.ssh/id_ed25519_github"

echo "== Generando SSH key si no existe =="

if [ ! -f "$KEY_PATH" ]; then
    ssh-keygen -t ed25519 -C "$EMAIL" -f "$KEY_PATH" -N ""
else
    echo "Key ya existe: $KEY_PATH"
fi

echo "== Iniciando ssh-agent =="
eval "$(ssh-agent -s)"

echo "== Añadiendo key al agente =="
ssh-add "$KEY_PATH"

echo "== Mostrando clave pública =="
echo "Copia esto en GitHub:"
echo "--------------------------------"
cat "$KEY_PATH.pub"
echo "--------------------------------"

echo ""
echo "Luego ve a:"
echo "https://github.com/settings/keys"
echo "y añade la clave manualmente."
