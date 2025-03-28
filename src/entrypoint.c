#include "entrypoint.h"
#include "sqlite3.h"
#include "knob.h"

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
    sqlite3_open("./fruits.db",&db);
    see_state_fruits(db,"FL");
    see_state_fruits(db,"CA");
    see_state_fruits(db,"NC");
    // add_random_fruit(db); //Uncomment to add a random fruit...
    sqlite3_close(db);
    return;
}