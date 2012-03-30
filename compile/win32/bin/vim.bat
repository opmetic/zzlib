@echo off
if "%1" == "" goto edit
start gvim -p --remote-tab-silent "%1"
goto :eof

:edit
start gvim
goto :eof

