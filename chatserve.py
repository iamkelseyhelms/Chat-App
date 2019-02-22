#!/bin/python
#
# Author: Kelsey Helms
# Date Created: May 4, 2017
# Filename: chatserve.py
#
# Overview: This implements the python server chat
#

from socket import *
import sys

MAX_MESSAGE_LENGTH = 500

############################################################
# chat: client initiates conversation and alternates chat.
#
# parameters: socket file descriptor, client's handle, and
# handle.
# returns: none.
############################################################

def chat(connection, clientname, handle):
    input = ""    #message string
    
    while 1:    #stay in chat until "\quit"
        output = connection.recv(MAX_MESSAGE_LENGTH+1)[0:-1]    #get message from client

        if output == "":  #if message is blank, print indicator and close connection
            print "Connection closed"
            print "Waiting for new connection"
            break

        print "{}> {}".format(clientname, output)   #print clientname and message

        input = ""  #message string
            
        while len(input) == 0 or len(input) > MAX_MESSAGE_LENGTH:   #while empty or too large
            input = raw_input("{}> ".format(handle))      #get input from user

        if input == "\quit":    #if input is "\quit", print indicator and close connection
            print "Connection closed"
            print "Waiting for new connection"
            break
        
        connection.send(input)   #send message to client


############################################################
# handshake: sends handles from client to server and vis
# versa.
#
# parameters: socket file descriptor and handle.
# returns: client's handle.
############################################################

def handshake(connection, handle):
    clientname = connection.recv(1024)   #receive handle from client
    connection.send(handle)    #send handle to client
    return clientname   #return client's handle


############################################################
# main: runs the program.
#
# parameters: port number.
# returns: none.
############################################################

if __name__ == "__main__":
    
    if len(sys.argv) != 2:  #if we have too few or too many args, print error and exit
        print "You must specify the port number for the server to run"
        exit(1)

    port = sys.argv[1]    #get port number from user
    sock = socket(AF_INET, SOCK_STREAM)     #create TCP socket
    sock.bind(('', int(port)))  #connect socket to port

    sock.listen(1)  #listen for incoming messages

    handle = ""   #user's handle
        
    while len(handle) == 0 or len(handle) > 10:     #handle must be 10 or less chars
        handle = raw_input("Please enter a user name of 10 characters or less: ")   #get handle from user
        print "The server is ready to receive incoming messages"
            
    while 1:    #keep listening
        connection, address = sock.accept()  #accept connection
        print "received connection on address {}".format(address)   #print address of connection
        chat(connection, handshake(connection, handle), handle)   #begin chat after handshake
        connection.close()   #close connection
