#include "entrypoint.h"
#include "sqlite3.h"
#include "knob.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define LOG_SQLITE3_ERROR(db) knob_log(KNOB_ERROR, "%s:%d: SQLITE3 ERROR: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db))

/*
* Fonction qui affiche des fruits spécifique, on utilise une valeur passé en paramètre
* pour donner une valeur à un ? dans une requêtes SQL.
*/
void see_state_fruits(sqlite3* db,char* state_name){
    sqlite3_stmt* stmt = NULL;
    char content[] =
    "SELECT fruit,price\n"
    "FROM FruitsForSale\n"
    "WHERE etat=?;";// <---------------IMPORTANT: Le ? seras défini plus tard avec la fonction bind_text (ligne 33)
    int ret = sqlite3_prepare_v2(db,content,-1,&stmt,NULL);

    if(ret != SQLITE_OK){
        LOG_SQLITE3_ERROR(db);
    }
    /*
    IMPORTANT:
        Est nécessaire à la requête car elle a un ?. On peut avoir plusieurs ?
        dans une requête donc nous aurons alors besoin d'autant de bind que de ?.
        Si vous ne binder pas de valeur au ?, il y aura une erreur.
        Si on a plusieurs ?, les index des ? seront 1,2,3,etc. Les index devront être
        spécifiés après le paramètre statement à chaque opération de bind.
        Voir exemple, add_random_fruit.
    */
    if(sqlite3_bind_text(stmt,1,state_name,-1,SQLITE_STATIC) != SQLITE_OK){
        LOG_SQLITE3_ERROR(db);
    }

    /*
    Ici On créer une boucle parce que les requêtes SQL vont retourner plusieurs ranger exemple:
        Orange|0.85 --- Première ranger
        Lemon|1.25  --- Deuxième ranger
    La boucle continue tant que step retourne une ranger(row)
    */
    for(int ret = sqlite3_step(stmt);ret != SQLITE_DONE;ret = sqlite3_step(stmt)){
        if(ret != SQLITE_ROW){
            LOG_SQLITE3_ERROR(db);
        }
        int column = 0;
        const unsigned char* fruit = sqlite3_column_text(stmt,column++); //Permet d'aller chercher le nom du fruit
        float price = sqlite3_column_double(stmt,column); //Permet d'aller chercher le prix du fruit
        knob_log(KNOB_INFO,"Etat %s a le fruit %s avec le prix %.2f$",state_name,fruit,price);
    }

    sqlite3_finalize(stmt);
}
const char* fruits[] = {
    "Banana",
    "Orange",
    "Grape",
    "Pear",
    "Kumqat"
};
const char* states[] = {
    "CA",
    "SC",
    "NC",
    "FL",
    "HA"
};
void add_random_fruit(sqlite3* db){
    /*
        Commence une TRANSACTIONS SQL. sqlite3_exec permet d'exécuter
        une requête SQL qui n'a pas à être préparée vu quel n'a pas de valeur à recevoir du programme C.
    */
    if (sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL) != SQLITE_OK) {
        LOG_SQLITE3_ERROR(db);
        return;
    }
    sqlite3_stmt* stmt = NULL;
    char content[] =
    "INSERT INTO FruitsForSale(fruit,etat,price)\n"
    "VALUES\n"
    "(?,?,?);";
    int ret = sqlite3_prepare_v2(db,content,-1,&stmt,NULL);
    int fruit = rand() %5;
    int state = rand() %5;
    double price = ((rand()+1) % 100) /100 + rand() % 2;
    if(sqlite3_bind_text(stmt,1,fruits[fruit],-1,SQLITE_STATIC) != SQLITE_OK){
        LOG_SQLITE3_ERROR(db);
    }
    if(sqlite3_bind_text(stmt,2,states[state],-1,SQLITE_STATIC) != SQLITE_OK){
        LOG_SQLITE3_ERROR(db);
    }
    if(sqlite3_bind_double(stmt,3,price) != SQLITE_OK){
        LOG_SQLITE3_ERROR(db);
    }
    ret = sqlite3_step(stmt);
    /*
        Ici on fait encore une requête SQL qui roule le SQL directement,
        mais maintenant on termine la TRANSACTION avec le COMMIT.
    */
    if (sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL) != SQLITE_OK) {
        LOG_SQLITE3_ERROR(db);
    }
}

