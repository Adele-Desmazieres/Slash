# Les structures de Commandes

## Command
Lors du développement du processus slash, nous avons principalement vu les commandes commes des objets qui avaient leurs attributs propre et qui nous renseignait sur son état actuel tel que 
* Son nom,
* Ses arguments, comprenant aussi le nom de la commande.
* Sa réussite pre-éxécution (utilisée si la commande échoue lors d'une redirection par exemple)
* Ses différentes redirections courantes. Valeur utilisé pour dup2 avant l'execution d'une commande.
* Deux arguments utilitaire :
    * targetRef : Qui est utilisé pour récupéré le chemin de destination lorsqu'on utilise la commande cd
    * logicalRef: Qui est utilisé pour determiné si la commande doit s'éxécuter dans un cadre physique ou logique.

Deux constructeur sont donc disponible pour cette structure :
- `buildCommand` et `buildCommandParsed`, leur seul différence résident dans l'argument de la fonction.
Si l'argument correspond déjà à une ligne parsée (cf. StringArray) alors utiliser `buildCommandParsed` sinon simplement `buildCommand`.

Des fonctions annexe pour cette structure sont proposé, entre autre `redirectErr`, `redirectOutput` et `redirectInput` qui redirige respectivements les sorties erreurs, standard et entrée standard le la commande ou encore `alterCommandArgs` qui modifiera les arguments de la commande, et changera son nom en conséquence. En effet, si la première case du tableau représentant les arguments, qui re présente le nom de la commande a été altéré, alors le nom de la commande change.

Une fonction `freeCommand` est aussi a disposition pour pouvoir libéré tout l'espace alloué à une structure de commande, arguments compris.

## CommandResult
Une fois qu'une commande est éxécutée, nous n'avons plus aucune raison de considéré les différentes propriétés de `Command`. De fait, à sa places, nous récupérons une strucutre approprié au résultat.
Cette structure contient les informations suivantes :
* La réussite de la commande exécutée post-éxécution.
* Si cette éxécution doit être fatal au processus slash à cause d'une grave erreur ou d'un appel à exit.
* Le code de retour de la commande exécutée.
* Si la commande a été interrompu via un signal ou non.
* Un message de résultat. Peut être aussi un message d'erreur.

Deux constructeur sont donc disponible pour cette structure :
- `buildCommandResult` et `buildFatalCommandResult`, leur seul réside dans le fait que la `commandResult` généré sera fatale au programme ou non.

Une fonction `freeCommandResult` est aussi a disposition pour pouvoir libéré tout l'espace alloué à une structure `commandResult`.

# Parsing
Pour faciliter au mieux l'éxéction des commandes, il est nécessaire de faire du parsing, en séparant la ligne écrite par l'utilisateur en plusieurs chaînes de caractères plus petite représentant chacune un charactère.

Cependant nous avons remarqué pendant le développement que les tableau de chaînes de caractères nous était trés utile en général.
De fait, nous avons mis en place une sorte de mini librairie qui nous permettrait d'utiliser ces tableaux sans avoir à écrire du code redondant qui pourrait potentiellement créer des bugs ou des fuites de mémoires.

## String Array
Comme son nom l'indique cette structure représente un tableau de chaîne de caractère. Elle possède seulement deux propriétés :
* Le tableau effectif de String
* La taille de ce tableau

Un seul constructeur est disponible `createStringArray`. Celui-ci crée donc un StringArray vide, entre autre avec un tableau de string vide et une taille de 0.

Plusieurs fonctions sont proposées pour pouvoir manipuler librement cette StringArray :
* `SA_add`                    - Ajoute une chaînes de charactères en fin de tableau
* `SA_addFinalNull`           - Ajoute un pointeur NULL en fin de tableau
* `SA_remove`                 - Supprime les n derniers éléments du tableau
* `SA_print`                  - Affiche le contenu de la String Array
* `SA_splice`                 - Retire N élément à partir d'un index donnée et retourne ces élément supprimé. Aucune fonctionnalité de remplacement n'a cependant était intégrée.
* `SA_split`                  - Sépare la string array courante considérant un séparateur spécifié. Ces séparateurs ne serons pas ajouté aux String Array résultantes.
* `SA_indexOf`                - Cherche un élémént et retourne son index dans la String Array sinon -1
* `SA_indexOfArray`           - Cherche un élémént parmis une liste d'éléments et retourne son index dans la String Array sinon -1
* `SA_parseString`            - Transforme une chaîne de caractères en StringArray en se basant sur un délimiteur espace
* `SA_parseArray`             - Transforme un tableau de chaîne de caractère en un String Array
* `SA_parseStringWithQuotes`  - Transforme une chaîne de caractères en StringArray en se basant sur un délimiteur espace. Cependant, ici un bloc entouré par des guillemet est compris comme un argument à part.
* `SA_free(stringArr* array)` - Une fonction qui libère la mémoire de la structure.

# Commandes internes

## La commande exit
La commande exit procède en deux étapes :
Elle prend en charge les arguments qui lui sont donnés. Plus exactement, seulement le premier.
- Handler :
    - Si celui-ci n'existe pas, renvoie la valeur de retour de la dernière commande exécutée.
    - Si celui-ci n'est pas un nombre, renvoie -1
    - Si celui-ci est un nombre valide, renvoie ce nombre
- Runner :
    - Si la valeur de retour est inférieur à 0, une erreur est générée.
    - Si la valeur de retour est un chiffre valide, le programme exitera avec cette valeur.

## La commande pwd
Cette commande procède en 3 étapes :
Elle prend en charge le premier argument qui lui est passé.
- Handler :
    - Si celui-ci n'existe pas la valeur de la structure commmand `logicalRef` est inchangé (puisque TRUE par défaut).
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