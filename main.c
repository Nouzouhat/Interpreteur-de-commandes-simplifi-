int main() {
    char buffer[MAX_COMMAND_LENGTH];
    
    // Message de bienvenue et instructions
    printf("=== Shell Simplifié ===\n");
    printf("Tapez 'exit' pour quitter\n");
    printf("Commandes disponibles:\n");
    printf("- Commandes système (ls, pwd, cat, etc.)\n");
    printf("- Commandes internes : cd, history, setenv, printenv, alias\n");
    printf("- Fonctionnalités : redirection (<, >), pipeline (|), arrière-plan (&)\n");
    printf("- Gestion des alias : alias nom='commande'\n");
    printf("- Complétion automatique : utilisez la touche Tab\n\n");
    
    // Initialiser readline et charger les alias
    initialiser_readline();
    loadAliases();
    
    while (1) {
        lireCommande(buffer);
        
        if (strlen(buffer) > 0) {
            ajouterHistorique(buffer);
            
            if (strchr(buffer, '|') != NULL) {
                gererPipeline(buffer);
            } else {
                char* arguments[MAX_ARGS];
                parserCommande(buffer, arguments);
                
                if (!executerCommandeInterne(arguments)) {
                    executerCommande(arguments);
                }
            }
        }
    }
    
    return 0;
}