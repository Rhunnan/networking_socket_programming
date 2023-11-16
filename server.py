import socket
import threading

# List to store all connected clients
connected_clients = []

def handle_client(client_socket):
    # Add the client to the list of connected clients
    connected_clients.append(client_socket)

    try:
        while True:
            # Receive a message from the client
            message = client_socket.recv(1024)
            if not message:
                break

            # Broadcast the message to all connected clients
            broadcast(message)

    except Exception as e:
        print(f"Error handling client: {e}")

    finally:
        # Remove the client from the list when it disconnects
        connected_clients.remove(client_socket)
        client_socket.close()

def broadcast(message):
    # Broadcast the message to all connected clients
    for client in connected_clients:
        try:
            client.send(message)
        except Exception as e:
            print(f"Error broadcasting message: {e}")
            # Remove the client if there is an error sending a message
            connected_clients.remove(client)
            client.close()

def main():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    local_ip = socket.gethostbyname(socket.gethostname())
    server_address = (local_ip, 8080)

    server_socket.bind(server_address)
    server_socket.listen(5)

    print(f"Server listening on {server_address[0]}:{server_address[1]}...")

    try:
        while True:
            client_socket, client_address = server_socket.accept()
            print(f"Accepted connection from {client_address[0]}:{client_address[1]}")

            # Create a new thread for each client
            client_thread = threading.Thread(target=handle_client, args=(client_socket,))
            client_thread.start()

    except KeyboardInterrupt:
        # Close the server on keyboard interrupt
        print("Server shutting down...")
        for client_socket in connected_clients:
            client_socket.close()
        server_socket.close()

if __name__ == "__main__":
    main()
