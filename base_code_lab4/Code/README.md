## Choix d'implémentation

### `struct s_SkipListElement` et `SkipListElement`
```c
typedef struct s_SkipListElement {
    int value;
    int level;
    struct s_SkipListElement** next_references;
    struct s_SkipListElement** back_references;
}* SkipListElement;
```

Cette structure est l'implémentation des éléments d'une SkipList : on retrouve le niveau et les tableaux (dynamiques) de références nécessaires pour la suite.

### `struct s_SkipList`
```c
struct s_SkipList {
    unsigned int size;
    RNG rng;
    SkipListElement sentinel;
};
```

Dans cette structure, j'ai fait le choix de ne pas ajouter de champ `int nblevels` représentant le niveau maximum de la SkipList.

Néanmoins, le niveau maximum de notre SkipList est stocké dans la sentinelle. Etant donné que la sentinelle est un élément fixe ne pouvant être modifié ou supprimé, son niveau de change pas et correspond bien au niveau maximum de la SkipList.

### `skiplist_element_level` _(extension)_
```c
int skiplist_element_level(SkipListElement element)
```

Cette fonction est une fonction annexe utile pour factoriser en partie le code et le rendre plus compréhensible/explicite.

### `skiplist_level` _(extension)_
```c
int skiplist_level(SkipList l)
```

Cette fonction est une fonction annexe utile pour factoriser en partie le code et le rendre plus compréhensible/explicite.

Comme dit précédemment, la sentinelle étant un élément, la fonction `skiplist_level` peut ainsi être résumée par un appel à la fonction `skiplist_element_level` sur la sentinelle de la liste.

### `skiplist_create_element` _(extension)_
```c
SkipListElement skiplist_create_element(int value, int level)
```

Cette fonction est une fonction annexe utile pour factoriser en partie le code et le rendre plus compréhensible/explicite.
Cette fonction m'a aussi permis d'éviter certaines erreurs d'initialisation lors de la création d'éléments.

### `skiplist_create`
```c
SkipList skiplist_create(int nblevels)
```

Cette fonction crée et renvoie une nouvelle skiplist de niveau maximum `nblevels`.

L'implémentation de cette fonction est assez courte grâce à l'utilisation de la fonction `skiplist_element_create`.

### `skiplist_next_element` et `skiplist_back_element` _(extension)_
```c
SkipListElement skiplist_next_element(SkipListElement element, int level_index)

...

SkipListElement skiplist_back_element(SkipListElement element, int level_index)
```

Ces deux fonctions sont des fonctions annexes utiles pour factoriser en partie le code et le rendre plus compréhensible/explicite.

Elles m'ont été particulièrement utiles pour débugger mon code avec quelques assertions.

### `skiplist_delete_element` _(extension)_
```c
void skiplist_delete_element(SkipListElement element)
...

SkipListElement skiplist_back_element(SkipListElement element, int level_index)
```

Ces deux fonctions sont des fonctions annexes utiles pour factoriser en partie le code et le rendre plus compréhensible/explicite.

Elle m'a aussi permis d'éviter certaines erreurs d'allocation mémoire en offrant une manière générale de supprimer proprement un élément.

### `skiplist_delete`
```c
void skiplist_delete(SkipList d)
```

Permet de supprimer proprement une skiplist donnée.

### `skiplist_size`
```c
unsigned int skiplist_size(SkipList l)
```

Renvoie la taille d'une skiplist donnée, ce qui correspond au champ `size` d'une `struct s_SkipList`.

### `skiplist_ith`
```c
int skiplist_ith(SkipList d, unsigned int i)
```

Renvoie le `i`-ème élément d'une skiplist.
Cette fonction effectue de façon très classique un parcours élément par élément jusqu'au `i`-èeme élément de la liste.

### `skiplist_map`
```c
void skiplist_map(SkipList d, ScanOperator f, void* user_data)
```

Cette fonction effectue le même type de parcours que `skiplist_ith` et applique une fonction en passant sur chaque élément.

### `SkipListParkourChecker` _(extension)_
```c
typedef bool (*SkipListParkourChecker)(
    int value,
    SkipListElement element,
    int level_index,
    void* userdata
);
```

Ce `functor` permet de généraliser l'application d'opérations lors du passage d'un niveau à un autre dans un parcours de skiplist.

