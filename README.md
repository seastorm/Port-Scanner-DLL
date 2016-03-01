# Port-Scanner-DLL
A simple port scanner for Windows implemented as DLL.

# How to run
The port scanner starts when the DLL is loaded into the address space of another process.
This can be done in multiple ways. Basically, any other program which can do LoadLibrary() on portscan.dll is able to run the scanner.

For instance:
* rundll32.exe portscan.dll,***Wait-Before-Pressing-OK***
* Microsoft Excel macro - see the provided example
* Metasploit's windows/loadlibrary payload

# Input file
This port scanner tries to read the file "scan_input.txt" in order to read the scan parameters.
The format of this file must be (line by line):
- First three bytes of the IP address range, ended by a dot.
- Last byte of the start IP address
- End byte of the start IP address
- Port number

For instance, the following input file:
```
192.168.12.
1
254
8080
```
instructs the scanner to scan the IP range 192.168.12.1-254 on port 8080.

The results will be written to the file "scan_output.txt".
