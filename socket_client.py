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
# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', 4444)
print ('connecting to %s port %s' % server_address)
sock.connect(server_address)

count=0
message = 'This is the message.  It will be repeated.'.encode()
try:
    while True:
    
        exit = False
        
        #send the server init packet
        conn_req = 'Client requesting to connect'.encode()
        sock.sendall(conn_req)

        # Look for the response
        amount_received = 0
        amount_expected = len(message)
        
        while amount_received < amount_expected:
            client_id = 0
            data = sock.recv(1024)
            amount_received += len(data)
            mess = data.decode()

            if "REJECT" in mess:
                print("Connection rejected by server");
                break;
            if "My politely respondance" in mess:
                print("Receieved polite reponse from server")
                sock.sendall('INIT'.encode())
            if "WELCOME" in mess:
                client_id = int(mess.replace('WELCOME,', ''))
                print(client_id)
            if "The games will begin shortly" in mess:
                print("The games will begin shortly")
            if "START" in mess:
                print("The game has now started")
                choice = input()
                sock.sendall()
            elif "You lose" in mess:
                print("We lost, closing connection")
                exit = True
                break
            else:
                print ( 'received "%s"' % mess)
        if exit:
            break
finally:    
    print ('closing socket')
    sock.close()
