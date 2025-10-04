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

echo "Connexion avec:"
echo "  Nickname: $NICKNAME"
echo "  Real name: $REALNAME"
echo "  Password: $PASSWORD"
echo ""

# Envoyer les commandes IRC avec CRLF
{
    printf "PASS %s\r\n" "$PASSWORD"
    printf "NICK %s\r\n" "$NICKNAME"
    printf "USER %s 0 * :%s\r\n" "$NICKNAME" "$REALNAME"
    # Garder la connexion ouverte pour interaction
    cat
} | nc localhost 9999

