Cette nouvelle architecture permettra une meilleure intégration des sous-projets (Database, Business and Services) dans la solution finale, ainsi qu'une future exportation vers Linux.

Des tests unitaires y sont déjà inclus __``Audit/Global.TU.c``__

## Architecture

### Database

Fonctionnalités :  
 - créer une base de données
 - utiliser une base de données existante
 - ajouter une table
 - supprimer une table
 - détruire la base de données

Les tables de la base de données doivent pouvoir contenir des colonnes de type __entier__, __réel__, __caractère__ et __chaine de caractères__.  
Il ne doit pas y avoir de limite de table dans votre base donnée.

### Business

Il est maintenant souhaité de rajouter un outil permettant de proposer à l'utilisateur d'exécuter des requêtes __SQL__ afin d'ajouter, supprimer et modifier des données dans votre base de données YAML.

Fonctionnalités :  
 - faire une requête ``select`` sur une table
 - ajouter une ligne dans une table
 - modifier une ligne dans une table
 - supprimer une ligne dans une table

Il ne doit pas y avoir de limite en nombre de ligne dans vos tables.

### Services

Les bases de données sont bien plus facile d’utilisation pour un utilisateur final, si elles proposent une interface d’administration graphique (ex: phpmyadmin, adminer, mongodb compass, …).

Construire un ensemble de fonctions permettant d’exécuter vos requêtes __SQL__, visualiser / éditer vos tables, ajouter des informations depuis une interface graphique développée avec __GTK__.

### Common

Ce dossier contient un ensemble de fonctions permettant de faciliter le développement.

### Dépôt

Emplacement par défaut de la base de données dans le dossier ``HOMEPATH\\.yaml`` sur Windows et ``~\\.yaml`` sur Linux.

Dans les dossiers Database, Business and Services, des sous-dossiers ``manager/`` sont créés et contiennent un point d'entrée vers le sous-projet (__``main.c``__).