Son rôle principal est d'indiqué si le parcours doit s'arrêter (`return true`, on peut dire que le parcours est vérifié/terminé) ou continuer (`return false`, on peut dire que le parcours n'est pas encore vérifié/terminé).

Il est utilisé dans la fonction annexe `skiplist_parkour` qui généralise l'implémentation du parcours d'une skiplist.

### `skiplist_parkour` _(extension)_
```c
SkipListElement skiplist_parkour(
    SkipList d,
    int value,
    SkipListParkourChecker checker,
    void* userdata,
    unsigned int* visit_counter
)
```

Cette fonction annexe est une généralisation du parcours d'une skiplist.

Elle permet de factoriser très efficacement les fonctions `skiplist_insert`, `skiplist_remove` et `skiplist_search` qui effectuent toutes ce même parcours typique des skiplist.

### `skiplist_update_checker` _(extension)_
```c
bool skiplist_update_checker(
    int value,
    SkipListElement element,
    int level_index,
    void* userdata
)
```

Cette fonction annexe permet de stocker les références précédant un élément à insérer/supprimer rencontrées durant un `skiplist_parkour`.

Cette fonction ne vérifiant rien quant au fait que le parcours est terminé ou non, elle renvoie constamment `false`.

### `skiplist_insert`
```c
SkipList skiplist_insert(SkipList d, int value)
```

Cette fonction permet d'insérer une nouvelle valeur dans une skiplist donnée.
Elle respecte l'implémentation donnée dans [le document de présentation des skiplists](https://github.com/theosoch/tp-algo3/blob/main/sujets/sujet-lab1.pdf) fourni sur moodle.

Comme indiqué précédemment, cette fonction effectue un `skiplist_parkour`, puis utilise l'élément retourné (correspondant à l'élément avant lequel insérer le nouvel élément) pour vérifier si l'élément à insérer n'existe pas déjà et rajoute un nouvel élément avec un niveau aléatoirement donné par la loi de probabilité de la skiplist donnée.

Cette fonction fait aussi appel à `skiplist_update_checker` pour récupérer les références précédant l'élément à supprimer afin d'actualiser leurs chaînages.

### `skiplist_remove`
```c
SkipList skiplist_remove(SkipList d, int value)
```

Cette fonction permet de supprimer une valeur existante, dans une skiplist donnée.
Elle respecte l'implémentation donnée dans [le document de présentation des skiplists](https://github.com/theosoch/tp-algo3/blob/main/sujets/sujet-lab1.pdf) fourni sur moodle.

Comme indiqué précédemment, cette fonction effectue un `skiplist_parkour`, puis utilise l'élément retourné (correspondant normalement à l'élément recherché s'il existe, sinon à son successeur direct dans la skiplist) pour vérifier si l'élément à supprimer existe et le supprimer en conséquence.

Cette fonction fait aussi appel à `skiplist_update_checker` pour récupérer les références précédant l'élément à supprimer afin d'actualiser leurs chaînages.

### `s_SkiplistSearchCheckData` et `SkiplistSearchCheckData` _(extension)_
```c
typedef struct s_SkiplistSearchCheckData {
    SkipList list;
    bool found;
} SkiplistSearchCheckData;
```

Cette structure est nécessaire à la fonction `skiplist_search_found_checker`.

### `skiplist_search_found_checker` _(extension)_
```c
bool skiplist_search_found_checker(
    int value,
    SkipListElement cursor,
    int level_index,
    void* userdata
)
```

Cette fonction annexe vérifie si le `skiplist_parkour` a rencontré l'élément recherché par `skiplist_search` et interrompt le parcours en conséquence.

### `skiplist_search`
```c
bool skiplist_search(SkipList d, int value, unsigned int *nb_operations)
```

Cette fonction indique si une valeur existe dans une skiplist donnée.
Elle respecte l'implémentation donnée dans [le document de présentation des skiplists](https://github.com/theosoch/tp-algo3/blob/main/sujets/sujet-lab1.pdf) fourni sur moodle.

Comme indiqué précédemment, cette fonction effectue un `skiplist_parkour` mais cette fois sans ce soucier de la valeur de retour de cette fonction.

En effet, la fonction annexe `skiplist_search_found_checker` nous indique déjà si l'élément cherché a été trouvé durant le parcours en modifiant une structure de données utilisateur passée en paramètre du `skiplist_parcours`.

### `skiplist_iterator_forward` _(extension)_
```c
SkipListElement skiplist_iterator_forward(SkipListElement element)
```

Cette fonction annexe renvoie l'élément suivant un élément de skiplist donné. Cela permet à un itérateur directionnel d'avancer du début vers la fin d'une skiplist.

### `skiplist_iterator_backward` _(extension)_
```c
SkipListElement skiplist_iterator_backward(SkipListElement element)
```

