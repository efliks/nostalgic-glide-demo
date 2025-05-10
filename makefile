
CCOPTS = -2 -fp2 -ml -q -zt=100 -I.\dummy

GLOBOBJS = &
 bitmap.obj &
 bumpmain.obj &
 bumpobj.obj &
 ptpoly2.obj &
 vga13h.obj &
 sys.obj &
 main.obj &
 math3d.obj &
 raster.obj &
 template.obj &
 texman.obj &
 texture.obj &
 envmap.obj

all: cubedemo.exe

cubedemo.exe: $(GLOBOBJS)
	wlink @cubedemo.lnk

.c.obj:
	wcc $(CCOPTS) -fo=$[*.obj $[*.c

clean: .SYMBOLIC
	del *.obj
	del *.err
	del cubedemo.exe
