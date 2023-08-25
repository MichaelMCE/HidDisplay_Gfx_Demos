@echo off

REM Convert everything from the original .BDF except glpyhs 0 to 31

SET UFARGS=-e +32_65535


call BUILD_fonts.bat

cd bdf
move /Y *.uf "../uf"
