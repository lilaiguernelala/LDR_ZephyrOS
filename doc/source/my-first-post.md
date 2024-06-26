+++
title = 'Rapport de stage - IGUERNELALA Lila'
draft = false
date = 2024-06-04
+++

## Introduction
Dans le cadre des travaux de recherche menés au LDR sur les plateformes embarquées, les chercheurs souhaitent mettre en place des environnements de développements et de tests plus récents et robustes pour 2 applications en particulier, dans l’ordre d’importance :  

- La programmation embarquée, en particulier avec OS Temps Réel. 
Mettre en place un environnement de dev/tests avec sondes logicielles pour le temps réel. Pour réaliser des tests de systèmes contraints, possiblement utilisant des architectures matérielles diverses (ARM, RISC-V, etc.), les chercheurs ont besoin d’un environnement clair, générant des traces analysables (quantité de mémoire utilisée, temps passé sur le processeur et lors des accès aux périphériques, décisions d’ordonnancement, etc.).

- Simulateur pour robotique/drone. 
Mettre en place un environnement de dev/tests avec, par exemple CoppeliaSim et Python/ROS, génération de scénario et récupération des données de vol et de l’environnement simulé.  
Différents travaux sont réalisés depuis plusieurs années avec l’ONERA sur des simulations utilisant de drones. Ces travaux utilisent d’anciens simulateurs difficiles à maintenir aujourd’hui, ce qui rend complexe la reproduction de certains résultats. Plusieurs exemples existent pour des applications spécifiques, comme avec le robot Poppy et ROS.


## Embarqué

La première étape de cette partie consiste à mettre en place un code, à partir du code NoGUI vu en TP du module « Programmation temps réel » sur la carte ST STM32F429I Discovery avec ZephyrOS en se basant sur l'IDE PlatformIO qui : 

- Récupère des traces simples de chaque thread.
- Exporte les données et les affiches sur SEGGER SystemView.

**Qu’est-ce que Zephyr OS ?**

Zephyr OS est un système d'exploitation temps réel open source. Il est reconnu pour sa légèreté, sa modularité et le fait qu’il supporte plusieurs plateformes, ce qui le rend adapté à une large gamme de microcontrôleurs. Avec une capacité à supporter plusieurs protocoles de communication.

**Qu’est-ce que PlatformIO ?**

PlatformIO est un IDE, son environnement de développement supporte le langage C/C++ pour les systèmes embarqués. Compatible avec plusieurs systèmes d'exploitation (Windows, Mac et Linux), il s'intègre également en tant qu'extension aux éditeurs de texte existants comme VSCode.

**Définition du Tracing**

L’utilisation du tracing dans un système d’exploitation temps réel comme **ZephyrOS** est bénéfique car il permet d’analyser le comportement du système en temps réel en enregistrant les évènements tel que les changements de threads. Cela aide à identifier les bugs ainsi que les problèmes de performances. 
On peut également détecter les threads qui consomment trop de temps cela nous permet d’optimiser le système. 

Il nous permet de comprendre le comportement de threads entre eux, à quelle fréquence les ressources système sont utilisées, cela nous permet d’avoir un aperçu sur le fonctionnement interne du système
Le tracing facilite l’analyse comparative et la reproductibilité des tests, on peut aussi vérifier si toutes les parties du code sont bien testées en conditions réelles

**Les étapes à suivre pour récupérer des traces d’un code** 

Modifier le fichier .conf de notre projet NoGUI en ajoutant les lignes 

**code**

```c++
CONFIG_TRACING=y
CONFIG_SEGGER_SYSTEMVIEW=y
CONFIG_SEGGER_SYSTEMVIEW_BOOT_ENABLE=n
CONFIG_SEGGER_SYSVIEW_POST_MORTEM_MODE=n
CONFIG_SEGGER_RTT_BUFFER_SIZE_UP=4096
```

Zephyr utilise RTT (Real-Time Transfer) comme moyen de transport principal pour SystemView, dans ce cas l'exportation des traces nécessite toujours l'utilisation d'un appareil J-Link car ZephyrOS ne supporte pas actuellement l'exportation des traces via d'autres moyens comme l'UART ou le réseau, qui pourraient permettre une transmission directe sans hardware spécifique.

D’où l’intérêt d’utiliser un **J-Link Segger EDU** pour pouvoir exporter les traces de chaque thread et les afficher sur SEGGER SystemView.
Ce qu’il faut installer pour la réalisation de cette partie :

-	J-Link software 
-	SEGGER SystemView 

# ⚠️ warning

