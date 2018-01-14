# Demo Framework

This comes with two build scripts, one which enables the use of the
c stdlib, and one without it.

## compile.bat
makes the binary really tiny;
it's compressed/self-extracting, so it may have noticable spin-up time.
doesn't include stdlib, so no printf, no malloc, and no fopen
- This expects crinkler.exe to be in an adjacent directory; Crinkler is available at git remote add origin <http://crinkler.net>

## debug_compile.bat
doesn't make the binary tiny;
Has stdlib, so it'll provide useful debug info on why your shaders
don't compile, and which version of opengl you are running

The framework comes packaged with a totally barebones gl function importer, gl_lite.h;
It probably doesn't have what you are looking for, so you'll have to add it.
It's juuuuust enough for demo purposes + simple debugging
