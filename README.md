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
Internet Relay Chat project at 42 (a text-based communication protocol on the Internet)

---
## 📋 ft_IRC - Sujet

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

### 🧪 Tests automatiques

Nous pourrons ajouter un workflow GitHub Actions pour :
- Compiler le projet automatiquement à chaque PR
- Vérifier qu’aucune PR ne casse le build

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
