#!/bin/bash
# Setup du sandbox pour le TP Ransomware - Jour 5
# Ce script crée un environnement de test sécurisé avec ~50 fichiers

set -e

SANDBOX_DIR="sandbox"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "╔═══════════════════════════════════════════════════════╗"
echo "║   Setup Sandbox - TP Ransomware Jour 5              ║"
echo "╚═══════════════════════════════════════════════════════╝"
echo ""

# Vérifier si le sandbox existe déjà
if [ -d "$SANDBOX_DIR" ]; then
    # En mode non-interactif (pipe), supprimer automatiquement
    if [ -t 0 ]; then
        read -p "⚠️  Le dossier sandbox/ existe déjà. Le supprimer ? (o/N) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Oo]$ ]]; then
            rm -rf "$SANDBOX_DIR"
            echo "✓ Ancien sandbox supprimé"
        else
            echo "❌ Annulé. Supprimez manuellement le dossier sandbox/ avant de relancer."
            exit 1
        fi
    else
        # Mode automatique : lire la réponse depuis stdin
        read -r REPLY
        if [[ $REPLY =~ ^[Oo]$ ]]; then
            rm -rf "$SANDBOX_DIR"
            echo "✓ Ancien sandbox supprimé"
        else
            echo "❌ Annulé."
            exit 1
        fi
    fi
fi

# Créer la structure du sandbox
echo "[1/5] Création de la structure..."
mkdir -p "$SANDBOX_DIR"/{documents,images,archives,sensitive,logs,config}
mkdir -p "$SANDBOX_DIR"/documents/{reports,notes,drafts}
mkdir -p "$SANDBOX_DIR"/sensitive/{credentials,keys}
mkdir -p "$SANDBOX_DIR"/.hidden

echo "[2/5] Génération des fichiers documents (20 fichiers)..."
# Documents texte
for i in {1..10}; do
    cat > "$SANDBOX_DIR/documents/report_$i.txt" << EOF
RAPPORT CONFIDENTIEL #$i
Date: $(date +%Y-%m-%d)
Contenu: Données de test pour le TP ransomware
Status: En cours d'analyse

Ce fichier contient des informations de test.
Il sera utilisé pour démontrer le chiffrement.
Ligne de remplissage numéro $i
Fin du rapport.
EOF
done

# Notes markdown
for i in {1..5}; do
    cat > "$SANDBOX_DIR/documents/notes/note_$i.md" << EOF
# Note de Projet $i

## Objectifs
- Tester le scanner récursif
- Valider le chiffrement XOR
- Vérifier la checksum CRC32

## Résultats
Fichier de test numéro $i
EOF
done

# Drafts
for i in {1..5}; do
    echo "Brouillon $i - Texte de test $(date +%s)" > "$SANDBOX_DIR/documents/drafts/draft_$i.txt"
done

echo "[3/5] Génération des fichiers sensibles (15 fichiers)..."
# Fichiers "sensibles"
cat > "$SANDBOX_DIR/sensitive/credentials/passwords.txt" << EOF
# Base de données des accès (TEST)
admin:test123
user1:password
user2:qwerty
root:toor
EOF

cat > "$SANDBOX_DIR/sensitive/credentials/api_keys.txt" << EOF
API_KEY_TEST=abc123def456
SECRET_TOKEN=xyz789uvw012
DATABASE_PASSWORD=testdb2026
EOF

cat > "$SANDBOX_DIR/sensitive/keys/private_key.txt" << EOF
-----BEGIN TEST PRIVATE KEY-----
CECI EST UNE FAUSSE CLE PRIVEE POUR LE TP
NE JAMAIS UTILISER EN PRODUCTION
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMI
-----END TEST PRIVATE KEY-----
EOF

# Fichiers de configuration sensibles
for i in {1..10}; do
    cat > "$SANDBOX_DIR/sensitive/config_$i.conf" << EOF
[server]
host=192.168.$((RANDOM % 255)).$((RANDOM % 255))
port=$((4000 + i))
key=testkey$i
EOF
done

# Hidden files
echo "hidden_data_$RANDOM" > "$SANDBOX_DIR/.hidden/secret.txt"

echo "[4/5] Génération des logs (10 fichiers)..."
# Logs
for i in {1..10}; do
    cat > "$SANDBOX_DIR/logs/app_$i.log" << EOF
[$(date +%Y-%m-%d\ %H:%M:%S)] INFO: Application $i démarrée
[$(date +%Y-%m-%d\ %H:%M:%S)] DEBUG: Module scanner chargé
[$(date +%Y-%m-%d\ %H:%M:%S)] INFO: Scan terminé: $((RANDOM % 100)) fichiers
[$(date +%Y-%m-%d\ %H:%M:%S)] WARNING: Fichier .git ignoré
[$(date +%Y-%m-%d\ %H:%M:%S)] SUCCESS: Opération complétée
EOF
done

echo "[5/5] Génération des archives (5 fichiers)..."
# Créer quelques fichiers zip/tar (simulation)
for i in {1..5}; do
    echo "Archive de test $i - contenu binaire simulé: $(openssl rand -hex 100)" > "$SANDBOX_DIR/archives/backup_$i.zip"
done

# Fichier .gitignore pour éviter de commiter le sandbox
cat > "$SANDBOX_DIR/.gitignore" << EOF
# Ignorer tout le contenu du sandbox
*
!.gitignore
EOF

echo ""
echo "╔═══════════════════════════════════════════════════════╗"
echo "║              ✓ SANDBOX CRÉÉ AVEC SUCCÈS             ║"
echo "╚═══════════════════════════════════════════════════════╝"
echo ""

# Statistiques
TOTAL_FILES=$(find "$SANDBOX_DIR" -type f | wc -l)
TOTAL_DIRS=$(find "$SANDBOX_DIR" -type d | wc -l)
TOTAL_SIZE=$(du -sh "$SANDBOX_DIR" | cut -f1)

echo "📊 Statistiques:"
echo "   • Fichiers créés: $TOTAL_FILES"
echo "   • Dossiers: $TOTAL_DIRS"
echo "   • Taille totale: $TOTAL_SIZE"
echo ""
echo "📁 Structure:"
tree -L 2 "$SANDBOX_DIR" 2>/dev/null || find "$SANDBOX_DIR" -maxdepth 2 -print
echo ""
echo "🎯 Le sandbox est prêt pour vos tests !"
echo "   Testez le scanner: ./ransomware (option 1)"
echo ""
echo "⚠️  RAPPEL: N'exécutez JAMAIS le ransomware en dehors du sandbox !"
echo ""
