#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

BOOL do_connect(char* ip, int port, int connect_timeout)
{
    WSADATA			wsa;
    SOCKET				sock;
    struct sockaddr_in	server_info;
    unsigned long 		sock_mode;
	int					result;
    fd_set 				write_set, err_set;
    TIMEVAL 			timeout;
	
    timeout.tv_sec = 0;
    timeout.tv_usec = connect_timeout;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        MessageBox(NULL, "WSAStartup failed", "", MB_OK);
        return FALSE;
    }

    if((sock = socket(AF_INET , SOCK_STREAM , 0)) == INVALID_SOCKET) {
        MessageBox(NULL, "Could not create socket", "", MB_OK);
        return FALSE;
    }

    server_info.sin_addr.s_addr	= inet_addr(ip);
    server_info.sin_family		= AF_INET;
    server_info.sin_port        = htons(port);

    // Set the socket in non-blocking mode
	sock_mode = 1;
    if (ioctlsocket(sock, FIONBIO, &sock_mode) != 0) {
        MessageBox(NULL, "ioctlsocket failed", "", MB_OK);
		return FALSE;
    }
	    
    if(connect(sock,(struct sockaddr *)&server_info, sizeof(server_info)) != 0) {
        return FALSE;
    }	
	
    // Restore the socket mode
    sock_mode = 0;
    if (ioctlsocket(sock, FIONBIO, &sock_mode) != 0) {
        MessageBox(NULL, "ioctlsocket failed", "", MB_OK);
		return FALSE;
    }
 
    FD_ZERO(&write_set);
    FD_ZERO(&err_set);
    FD_SET(sock, &write_set);
    FD_SET(sock, &err_set);
 
    // Check if the socket is ready
    select(0, NULL, &write_set, &err_set, &timeout);			
    if(FD_ISSET(sock, &write_set)) {
		closesocket(sock);
        return TRUE;
    }
 
    return FALSE;
}


void do_scan()
{
	char*	input_file 	= "scan_input.txt";
	char*	output_file 	= "scan_output.txt";
	FILE*	inf 	= NULL;
	FILE*	outf	= NULL;
	char	ip_address[20];
	char	ip_base[20];
	char	start_byte_str[10];
	char	end_byte_str[10];
	char	port_str[10];
	
	int 	start_byte;
	int 	end_byte;
	int		port;
	int		byte;

	// Open the input file and read the scan parameters
	if ((inf = fopen(input_file, "r")) == NULL) {
		MessageBox(NULL, "Could not open input file", "", MB_OK);
		return;
	}
	
	if(fgets(ip_base, sizeof(ip_base)-1, inf) == NULL) {
		MessageBox(NULL, "Could not read ip_base", "", MB_OK);
		return;	
	}
	if(fgets(start_byte_str, sizeof(start_byte_str)-1, inf) == NULL) {
		MessageBox(NULL, "Could not read start_byte", "", MB_OK);
		return;	
	}
	if(fgets(end_byte_str, sizeof(end_byte_str)-1, inf) == NULL) {
		MessageBox(NULL, "Could not read end_byte", "", MB_OK);
		return;	
	}	
	if (fgets(port_str, sizeof(port_str)-1, inf) == NULL) {
		MessageBox(NULL, "Could not read port", "", MB_OK);
		return;		
	}
	fclose(inf);
	
	// Do some basic checks on input parameters
	if (strlen(ip_base) < 6) {
		MessageBox(NULL, "Invalid ip_base (too short)", "", MB_OK);
		return;			
	}
	if (ip_base[strlen(ip_base) - 2] != '.') {
		MessageBox(NULL, "ip_base must end with '.'", "", MB_OK);
		return;			
	}
	ip_base[strlen(ip_base) - 1] = 0; 	// Remove newline character 
	
	if ((start_byte = atoi(start_byte_str)) == 0) {
		MessageBox(NULL, "Invalid start byte", "", MB_OK);
		return;			
	}
	if ((end_byte = atoi(end_byte_str)) == 0) {
		MessageBox(NULL, "Invalid end byte", "", MB_OK);
		return;			
	}
	if (start_byte > end_byte) {
		MessageBox(NULL, "Start byte is less than end byte", "", MB_OK);
		return;				
	}
	if ((port = atoi(port_str)) == 0) {
		MessageBox(NULL, "Invalid port", "", MB_OK);
		return;			
	}

	// Open the output file
	if ((outf = fopen(output_file, "w")) == NULL) {
		MessageBox(NULL, "Could not open output file", "", MB_OK);
		return;
	}
	
	// Start scanning
	for (byte = start_byte; byte <= end_byte; byte++) {
		sprintf(ip_address, "%s%i", ip_base, byte);

		// Try to connect with a timeout of 50 milliseconds
		if (do_connect(ip_address, port, 50000) == TRUE) {
			fprintf(outf, "%s\tTCP/%i: Open\n", ip_address, port);
		} else {
			fprintf(outf, "%s\tTCP/%i: Closed\n", ip_address, port);
		}
		fflush(outf);
	}
	
	fclose(outf);
}


BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD  fdwReason, LPVOID lpReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
			MessageBox(NULL, "Starting portscan DLL ...", "", MB_OK);
			do_scan();
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}


