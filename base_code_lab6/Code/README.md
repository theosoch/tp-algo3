## Choix d'implémentation

### TP 5 - Arbres binaires de recherche

#### Remarque(s) générale(s) :

Plusieurs énumérations de cas sont effectuées dans les fonctions nécéssaires à la suppression d'un noeud, certaines pourraient être simplifiée mais j'ai choisi de les garder telles quelles pour la facilité de compréhension qu'elles représentent.

### `bstree_search_node` _(extension)_
```c
BinarySearchTree* bstree_search_node(const BinarySearchTree *t, int v)
```

Cette fonction permet de factoriser les parcours de recherche d'un noeud par sa valeur présents dans les fonctions `bstree_search` et `bstree_remove`.

Dans `bstree_search`, si la valeur retournée par `bstree_search_node` est nulle, alors le noeud n'a pas été trouvé et donc la fonction renvoie `false`, sinon, elle renvoie `true`.

Dans `bstree_remove`, la valeur retournée par `bstree_search_node` correspond au noeud à supprimer (s'il n'est pas vide (`NULL`)).

### `bstree_nearest` _(extension)_
```c
typedef enum {
    left, right
} NearestDirection;

BinarySearchTree* bstree_nearest(const BinarySearchTree* x, NearestDirection direction)
```

Cette fonction est une généralisation des fonctions `bstree_successor` et `bstree_predecessor` en ajoutant une notion de direction grâce à l'énumération `NearestDirection` composée 2 membres : `left` (pour le prédécesseur) et `right` (pour le successeur).

Elle n'est utile que pour l'implémentation et permet de factoriser les deux fonctions qu'elle généralise.

### `goto_extremum` _(extension)_
```c
typedef enum {
    minimum, maximum
} Extremum;

const BinarySearchTree *goto_extremum(const BinarySearchTree *e, Extremum type)
```

Cette fonction est une généralisation des fonctions `goto_min` et `goto_max` en ajoutant une notion de "type" d'extremum grâce à l'énumération `Extremum` composée de deux membres : `minimum` et `maximum`.

Elle n'est utile que pour l'implémentation et permet de factoriser les deux fonctions qu'elle généralise.

### `bstree_delete_free_node` _(extension)_
```c
void bstree_delete_free_node(const BinarySearchTree* node, void* userData)
```

Cette fonction est un functor utilisé dans `bstree_delete` pour libérer les noeuds de l'arbre à supprimer.

### `bstree_delete`
```c
void bstree_delete(ptrBinarySearchTree *t)
```

Cette fonction possédait déjà une implémentation par défaut dans le code fourni, mais pour des raisons d'optimisation : l'implémentation par défaut faisait appelle à la fonction `bstree_remove_node` qui effectue des opérations inutiles à la suppression totale d'un arbre, ce qui faisait perdre du temps. Pour accélérer le processus, on peut simplement effectuer le parcours en profondeur postfixe et supprimer chaque noeud rencontré avec le functor `bstree_delete_free_node`.

### TP 6 - Arbres rouges-noirs

#### Exercice 1

### `enum NodeColor`
```c
typedef enum {red, black} NodeColor;
```

Cet enum doit être placé dans l'implémentation (`.c`) uniquement, l'utilisateur n'a pas à se soucier de la couleur des noeuds.

### `printNode`
```c
void printNode(const BinarySearchTree *n, void *out)
```

Cette fonction n'est utile qu'à l'implémentation de la fonction `rbtree_export_dot`, elle doit donc être écrite dans le `.c`.

#### Exercice 2

### `leftrotate` et `rightrotate`
```c
void leftrotate(BinarySearchTree *x)

...

void rightrotate(BinarySearchTree *y)
```

Ces deux fonctions ne sont utiles qu'à l'implémentation du rééquilibrage des arbres rouges-noirs. En définitive, l'utilisateur n'a pas à se soucier de la couleur des noeuds ni du bon équilibrage de l'arbre, l'implémentation seulement est censée le garantire. Ces deux fonctions doivent donc être écrite dans le `.c`.

#### _(global)_

### `brother_by_parent` _(extension)_
```c
BinarySearchTree* brother_by_parent(BinarySearchTree* parent, BinarySearchTree* n)
```

Cette fonction permet de trouver le frère d'un noeud étant donné son parent supposé. Cela permet notamment de trouver le frère d'un noeud vide (cas assez particulier rencontré lors des rééquilibrages).

Cette fonction est très utile pour factoriser les fonctions `fixredblack_remove_case1` et `fixredblack_remove_case2`.

### `brother` _(extension)_
```c
BinarySearchTree* brother(BinarySearchTree* n)
```

