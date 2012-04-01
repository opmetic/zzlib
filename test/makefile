vpath=.
cc=cl
link=link

cflags=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "TELNET" /D "GAPING_SECURITY_HOLE" /YX /FD /c 
lflags=kernel32.lib user32.lib wsock32.lib winmm.lib /nologo /subsystem:console /incremental:yes /machine:I386 /out:aa.exe

all: aa.exe

file.obj: file.c
	$(cc) $(cflags) file.c 

printdata.obj: ../printdata/printdata.c
	$(cc) $(cflags) ../printdata/printdata.c

aa.exe: file.obj printdata.obj 
	$(link) file.obj printdata.obj $(lflags)
    
clean:
	rm -f *.ilk *.obj *.exe
