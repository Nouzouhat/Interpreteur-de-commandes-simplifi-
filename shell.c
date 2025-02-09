/**
 * shell.c - Implémentation de l'interpréteur de commandes
 * Contient toutes les fonctions principales du shell
 */

#include "shell.h"
#include <errno.h>

// Variables globales pour l'historique et les alias
HistoryNode* historique = NULL;
Alias* aliasList = NULL;


/**
 * Lit une commande depuis l'entrée utilisateur avec readline
 * @param buffer Buffer où stocker la commande lue
 */
void lireCommande(char* buffer) {
    char* ligne = readline("interprèteur> ");
    
    if (ligne == NULL) {
        printf("\nAu revoir!\n");
        exit(0);
    }
    
    // Si la ligne n'est pas vide, l'ajouter à l'historique readline
    if (strlen(ligne) > 0) {
        add_history(ligne);
    }
    
    // Copier la ligne dans le buffer
    strncpy(buffer, ligne, MAX_COMMAND_LENGTH - 1);
    buffer[MAX_COMMAND_LENGTH - 1] = '\0';
    
    // Libérer la mémoire allouée par readline
    free(ligne);
}


/**
 * Analyse et découpe la commande en arguments
 * @param buffer Commande brute à parser
 * @param arguments Tableau où stocker les arguments parsés
 */
void parserCommande(char* buffer, char** arguments) {
    char* token = strtok(buffer, " \t");
    int i = 0;
    
	// Découpe la commande en arguments
    while (token != NULL && i < MAX_ARGS - 1) {
        arguments[i++] = token;
        token = strtok(NULL, " \t");
    }
    arguments[i] = NULL;
}


/**
 * Exécute une commande avec gestion du mode arrière-plan
 * @param arguments Tableau des arguments de la commande
 */
void executerCommande(char** arguments) {
    pid_t pid;
    int status;
    int background = 0;
    
	// Vérifie si la commande doit s'exécuter en arrière-plan
    for (int i = 0; arguments[i] != NULL; i++) {
        if (strcmp(arguments[i], "&") == 0) {
            background = 1;
            arguments[i] = NULL;
            break;
        }
    }
    
    pid = fork();
    if (pid == 0) { // Processus enfant
        gererRedirection(arguments);
        if (execvp(arguments[0], arguments) == -1) {
            perror("Erreur d'exécution");
            exit(1);
        }
    } else if (pid < 0) {
        perror("Erreur de fork");
    } else if (!background) {
       f (waitpid(pid, &status, 0) == -1) { 

                perror("Erreur lors de l'attente du processus enfant");

            }

        } else {

            printf("Processus en arriere-plan lance \n");

        }

        while ((pid_z = waitpid(-1, NULL, WNOHANG)) > 0) { // Nettoyage de tous les processus zombies présents lors de l'éxécution de la commande
                printf("Processus zombies nettoyes\n");
            }
}


/**
 * Gère l'exécution des commandes internes du shell
 * @param arguments Tableau des arguments de la commande
 * @return 1 si commande interne exécutée, 0 sinon
 */
int executerCommandeInterne(char** arguments) {
    if (arguments[0] == NULL) return 1;
	
    // Gestion de setenv
    if (strcmp(arguments[0], "setenv") == 0) {
        setEnvVar(arguments);
        return 1;
    }
    
	// Gestion de printenv
    if (strcmp(arguments[0], "printenv") == 0) {
        printEnvVars();
        return 1;
    }
    
	// Gestion des alias
    if (strcmp(arguments[0], "alias") == 0) {
        if (arguments[1] == NULL) {
            listAliases();
        } else {
            addAlias(arguments);
        }
        return 1;
    }
    
	// Vérification et exécution des alias existants
    char* aliasCmd = checkAlias(arguments[0]);
    if (aliasCmd != NULL) {
        char newCmd[MAX_COMMAND_LENGTH] = {0};
        strncpy(newCmd, aliasCmd, MAX_COMMAND_LENGTH - 1);
        
		// Ajout des arguments à la commande alias
        for (int i = 1; arguments[i] != NULL; i++) {
            strncat(newCmd, " ", MAX_COMMAND_LENGTH - strlen(newCmd) - 1);
            strncat(newCmd, arguments[i], MAX_COMMAND_LENGTH - strlen(newCmd) - 1);
        }
        
        char* newArguments[MAX_ARGS];
        parserCommande(newCmd, newArguments);
        executerCommande(newArguments);
        return 1;
    }
    
	// Gestion de la commande exit
    if (strcmp(arguments[0], "exit") == 0) {
        cleanupHistorique();
        cleanupAliases();
        printf("Au revoir!\n");
        exit(0);
    }
    
    if (strcmp(arguments[0], "cd") == 0) {
        if (arguments[1] == NULL) {
            chdir(getenv("HOME"));
        } else if (chdir(arguments[1]) != 0) {
            perror("cd");
        }
        return 1;
    }
    
	// Gestion de history
    if (strcmp(arguments[0], "history") == 0) {
        afficherHistorique();
        return 1;
    }
    
    return 0;
}


