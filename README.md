# Projet ARM Simulator

## Cloner le projet
`git clone https://github.com/MatteoDoyen/ProjetArm.git`

### Décompression des sources supplémentaires
`unzip supp_sources.zip`

### Configuration du projet
`./configure CFLAGS='-Wall -Werror -g'`

_Si permission refusée : `chmod 755 configure`_

`make`

## Exécution des tests
Listes des tests automatisés :
* memory_test
* load_store_test
* registers_test
* tests.sh (tous les tests ci-dessus)

Dans un terminal :  
`./<nom du test>`
  
Test non-automatisé :
* test_data_processing.s
