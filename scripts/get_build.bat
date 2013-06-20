@echo off
set /P old_build=<BUILD
set /A new_build=%old_build%+1
echo %new_build%>BUILD
echo %new_build%