/**
 * Gère la redirection des entrées/sorties
 * @param arguments Tableau des arguments de la commande
 */
void gererRedirection(char** arguments) {
    FILE* file = NULL;  // Fichier créé pour la redirection
    char* mode = NULL;  // Mode pour freopen
    
    // Parcours du tableau d'arguments
    for (int i = 0; arguments[i] != NULL; i++) {
        FILE* std = NULL;  // Précision de la sortie/entrée standard
        
        // Détection des chevrons
        if (strcmp(arguments[i], "<") == 0) {  // entrée
            mode = "r";
            std = stdin;
        }
        else if (strcmp(arguments[i], ">") == 0) {  // sortie simple
            mode = "w";
            std = stdout;
        }
        else if (strcmp(arguments[i], ">>") == 0) {  // sortie sans écrasement
            mode = "a";
            std = stdout;
        }
        else if (strcmp(arguments[i], "2>") == 0) {  // Cas erreur
            mode = "w";
            std = stderr;
        }

        // Redirection
        if (std != NULL) {  // Cas où il y a bien un chevron
            // Gestion d'erreurs si absence de fichier
            if (arguments[i + 1] == NULL) {
                fprintf(stderr, "Erreur, aucun fichier spécifié pour la redirection\n");
                exit(EXIT_FAILURE);
            }
            
            // Tentative de redirection
            FILE* new_stream = freopen(arguments[i + 1], mode, std);
            if (new_stream == NULL) {
                perror("Erreur de redirection");
                exit(EXIT_FAILURE);
            }
            
            // Supression des arguments liés à la redirection
            arguments[i] = NULL;
            arguments[i + 1] = NULL;
            i++;  // Pour que le parcours de la boucle continue après le fichier de redirection
        }
    }
}


/**
 * Définit une variable d'environnement
 * @param arguments Tableau contenant la variable et sa valeur
 */
void setEnvVar(char** arguments) {
    if (arguments[1] == NULL || arguments[2] == NULL) {
        printf("Usage: setenv VARIABLE VALEUR\n");
        return;
    }
    
    if (setenv(arguments[1], arguments[2], 1) != 0) {
        perror("setenv");
    }
}


/**
 * Affiche toutes les variables d'environnement
 */
void printEnvVars(void) {
    extern char **environ;
    for (char **env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }
}


/**
 * Ajoute un nouvel alias
 * @param arguments Tableau contenant le nom et la commande de l'alias
 */
void addAlias(char** arguments) {
    if (arguments[1] == NULL || arguments[2] == NULL) {
        printf("Usage: alias nom='commande'\n");
        return;
    }
    
	// Allocation et initialisation du nouvel alias
    Alias* newAlias = malloc(sizeof(Alias));
    if (newAlias == NULL) {
        perror("malloc");
        return;
    }
    
	// Copie sécurisée des données
    strncpy(newAlias->name, arguments[1], MAX_ALIAS_NAME - 1);
    strncpy(newAlias->command, arguments[2], MAX_ALIAS_COMMAND - 1);
    newAlias->next = aliasList;
    aliasList = newAlias;
    
	// sauvegarde
    saveAliases();
}

