# Μεταβλητές (Compiler και Flags)
CC = gcc
CFLAGS = -Wall -Wextra -g -pthread

# Τα ονόματα των τελικών εκτελέσιμων
TARGETS = server viewer test_client

# Ο default στόχος αν γράψεις απλά 'make'
all: $(TARGETS)

# Κανόνας για τον Server
server: thread_per_client.c replayer.c client_handler.c
	$(CC) $(CFLAGS) thread_per_client.c replayer.c client_handler.c -o server

# Κανόνας για τον Viewer
viewer: viewer.c ui_colors.h
	$(CC) $(CFLAGS) viewer.c -o viewer

# Κανόνας για τον Test Client
test_client: tests.c ui_colors.h
	$(CC) $(CFLAGS) tests.c -o test_client

# Ψευδο-στόχος για να τρέχεις εύκολα το test
test: test_client
	@echo "======================================"
	@echo "       RUNNING SEQUENCE TEST"
	@echo " Make sure the server is running fast!"
	@echo " (e.g., ./server -p 9000 -n 1 -s 10000)"
	@echo "======================================"
	./test_client -h localhost -p 9000

# Καθαρισμός των εκτελέσιμων (make clean)
clean:
	rm -f $(TARGETS)