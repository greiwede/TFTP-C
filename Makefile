all:
	gcc -o client_appl Client/client.c Shared/read_packets.c Shared/packets.c
	gcc -o server_appl Server/server.c Server/connection_establishment.c Shared/read_packets.c Shared/packets.c
