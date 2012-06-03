vpath=.
cc=cl
link=link

cflags=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "TELNET" /D "GAPING_SECURITY_HOLE" /YX /FD /c 
lflags=kernel32.lib user32.lib wsock32.lib winmm.lib /nologo /subsystem:console /incremental:yes /machine:I386

all: aa.exe bb.exe

pipe.obj: pipe.c
	$(cc) $(cflags) pipe.c 

printdata.obj: ../printdata/printdata.c
	$(cc) $(cflags) ../printdata/printdata.c

aa.exe: pipe.obj printdata.obj 
	$(link) pipe.obj printdata.obj $(lflags) /out:aa.exe
    

client.obj: client.c
	$(cc) $(cflags) client.c 

bb.exe: client.obj printdata.obj 
	$(link) client.obj printdata.obj $(lflags) /out:bb.exe 

clean:
	rm -f *.ilk *.obj *.exe
