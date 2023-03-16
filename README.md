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

basic functionality
- send files any kind (e.g. .txt, .PNG) to server directory
- receive files from server to client directory
- supported sending modes:
    - netascii: converts requested file to netascii format for sending and converts it to local
                after receiving
    - octet: sends bytes of file as read in by file descriptor

## Getting Started

Steps:
1. make
2. cd to Server/
3. execute ./server\_appl <server-ip>
    --- make sure to be in admin mode for this (bindinig to port 69)
    --- the server-IP is optional, the default value is 127.0.0.1
4. cd to Client/
5. execute ./client\_appl <server-ip>
    --- the server-IP is optional, the default value is 127.0.0.1
6. follow instructions on ./client\_appl console
