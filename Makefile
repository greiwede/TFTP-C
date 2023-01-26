all:
	gcc -o client_appl Client/client.c Client/user_interaction.c Shared/read_packets.c Shared/packets.c Shared/utils.c
	gcc -o server_appl Server/server.c Server/connection_establishment.c Shared/read_packets.c Shared/packets.c Shared/utils.c
