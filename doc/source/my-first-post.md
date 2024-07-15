# Introduction
Dans le cadre des travaux de recherche menés au LDR sur les plateformes embarquées, les chercheurs souhaitent mettre en place des environnements de développements et de tests plus récents et robustes pour 2 applications en particulier, dans l’ordre d’importance :  

- La programmation embarquée, en particulier avec OS Temps Réel. 
Mettre en place un environnement de dev/tests avec sondes logicielles pour le temps réel. Pour réaliser des tests de systèmes contraints, possiblement utilisant des architectures matérielles diverses (ARM, RISC-V, etc.), les chercheurs ont besoin d’un environnement clair, générant des traces analysables (quantité de mémoire utilisée, temps passé sur le processeur et lors des accès aux périphériques, décisions d’ordonnancement, etc.).

- Simulateur pour robotique/drone. 
Mettre en place un environnement de dev/tests avec, par exemple CoppeliaSim et Python/ROS, génération de scénario et récupération des données de vol et de l’environnement simulé.  
Différents travaux sont réalisés depuis plusieurs années avec l’ONERA sur des simulations utilisant de drones. Ces travaux utilisent d’anciens simulateurs difficiles à maintenir aujourd’hui, ce qui rend complexe la reproduction de certains résultats. Plusieurs exemples existent pour des applications spécifiques, comme avec le robot Poppy et ROS.

La documentation de la partie "Simulation" est disponible dans le dossier LDR_Simulation

## Embarqué

La première étape de cette partie consiste à mettre en place un code, à partir du code NoGUI vu en TP du module « Programmation temps réel » sur la carte ST STM32F429I Discovery avec ZephyrOS en se basant sur l'IDE PlatformIO qui : 

- Récupère des traces simples de chaque thread.
- Exporte les données et les affiches sur SEGGER SystemView.

### Qu’est-ce que Zephyr OS ?

Zephyr OS est un système d'exploitation temps réel open source. Il est reconnu pour sa légèreté, sa modularité et le fait qu’il supporte plusieurs plateformes, ce qui le rend adapté à une large gamme de microcontrôleurs. Avec une capacité à supporter plusieurs protocoles de communication.

### Qu’est-ce que PlatformIO ?

PlatformIO est un IDE, son environnement de développement supporte le langage C/C++ pour les systèmes embarqués. Compatible avec plusieurs systèmes d'exploitation (Windows, Mac et Linux), il s'intègre également en tant qu'extension aux éditeurs de texte existants comme VSCode.

### Définition du Tracing

L’utilisation du tracing dans un système d’exploitation temps réel comme **ZephyrOS** est bénéfique car il permet d’analyser le comportement du système en temps réel en enregistrant les évènements tel que les changements de threads. Cela aide à identifier les bugs ainsi que les problèmes de performances. 
On peut également détecter les threads qui consomment trop de temps cela nous permet d’optimiser le système. 

Il nous permet de comprendre le comportement de threads entre eux, à quelle fréquence les ressources système sont utilisées, cela nous permet d’avoir un aperçu sur le fonctionnement interne du système
Le tracing facilite l’analyse comparative et la reproductibilité des tests, on peut aussi vérifier si toutes les parties du code sont bien testées en conditions réelles

#### Les étapes à suivre pour récupérer des traces d’un code 

Modifier le fichier .conf de notre projet NoGUI en ajoutant les lignes 


```c++

CONFIG_TRACING=y
CONFIG_SEGGER_SYSTEMVIEW=y
CONFIG_SEGGER_SYSTEMVIEW_BOOT_ENABLE=n
CONFIG_SEGGER_SYSVIEW_POST_MORTEM_MODE=n
CONFIG_SEGGER_RTT_BUFFER_SIZE_UP=4096

```

Zephyr utilise RTT (Real-Time Transfer) comme moyen de transport principal pour SystemView, dans ce cas l'exportation des traces nécessite toujours l'utilisation d'un appareil J-Link car ZephyrOS ne supporte pas actuellement l'exportation des traces via d'autres moyens comme l'UART ou le réseau, qui pourraient permettre une transmission directe sans hardware spécifique.

D’où l’intérêt d’utiliser un 