Cette fonction annexe renvoie l'élément précédant un élément de skiplist donné. Cela permet à un itérateur directionnel d'avancer de la fin vers le début d'une skiplist.

### `s_SkipListIterator` _(extension)_
```c
struct s_SkipListIterator {
    SkipList list;
    SkipListElement cursor;
    unsigned char direction;
    SkipListElement (*next)(SkipListElement element);
};
```

Dans cette structure, j'ai choisi de rajouter un champ permettant de stocker la fonction de déplacement `next`, évitant ainsi d'effectuer des vérifications de la direction à chaque appel aux fonctions `skiplist_iterator_begin` et `skiplist_iterator_next`.

### `skiplist_iterator_create`
```c
SkipListIterator skiplist_iterator_create(SkipList d, unsigned char w)
```

Cette fonction crée et renvoie un itérateur directionnel sur une skiplist donnée.

### `skiplist_iterator_delete`
```c
void skiplist_iterator_delete(SkipListIterator it)
```

Cette fonction permet de supprimer proprement un itérateur directionnel.

### `skiplist_iterator_begin`
```c
SkipListIterator skiplist_iterator_begin(SkipListIterator it)
```

Initialise un itérateur au début de son parcours de liste (ex: déplacement de la fin vers le début -> place l'itérateur sur le dernier élément).

### `skiplist_iterator_end`
```c
bool skiplist_iterator_end(SkipListIterator it)
```

Indique si le parcours d'un itérateur donné est arrivé à sa fin ou non.

### `skiplist_iterator_next`
```c
SkipListIterator skiplist_iterator_next(SkipListIterator it)
```

Permet d'avancer dans le parcours d'un itérateur donné en passant à l'élément suivant.

### `skiplist_iterator_value`
```c
int skiplist_iterator_value(SkipListIterator it)
```

Retourne la valeur de l'élément courant du parcours d'un itérateur donné.

## Comportement sur les jeux de test

Le code passe tous les jeux de tests. Il n'y a pas grand-chose à dire à ce sujet pour ce TP, étant donné l'implémentation très stricte demandée pour respecter les jeux de test.

## Analyse personnelle

Dans ce TP, les jeux de tests étaient la chose la plus difficile à respecter à la lettre, notamment pour le `skiplist_search`.

J'ai trouvé que l'ordre de génération des nombres aléatoires était un détail assez piégeur, nous pouvions très facilement obtenir des résultats décalés sans comprendre pourquoi : les niveaux des éléments insérés doivent être générés après avoir vérifié que leur valeur n'existe pas déjà dans la liste, sinon, un décalage dans la suite de nombres aléatoires pouvait apparaître.

Pour des raisons d'optimisation en temps de calcul / nombre d'opérations ou plus précisément de nombre de visites de noeuds durant un parcours de skiplist, notre parcours de skiplist lors de la recherche d'un élément existant devait s'interrompre dès l'élément trouvé. Un détail néanmoins important et faisant la différence sur les jeux de test.

J'ai préféré introduire quelques fonctions et structures de données annexes toutes utiles pour l'implémentation (uniquement).

Ainsi, j'ai factorisé le code du parcours optimal d'une skiplist en une fonction générique nommée `skiplist_parkour`, utilisée dans `skiplist_insert`, `skiplist_search` et `skiplist_remove`.

Cela m'a permis de simplifier en très grande partie ces trois fonctions, et de mieux expliciter leur fonctionnement dans le code en lui-même.

### 3.3.2 Analyse de la différence en temps de calcul entre un itérateur et un parcours de skiplist

Grâce aux tests `test_search` et `test_search_iterator`, on constate une très grande différence d'efficacité entre le parcours d'une skiplist (un `skiplist_parkour`) et un itérateur classique, notamment lorsqu'on compare les résultats de `./skiplisttest -s 4` et `./skiplisttest -i 4`:

| Test | Taille de la liste | Elements trouvés | Elements non-trouvés | Nombre minimal de noeuds visités | Nombre maximal de noeuds visités | Nombre moyen de noeuds visités |
| :--: | :----------------: | :--------------: | :------------------: | :------------------------------: | :------------------------------: | :----------------------------: |
| `./skiplisttest -s 4` | `10922` | `8209` | `28826` | `1` | `30` | `12` |
| `./skiplisttest -i 4` | `10922` | `8209` | `28826` | `2` | `10922` | `9707` |

On constate que pour un parcours de skiplist, la complexité est de l'ordre de `O(log2(n))` contre `O(n)` pour les itérateurs.

L'efficacité d'un parcours de skiplist semble comparable à la recherche d'un noeud dans un arbre binaire de recherche.