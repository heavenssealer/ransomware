# Makefile pour le Ransomware Éducatif - Jour 5

CC = gcc
CFLAGS = -Wall -Wextra -I.
LDFLAGS = -pthread

# Cible par défaut
all: ransomware

# Compilation du ransomware principal (avec modules.c)
ransomware: ransomware.c modules.c
	$(CC) $(CFLAGS) -o ransomware ransomware.c modules.c

# Compilation du serveur C2 (MODULE 9 - optionnel)
c2_server: c2_server.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o c2_server c2_server.c

# Nettoyage
clean:
	rm -f ransomware c2_server *.o

# Aide
help:
	@echo "Commandes disponibles:"
	@echo "  make           - Compile le ransomware (modules.c)"
	@echo "  make ransomware - Compile le ransomware"
	@echo "  make c2_server  - Compile le serveur C2 (MODULE 9)"
	@echo "  make clean     - Supprime les binaires"
	@echo "  make help      - Affiche cette aide"

.PHONY: all clean help
