# Basic-C-RAT
Very basic Windows RAT written in C.

Requires the victim to have Discord installed.

Hijacks Update.exe found in the Discord folder in AppData for persistence.
After persistence is established, the program attempts to make a TCP connection to a specified IP and PORT.
Once a connection is made, it opens a cmd and pipes standard input, output, and error through the socket connection.

Discord was chosen because it installs itself in the user's appdata folder and creates a registry key so that it runs itself on startup.
Because the appdata folder can be read and written to, Update.exe can easily be hijacked by a malicious program to gain persistence and be run everytime on startup.

I got the idea to write this when I read this paper: 
https://github.com/vxunderground/VXUG-Papers/blob/main/The%20Persistence%20Series/Masquerading%20the%20HKCU%20Run%20Key/MasqueradingtheHKCURunKey.pdf
