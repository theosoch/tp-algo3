## Choix d'implémentation

### `isSymbol`
```c
bool isSymbol(const char c)
```

Pour gagner en performance, j'ai fait le choix d'utiliser un switch pour énumérer le peu de cas possibles.

### `isDigit` _(extension)_
```c
bool isDigit(const char c)
```

En C, les caractères sont les numéros correspondants dans la table ASCII.
De plus, dans cette table, les chiffres de `0` à `9` sont tous numérotés à la suite.

Ainsi, `0` a le numéro `n` dans la table ASCII, et `1` a le numéro `n+1`, `2` -> `n+2`, ..., `9` -> `n+9`;

Donc si le numéro d'un caractère est compris entre `n` (`n <= c`) et `n+9` (`c <= n+9`), alors c'est un chiffre.

C'est cette idée qui est utilisée dans la fonction `isDigit`.

### `printToken`
```c
void printToken(FILE* f, void* e)
```

Dans cette fonction, on doit vérifier le type de token de `e` pour récupérer en conséquence la valeur correspondant au token:
- si c'est un nombre -> on récupère sa valeur numérique via `tokenGetValue`
- si c'est un opérateur -> on récupère le symbole de cet opérateur via `tokenGetOperatorSymbol`
- si c'est une parenthèse -> on récupère le symbole de cette parenthèse via `tokenGetParenthesisSymbol`

Puis on affiche la valeur avec un `printf` adapté.

Etant donné que dans le sujet les valeurs affichées sont toutes espacées de 1, on rajoute un `printf(" ")` à la fin de notre fonction.

### `stringToTokenQueue`
```c
Queue* stringToTokenQueue(const char* expression)
```

Dans cette fonction, on parcourt notre expression sous forme de chaine de caractère en respectant l'algorithme donné dans le sujet.

### `shuntingYard`
```c
Queue* shuntingYard(Queue* infix)
```

Ici, je me suis contenté d'implémenter l'algorithme proposé depuis Wikipédia.

Toutefois, pour préciser les choses : dans cet algorithme, on a besoin d'une pile pour stocker temporairement les opérateurs rencontrés lors du parcours de la file infixe.

### `evaluateOperator`
```c
Token* evaluateOperator(Token* arg1, Token* op, Token* arg2)
```

Pour cette fonction, on a besoin de vérifier quelques préconditions :
nos arguments doivent être du bon type, étant donné que `Token` est un type générique pour les nombres, les opérateurs et les parenthèses.

Pour le reste, on peut se contenter d'un switch, énumérant les opérations possibles et retournant en conséquence un token contenant le résultat.

### `evaluateExpression`
```c
float evaluateExpression(Queue* postfix)
```

Pour pouvoir évaluer notre expression postfixe, on doit stocker les valeurs à utiliser lors de l'application de nos opérateurs lorsqu'on les rencontre.

Ainsi, on a besoin d'une **pile de valeurs** à laquelle on rajoute chaque token représentant un nombre à chaque fois que l'on en rencontre un dans la file postfixe.

Lorsqu'on rencontre un opérateur, on dépile donc les deux valeurs au sommet de notre pile de valeurs et on applique notre opérateur dessus.
La première valeur dépilée correspond au second argument et la seconde valeur dépilée au premier argument de notre opérateur.

Ici, on a des cas particuliers de libération de la mémoire : étant donné que les arguments passés lors de l'évaluation d'un opérateur ne sont plus utilisés par la suite dans l'évaluation, on doit les libérer.

Pour continuer l'algorithme de manière itérative, le résultat de notre opération est empilé dans la pile de valeurs pour la prochaine opération à effectuer dessus.

Enfin, on vérifie que l'évaluation s'est bien déroulé en mode DEBUG avec quelques assertions vérifiant que notre pile de valeur contient bien une unique valeur qui est le token contenant le résultat de l'évaluation.

Et enfin, on retourne ce résultat.

### `computeExpressions`
```c
void computeExpressions(FILE* input)
```

Dans cette fonction, j'ai choisi d'allouer un espace mémoire maximal dans lequel stocker la ligne courante lue en parcourant l'`input`.
Cela permet de limiter le nombre de `malloc` et `free` utilisés pour l'ensemble des lignes à `1`.

Dans la boucle de lecture de l'`input`, on calcule la taille de la ligne (=nombre de caractères) sans compter le retour à la ligne (`\n`) qui est d'ailleurs retiré de la ligne.
Ceci permet de détecter les lignes vides et de les ignorer.

En-dehors de ces quelques points, la fonction correspond aux attentes du sujet :
- affichage de l'expression lue (=la ligne lue)
- conversion de l'expression sous forme de file de tokens infixe
- affichage de cette file
- conversion de la file de tokens infixe en file de tokens postfixe
- affichage de cette file
- évaluation et affichage du résultat à partir de la file de tokens postfixe
- gestion de la mémoire : les files sont vidées pour libérer les valeurs contenues et sont elles-mêmes libérées par la suite.

## Comportement sur les jeux de test

Le programme passe tous les tests en prenant en compte tous les cas traités dans le sujet sans erreur (testé avec Valgrind et GDB).

## Analyse personnelle

Dans ce TP, la difficulté était surtout au niveau de la gestion de la mémoire, le reste consistant simplement à appliquer les consignes du sujet.

Les fonctions les plus intéressantes, que ce soit en termes de gestion de la mémoire ou d'élaboration sont à mon sens les fonctions `shuntingYard`, `computeExpressions` et `evaluateExpression`.

Pour aller plus loin, il aurait pu être intéressant d'implémenter l'opérateur unaire `-`, qui aurait pu être implémenté soit en tant que token à part entière, soit en tant que symbole marquant le début d'un nombre négatif.

Pour le debugging, les outils Valgrind et GDB ont été d'une grande aide et permettent notamment de rapidement cibler les problèmes de gestion de la mémoire.