# TCP-IP_low-level_chat
client-server chat system to simultaneously send and receive messages

It is telnet-like terminal chat system, based on Brian "Beej Jorgensen" Hall tutorial.
I have modified it so that another person doesn't have to wait for their turn to send a message.
In original example it worked sort of like a "ping-pong" (send a message and wait for response).

Now users can simultaneously send and receive their messages.
On one terminal line...

If I have ignited your curiosity how it looks like - feel free to try it out:

I have tested it on VirtualBox Ubuntu launching server and client in two separate terminal windows.

First you need to modify line 74 in Client/main.cpp:
    if ( (status = getaddrinfo("dawid-VirtualBox", "3490", &hints, &servinfo)) != 0 )
as "dawid-VirtualBox" please enter your localhost name (shown on top of your terminal window bar)



Reference:
"Beej's Guide to Network Programming":
https://beej.us/guide/bgnet/html/#a-simple-stream-server
https://beej.us/guide/bgnet/html/#a-simple-stream-client