[J-Link Segger EDU](https://fr.farnell.com/segger/8-08-91-j-link-edu-mini/unit-d-education-cortex-m/dp/3106578?gross_price=true&CMP=KNC-GFR-GEN-SHOPPING-Catch-All-GA4-Other-Channel-Audiences-Test-03-Nov-23&mckv=_dc%7Cpcrid%7C%7Cplid%7C%7Ckword%7C%7Cmatch%7C%7Cslid%7C%7Cproduct%7C3106578%7Cpgrid%7C%7Cptaid%7C&gad_source=1&gclid=Cj0KCQjw3tCyBhDBARIsAEY0XNlGYtov5E70cmn43-1cGxXln7vgYtljY4FBiqdjiT4U-VoNwqK6bkgaAnjHEALw_wcB)



pour pouvoir exporter les traces de chaque thread et les afficher sur SEGGER SystemView.

Ce qu’il faut installer pour la réalisation de cette partie :


-	[J-Link software](https://www.segger.com/downloads/jlink/)


-	[SEGGER SystemView](https://www.segger.com/downloads/systemview/)



[!WARNING]

Sélectionnez le protocole SWD pendant la connexion du J-Link, tout en laissant le reste des paramètres par défaut (comme sur l'image en dessous)


![Terminal configuration](/assets/images/Image2.jpg "connecting to J-Link")

Après avoir exécuter le code sur VSCode, et connecter le J-Link, les traces seront affichées en commençant l’enregistrement sur l’interface graphique de SEGGER SystemView.

![SystemView traces](/assets/images/Image1.png "traces des threads")


#### A quoi servent les traces ?

Dans notre code on a trois threads (2 LEDs et un switch), le traçage permet de :
-	Surveiller quand et combien de fois chaque thread est exécuté, c’est important pour analyser le comportement des threads. 
-	Savoir par exemple la durée que prend chaque tâche pour exécuter une boucle, ou comment la pression du switch affecte le comportement des LEDs.
-	Si on voit que la LED 1 par exemple monopolise les ressources plus que prévu, on peut jouer sur le nombre d’itérations ou sur la priorité des threads.
-	Si un problème est détecté, on peut consulter les traces pour voir exactement ce qui se passait juste avant l’erreur

#### Les moyens de suivre des traces d'un code embarqué sans ZephyrOS

-	Freertos SystemView peut être utilisé pour enregistrer des applications qui utilisent FreeRTOS V8, V9, V10 ou V11. Jusqu'à la V10, la source FreeRTOS doit être légèrement modifiée pour afficher correctement l'exécution de l'ordonnanceur FreeRTOS. A partir de la V11, aucune modification du noyau n'est nécessaire.
RT-Thread Contraintes de TraceX
-	TraceX présente les contraintes suivantes :
-	TraceX ne peut pas mesurer avec précision les intervalles entre les événements supérieurs à la période de la minuterie.

![Traces sur TraceX](/assets/images/Image3.png "Azure RTOS TraceX")

### Tableau pour ajouter de nouvelles architectures au projet PlatformIO : autres cartes ST, RISC-V

| RISC-V Boards | Zephyr version| Platformio | Zephyr + PlatformIO |  Prix  |
| ------------  | ------------  | ---------- | ------------------- | ------ |
| Seeed Studio XIAO ESP32C3 | ✅ | ✅ | ❌ |  5,96 € |
| TTGO T-OI PLUS RISC-V ESP32-C3 | ✅ | ✅ | ❌ | $5.50  |
| Espressif ESP32-C3-DevKitM-1 | ✅ | ✅ | ❌ | 15,90 € TTC |
| Espressif ESP32-C3-DevKitC-02 | ✅  | ✅  |  ❌ | 23,66 € |
|DFRobot Beetle ESP32-C3 | ✅ | ✅ | ❌ | 10,50 € TTC |
| GAPuino GAP8 | ❌ | ✅ | ❌ | 230 €|



**Liens des documentations platformio de chacune des cartes précédentes**


[Seeed Studio XIAO ESP32C3](https://docs.platformio.org/en/stable/boards/espressif32/seeed_xiao_esp32c3.html)


[TTGO T-OI PLUS RISC-V ESP32-C3](https://docs.platformio.org/en/stable/boards/espressif32/ttgo-t-oi-plus.html)


[Espressif ESP32-C3-DevKitM-1](https://docs.platformio.org/en/stable/boards/espressif32/esp32-c3-devkitm-1.html)


[Espressif ESP32-C3-DevKitC-02](https://docs.platformio.org/en/stable/boards/espressif32/esp32-c3-devkitc-02.html)


[DFRobot Beetle ESP32-C3](https://docs.platformio.org/en/stable/boards/espressif32/dfrobot_beetle_esp32c3.html#dfrobot-beetle-esp32-c3)


[GAPuino GAP8 "Un kit orienté IA"](https://docs.platformio.org/en/stable/boards/riscv_gap/gapuino.html)



### Application réelle

Créer un parking system, le vrai but derrière c'est de réussir à rajouter le plus de composants possibles (capteurs, actionneurs...) sur le projet ZephyrOS déjà existant.

***Explication sur le parking system***
À l'entrée du système de stationnement, un scanner permet de détecter l'arrivée d'un véhicule. Une fois que l'ISR est déclenché, un mot de passe est envoyé au système et s'il correspond au mot de passe correct, le portail s'ouvre pour permettre au véhicule d'entrer. Dans le cas contraire, le portail reste verrouillé. Si le véhicule entre dans le parking, le nombre de places de parking est mis à jour. S'il n'y a pas de place de parking disponible, le système enverra le message "Parking non disponible" afin que les gens gagnent du temps.
Capteur ultrason n’utilise pas d’interface de communication (I2C ou UART), ils utilisent généralement que des broches GPIO pour se connecter.
Pour pouvoir rajouter le capteur de distance à notre projet zephyr, il faut créer un driver puisqu’il n’existe pas encore 


 #### Capteur de distance HCSR04 

à rajouter dans le fichier "overlay" du projet 

```js
&hcsr04 {
    compatible = "elecfreaks,hc-sr04";
    label = "HCSR04";
    trig-gpios = <&gpio0 4 GPIO_ACTIVE_HIGH>;
    echo-gpios = <&gpio0 5 GPIO_ACTIVE_HIGH>;
    status = "okay";
};
```
**PARTIE A COMPLETER (MODULE FAIT PAR MR COURBIN)**
***A completer***

#### ServoMoteur FS90 

**Ce qu'il faut modifier pour pouvoir rajouter un servomoteur à notre projet zephyr**

Les canaux de PWM sur le fichier overlay ça va de 0 à 3 et même si on écrit 4 il le prend comme un 3 par défaut directement. 

pour rajouter un servomoteur à notre projet zephyrOS 
créer un nouveau dossier dts/bindings et mettre dedans ce fichier .yaml : 

```js
description: PWM-driven servo motor.

compatible: "pwm-servo"

include: base.yaml

properties:
  pwms:
    required: true
    type: phandle-array
    description: PWM specifier driving the servo motor.

  min-pulse:
    required: true
    type: int
    description: Minimum pulse width (nanoseconds).

  max-pulse:
    required: true
    type: int
    description: Maximum pulse width (nanoseconds).
```

Dans le fichier overlay compatible = "pwm-servo" on doit avoir le meme nom que sur le compatible de yaml

```js
    servo: servo {
        **compatible = "pwm-servo";**
        pwms = <&pwm1 3 PWM_MSEC(20) PWM_POLARITY_NORMAL>;
        min-pulse = <PWM_USEC(700)>;
        max-pulse = <PWM_USEC(2500)>;
    };
```

```js pinctrl-0 = <&tim1_ch3_pe13>;```  cette ligne veut dire: channel 4 sur le pin PE13

Activer le PWM dans le fichier prj.conf se fait avec la ligne suivante: CONFIG_PWM=y
Rajouter cette bibliotheque #include <zephyr/drivers/pwm.h> dans le main principal 


**PS :** Le nom de notre fichier overlay doit être le même que celui de notre board dans platformio.ini, ce n'est pas trop le même que celui de ZephyrOS

dans ZephyrOS : disco_f429zi
dans platformio : stm32f429i_disc1

**Liens importants :**



- [Exemple d'un code servomoteur](https://github.com/zephyrproject-rtos/zephyr/blob/v3.6-branch/samples/basic/servo_motor)



- [Exemple du PWM](https://github.com/zephyrproject-rtos/zephyr)



- [Documentation du PWM](https://docs.zephyrproject.org/latest/reference/drivers/pwm.html)



- [Exemple de la connectivité wifi de la carte stm32](https://github.com/gd91/stm32_winc1500_station_example)



- [Création d'un driver hcsr04](https://github.com/inductivekickback/hc-sr04/blob/main/nrf/drivers/sensor/hc_sr04/hc_sr04.c#L240)



- [Lien pour l'overlay du servomoteur](https://github.com/zephyrproject-rtos/zephyr/discussions/53924)  



- [Formule du servomoteur](https://stackoverflow.com/questions/12931115/algorithm-to-map-an-interval-to-a-smaller-interval)



***A retravailler***
code : //largeur de repetition c'est 20ms elle est choisie 2500 et 700 c'est la durée qu'il prend sur chaque coté en microseconde on peut la changer en fonction de la rapidité qu'on veut avoir
elle va de 900 jusqu'à 2100 c'est une pulsation

on peut changer la valeur de la pulsation en fonction de la rapidité qu'on veut avoir
on peut changer la valeur de la largeur de repetition en fonction de la rapidité qu'on veut

un code simple qui fonctionne tout seul sans pour autant l'integrer dans le dossier 
ensuite integrer le capteur de hcsr04 dans le driver 
***jusqu'ici***

**PS** au moment de renommer un dossier, évitez les espaces ou caractères spéciaux 

**A noter**
Pour que le servomoteur compile, s'exécute correctement et réalise les tâches qu'on lui demande de faire, on ne doit pas l'intégrer à un projet qui contient déjà un Display, par contre il fonctionne bien avec d'autres composants à la fois mais il faut respecter certains paramêtres. 

**Exemple** 
- Check la DOC de zephyr: 


[DOC Zephyr](https://docs.zephyrproject.org/latest/samples/basic/servo_motor/README.html)


The corresponding PWM pulse widths for a 0 to 180 degree range are 700 to 2300 microseconds, respectivel


- Le code compile sans erreurs quand y a l'écran mais le servomoteur ne fonctionne pas, même s'il reçoit un signal (on entend un bruit)