"""
This is a simple example of a client program written in Python.
Again, this is a very basic example to complement the 'basic_server.c' example.


When testing, start by initiating a connection with the server by sending the "init" message outlined in 
the specification document. Then, wait for the server to send you a message saying the game has begun. 

Once this message has been read, plan out a couple of turns on paper and hard-code these messages to
and from the server (i.e. play a few rounds of the 'dice game' where you know what the right and wrong 
dice rolls are). You will be able to edit this trivially later on; it is often easier to debug the code
if you know exactly what your expected values are. 

From this, you should be able to bootstrap message-parsing to and from the server whilst making it easy to debug.
Then, start to add functions in the server code that actually 'run' the game in the background.  
"""

import socket
from time import sleep

def main():
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect the socket to the port where the server is listening
    server_address = ('localhost', 5555)
    print ('connecting to %s port %s' % server_address)
    sock.connect(server_address)
    sock.sendall("INIT".encode())
    print("Joining game")
    client_id = 0
    players = 0
    lives = 0
    last_msg = ""
    try:    
        exit = False        
        while True:
            data = sock.recv(1024)
            mess = data.decode()
            if "WELCOME" in mess:
                print(mess)
                last_msg, client_id = mess.split(",")
                print("Joined as player ", client_id)
            elif "START" in mess:
                print(mess) 
                last_msg, players, lives = mess.split(",")
                lives = int(lives)
                print("Starting game with", players, "and", lives, "lives")
                move = client_id + construct_move()
                print("Sending move: " + move)
                sock.sendall((move).encode())
            elif "PASS" in mess:
                print("Passed round!")
                print(mess)
                move = client_id + construct_move()
                print("Sending move: " + move)
                sock.sendall((move).encode())
            elif "FAIL" in mess:
                print("Failed round!")
                print(mess)
                lives -= 1
                move = client_id + construct_move()
                print("Sending move: " + move)
                sock.sendall((move).encode())
            elif "REJECT" in mess:
                print("Got rejected, exiting...")
                exit = True
                break
            elif "CANCEL" in mess:
                print("Game canceled by the server. Closing...")
                exit = True
                break
            elif "KICK" in mess:
                print("Kicked from game.")
                print(mess)
                exit = True
                break
            else:
                print ( 'received "%s"' % mess)
            if "VICT" in mess:
                print("WE WON!")
                print(mess)
                exit = True
                break
            elif "ELIM" in mess:
                print("We lost :(")
                print(mess)
                exit = True
                break
            if exit:
                break
    finally: 
        sock.close()   
        print ('closing socket')
    
def construct_move():
    moves = ["ODD", "EVEN", "DOUB", "CON"]

    move_num = input("Choose a number 1(Odd) 2(Even) 3(Doubles) 4(Contains)")
    if move_num != "":
        move_num = int(move_num)
    con_num = ""
    if move_num == 4:
        con_num = input("What number would you like to guess (1-6)")
    move = ",MOV,"+moves[move_num-1]+","+ con_num
    return move

main()