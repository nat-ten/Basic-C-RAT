# Basic-C-RAT
Very basic Windows RAT written in C.

Requires the victim to have Discord installed.

Hijacks Update.exe found in the Discord folder in AppData for persistence.
After persistence is established, the program attempts to make a TCP connection to a specified IP and PORT.
Once a connection is made, it opens a cmd and pipes standard input, output, and error through the socket connection.
