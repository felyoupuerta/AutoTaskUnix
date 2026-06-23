#!/bin/bash

REPO_ROOT=$(git rev-parse --show-toplevel 2>/dev/null)
cd "$REPO_ROOT" || exit 1

CURRENT_BRANCH=$(git branch --show-current)

echo "=================================="
echo "Git Helper"
echo "=================================="
echo ""
echo "Rama actual: $CURRENT_BRANCH"
echo ""
echo "1) Subir cambios a DEV"
echo "2) Pasar DEV a MAIN"
echo "3) Salir"
echo ""

read -p "Opción: " OPTION

case $OPTION in

1)
    if git show-ref --verify --quiet refs/heads/DEV; then
        git checkout DEV
    else
        git checkout -b DEV
    fi

    git add -A

    if git diff --cached --quiet; then
        echo "No hay cambios"
        exit 1
    fi

    read -p "Mensaje del commit: " MESSAGE

    if [ -z "$MESSAGE" ]; then
        exit 1
    fi

    git commit -m "$MESSAGE" || exit 1
    git push origin DEV || exit 1
;;

2)
    read -p "Confirmar merge DEV a main (s/n): " CONFIRM

    if [ "$CONFIRM" != "s" ]; then
        exit 0
    fi

    git checkout main || exit 1
    git pull origin main || exit 1

    git merge DEV || exit 1
    git push origin main || exit 1
;;

3)
    exit 0
;;

*)
    exit 1
;;

esac 
