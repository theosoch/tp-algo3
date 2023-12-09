## Choix d'implémentation

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

## Comportement sur les jeux de test

Tous les exercices fonctionnent comme attendu dans le sujet.

## Analyse personnelle

Les fonctions `bstree_swap_nodes` et `bstree_remove_node` ont été les fonctions les plus compliquées à implémenter : la mise à jour des liens entre les noeuds était assez délicate et m'a demandé beaucoup de temps et de patience avant de comprendre comment proprement permuter et plus simplement modifier les liens d'un noeud sans s'y perdre.

### Exercice 5

En modifiant la permutation d'un noeud à deux fils avec son successeur contre celle avec son prédécesseur, on constate que le parcours infixe ne change pas, comme on aurait pu s'y attendre, mais que l'arbre qui en résulte après suppression n'est pas le même : cela s'explique par la position du successeur et du prédécesseur dans l'arbre, qui ne se trouve pas au même endroit.