AUTEURS : LOUHOU GODWILL | BENOIT ENZO | LAKRIB IKRAM | MERLE OLIVIER | BA ISSIAKHA

---

# Projet Ransomware Éducatif

**Avertissement :** *Ce projet est réalisé dans un but strictement éducatif dans le cadre d'une formation en sécurité informatique. Il a pour objectif de comprendre les mécanismes d'une attaque pour mieux s'en défendre. Ne jamais exécuter ce code sur des données sensibles ou en dehors de l'environnement de test (`sandbox`).*

---

## Description

Ce projet simule le comportement d'un rançongiciel (ransomware) basique. Il permet d'étudier le cycle de vie d'une infection :
1. **Exploration** du système de fichiers (Scan).
2. **Communication** avec un serveur de commande et contrôle (C2).
3. **Chiffrement** des données utilisateur.
4. **Demande de rançon** (Note).
5. **Restauration** des données (Déchiffrement).

Le programme est conçu pour opérer **uniquement** dans un dossier cible nommé `sandbox/` afin d'éviter tout accident.

## Fonctionnalités

### 1. Client (Le Malware)
* **Scan Récursif :** Parcourt le dossier `sandbox/` et ses sous-dossiers.
* **Chiffrement XOR :** Chiffre les fichiers trouvés en utilisant une clé symétrique.
* **Verrouillage :** Renomme les fichiers avec l'extension `.locked`.
* **Communication C2 :** Se connecte à un serveur distant pour récupérer des instructions ou rapporter son statut.
* **Note de Rançon :** Génère un fichier `RANSOM_NOTE.txt` expliquant la situation à l'utilisateur.
* **Déchiffrement :** Restaure les fichiers originaux si la bonne clé est fournie.

### 2. Serveur C2 (Command & Control)
* **Multi-thread :** Peut gérer plusieurs clients simultanément.
* **Protocole simple :** 
    * Réception de commandes (`STATUS`, `ENCRYPT`).
    * Envoi d'instructions (Mode de chiffrement, Clés).

---

## Installation et Compilation

### Prérequis
* Compilateur GCC.
* Système type Unix/Linux (pour les sockets et pthreads).

### Structure des fichiers
Assurez-vous d'avoir les fichiers sources suivants :
* `ransomware.c` (Programme principal)
* `c2_server.c` (Serveur C2)
* Modules : `scanner.c`, `xor_crypto.c`, `c2_client.c`, etc. (et leurs headers `.h`).

### Compilation

1. **Compiler le Serveur C2 :**
```bash
gcc c2_server.c -o server -pthread

```

2. **Compiler le Ransomware (Client) :**
```bash
# A compiler avec tous les modules dans le meme repertoire
gcc -I. -o ransomware ransomware.c module.c 

```


---

## Utilisation

### 1. Préparation de l'environnement

Créez le dossier de test et ajoutez-y des fichiers factices :

```bash
mkdir sandbox
echo "Données confidentielles" > sandbox/secret.txt
echo "Rapport financier" > sandbox/budget.doc

```

### 2. Démarrer le Serveur C2

Dans un terminal séparé, lancez le serveur pour écouter les connexions (Port 4444) :

```bash
./server

```

### 3. Exécuter le Ransomware

Le programme s'utilise en ligne de commande avec des arguments :

<img width="1121" height="194" alt="Screenshot from 2026-01-09 16-34-29" src="https://github.com/user-attachments/assets/d3ffe35e-2551-40e0-aaff-44d9c22763e0" />


#### A. Tester la connexion C2

Vérifie si le malware peut "appeler la maison" avant d'attaquer.

```bash
./ransomware c2_test

```
<img width="671" height="267" alt="Screenshot from 2026-01-09 16-38-47" src="https://github.com/user-attachments/assets/5ee999f7-7a4b-4d35-93f8-d8e4559b3a96" />


*Résultat attendu : Connexion établie, échange de messages STATUS et ENCRYPT.*

Cote serveur : 

<img width="655" height="237" alt="Screenshot from 2026-01-09 16-38-55" src="https://github.com/user-attachments/assets/aefdbe98-7234-44dd-b654-ac86a2036e21" />


#### B. Lancer l'attaque (Chiffrement)

Chiffre le contenu du dossier `sandbox`.

```bash
./ransomware encrypt

```

<img width="961" height="918" alt="Screenshot from 2026-01-09 16-36-00" src="https://github.com/user-attachments/assets/c66cbbdd-1352-4e8c-b855-be1091fe2e1c" />

<img width="707" height="369" alt="Screenshot from 2026-01-09 16-36-10" src="https://github.com/user-attachments/assets/c7071543-a646-48ef-bbe7-0a8a4e549c52" />

<img width="703" height="431" alt="Screenshot from 2026-01-09 16-37-03" src="https://github.com/user-attachments/assets/253ba9b7-e3c4-4d78-b310-d3b2bebbaa5f" />

<img width="523" height="1308" alt="Screenshot from 2026-01-09 16-37-36" src="https://github.com/user-attachments/assets/f11cd030-d55c-43ae-a3c0-50bf77181d39" />


*Action : Les fichiers deviennent `.locked`, la note de rançon apparaît.*

#### C. Restaurer les fichiers (Déchiffrement)

Tente de déchiffrer les fichiers.

```bash
./ransomware decrypt

```

<img width="690" height="1308" alt="Screenshot from 2026-01-09 16-37-58" src="https://github.com/user-attachments/assets/bdd461fc-b0c8-46b6-9950-48f18d51fc59" />


*La clé par défaut (Hardcoded) est : `MALWARE2026`.*

---

## Détails Techniques

| Composant | Détail |
| --- | --- |
| **Algorithme** | XOR (Opérateur bit à bit) |
| **Clé (Symétrique)** | `MALWARE2026` |
| **Cible** | Dossier `sandbox/` (Hardcoded) |
| **Protocole C2** | TCP Socket (Port 4444) |
| **Intégrité** | Calcul CRC32 (Simulation) |

---

## Sécurité et Limitations

* **Confinement :** Le code est bridé pour ne scanner que le répertoire défini par `TARGET_DIR` ("sandbox").
* **Chiffrement Faible :** Le XOR simple est trivial à casser et sert uniquement d'illustration pédagogique.
* **Nettoyage :** La fonction de déchiffrement supprime les fichiers `.locked` après restauration et efface la note de rançon.

---

*Projet réalisé dans le cadre du TP - Jour 5.*

