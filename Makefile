main:
		cl /nologo /c portscan_dll.c 
		link /nologo /dll /machine:x86 /out:portscan.dll portscan_dll.obj User32.lib Ws2_32.lib
		