
# Interpréteur de commandes simplifié

## Description
Ce projet est une implémentation d'un interpréteur de commandes simplifié en langage C. Il permet l'exécution de commandes système basiques et inclut plusieurs fonctionnalités avancées, telles que les redirections, les pipelines, les alias et la complétion automatique.

## Fonctionnalités
- Exécution de commandes système basiques
- Commandes internes (cd, exit, history)
- Redirections d'entrée et de sortie
- Support des pipelines (|)
- Exécution en arrière-plan (&)
- Historique des commandes
- Gestion des variables d'environnement
- Alias de commandes avec persistance
- Complétion automatique des commandes et fichiers

## Prérequis
- GCC (GNU Compiler Collection)
- Bibliothèque readline
- Système d'exploitation UNIX/Linux

### Installation des dépendances
Sur Ubuntu/Debian :
```bash
sudo apt-get update
sudo apt-get install gcc
sudo apt-get install libreadline-dev
```

## Compilation

Compilez le programme :
```bash
gcc -o interpréteur main.c shell.c -lreadline
```

## Exécution
Lancez le shell :
```bash
./interpréteur
```

## Utilisation

### Commandes de base
```bash
ls              # Liste les fichiers
pwd             # Affiche le répertoire courant
cd [dossier]    # Change de répertoire
exit            # Quitte le shell
```

### Redirections
```bash
ls > fichier.txt          # Redirige la sortie vers un fichier
cat < fichier.txt         # Lit depuis un fichier
```

### Pipelines
```bash
ls | grep .txt            # Filtre la sortie de ls
ls -l | grep ^d | sort    # Pipelines multiples
```

### Variables d'environnement
```bash
setenv VAR valeur         # Définit une variable d'environnement
printenv                  # Affiche les variables d'environnement
```

### Alias
```bash
alias ll='ls -l'          # Crée un alias
alias                     # Liste les alias
```

### Complétion automatique
- Utilisez la touche TAB pour la complétion des commandes et fichiers
- Double TAB pour voir toutes les possibilités

### Historique
```bash
history                   # Affiche l'historique des commandes
```

## Structure du Projet
- `main.c` : Point d'entrée du programme et boucle principale
- `shell.h` : Fichier d'en-tête avec les déclarations
- `shell.c` : Implémentation des fonctionnalités
- `.shellrc` : Fichier de configuration pour les alias (créé automatiquement)



## Dépannage
1. Erreur de compilation avec readline :
```bash
# Si vous obtenez une erreur liée à readline, vérifiez son installation :
sudo apt-get install libreadline-dev
```

2. Problèmes de droits d'exécution :
```bash
# Donnez les droits d'exécution au fichier compilé :
chmod +x interpréteur
```

3. Nettoyage et recompilation :
```bash
# Pour recompiler proprement :
rm interpréteur
gcc -o interpréteur main.c shell.c -lreadline
```

## Tests
Pour vérifier les fuites mémoire :
```bash
valgrind --leak-check=full ./interpréteur
```


## Autrices
- [ATHOUMANI AHAMADA Nouzouhati](https://gitlab.esiea.fr/athoumaniahama)
- [DOSSO Aminata](https://gitlab.esiea.fr/adosso)
- [GNAMBLOHOU Sessi Laurine](https://gitlab.esiea.fr/gnamblohou)
