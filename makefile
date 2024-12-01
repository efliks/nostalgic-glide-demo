
CCOPTS = -2 -fp2 -ml -q -zt=100

GLOBOBJS = &
 bumpmain.obj &
 bumpobj.obj &
 clock.obj &
 low.obj &
 main.obj &
 math3d.obj &
 raster.obj &
 template.obj &
 texture.obj

all: cubedemo.exe

cubedemo.exe: $(GLOBOBJS)
	wlink @cubedemo.lnk

.c.obj:
	wcc $(CCOPTS) -fo=$[*.obj $[*.c

clean: .SYMBOLIC
	del *.obj
	del *.err
	del cubedemo.exe
