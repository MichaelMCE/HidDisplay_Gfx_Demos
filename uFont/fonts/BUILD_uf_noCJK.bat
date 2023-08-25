@echo off

REM Everything apart from the East Asian glyph sets/planes. ie; no Chinese, Japanese, Korean, Viet, etc..

SET UFARGS=-e +32_65535,-11776_65039


call BUILD_fonts.bat


cd bdf
move /Y *.uf "../uf_noCJK"

