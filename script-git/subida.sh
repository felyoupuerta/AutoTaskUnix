#!/bin/bash

# ==========================================
# Git Helper - DEV / MAIN workflow
# Felipe
# ==========================================


echo "=================================="
echo "        Git Helper"
echo "=================================="

# Verificar que estamos en un repo git
if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
    echo "❌ No estás dentro de un repositorio Git"
    exit 1
fi


# Mostrar rama actual
BRANCH=$(git branch --show-current)

echo ""
echo "Rama actual: $BRANCH"
echo ""


echo "¿Qué quieres hacer?"
echo ""
echo "1) Subir cambios a DEV"
echo "2) Pasar DEV a MAIN"
echo "3) Salir"
echo ""

read -p "Opción: " OPTION



case $OPTION in


1)

    echo ""
    echo "➡️  Preparando subida a DEV"

    # Cambiar a DEV
    git checkout DEV

    echo ""
    echo "Archivos modificados:"
    git status


    echo ""
    read -p "Mensaje del commit: " MESSAGE


    if [ -z "$MESSAGE" ]; then
        echo "❌ El commit necesita un mensaje"
        exit 1
    fi


    echo ""
    echo "Añadiendo archivos..."
    git add .


    echo "Creando commit..."
    git commit -m "$MESSAGE"


    echo ""
    echo "Subiendo a DEV..."
    git push origin DEV


    echo ""
    echo "✅ Cambios subidos correctamente a DEV"

;;





2)

    echo ""
    echo "⚠️  Vas a fusionar DEV → MAIN"
    read -p "¿Continuar? (s/n): " CONFIRM


    if [[ "$CONFIRM" != "s" ]]; then
        echo "Cancelado"
        exit 0
    fi


    echo ""
    echo "Cambiando a MAIN..."
    git checkout main


    echo ""
    echo "Actualizando MAIN..."
    git pull origin main



    echo ""
    echo "Fusionando DEV..."
    git merge DEV



    echo ""
    echo "Subiendo MAIN..."
    git push origin main



    echo ""
    echo "✅ DEV fusionado correctamente en MAIN"


;;


3)

    echo "Saliendo..."
    exit 0

;;


*)

    echo "❌ Opción incorrecta"

;;

esac
