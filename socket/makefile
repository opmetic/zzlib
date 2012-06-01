vpath=.
cc=cl
link=link

cflags=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "TELNET" /D "GAPING_SECURITY_HOLE" /YX /FD /c 
lflags=kernel32.lib user32.lib wsock32.lib winmm.lib /nologo /subsystem:console /incremental:yes /machine:I386

all: server.exe client.exe

server.obj: server.c
	$(cc) $(cflags) server.c 

printdata.obj: ../printdata/printdata.c
	$(cc) $(cflags) ../printdata/printdata.c

server.exe: server.obj printdata.obj 
	$(link) server.obj printdata.obj $(lflags) /out:server.exe
    

client.obj: client.c
	$(cc) $(cflags) client.c 

client.exe: client.obj printdata.obj 
	$(link) client.obj printdata.obj $(lflags) /out:client.exe 

clean:
	rm -f *.ilk *.obj *.exe
