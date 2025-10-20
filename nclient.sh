#!/bin/bash

# Vérifier les arguments
if [ $# -lt 2 ]; then
    echo "Usage: $0 <nickname> <realname> [password]"
    echo "Exemple: $0 laurent \"laurent verbe\" [motdepasse]"
    exit 1
fi

# Récupérer les arguments
NICKNAME="$1"
REALNAME="$2"
PASSWORD="${3:-123456789}"  # Mot de passe par défaut si non fourni

echo "nb param: $#"
echo "Connexion avec:"
echo "  Nickname: $NICKNAME"
echo "  Real name: $REALNAME"
echo "  Password: $PASSWORD"
if [ $# -eq 4 ]; then
    CHAN="$4"
    echo " channel: $4"
fi
echo ""

# Envoyer les commandes IRC avec CRLF
{
    printf "PASS %s\r\n" "$PASSWORD"
    printf "NICK %s\r\n" "$NICKNAME"
    printf "USER username 0 * :%s\r\n" "$REALNAME"
    if [ $# -eq 4 ]; then
        printf "JOIN %s\r\n" "$CHAN"
    fi
    # Garder la connexion ouverte pour interaction
    cat
} | nc localhost 9999