Cette fonction permet de trouver le frère d'un noeud, si ce noeud n'est pas vide, et qu'il admet un parent. Cette fonction permet simplement de décomposer la fonction `uncle`.

### `bstree_swap_nodes`
```c
void bstree_swap_nodes(ptrBinarySearchTree *tree, ptrBinarySearchTree from, ptrBinarySearchTree to)
```

Par rapport au TP 5 sur les arbres binaires de recherche, on doit maintenant prendre en compte que l'échange/swap de de deux noeuds implique aussi d'échanger leur couleur.

Sans cette modification, durant les tests, lorsqu'on supprime un noeud, obtient des arbres qui ressemble à ce qu'on aurait pu obtenir en échangeant le noeud à supprimer avec son prédécésseur plutôt que son successeur (hypothèse à confirmer).

### `bstree_remove_node`
```c
void bstree_remove_node(ptrBinarySearchTree *t, ptrBinarySearchTree current)
```

Par rapport au TP 5 sur les arbres binaires de recherche, on doit maintenant corriger les éventuels problèmes de coloration, notamment si le noeud prenant la place de notre noeud supprimé devient "double-noir".

Pour prendre en compte ce nouveau comportement, on doit connaître le noeud qui prend la place du noeud à supprimer, aussi appelé le substituant (substitute) dans notre cas.

Après réflexion, on se rend compte que ce noeud correspond tout simplement à un noeud que nous utilisions déjà dans l'implémentation de `bstree_remove_node` au TP 5 : ce noeud correspond tout simplement au fils unique de notre noeud à supprimer (cf. tp5 - suppression, cas 1).

### `leftrotate` et `rightrotate`
```c
void leftrotate(BinarySearchTree *x)

...

void rightrotate(BinarySearchTree *y)
```

Ces deux fonctions ne sont utiles qu'à l'implémentation du rééquilibrage des arbres rouges-noirs. En définitive, l'utilisateur n'a pas à se soucier de la couleur des noeuds ni du bon équilibrage de l'arbre, l'implémentation seulement est censée le garantire. Ces deux fonctions doivent donc être écrite dans le `.c`.

### `fixredblack_insert_case2`, `fixredblack_remove_case1` et `fixredblack_remove_case2`
```c
ptrBinarySearchTree fixredblack_insert_case2(ptrBinarySearchTree x)

...

ptrBinarySearchTree fixredblack_remove_case1(ptrBinarySearchTree p, ptrBinarySearchTree x)

...

ptrBinarySearchTree fixredblack_remove_case2(ptrBinarySearchTree p, ptrBinarySearchTree x)
```

Ces fonctions sont initialement décomposées en 2 cas symétriques (gauche et droite) dans l'implémentation vue en cours.
J'ai fait le choix pour chacune de ces fonctions de factoriser les deux cas en une fonction car leur comportement était très similaire et les modifications nécessaires peu complexe (à mon avis, excepté possiblement pour ``).

## Comportement sur les jeux de test

### TP 5 & 6

Tous les exercices fonctionnent comme attendu dans le sujet.

## Analyse personnelle

### TP 5 - Arbres binaires de recherche

Les fonctions `bstree_swap_nodes` et `bstree_remove_node` ont été les fonctions les plus compliquées à implémenter : la mise à jour des liens entre les noeuds était assez délicate et m'a demandé beaucoup de temps et de patience avant de comprendre comment proprement permuter et plus simplement modifier les liens d'un noeud sans s'y perdre.

#### Exercice 5

En modifiant la permutation d'un noeud à deux fils avec son successeur contre celle avec son prédécesseur, on constate que le parcours infixe ne change pas, comme on aurait pu s'y attendre, mais que l'arbre qui en résulte après suppression n'est pas le même : cela s'explique par la position du successeur et du prédécesseur dans l'arbre, qui ne se trouve pas au même endroit.

### TP 6 - Arbres rouges-noirs

Durant ce TP, les fonctions les plus complexes à implémenter était les fonctions de rotations, pour les mêmes raisons qu'au TP précédent lors de l'implémentation de `bstree_swap_nodes`.

Beaucoup de cas particuliers des rééquilibrages pouvaient être factorisés, ce qui permet au final d'avoir un code plus compréhensible et général (toujours un bon point).

Durant l'implémentation du rééquilibrage de la suppresion d'un noeud, j'ai recontré une curiosité lié à l'oubli de la contrainte des couleurs lors de l'échange/swap entre deux noeuds (cf. Choix d'implémentation - TP 6 - `bstree_swap_nodes`).

La partie visualisation grâce à la génération de PDF était assez satisfaisante et permettait de mieux comprendre le comportement de notre code sur les arbres (ce qui a facilité le debbugage).