import socket
import threading

def betting(Money, betMoney):
    return Money - betMoney


def receiveMessagesAndHandleClientGame(client_socket, bet):
    #variables for betting and money of the player
    initialMoney = 500
    int(initialMoney)
    #initialngamatrixscore kay naa diri tnan update sa matrix mabutang
    initialMatrixScore = []
    matrixScore = []  #kani diri ang final na score sa user kay ang kwaon ani ang final update ra nga gekan sa initial matrix score
    #counter for the multiplyer of bet or earnings
    counter = 0
    #input to stop and calculate your winning
    signalSubmit = "25"
    #holder for bomb count if there is one then lose
    bombCounter = 0
    #initital earning
    earned = 0
    try:
        while True:
            #if naay bomb dli mo print ani
            if bombCounter < 1 or counter >1:
                print("Money: " + str(initialMoney))
                print("Bet: " + str(bet) )
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
                    initialMoney +=earned
                    print("You Won: " + str(earned) )
                    print("Money: " + str(initialMoney))
                    #ask user if he/she still want's to play then restart the check counter and the matrix value in the server
                    #then let the user input a new bet
                    redo = input("Enter (R) to Play again. Input (Q) to exit").lower
                    if redo == 'R':
                        counter = 0
                        bet = input("Input Bet: ")
                        client_socket.send(redo.encode('utf-8'))

                        #no need to undo the matrix Score and initial Matrix score since they are dependent 
                        # on the original matrix in the server so by making the server send an unscatched matrix then
                        # the matrixScore and initialMatrix score will also be updated
                        #the money remains the same except for bet 
                    else:#if redp == "q"
                        client_socket.send(redo.encode('utf-8'))


                
                print("User: " + message)
            else:#else if naka abot og 25 then mao ni ang data or message para UI sa terminal
                for i in range(len(message)):
                    #storing every updates to initial matrix bomb and score checking
                    initialMatrixScore.append(message[i])
                #for printing the UI in terminal
                    print("[" +message[i] +']', end= '')
                    if i in {4, 9 ,14 ,19,24}:
                        print()
                 # to check for boombs in every update
                  #getting the value from the last 25 index to last one it is the new update
                matrixScore = initialMatrixScore[-25:]
                for i in range(len(matrixScore)):
                   
                    if matrixScore[i] == "*":
                        bombCounter +=1

                #if there is bomb then tell the or the user click a bomb then tell the user he/she lose
                if bombCounter > 0:
                    initialMoney -= bet
                    print("Money: " + str(initialMoney))
                    print("Alert: You Tap a Bomb!!!\nYou Lose!!!\nBetter Luck Next Time :) :) :)")
                    #asking if the player wants to play again
                    redo = input("Enter (R) to Play again. Input (Q) to exit").lower
                    if redo == 'R':
                        bombCounter = 0
                        bet = input("Input Bet: ")
                        client_socket.send(redo.encode('utf-8'))

                        #no need to undo the matrix Score and initial Matrix score since they are dependent 
                        # on the original matrix in the server so by making the server send an unscatched matrix then
                        # the matrixScore and initialMatrix score will also be updated
                        #the money remains the same except for bet 
                    else:#if redp == "q"
                        client_socket.send(redo.encode('utf-8'))



    except Exception as e:
        print(f"Error receiving messages: {e}")
def main():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    local_ip = socket.gethostbyname(socket.gethostname())
    server_address = (local_ip, 8080)

    client_socket.connect(server_address)
    # bet for the game
    print("Initial Money: 500")
    bet = int(input("Enter your Bet: "))
    # money reamaining after betting
    # Start a thread to receive messages from the server
    receive_thread = threading.Thread(target=receiveMessagesAndHandleClientGame, args=(client_socket, bet))
    receive_thread.start()

    try:
       
        while True:
            # Get input from the user and send it to the server
           
            message = input()#for the input of the game
            #send to server to process the UI and then make the server send it back to the client to print it as the user input 
            client_socket.send(message.encode('utf-8'))

    except KeyboardInterrupt:
        # Close the client on keyboard interrupt
        print("Client shutting down...")
        client_socket.close()

if __name__ == "__main__":
    main()




