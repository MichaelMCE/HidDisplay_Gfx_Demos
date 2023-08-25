@echo off

REM Basic Latin fontset
REM 8364/euro

SET UFARGS=-e +32_688,+8364


call BUILD_fonts.bat

cd bdf
move /Y *.uf "../uf_Latin"
