CREATE TABLE lieux (
    id INT PRIMARY KEY,
    nom TEXT,
    description TEXT
);

CREATE TABLE objects (
    id INT PRIMARY KEY,
    nom TEXT,
    description TEXT,
    id_lieu INT,
    FOREIGN KEY (id_lieu) REFERENCES lieux(id)
);

CREATE TABLE joueurs (
    id INT PRIMARY KEY,
    nom TEXT,
    vie INT,
    force INT,
    position_id INT,
    FOREIGN KEY (position_id) REFERENCES lieux(id)
);

CREATE TABLE pnj (
    id INT,
    nom TEXT,
    description TEXT,
    id_lieu INT,
    dialogue TEXT,
    FOREIGN KEY (id_lieu) REFERENCES lieux(id)
);

CREATE TABLE ennemis (
        id INT,
        nom TEXT,
        vie INt,
        force INT,
        id_lieu INT,
        FOREIGN KEY (id_lieu) REFERENCES lieux(id)
);

CREATE TABLE inventaire (
        id INT,
        id_joueur INT,
        id_object INT,
        FOREIGN KEY (id_joueur) REFERENCES joueurs(id),
        FOREIGN KEY (id_object) REFERENCES objects(id)
);

CREATE TABLE quetes (
        id INT,
        description TEXT,
        est_complet BOOLEAN,
        id_joueur INT,
        id_pnj INT,
        FOREIGN KEY (id_joueur) REFERENCES joueurs(id)
);

CREATE TABLE pnj_quetes (
        id INT,
        id_pnj INT,
        id_quete INT,
        FOREIGN KEY (id_pnj) REFERENCES pnj(id),
        FOREIGN KEY (id_quete) REFERENCES quetes(id)
);

INSERT INTO lieux (id,nom,description)
VALUES 
    ('1','Mine Hante','Cette mine de charbon est hante depuis des siecles. Certaines personnes disent que les esprits des anciens mineurs se promene la nuit.'),
    ('2','Foret Sombre','Cette foret tient son nom du fait que les rayons du soleil ne penetre pas dans la foret du a la densite des feuilles.'),
    ('3','Village Abandonne','Il y a quelques annees un virus a transforme tous les villageois en zombies les gens du village a coter les surnome les BlueSteaks et depuis ce temp ils utilisent un remedes pour les guerir.'),
    ('4','Maison','Home Sweet Home.'),
    ('5','Village','Un endroit pour acheter ce dont vous avez de besoin.'),
    ('6','Marecage','Un endroit humide parfait pour une baignade.');


INSERT INTO objects (id,nom,description,id_lieu)
VALUES
    ('1','Hache','Outil tres utile pour couper du bois.','2'),
    ('2','Pioche','Outil tres utile pour recolter des minerais.','1'),
    ('3','epe','Outil très utile pour tuer des ennemis.','5'),
    --Donner par le pnj de la quete principale
    ('4','Torche','Il fait trop sombre pas de probleme la torche est ton ami.','5'),
    ('5','Potion de Vie','Oh non tu est blaisse ou une autre personne well have a drink.','5'),
    ('6','Roger Coin Coin','This rubber duck is your best friend keep it close to you. If you need to talk he would love to ear what you have to say.','4'),
    ('7','Quenouille','A tu vue les belles quenouilles','6');
    