> [!WARNING]
> Sélectionnez le protocole SWD pendant la connexion du J-Link, tout en laissant le reste des paramètres par défaut


Après avoir exécuter le code sur VSCode, et connecter le J-Link, les traces seront affichées en commençant l’enregistrement sur l’interface graphique de SEGGER SystemView.

**A quoi servent les traces ?**

Dans notre code on a trois threads (2 LEDs et un switch), le traçage permet de :
-	Surveiller quand et combien de fois chaque thread est exécuté, c’est important pour analyser le comportement des threads. 
-	Savoir par exemple la durée que prend chaque tâche pour exécuter une boucle, ou comment la pression du switch affecte le comportement des LEDs.
-	Si on voit que la LED 1 par exemple monopolise les ressources plus que prévu, on peut jouer sur le nombre d’itérations ou sur la priorité des threads.
-	Si un problème est détecté, on peut consulter les traces pour voir exactement ce qui se passait juste avant l’erreur

**Les moyens de suivre des traces d'un code embarqué sans ZephyrOS**

-	Freertos SystemView peut être utilisé pour enregistrer des applications qui utilisent FreeRTOS V8, V9, V10 ou V11. Jusqu'à la V10, la source FreeRTOS doit être légèrement modifiée pour afficher correctement l'exécution de l'ordonnanceur FreeRTOS. A partir de la V11, aucune modification du noyau n'est nécessaire.
RT-Thread Contraintes de TraceX
-	TraceX présente les contraintes suivantes :
-	TraceX ne peut pas mesurer avec précision les intervalles entre les événements supérieurs à la période de la minuterie.

https://www.rt-thread.org/download/rttdoc_1_0_0/group___hook.html 

## Simulation
**La restitution des avantages/inconvénients**

La simulation c’est la création d’un modèle virtuel d’une machine, tout en prenant en considération l’environnement de travail et tous les autres facteurs influents. Ça nous permet de savoir si la conception peut fonctionner après la fabrication. Ça nous permet de tester différentes machines/algorithmes dans un même environnement (plusieurs drones dans un parc urbain et voir comment chaque drone navigue entre les arbres et réagit aux obstacles), et de tester une machine dans différents environnements (simuler un seul modèle de drone au-dessus d'une forêt avec des vents variables, dans un désert avec des températures élevées ou à l’intérieur de la maison).

Comparaison entre CoppeliaSim et GazeboSim qui sont tous deux des simulateurs de robotique mais avec des caractéristiques distinctes, L’objectif de cette comparaison est de déterminer le simulateur le plus adapté à nos besoins.

 TABLEAU

**Les besoins** 
-	Un simulateur qui a l’interface graphique de coppeliasim mais qu’on pourra utiliser comme Gazebo, c’est-à-dire on pourra créer notre scène sur un fichier et avoir un seul dossier qui contient notre projet, on pourra lancer le projet dans un terminal sans ouvrir le simulateur avant.
-	Un simulateur facile à installer sur windows de preférence (car sur linux certaines exécutions bug). 
-	Un simulateur avec un facteur temps réel faible.

# ⚠️ warning
gitlens
remplacer while ready par un semaphore risque

> **Note:** Assurez-vous de lire attentivement cette section et de suivre les conseils fournis pour éviter tout problème potentiel.

|   Emoji   |   Nom |   Hex |   Dec |   Prix    |
|---    |:-:    |:-:    |:-:    |--:    |
|   &#x1F600;   |   GRINNING FACE   |   ```&#x1F600;``` |   ```&#128512;``` |   0.05 €  |
|   &#x1F602;   |   FACE WITH TEARS OF JOY  |   ```&#x1F602;``` |   ```&#128514;``` |   0.12 €  |
|   &#x1F923;   |   ROLLING ON THE FLOOR LAUGHING   |   ```&#x1F923;``` |   ```&#129315;``` |   0.09 €  |

> #### The quarterly results look great!
>
> - Revenue was off the chart.
> - Profits were higher than ever.
>
>  *Everything* is going according to **plan**.

I love supporting the **[EFF](https://eff.org)**.
This is the *[Markdown Guide](https://www.markdownguide.org)*.
See the section on [`code`](#code).

![The San Juan Mountains are beautiful!](/assets/images/image1.png "San Juan Mountains")
*A single track trail outside of Albuquerque, New Mexico.*

> :warning: **Warning:** Do not push the big red button.


> :memo: **Note:** Sunrises are beautiful.


> :bulb: **Tip:** Remember to appreciate the little things in life.

[!WARNING] coucou 