/**
* Charge les alias depuis le fichier de configuration
*/
void loadAliases(void) {
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", getenv("HOME"), CONFIG_FILE);
    
    FILE* file = fopen(path, "r");
    if (file == NULL) return;
    
	// Lecture ligne par ligne du fichier de configuration
    char line[MAX_ALIAS_NAME + MAX_ALIAS_COMMAND + 2];
    while (fgets(line, sizeof(line), file)) {
        char name[MAX_ALIAS_NAME];
        char command[MAX_ALIAS_COMMAND];
        
		// Parsing de chaque ligne d'alias
        if (sscanf(line, "%s %s", name, command) == 2) {
            Alias* newAlias = malloc(sizeof(Alias));
            if (newAlias) {
                strncpy(newAlias->name, name, MAX_ALIAS_NAME - 1);
                strncpy(newAlias->command, command, MAX_ALIAS_COMMAND - 1);
                newAlias->next = aliasList;
                aliasList = newAlias;
            }
        }
    }
    
    fclose(file);
}

/**
* Sauvegarde les alias dans le fichier de configuration
*/
void saveAliases(void) {
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", getenv("HOME"), CONFIG_FILE);
    
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        perror("Impossible de sauvegarder les alias");
        return;
    }
    
    Alias* current = aliasList;
    while (current != NULL) {
        fprintf(file, "%s %s\n", current->name, current->command);
        current = current->next;
    }
    
    fclose(file);
}


/**
* Vérifie si une commande est un alias
* @param command La commande à vérifier
* @return La commande associée à l'alias ou NULL
*/
char* checkAlias(const char* command) {
    Alias* current = aliasList;
    while (current != NULL) {
        if (strcmp(current->name, command) == 0) {
            return current->command;
        }
        current = current->next;
    }
    return NULL;
}


// Affiche la liste des alias définis
void listAliases(void) {
    Alias* current = aliasList;
    while (current != NULL) {
        printf("%s='%s'\n", current->name, current->command);
        current = current->next;
    }
}


/**
* Gère l'exécution des pipelines de commandes
* @param commande La ligne de commande complète avec pipes
*/
void gererPipeline(char* commande) {
   char* cmds[MAX_ARGS];
   int cmd_count = 0;
   
   // Séparation des commandes par les pipes
   char* token = strtok(commande, "|");
   while (token != NULL && cmd_count < MAX_ARGS - 1) {
       cmds[cmd_count++] = token;
       token = strtok(NULL, "|");
   }
   
   // Cas d'une commande unique sans pipe
   if (cmd_count == 1) {
       char* arguments[MAX_ARGS];
       parserCommande(cmds[0], arguments);
       if (!executerCommandeInterne(arguments)) {
           executerCommande(arguments);
       }
       return;
   }
   
   // Création des pipes nécessaires
   int pipes[MAX_ARGS - 1][2];
   for (int i = 0; i < cmd_count - 1; i++) {
       if (pipe(pipes[i]) < 0) {
           perror("Erreur de création du pipe");
           exit(1);
       }
   }
   
   // Exécution de chaque commande du pipeline
   for (int i = 0; i < cmd_count; i++) {
       pid_t pid = fork();
       
       if (pid == 0) {
           char* arguments[MAX_ARGS];
           parserCommande(cmds[i], arguments);
           
           // Configuration des redirections pour les pipes
           if (i > 0) {
               dup2(pipes[i-1][0], STDIN_FILENO);
           }
           if (i < cmd_count - 1) {
               dup2(pipes[i][1], STDOUT_FILENO);
           }
           
           // Fermeture des pipes inutilisés
           for (int j = 0; j < cmd_count - 1; j++) {
               close(pipes[j][0]);
               close(pipes[j][1]);
           }
           
           execvp(arguments[0], arguments);
           perror("Erreur d'exécution dans le pipeline");
           exit(1);
       }
   }
   
   // Fermeture des pipes dans le processus parent
   for (int i = 0; i < cmd_count - 1; i++) {
       close(pipes[i][0]);
       close(pipes[i][1]);
   }
   
   // Attente de la fin de tous les processus
   for (int i = 0; i < cmd_count; i++) {
       wait(NULL);
   }
}

