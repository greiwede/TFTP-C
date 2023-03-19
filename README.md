# TFTP implementation

This is a [TFTP](https://www.rfc-editor.org/rfc/rfc1350) implementation.
This implementation only runs on UNIX systems.

## Project Structure
The project is strucuted into 3 major packages:
- Client: control flow for TFTP client and user interaction functionality
- Server: control flow for TFTP server and basic connection establishment
- Shared: packet manipulation, data flow, file manipulation and utils that are used by both server
    and client

For each new client connection, the server opens a new thread to handle the file transfer, while
the main thread remains available to listen for new requests.

## Functionality

basic functionality for [TFTP](https://www.rfc-editor.org/rfc/rfc1350)
- send files of any kind (e.g. .txt, .PNG) to server directory
- receive files of any kind from server to client directory
- supported sending modes:
    - netascii: converts requested file to netascii format for sending and converts it to local
                after receiving
    - octet: sends bytes of file as read in by file descriptor
    - mail: *deprecated*

## Getting Started

Steps:
1. make
2. cd to Server/
3. execute ./server\_appl \<server-ip\> <br />
    --- make sure to be in admin mode for this (bindinig to port 69) \
    --- the server-IP is optional, the default value is 127.0.0.1
4. cd to Client/
5. execute ./client\_appl \<server-ip\> <br />
    --- the server-IP is optional, the default value is 127.0.0.1
6. follow instructions on ./client\_appl console

## Tested Cases

- TFTP functionality
    - RRQ and WRQ in both octet and netascii mode
    - server can handle Requests from multiple clients at once
    - last packet has 512 bytes data thus another 0 data byte packet is sent
    - correct handling of errors like
        - requesting missing file from server
        - exceeding maximum timeouts
- verification with third party tftp tools
    - server was tested atftp
    - client was tested with the solar winds tftp server
- data sending over LAN
