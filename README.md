
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
* `ransomware_complet.c` (Programme principal)
* `server_c2.c` (Serveur C2)
* Modules : `scanner.c`, `xor_crypto.c`, `c2_client.c`, etc. (et leurs headers `.h`).

### Compilation

1. **Compiler le Serveur C2 :**
```bash
gcc server_c2.c -o server_c2 -pthread

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
./server_c2

```

### 3. Exécuter le Ransomware

Le programme s'utilise en ligne de commande avec des arguments :

#### A. Tester la connexion C2

Vérifie si le malware peut "appeler la maison" avant d'attaquer.

```bash
./ransomware_complet c2_test

```

*Résultat attendu : Connexion établie, échange de messages STATUS et ENCRYPT.*

#### B. Lancer l'attaque (Chiffrement)

Chiffre le contenu du dossier `sandbox`.

```bash
./ransomware_complet encrypt

```

*Action : Les fichiers deviennent `.locked`, la note de rançon apparaît.*

#### C. Restaurer les fichiers (Déchiffrement)

Tente de déchiffrer les fichiers.

```bash
./ransomware_complet decrypt

```

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

```
