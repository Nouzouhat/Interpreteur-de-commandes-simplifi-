/**
 * shell.h - En-tête pour l'interpréteur de commandes
 * Contient les déclarations des structures et fonctions
 */

#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>

// Constantes pour les tailles maximales
#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64
#define HISTORY_SIZE 100
#define MAX_ALIAS_NAME 50
#define MAX_ALIAS_COMMAND 100
#define CONFIG_FILE ".shellrc"

// Structure pour stocker l'historique des commandes
typedef struct HistoryNode {
    char* command;
    struct HistoryNode* next;
} HistoryNode;

// Structure pour stocker les alias
typedef struct Alias {
    char name[MAX_ALIAS_NAME];
    char command[MAX_ALIAS_COMMAND];
    struct Alias* next;
} Alias;

// Variables globales
extern HistoryNode* historique;
extern Alias* aliasList;

// Fonctions principales du shell
void lireCommande(char* buffer);
void parserCommande(char* buffer, char** arguments);
void executerCommande(char** arguments);
int executerCommandeInterne(char** arguments);

// Fonctions de gestion des redirections et pipelines
void gererRedirection(char** arguments);
void gererPipeline(char* commande);

// Fonctions de gestion de l'historique
void ajouterHistorique(char* commande);
void afficherHistorique(void);
void cleanupHistorique(void);

// Fonctions de gestion des variables d'environnement
void setEnvVar(char** arguments);
void printEnvVars(void);

// Fonctions de gestion des alias
void addAlias(char** arguments);
void loadAliases(void);
void saveAliases(void);
char* checkAlias(const char* command);
void listAliases(void);
void cleanupAliases(void);

// Fonctions de complétion automatique
char **completion_commande(const char *texte, int deb, int fin);
void initialiser_readline(void);

#endif /* SHELL_H */