/**
* Ajoute une commande à l'historique
* @param commande La commande à ajouter
*/
void ajouterHistorique(char* commande) {
   HistoryNode* nouveau = malloc(sizeof(HistoryNode));
   if (nouveau == NULL) {
       perror("Erreur d'allocation mémoire pour l'historique");
       return;
   }
   
   // Copie de la commande dans la nouvelle entrée
   nouveau->command = strdup(commande);
   if (nouveau->command == NULL) {
       free(nouveau);
       perror("Erreur d'allocation mémoire pour la commande");
       return;
   }
   
   // Ajout en tête de liste
   nouveau->next = historique;
   historique = nouveau;
}

/**
* Affiche l'historique des commandes
*/
void afficherHistorique() {
   int index = 1;
   HistoryNode* courant = historique;
   
   while (courant != NULL) {
       printf("%d  %s\n", index++, courant->command);
       courant = courant->next;
   }
}

/**
* Libère la mémoire de l'historique
*/
void cleanupHistorique() {
   HistoryNode* courant = historique;
   while (courant != NULL) {
       HistoryNode* suivant = courant->next;
       free(courant->command);
       free(courant);
       courant = suivant;
   }
   historique = NULL;
}

/**
* Libère la mémoire des alias
*/
void cleanupAliases() {
   Alias* courant = aliasList;
   while (courant != NULL) {
       Alias* suivant = courant->next;
       free(courant);
       courant = suivant;
   }
   aliasList = NULL;
}

/**
* Fonction de complétion des commandes pour readline
* @param texte Le texte à compléter
* @param deb Position de début
* @param fin Position de fin
* @return Tableau des suggestions de complétion
*/
char **completion_commande(const char *texte, int deb, int fin) {
   char **suggestions = NULL;
   // Liste des commandes disponibles pour l'auto-complétion
   char *liste_commandes[] = {
       "ls", "cd", "pwd", "exit", "mkdir", "rmdir", "touch", "rm",
       "echo", "whoami", "cat", "cp", "mv", "grep", "setenv",
       "printenv", "alias", "history", NULL
   };
   int cpt = 0;

   // Complétion des commandes au début de la ligne
   if (deb == 0) {
       for (int i = 0; liste_commandes[i] != NULL; i++) {
           if (strncmp(liste_commandes[i], texte, strlen(texte)) == 0) {
               suggestions = realloc(suggestions, sizeof(char *) * (cpt + 1)); // allouer ou redimensionner le tableau de suggestion

               if (suggestions == NULL) {
                   perror("Erreur d'allocation mémoire");
                   exit(EXIT_FAILURE);
               }
			   
			   //copie de la commande dans la liste des suggestions
               suggestions[cpt] = strdup(liste_commandes[i]);
               cpt++;
           }
       }
   } 
   // Complétion des noms de fichiers ailleurs dans la ligne
   else {
       DIR *repertoire = opendir(".");
       struct dirent *entree;
       
       if (repertoire != NULL) {
           while ((entree = readdir(repertoire)) != NULL) { //lecture du répertoire
               if (strncmp(entree->d_name, texte, strlen(texte)) == 0) {
                   suggestions = realloc(suggestions, sizeof(char *) * (cpt + 1));
                   if (suggestions == NULL) {
                       perror("Erreur d'allocation mémoire");
                       exit(EXIT_FAILURE);
                   }
				   
				   //copie du nom du fichier-repertoire dans la liste des suggestions et on incrémente le compteur
                   suggestions[cpt] = strdup(entree->d_name);
                   cpt++;
               }
           }
           if (closedir(repertoire) == -1)
               perror("Erreur lors de la fermeture du répertoire");
       } else {
           perror("Erreur d'ouverture du répertoire");
       }
   }

   // Finalisation du tableau de suggestions
   if (cpt > 0) {
       suggestions = realloc(suggestions, sizeof(char *) * (cpt + 1)); // allouer ou redimensionner le tableau de suggestion

       if (suggestions == NULL) {
           perror("Erreur d'allocation mémoire");
           exit(EXIT_FAILURE);
       }
	   
	   //copie de la commande dans la liste des suggestions
       suggestions[cpt] = NULL;
   }
   
   return suggestions;
}

/**
* Initialise les fonctions de complétion de readline
*/
void initialiser_readline() {
   rl_bind_key('\t', rl_complete);
   rl_attempted_completion_function = completion_commande;
}