INSERT INTO pnj (id,nom,description,id_lieu,dialogue)
VALUES
        --source de noms : ma soeur une estit de crisse de calice de folle.

        ('1','Bernadette-Alfonsinne 3e du nom','La troisieme generation des Bernadette-Alfonsinne','5','Depuis quelque siecles nous avons perdu un artefacte tres ancien plusieurs personnes ont essaye de le recuperer, mais personne n"y ai parvenus. Voulez-tentez votre chance?'), 
        --Choix 1 : Oui. Réponse : Bon courage aventurier si vous réussissez revenez me voir.    
        --Choix 2 : Non. Réponse : Domage, si vous chagez d"idée n"ésitez pas à venir me voir.
        --Revien voir sans artefacte : Alors comment ce passe votre aventure?
        --Choix 1 : Très bien! Réponse : Wow continuer comme ça et vous récuperiez l'artefacte en un rien de temp!
        --Choix 2 : Très mal. Réponse : N'abandonné pas.
        --Revien voir avec artefacte : Wow vous avez réussis vous ête le meilleur des meilleurs.

        ('2','Jean-Pierre-Louis','Nageur professionnel','6','Bonjour aventurier, venez donc faire trempette avec moi, mais faite attention ce marecage est remplis de creature gluante avec une bave collante et pas bonne au gout je les ai batise ziz.'),
        --Choix 1 : Oui : Je vien vous rejoindre tout de suite. Réponse : YIPPEEEEEEE!!!   
        --Choix 2 : Non : je vais passer mon tour. Réponse : Bon ok à la prochaine.

        ('3','Winniefred','Gardienne de la mine','1','Si vous entrez dans la mine faite attention a Jack LeSourd'),
        --Choix 1 : Entrez. Réponse : Bon courage aventurier.
        --Choix 2 : Ne pas entrez. Réponse : Au revoir et à la prochaine.
                                                                                                                                                                               --matériaux pour pioche (1 manche + tête de pioche)
        ('4','Matrona','Blacksmith du village','5','Bonjour aventurier, je vois que vous etes à la recherche de l"artefacte ancien, cette artefacte peut uniquement etre recupere à l"aide d"une pioche. Pouriez-vous aller ramasser du bois dans la forêt sombre pour fabriquer un manche?'), --Oui : Pour pouvoir récolter du bois il vous faut une hache revenez me voir lorsque vous aurez les matériaux nécessaire pour fabriquer une hache. 
        --Non : Au revoir, si vous chagez d"idée je serais toujours là.
        --Après avoir matériaux nécessaire pour la hache : Suivez moi je vais vous montrez comment fabriquez une hache comme ça si vous devez en fabriquer une autre vou saurez comment faire.   
        --Débloque accès à la pioche (pour récupérer l'artéfacte)

        ('5','Panoramix','Druide du village','5','Vous connaissez les BlueSteaks et bien j"ai cree une formule pour les guerir et il me reste juste un element voudriez-vous aller le recuperer?'),  --Oui : Merci, l'élement manquant est du quenouille.
        --Non : Comment osez-vous ne pas vouloir guérir ces pauvres gens.
        --Quête complété : Merci, je vais préparer la potion et après vous irez guérir les BlueSteaks.
        --Quête Guérir les BlueSteaks : Voila la potion est prête. Allez libérer les BlueSteak de leur malédiction.
        --Quête réussi : Merci de votre sacrifice vous auriez pu mourir voici les matériaux nécessaire a la fabriquation d"une hache (1/2 d'un manche et la tête d'une hache) mais il manque de quoi pour fusionner les deux (bave de ziz).
       
        ('6','Chloe La Grosse Torche','Chloe (Chat) la grosse torche ressemble a une flaque lorsqu"elle est couche','5','Miaou Miaou Miaou Miaou Miaou Miaou');

INSERT INTO ennemis (id,nom,vie,force,id_lieu)
VALUES
    --Jack LeSourd = Final Boss
    --BlueSteaks = zombies
    --Tortuefuels = esprit des anciens mineurs
    --Tortufuels drop chance tête de pioche = 1/10
    ('1','Jack LeSourd','200','30','1'),
    ('2','Les BlueSteaks','100','15','3'),
    ('3','Les Tortufuels','150','10','1'),
    ('4','Les Ziz','20','5','6');

INSERT INTO quetes (id,description,id_pnj)
VALUES
    ('1','Recuperer l"artefact ancien dans la mine hantée.','1'),
    ('2','Recolter des quenouilles pour la potion de vie','5'),
    ('3','Guerir les BlueSteaks','5'),
    ('4','Trouver quelque chose pour la hache','4'),
    ('5','Fabriquation d"une hache','4'),
    ('6','Ramasser du bois dans la forêt sombre pour fabriquer un manche','4'),
    ('7','Fabriquer le manche de la pioche','4'),
    ('8','Combatre Tortufuels','4'), --Pour tête pioche
    ('9','Trouver quelque chose pour la pioche','4'),
    ('10','Fabriquer la pioche','4'),
    ('11','Combattre Jack LeSourd','3');
   


INSERT INTO pnj_quetes (id,id_pnj,id_quete)
VALUES
    ('1','1','1'),
    ('2','5','3'),
    ('3','5','3'),
    ('4','4','4'),
    ('5','4','5'),
    ('6','4','6'),
    ('7','4','7'),
    ('8','4','8'),
    ('9','4','9'),
    ('10','4','10'),
    ('11','3','11');


   







-- SELECT 
--     id,
--     nom,
--     vie,
--     force,
--     id_lieu
-- FROM ennemis;
