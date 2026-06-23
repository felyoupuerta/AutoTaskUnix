#!/bin/bash
# ==========================================
# Git Helper 
# Felipe Angeriz Estefanell
#
#23/06/2026
# ==========================================


echo "=================================="
echo "        Git Helper"
echo "=================================="


if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1
then
    echo " No estás dentro de un repositorio Git"
    exit 1
fi



CURRENT_BRANCH=$(git branch --show-current)

echo ""
echo "Rama actual: $CURRENT_BRANCH"
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
    echo "Preparando subida a DEV"
    echo ""


    # Crear DEV si no existe
    if git show-ref --verify --quiet refs/heads/DEV
    then
        echo "Rama DEV encontrada"
        git checkout DEV
    else
        echo "Rama DEV no existe"
        echo "Creando DEV..."
        git checkout -b DEV
    fi



    echo ""
    echo "Archivos modificados:"
    echo ""

    git status


    echo ""

    read -p "Mensaje del commit: " MESSAGE


    if [ -z "$MESSAGE" ]
    then
        echo "El commit necesita un mensaje"
        exit 1
    fi



    echo ""
    echo "Añadiendo archivos..."

    git add .



    echo ""
    echo "Creando commit..."

    if git commit -m "$MESSAGE"
    then
        echo "Commit creado"
    else
        echo "Error creando commit"
        exit 1
    fi



    echo ""
    echo "Subiendo a DEV..."


    if git push origin DEV
    then
        echo ""
        echo "Cambios subidos correctamente a DEV"
    else
        echo ""
        echo "Error subiendo a DEV"
        exit 1
    fi


;;



2)

    echo ""
    echo "Vas a fusionar DEV → MAIN"
    echo ""

    read -p "¿Continuar? (s/n): " CONFIRM


    if [[ "$CONFIRM" != "s" ]]
    then
        echo "Cancelado"
        exit 0
    fi



    # Verificar DEV

    if git show-ref --verify --quiet refs/heads/DEV
    then
        echo " DEV existe"
    else
        echo " No existe la rama DEV"
        exit 1
    fi



    echo ""
    echo "Cambiando a main..."

    git checkout main



    echo ""
    echo "Actualizando main..."

    git pull origin main



    echo ""
    echo "Fusionando DEV..."

    if git merge DEV
    then
        echo " Merge correcto"
    else
        echo " Conflicto en merge"
        echo "Resuelve los conflictos y vuelve a ejecutar"
        exit 1
    fi



    echo ""
    echo "Subiendo MAIN..."


    if git push origin main
    then
        echo ""
        echo "✅ DEV fusionado en MAIN correctamente"
    else
        echo ""
        echo " Error subiendo MAIN"
        exit 1
    fi


;;



3)

    echo "Saliendo..."
    exit 0

;;



*)

    echo " Opción inválida"

;;

esac 
