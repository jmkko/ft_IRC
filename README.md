# ft_IRC
```bash
    ██████   █████              █████ ███████████     █████████ 
   ███░░███ ░░███              ░░███ ░░███░░░░░███   ███░░░░░███
  ░███ ░░░  ███████             ░███  ░███    ░███  ███     ░░░ 
 ███████   ░░░███░              ░███  ░██████████  ░███         
░░░███░      ░███               ░███  ░███░░░░░███ ░███         
  ░███       ░███ ███           ░███  ░███    ░███ ░░███     ███
  █████      ░░█████  █████████ █████ █████   █████ ░░█████████ 
 ░░░░░        ░░░░░  ░░░░░░░░░ ░░░░░ ░░░░░   ░░░░░   ░░░░░░░░░
```
Internet Relay Chat project at 42 (a text-based communication protocol on the Internet)<br />

![42](https://img.shields.io/badge/-42-black?style=for-the-badge&logo=ft_IRC&logoColor=white) ![Static Badge](https://img.shields.io/badge/Language-C%2B%2B-blue) ![Static Badge](https://img.shields.io/badge/Protocole-IRC-blue) ![Static Badge](https://img.shields.io/badge/Doc-Doxygen-blue)

---

## 📋 ft_IRC - Subjet

This project is about creating your own IRC server.
You are required to develop an IRC server using the C++ 98 standard.
Your executable will be run as follows: `./ircserv <port> <password>`
### ⚙️ Requirements
- The server must be capable of handling multiple clients simultaneously without hanging.
- Only 1 poll() (or equivalent) can be used for handling all these operations (read, write, but also listen, and so forth).
- Several IRC clients exist. You have to choose one of them as a reference. Your reference client will be used during the evaluation process.
- Communication between client and server has to be done via TCP/IP (v4 or v6).
### 🤖 Features
- authenticate, set a nickname, a username, join a channel, send and receive private messages using your reference client.
- All the messages sent from one client to a channel have to be forwarded to every other client that joined the channel.
- You must have operators and regular users.
- Commands that are specific to channel operators:
  - KICK- Eject a client from the channel
  - INVITE- Invite a client to a channel
  - TOPIC- Change or view the channel topic
  - MODE- Change the channel’s mode:
    - i: Set/remove Invite-only channel
    - t: Set/remove the restrictions of the TOPIC command to channel operators
    - k: Set/remove the channel key (password)
    - o: Give/take channel operator privilege
    - l: Set/remove the user limit to channel
### 💪 Bonus   
- Handle file transfer.
- bot.
---

## 🛰️ ft_IRC — Workflow de collaboration

Ce dépôt est le dépôt principal du projet ft_IRC (école 42).
Nous travaillons en équipe de 3 étudiants, chacun sur un fork personnel de ce dépôt.
Toutes les contributions passent par des Pull Requests (PR) vers ce dépôt principal.

### 🧩 Organisation générale

- `main` — branche stable, propre, toujours fonctionnelle.
- `feature/...` — branches créées dans chaque fork pour développer une fonctionnalité.

### ⚙️ Mise en place du dépôt local (une seule fois)

1. Forker le dépôt principal
2. Cloner son fork personnel :
```bash
git clone git@github.com:MON-USERNAME/ft_IRC.git
cd ft_IRC
```
3. Relier le dépôt principal en `upstream` :
```bash
git remote add upstream https://github.com/TON-USERNAME/ft_IRC.git
```
4. Vérifier :
```bash
git remote -v
# doit montrer origin -> votre fork, et upstream -> dépôt principal
```
### 🚀 Workflow de développement
#### Créer une nouvelle fonctionnalité
```bash
# Se mettre à jour avant tout
git checkout main
git pull upstream main
git push origin main

# Créer une branche de fonctionnalité
git checkout -b feature/ma_fonction
```

#### Coder, puis :
```bash
git add .
git commit -m "Ajoute ma_fonction"
git push origin feature/ma_fonction
```
#### Créer une Pull Request (PR)

1. Aller sur GitHub → votre fork
2. Sur votre branche → cliquer « Compare & pull request »
3. Vérifier :
	- base : `TON-USERNAME/ft_IRC` → `main`
	- compare : `MON-USERNAME/ft_IRC` → `feature/ma_fonction`
4. Créer la PR

### ♻️ Mettre à jour votre fork

Avant de commencer une nouvelle fonctionnalité, synchronisez votre main local et votre fork :
```bash
git checkout main
git pull upstream main
git push origin main
````
### 📏 Conventions de code

- Commits clairs et concis (présent)  
	✅ Ajoute la gestion du JOIN  
	❌ J'ai codé join  
- Branches : `feature/...` ou `fix/...`
- Jamais coder sur main
- Faire des PR petites et régulières

### 🧪 Tests

We will strive to make different tests to prevent regressions while shipping new features.
This part of the project is not meant to be evaluated.
Therefore, we will use a more recent standard (c++20) and give a try at new features such as : 
- lambdas
- introspection with type traits (ex: printing the name of an exception being thrown)

Once implemented, each person implementing a new feature or a fix should be also responsible for the tests.

- unit tests to check the output of a function, for normal and edge cases.
- integration test, based on a basic client, to check that a command or series or command produces the expected output from the server.

#### Assertions

- `AssertUtils` contains utility functions to perform basic checks

#### Continuous integration and development

If possible, we will try to add a Github Action workflow in order to check at each PR that
- the projects compiles
- the norm is respected (using a linter based on [LLVM code rules](https://llvm.org/docs/CodingStandards.html))
- the codebase doesn't have potential bugs (using [CPPcheck](https://github.com/danmar/cppcheck), a static analysis checker)

### 🧠 Résumé visuel
```bash
          +-----------+
          | upstream  |
          | (principal)|
          +-----+-----+
                ^
                |
      +---------+----------+
      |                    |
+-----+-----+        +-----+-----+
|  fork 1   |        |  fork 2   |
|  membre A |        |  membre B |
+-----------+        +-----------+
```

✨ Important : toujours synchroniser `main` avant de créer une branche de feature.
---

## 📐ft_IRC — Stucture

```mermaid

```

---

## ft_IRC - Compétences acquises

Ce projet est un projet synthèse du tronc commun de 42 qui nous a permis de mener
un travail en groupe et d'utiliser toutes les compétences apprises.

- Projet en `C++` - Programmation orientée objet sur un projet complet
- Projet réseau - TCP , Socket , poll
- Protocol `IRC` - Syntaxe des messages
- Les `design pattern` - Nous avons utiliser `Singleton` et `Fabric`
- `Clang tidy` - Respect de règles de nommage et de qualité du code
- `Github` - Nous avons utiliser toutes les services disponibles
  - `Fork` - Un repo principale + deux autres repos  
  - `Actions - Lancement Compilation du projet, puis d'un testeur
  - `Rules` - Obligation d'avoir deux validation pour merge sur la main
  - `Projet` - Roadmap, Issue (avec branch pour chacune)
  - `Wiki`
  - `Pages` - pour notre Documentation `Doxygen`
- `Doxygen` - Documentation automatique grace à des commentaires dans notre code
- `Mardown` et `Mermaid` - pour notre README

---

## ft_IRC - Principales fonctions réseaux

```c++
/*
Crée un socket avec les paramètres passés. 
-family définit la famille du socket. Les valeurs principales sont AF_INET pour un socket IPv4, AF_INET6 pour un support IPv6. 
-type spécifie le type de socket. Les valeurs principales utilisées sont SOCK_STREAM pour TCP, SOCK_DGRAM pour UDP. 
-protocol définit le protocole à utiliser. Il sera dépendant du type de socket et de sa famille. Les valeurs principales sont IPPROTO_TCP pour un socket TCP, IPPROTO_UDP pour un socket UDP.
*/
 int socket(int family, int type, int protocol);
 
 //close socket
 int close(int socket);
//Les fonctions de cette forme sont les fonctions Host/Home to Network .
//Elles servent à convertir les données numériques de la machine en données
//« réseau ».
short htons(short value);
long htonl(long value);
//Il s’agit des fonctions inverses des hton*.
short ntohs(short value);
long ntohl(long value);
 /*
 _socket est le socket à connecter. 
server la structure représentant le serveur auquel se connecter. 
serverlen est la taille de la structure server. socklen_t est un type spécifique aux plateformes UNIX et peut être un int ou unsigned int . Généralement un sizeof(server).
 
 L’appel à cette fonction est bloquant tant que la connexion n’a pas été effectuée. Autrement dit : si cette fonction retourne, c’est que votre connexion a été effectuée et acceptée par l’ordinateur distant. Sauf si elle retourne une erreur bien sûr.

 */
 int connect(int _socket, const struct sockaddr* server, socklen_t serverlen); 
 
//ex:
sockaddr_in server;
server.sin_addr.s_addr = inet_addr(const char* ipaddress);
server.sin_family = AF_INET;
server.sin_port = htons(int port);
/*
socket est le socket auquel envoyer les données. 
datas les données à envoyer. 
len est la taille maximale des données à envoyer en octets. 
flags un masque d'options. Généralement 0.
*/
int send(int socket, const void* datas, size_t len, int flags);
int recv(int socket, void* buffer, size_t len, int flags);
/*
La fonction bind est utilisée pour assigner une adresse locale à un socket.  

sckt est le socket auquel est assigné l'adresse.  
name est la structure à assigner au socket.  
namelen est la taille de cette structure -> sizeof.
Retourne SOCKET_ERROR -1 en cas d'erreur, 0 sinon.
*/
int bind(SOCKET sckt, const struct addr* name, int namelen);
//ex
sockaddr_in addr;
addr.sin_addr.s_addr = INADDR_ANY; // indique que toutes les sources seront acceptées
addr.sin_port = htons(port); // toujours penser à traduire le port en réseau<br />
addr.sin_family = AF_INET; // notre socket est TCP
/*
sckt est le socket auquel les clients vont se connecter.
backlog est le nombre de connexions en attente qui peuvent être gérées. La valeur SOMAXCONN peut être utilisée pour laisser le système choisir une valeur correcte selon sa configuration.
*/
int listen(SOCKET sckt, int backlog) ;
/*
Accepte une connexion entrante. 

sckt est le socket serveur qui attend les connexions. 
addr recevra l'adresse du socket qui se connecte. 
addrlen est la taille de la structure pointée par addr.
*/
SOCKET accept(SOCKET sckt, struct sockaddr* addr, int* addrlen);
/*
Permet de récupérer l'adresse IP d'un socket, IPv4 ou IPv6, sous forme lisible. 

family est la famille du socket. 
src le pointeur vers l'adresse du socket. 
dst un pointeur vers un tampon où stocker l'adresse sous forme lisible. 
size la taille maximale du tampon.
*/
const char* inet_ntop(int family, const void* src, char* dst, socklen_t size);

/*poll() permet de surveiller plusieurs file descriptors (sockets, fichiers, etc.)
pour détecter quand ils sont prêts pour certaines opérations (lecture, écriture, erreur)
sans bloquer. */
#include <poll.h>
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct pollfd {
    int fd;         // File descriptor à surveiller
    short events;   // Événements à surveiller (INPUT)
    short revents;  // Événements qui se sont produits (OUTPUT)
};


```

---

## ft_IRC - Source - Documentation

[RFC-2812](https://www.rfc-editor.org/rfc/rfc2812)<br />
[Server TCP](https://bousk.developpez.com/cours/reseau-c++/TCP/01-premiers-pas/)<br />
[Poll](https://devarea.com/linux-io-multiplexing-select-vs-poll-vs-epoll/)<br />
[Network programing](https://beej.us/guide/bgnet/html/)<br />
[Serveur IRC](https://www.cs.cmu.edu/~srini/15-441/S10/project1/pj1_description.pdf)<br />
[Projet IRC](http://chi.cs.uchicago.edu/chirc/index.html)<br />
[Design Pattern](https://refactoring.guru/fr/design-patterns/singleton)<br />
[Mermaid](https://mermaid.js.org/syntax/classDiagram.html)<br />