void raylib_start(void){
    srand(time(NULL));
    sqlite3* db = NULL;
    sqlite3_open("./aventure_quete.db",&db);
    

    int choix = 0;
    int decision = 0;
    int choixpnj = 0;
    int choixlieux = 0;
    char PlayerName[120];
    int idJoueur = 0;
    int viejoueur = 100;

    printf("Creer un personnage\n");
    scanf("%s",&PlayerName);
    printf("Voulez-vous avoir l'histoire?\n");
    printf("1: OUI\n2: NON\n");
    scanf("%d",&choix);

    if(choix == 1){
        printf("\nHistoire : Vous etes dans la peau de quelqu'un qui c'est fait kick out de son village parcequ'il ne servait a rien. \n");
        printf("Vous partez de votre village avec des provisions pour survivre. Vous marchez donc jour et nuit a la recherche d'un \nendroit ou vivre. ");
        printf("Apres quelques jours vous apercevez un village au loin et vous decidez d'aller y jeter un coup \nd'oeil. ");
        printf("Une fois rendu au village vous demandez aux villageois si vous pouvez vous installer en dehors du village.");
        printf("\nIls vous demandent pourquoi vous etes ici. Vous leurs raconter votre histoire.");
        printf("Ils acceptent votre demande et ils vous \npropose de les laisser vous batir une maison. Vous et votre lacheter vous acceptez, mais vous leur disez merci.\n");
        printf("Apres deux jours votre maison est construite, vous les remerciez.\n");
    }
    if(choix == 2){
        printf("Dommage\n");
    }

    printf("Ou voulez-vous aller?\n");
    printf("1 : Mine Hante\n2 : Foret Sombre\n3 : Village Abandonne\n4 : Maison\n5 : Village\n6 : Marecage\n7 : Quitter\n");
    scanf("%d", &choixlieux);
    while(choixlieux !=7){
            sqlite3_stmt* stmt;

        if(choixlieux == 1)
        {
            printf("Voulez-vous une description du lieu?\n");
            printf("1: OUI\n2: NON\n");
            scanf("%d",&choix);
            if(choix == 1){
                const char* sql = "SELECT description FROM lieux WHERE id=1";
                int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
            
                while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                    const unsigned char *description = sqlite3_column_text(stmt,0);
                    printf("%s\n",description);
                }
            
            }
            if(choix == 2){
                printf("OK\n");
            }
            printf("Vous arrivez a la Mine Hante que faite-vous?\n");
            printf("1 : Winniefred\n");
            scanf("%d", &choixpnj);
        
            if(choixpnj == 1)
            {
                printf("Que voulez-vous faire?\n");
                printf("1: description\n2: dialogue\n");
                scanf("%d",&decision);
                if(decision == 1){
                    
                    const char* sql = "SELECT description FROM pnj WHERE id=3";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
 
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char * description = sqlite3_column_text(stmt,0);
                        printf("%s\n",description);
                    }
                }
                if(decision == 2){
                    const char* sql = "SELECT dialogue FROM pnj WHERE id=3";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
 
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char * dialogue = sqlite3_column_text(stmt,0);
                        printf("%s\n",dialogue);
                    }
                }    
            }
            printf("Ou voulez-vous aller maintenant?\n");
            printf("2 : Foret Sombre\n3 : Village Abandonne\n4 : Maison\n5 : Village\n6 : Marecage\n7 : Quitter\n");
            scanf("%d",&choixlieux);
        }

        if(choixlieux == 2)
        {
            printf("Voulez-vous une description du lieu?");
            printf("1: OUI\n2: NON\n");
            scanf("%d",&choix);
            if(choix == 1){
                const char* sql = "SELECT description FROM lieux WHERE id=2";
                int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                    const unsigned char *description = sqlite3_column_text(stmt,0);
                    printf("%s\n",description);
                }
            }
            if(choix != 1)
            {
                printf("OK\n");
            }     
            printf("Ou voulez-vous aller maintenant?\n");
            printf("1 : Mine Hante\n3 : Village Abandonne\n4 : Maison\n5 : Village\n6 : Marecage\n7 : Quitter\n");
            scanf("%d",&choixlieux);
        }

        if(choixlieux == 3)
        {
            printf("Voulez-vous une description du lieu?");
            printf("1: OUI\n2: NON\n");
            scanf("%d",&choix);
            if(choix == 1){
                const char* sql = "SELECT description FROM lieux WHERE id=3";
                int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                    const unsigned char *description = sqlite3_column_text(stmt,0);
                    printf("%s\n",description);
                }
            }
            if(choix != 1){
                printf("OK\n");
            }
            printf("Ou voulez-vous aller maintenant?\n");
            printf("1 : Mine Hante\n2 : Foret Sombre\n4 : Maison\n5 : Village\n6 : Marecage\n7 : Quitter\n");
            scanf("%d",&choixlieux);
        }

        if(choixlieux == 4)
        {
            printf("Voulez-vous une description du lieu?");
            printf("1: OUI\n2: NON\n");
            scanf("%d",&choix);
            if(choix == 1){
                const char* sql = "SELECT description FROM lieux WHERE id=4";
                int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                    const unsigned char *description = sqlite3_column_text(stmt,0);
                    printf("%s\n",description);
                }
            }
            if(choix != 1){
                printf("OK\n");
            }
            printf("Ou voulez-vous aller maintenant?\n");
            printf("1 : Mine Hante\n2 : Foret Sombre\n3 : Village Abandonne\n5 : Village\n6 : Marecage\n");
            scanf("%d",&choixlieux);
        }

        if(choixlieux == 5){
            printf("Voulez-vous une description du lieu?");
            printf("1: OUI\n2: NON\n");
            scanf("%d",&choix);
            
            if(choix == 1){
                
                const char* sql = "SELECT description FROM lieux WHERE id=5";
                int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                    const unsigned char *description = sqlite3_column_text(stmt,0);
                    printf("%s\n",description);
                }
            }
            if(choix != 1){
                printf("OK\n");
            }

            printf("Vous arrivez au village que faite-vous? \n");
            printf("1 : Bernadette-Alfonsinne 3e du nom\n 2 : Matrona\n 3 : Panoramix\n 4 : Chloe La Grosse Torche\n");
            scanf("%d", &choixpnj);
            if(choixpnj == 1)
            {
                
                printf("Que voulez-vous faire?\n");
                printf("1: description\n2: dialogue\n");
                scanf("%d",&decision);
                if(decision == 1){
                    
                    const char* sql = "SELECT description FROM pnj WHERE id=1";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
 
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char * description = sqlite3_column_text(stmt,0);
                        printf("%s\n",description);
                    }
                }
                if(decision == 2){
                    const char* sql = "SELECT dialogue FROM pnj WHERE id=1";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                
                    
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char * dialogue = sqlite3_column_text(stmt,0);
                        printf("%s\n",dialogue);
                    }
                }
            }
                
            
            if(choixpnj == 2)
            {
                printf("Que voulez-vous faire?\n");
                printf("1: description\n2: dialogue\n");
                scanf("%d",&decision);
                if(decision == 1){
                    
                    const char* sql = "SELECT description FROM pnj WHERE id=4";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char *description = sqlite3_column_text(stmt,0);
                        printf("%s\n",description);
                    }
                }
                if(decision == 2){
                    const char* sql = "SELECT dialogue FROM pnj WHERE id=4";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                
                    
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char * dialogue = sqlite3_column_text(stmt,0);
                        printf("%s\n",dialogue);
                    }
                }
            }

            if(choixpnj == 3)
            {
                printf("Que voulez-vous faire?\n");
                printf("1: description\n2: dialogue\n");
                scanf("%d",&decision);
                if(decision == 1){
                    
                    const char* sql = "SELECT description FROM pnj WHERE id=5";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char *description = sqlite3_column_text(stmt,0);
                        printf("%s\n",description);
                    }
                }
                if(decision == 2){
                    const char* sql = "SELECT dialogue FROM pnj WHERE id=5";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                
                    
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char * dialogue = sqlite3_column_text(stmt,0);
                        printf("%s\n",dialogue);
                    } 
                }
            }
            if(choixpnj == 4)
            {
                printf("Que voulez-vous faire?\n");
                printf("1: description\n2: dialogue\n");
                scanf("%d",&decision);
                if(decision == 1){
                    
                    const char* sql = "SELECT description FROM pnj WHERE id=6";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char *description = sqlite3_column_text(stmt,0);
                        printf("%s\n",description);
                    }
                }
                if(decision == 2){
                    const char* sql = "SELECT dialogue FROM pnj WHERE id=6";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                
                    
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char * dialogue = sqlite3_column_text(stmt,0);
                        printf("%s\n",dialogue);
                    }
                    
                }
            }
            printf("Ou voulez-vous aller maintenant?\n");
            printf("1 : Mine Hante\n2 : Foret Sombre\n3 : Village Abandonne\n4 : Maison\n6 : Marecage\n7 : Quitter\n");
            scanf("%d",&choixlieux);
        }
    
        if(choixlieux == 6)
        {
            printf("Voulez-vous une description du lieux");
            printf("1: OUI\n2: NON\n");
            scanf("%d",&choix);
            if(choix == 1){
                const char* sql = "SELECT description FROM lieux WHERE id=6";
                int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
            
                while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                    const unsigned char *description = sqlite3_column_text(stmt,0);
                    printf("%s\n",description);
                }
            }
            if(choix != 1){
                printf("OK\n");
            }
            printf("Vous arrivez au marécage que faite-vous?\n");
            printf("1 : Jean-Pierre-Louis\n");
            scanf("%d", &choixpnj);

            if(choixpnj == 1)
            {
                printf("Que voulez-vous faire?\n");
                printf("1: description\n2: dialogue\n");
                scanf("%d",&decision);
                if(decision == 1){
                        const char* sql = "SELECT description FROM pnj WHERE id=2";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
                
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char *description = sqlite3_column_text(stmt,0);
                        printf("%s\n",description);
                    }
                }
                if(decision == 2){
                    const char* sql = "SELECT dialogue FROM pnj WHERE id=2";
                    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
            
                    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
                        const unsigned char *dialogue = sqlite3_column_text(stmt,0);
                        printf("%s\n",dialogue);
                    }
                }
                
            }
            printf("Ou voulez-vous aller maintenant?\n");
            printf("1 : Mine Hante\n2 : Foret Sombre\n3 : Village Abandonne\n4 : Maison\n5 : Village\n7 : Quitter\n");
            scanf("%d",&choixlieux);
        }


        if(choixlieux == 7){
            sqlite3_close(db);
            return;
        }
    }
    
}

    
