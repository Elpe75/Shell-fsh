# include <stdlib.h> 
# include <stdio.h> 
#include <string.h>
#include <dirent.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>

#include "../include/externe.h"
#include "../include/prompt.h"
#include "../include/pwd.h"
#include "../include/decoupeCmd.h"
#include "../include/exit.h"
#include "../include/boucle_for_simple.h"
#include "../include/cd.h"
#include "../include/ftype.h"
#include "../include/commande_structuree.h"
#include "../include/if_else.h"

int execute_commande_quelconque(char **args, int last_status, char *command, int bool){
    // Quitte la boucle si le user ecrit exit 
    if(strcmp(args[0], "exit") == 0){
        char *exit_arg; // initialisation de la val d'exit
        if(args[1] != NULL) {
            exit_arg = args[1];
        } else {
            exit_arg = NULL;
        }
        //printf("val de retour après exit : %d \n", func_exit(exit_arg,last_status));
        last_status = func_exit(exit_arg,last_status); // exit 
    }
    // Gère le cas de la commande pwwd
    else if (strcmp(args[0], "pwd") == 0){
        last_status = 0;
        if (bool == 0) printf("%s\n", chemin_absolu());
    }

    // Gère le cas de la commande for
    else if (strcmp(args[0], "for") == 0){
        last_status = boucle_for_simple(args[3], args[4], last_status, bool);
    }

    else if (strcmp(args[0], "cd") == 0){
        last_status = cd(args);
    }
    else if (strcmp(args[0], "ftype") == 0){
        last_status = ftype(args, bool);
    }
    else if(strcmp(args[0], "if") == 0){
        last_status = executer_commande_if_else(args, last_status);
    }
    else{
        //printf("%s : commande invalide ou pas encore implémentée !\n", command);
        last_status = commande_externe(args);
    }

    return last_status;
}

int main(){
    char *command; 
    int last_status = 0;
    rl_outstream = stderr;

    while(1){
        // Création du prompt
        char prompt[PATH_MAX + 50];
        create_prompt(last_status, prompt, sizeof(prompt));
        fprintf(stderr, "%s", prompt);
        fflush(stderr);

        // Lit la commande du user 
        command = readline(""); 

        if(command == NULL){
            break;
        } 

        if (strlen(command) == 0) {
            free(command);  // Libère la mémoire allouée pour une commande vide
            continue;  // Passe à la prochaine itération de la boucle sans découper une commande vide
        }

        if (strlen(command) > 0) {
            add_history(command);  // Ajouter la commande à l'historique
        }

        if (is_structured(command)){
            int *result;
            result = execute_structured_command(command, last_status);
            last_status = result[1];
            if (result[0] != 0) return last_status;
        }
        else{
            // decoupe la commande
            char **args = decoupe(command);

            printf("Affichage dans fsh: \n");
            
            int taille = sizeof(args) / sizeof(args[0]);
            for (int i = 0; args[i] != NULL; i++) {
                printf("%s#", args[i]);
            }
            //printf("args[0] = %s et args[1] = %s\n", args[0], args[1]);
            printf("\n");

            // Quitte la boucle si le user ecrit exit 
            if(strcmp(args[0], "exit") == 0){
                last_status = execute_commande_quelconque(args, last_status, command, 0);
                return last_status;
            }
            else{
                last_status = execute_commande_quelconque(args, last_status, command, 0);
            }
            free(command); 
            for (int i = 0; args[i] != NULL; i++) {
                free(args[i]);
            }
            free(args);
        }
    }
    
    return last_status;
}
