@echo off

REM fonts intended for time and readout displays
REM space *-+:,. 0-9  AMP amp

SET UFARGS=-e -1_65535,+32,+42_58,-47,+65,+77,+80,+97,+109,+112


call BUILD_fonts.bat


cd bdf
move /Y *.uf "../uf_Digits"
