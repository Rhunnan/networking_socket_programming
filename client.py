import socket
import threading

def betting(Money, betMoney):
    return Money - betMoney

# //pagbuhat rules para sa bumba if maka click og bumba i minus or i lose siya sa iyang ge bet

def receive_messages(client_socket, bet):
    #initialngamatrixscore kay naa diri tnan update sa matrix mabutang
    initialMatrixScore = []
    #kani diri ang final na score sa user kay ang kwaon ani ang final update ra nga gekan sa initial matrix score
    matrixScore = []
    #counter for the multiplyer
    counter = 0
    #input to stop and calculate your winning
    signalSubmit = "25"
    bombCounter = 0
    #
    earned = 0
    try:
        while True:
            # Receive and print messages from the server
            message = client_socket.recv(1024)
            message = message.decode('utf-8')

            print("From The Game")
            #if ang length sa message kay wakaabot og 25 then message ni paras input sa user
            if len(message) not in (25, 26):
                # if ang message ge input sa user kay 26 then mo stop nani siya for submission and i count ang iyang win
                if signalSubmit == message:
                    matrixScore = initialMatrixScore[-25:]
                    for i in range(len(matrixScore)):
                        if(matrixScore[i]== '/'):
                            counter +=1
                    earned = bet*counter
                    print("You Won: " + str(earned) )
                
                
                print("User: " + message)
            else:#else if naka abot og 25 then mao ni ang data or message para UI sa terminal
                for i in range(len(message)):
                    initialMatrixScore.append(message[i])
                    print("[" +message[i] +']', end= '')
                    if i in {4, 9 ,14 ,19,24}:
                        print()

    except Exception as e:
        print(f"Error receiving messages: {e}")

def main():
    #variables for betting and money of the player
    initialMoney = 500
    int(initialMoney)


    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    local_ip = socket.gethostbyname(socket.gethostname())
    server_address = (local_ip, 8080)

    client_socket.connect(server_address)
    # bet for the game
    print("Money: " + str(initialMoney))
    bet = int(input("Enter your Bet: "))
    # money reamaining after betting
    initialMoney = betting(initialMoney, bet)
    # Start a thread to receive messages from the server
    receive_thread = threading.Thread(target=receive_messages, args=(client_socket, bet))
    receive_thread.start()

    try:
       
        while True:
            # Get input from the user and send it to the server
            print("Money: " + str(initialMoney))
            print("Bet: " + str(bet) )
            message = input()
            client_socket.send(message.encode('utf-8'))

    except KeyboardInterrupt:
        # Close the client on keyboard interrupt
        print("Client shutting down...")
        client_socket.close()

if __name__ == "__main__":
    main()




