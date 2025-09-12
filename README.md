# ft_IRC
Internet Relay Chat project at 42 (a text-based communication protocol on the Internet)

---
## 🛰️ ft_IRC — Workflow de collaboration

Ce dépôt est le dépôt principal du projet ft_IRC (école 42).
Nous travaillons en équipe de 3 étudiants, chacun sur un fork personnel de ce dépôt.
Toutes les contributions passent par des Pull Requests (PR) vers ce dépôt principal.

### 🧩 Organisation générale

- `main` — branche stable, propre, toujours fonctionnelle.
- =dev= (optionnel) — branche commune de développement (avant main).
- =feature/...= — branches créées dans chaque fork pour développer une fonctionnalité.

### ⚙️ Mise en place du dépôt local (une seule fois)

1. Forker le dépôt principal (bouton Fork sur GitHub)
2. Cloner son fork personnel :
```bash
git clone git@github.com:MON-USERNAME/ft_IRC.git
cd ft_IRC
```
3. Relier le dépôt principal en upstream :
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
	- base : TON-USERNAME/ft_IRC → main
	- compare : MON-USERNAME/ft_IRC → feature/ma_fonction
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

- Branches : feature/... ou fix/...
- Jamais coder sur main
- Faire des PR petites et régulières

### 🧪 (Optionnel) Tests automatiques

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

✨ Astuce : toujours synchroniser main avant de créer une branche de feature.
---
