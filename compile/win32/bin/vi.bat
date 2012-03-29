@echo off
if "%1" == "" goto edit
vim %1
goto :eof

:edit
vim
goto :eof