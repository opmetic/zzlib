vpath=.
cc=cl
link=link

cflags=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "TELNET" /D "GAPING_SECURITY_HOLE" /YX /FD /c 
lflags=kernel32.lib user32.lib wsock32.lib winmm.lib /nologo /subsystem:console /incremental:yes /machine:I386 /out:aa.exe

all: aa.exe

hellohack.obj: hellohack.c
	$(cc) $(cflags) hellohack.c 

getopt.obj: getopt/getopt.c
	$(cc) $(cflags) getopt/getopt.c

aa.exe: hellohack.obj 
	$(link) hellohack.obj $(lflags)
    
clean:
	rm -f *.ilk *.obj *.exe
