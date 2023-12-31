# Les structures de Commandes

## Command
Lors du développement du processus slash, nous avons principalement vu les commandes comme des objets qui avaient leurs attributs propres et qui nous renseignaient sur son état actuel tel que 
* Son nom,
* Ses arguments, comprenant aussi le nom de la commande.
* Sa réussite pre-éxécution (utilisée si la commande échoue lors d'une redirection par exemple)
* Ses différentes redirections courantes. Valeur utilisée pour dup2 avant l'execution d'une commande.
* Deux arguments utilitaires :
    * targetRef : Qui est utilisée pour récupérer le chemin de destination lorsqu'on utilise la commande cd
    * logicalRef: Qui est utilisée pour determiner si la commande doit s'éxécuter dans un cadre physique ou logique.

Deux constructeurs sont donc disponibles pour cette structure :
- `buildCommand` et `buildCommandParsed`, leur seule différence résident dans l'argument de la fonction.
Si l'argument correspond déjà à une ligne parsée (cf. StringArray) alors on utilise `buildCommandParsed`, sinon simplement `buildCommand`.

Des fonctions annexes pour cette structure sont proposées, entre autre `redirectErr`, `redirectOutput` et `redirectInput` qui redirigent respectivement la sortie erreur, la sortie standard et l'entrée standard de la commande ou encore `alterCommandArgs` qui modifiera les arguments de la commande, et changera son nom en conséquence. En effet, si la première case du tableau représentant les arguments (qui représente le nom de la commande) a été altérée, alors le nom de la commande change.

Une fonction `freeCommand` est aussi a disposition pour pouvoir libérer tout l'espace alloué à une structure de commande, arguments compris.

## CommandResult
Une fois qu'une commande est éxécutée, nous n'avons plus aucune raison de considérer les différentes propriétés de `Command`. De fait, à sa place, nous récupérons une structure appropriée pour le résultat.
Cette structure contient les informations suivantes :
* La réussite de la commande exécutée post-éxécution.
* Si cette éxécution doit être fatale au processus slash à cause d'une grave erreur ou d'un appel à exit.
* Le code de retour de la commande exécutée.
* Si la commande a été interrompue via un signal ou non.
* Un message de résultat qui peut aussi être un message d'erreur.

Deux constructeurs sont donc disponibles pour cette structure :
- `buildCommandResult` et `buildFatalCommandResult`, leur principe réside dans le fait que la structure `commandResult` générée sera fatale au programme ou non.

Une fonction `freeCommandResult` est aussi a disposition pour pouvoir libérer tout l'espace alloué à une structure `commandResult`.

# Parsing
Pour faciliter au mieux l'éxécution des commandes, il est nécessaire de faire du parsing, en séparant la ligne écrite par l'utilisateur en plusieurs chaînes de caractères plus petites représentant chacune un seul argument.

Cependant nous avons remarqué pendant le développement que les tableaux de chaînes de caractères nous étaient très utiles en général.
De fait, nous avons mis en place une sorte de petite librairie qui nous permettent d'utiliser ces tableaux sans avoir à écrire du code redondant qui pourrait potentiellement créer des bugs ou des fuites de mémoires.

## String Array
Comme son nom l'indique cette structure représente un tableau de chaîne de caractères. Elle possède seulement deux propriétés :
* Le tableau effectif de string
* La taille de ce tableau

Un seul constructeur est disponible `createStringArray`. Celui-ci crée donc un StringArray vide, entre autre avec un tableau de string vide et une taille de 0.

Plusieurs fonctions sont proposées pour pouvoir manipuler librement cette StringArray :
* `SA_add`                    - Ajoute une chaîne de charactères en fin de tableau
* `SA_addFinalNull`           - Ajoute un pointeur NULL en fin de tableau
* `SA_remove`                 - Supprime les n derniers éléments du tableau
* `SA_print`                  - Affiche le contenu de la string Array
* `SA_splice`                 - Retire n éléments à partir d'un index donné et renvoie les éléments supprimés. Aucune fonctionnalité de remplacement n'a cependant été intégrée.
* `SA_split`                  - Sépare la string array courante considérant un séparateur spécifié. Ces séparateurs ne seront pas ajoutés aux string Array résultantes.
* `SA_indexOf`                - Cherche un élémént et renvoie son index dans la string Array sinon -1.
* `SA_indexOfArray`           - Cherche un élémént dans une liste d'éléments et renvoie son index dans la String Array sinon -1
* `SA_parseString`            - Transforme une chaîne de caractères en StringArray en se basant sur un délimiteur espace
* `SA_parseArray`             - Transforme un tableau de chaîne de caractères en un String Array
* `SA_parseStringWithQuotes`  - Transforme une chaîne de caractères en StringArray en se basant sur un délimiteur espace. Cependant, ici un bloc entouré par des guillemets est compris comme un argument à part.
* `SA_free(stringArr* array)` - Une fonction qui libère la mémoire de la structure.

# Commandes internes

## La commande exit
La commande exit procède en deux étapes :
Elle prend en charge les arguments qui lui sont donnés. Plus exactement, seulement le premier.
- Handler :
    - Si celui-ci n'existe pas, renvoie la valeur de retour de la dernière commande exécutée.
    - Si celui-ci n'est pas un nombre, renvoie -1.
    - Si celui-ci est un nombre valide, renvoie ce nombre.
- Runner :
    - Si la valeur de retour est inférieure à 0, une erreur est générée.
    - Si la valeur de retour est un chiffre valide, le programme exitera avec cette valeur.

## La commande pwd
Cette commande procède en 3 étapes :
Elle prend en charge le premier argument qui lui est passé.
- Handler :
    - Si celui-ci n'existe pas la valeur de la structure command `logicalRef` est inchangé (puisque TRUE par défaut).
    - Si celui-ci est un `-L` applique le même comportement.
    - Si celui-ci est un `-R` change la valeur de `logicalRef` à FALSE.
    - Si aucun des cas précédent n'est rencontré, l'attribue `success` de la command est mis à false.
- Runner :
    En fonction de la valeur de `logicalRef`, lance la fonction correspondante
    - Si FALSE, lance `pwdLogical`
    - Si TRUE, lance  `pwdPhysical`
- Execution :
    - `pwdLogical` : on récupère la variable d'environnement "PWD" qui est initialisée au début du programme et mise à jour par chaque appel à CD
    - `pwdPhysical`: on récupère seulement le résultat de la fonction getcwd(...) de la bibliothèque unistd.h

## La commande cd
1. Traitement des arguments de cd grâce à la structure command. 

2. Chemin physique : le chemin donné en argument est utilisé pour se déplacer dedans via chdir(). On met à jour les variables d'environnement :  OLDPWD prend la valeur de PWD, puis PWD prend la valeur du répertoire courant renvoyée par getcwd(). 

3. Chemin logique : le chemin renvoyé en argument est traité avant d'être utilisé. C'est-a-dire le chemin actuel est splité en tokens (séparés par "/" et) ajoutés à une pile, et le chemin en argument aussi dans une autre pile. Puis ce dernier est dépilé au fur et à mesure, en ajoutant ses tokens dans la pile du chemin actuel. Seulement quand on rencontre ".." on dépile le chemin actuel plutôt que d'empiler ce token, et on ignore les tokens ".". A la fin de ce traitement, la pile contient le chemin vers le nouveau répertoire, donc on change de répertoire avec chdir() et on met à jour les variables d'environnement OLDPWD et PWD en conséquence.

# Redirections

## Pipes

Les pipes sont générés presque en même temps que les commandes. En effet, nous attendons lorsque des pipes sont nécessaires, nous créeons un tableau de tubes de taille n-1 ou n est le nombre de commande detecté par nos différent parseur (`SA_parseStringWithQuotes` ou `SA_parseStringWithQuotes`) et séparateur `SA_split`.

Une fois ceci fait, nous itérons sur le tableau de commande. On redirige la sortie de la commande i sur le tube i et l'entrée de la commande i+1 sur ce même tube i.

Cependant ces redirections peuvent être "écrasés" par les redirection fichiers.

## Redirections Fichier

Ces redirections sont toujours fait après les redirection pour les tubes. Cela viens notamment d'un choix de notre part. On donne une priorité à ce qui le plus proche de la commande entre les redirections fichiers et tubes.

Ces redirections se font grace aux fonction de `StringArray` en particulier, on initialise un tableau de string représentant chaque symbole pour la redirections vers un fichier et on les cherches avec `SA_indexOfArray`. Une fois trouvé, on supprime ce symbole et l'argument qui suit avec `SA_splice` et on détermine avec un switch/case et selon le symbole de redirections quel cas appliqué (comment ouvrir le fichier et où le rediriger.) plus exactement :

- `<`   : REDIRECT_INPUT         --> Redirige l'entrée standard
- `>`   : REDIRECT_OUTPUT        --> Redirige la sortie standard dans un fichier. Création de ce fichier si il n'existe pas.
- `>|`  : REDIRECT_OUTPUT_TRUNC  --> Redirige la sortie standard dans un fichier en écrasant son contenu.
- `>>`  : REDIRECT_OUTPUT_CONCAT --> Redirige la sortie standard dans un fichier en ajoutant les données à la fin du fichier.  
- `2>`  : REDIRECT_ERROR         --> Redirige la sortie erreur dans un fichier Création de ce fichier si il n'existe pas.
- `2>>` : REDIRECT_ERROR_TRUNC   --> Redirige la sortie erreur dans un fichier en écrasant son contenu.
- `2>|` : REDIRECT_ERROR_CONCAT  --> Redirige la sortie erreur dans un fichier en ajoutant les données à la fin du fichier.

# Expansions

Pour chaque arguments d'une commande, son nom inclu nous vérifions si cette chaîne contiens une étoile.

Si une étoile est trouvé une fonction `jokerSimple` sera appelé. Celle-ci va notamment determiné si l'étoile détecter est une étoile ou une double étoile et effectuer les appels en conséquendes.

- `*` : On appel `parcourirRepertoire`. Cette fonction va s'occuper de générer un suffixe correspondant à celui rechercher par l'étoile. Celui-ci sera appliqué à tout les fichier correspondant à l'emplacement de l'étoile dans un chemin spécifié par l'utilisateur. Si ce suffixe est `./` ou `../`, nous ajoutons simplement leurs noms au chemin courant de la recherche.
Sinon, nous determinons, si le noeud recherché est un dossier avec la présence d'un `/` à la fin. Si oui, nous traiterons que les dossier, sinon un parcourir classique est effectué.
Plus exactement, si le fichier match avec le suffice, ce qu'on vérifier avec `isSuffix`, on ajoute le chemin depuis le dossier initial de la recherche à la liste des chemins trouvé (un `listNode`).
- `**` : La double étoile fonctione principalement en utilisant `parcourirRepertoire` sur chaque noeud de l'arborescence.