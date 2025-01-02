
# <<< Download and unpack texturepak.zip >>>

# <<< Scale images >>>
convert 02camino.png -resize x128 camino2.png

convert 01tizeta_floor_d.png -resize x128 floor_d.png
convert 01tizeta_floor_f.png -resize x128 floor_f.png
convert 01tizeta_floor_g.png -resize x128 floor_g.png

convert "metal plate tex.png" -resize x128 metal.png

convert trak_tile_g.png -resize x128 trak_g.png

convert 04univ2.png -resize x128 univ42.png
convert 04univ3.png -resize x128 univ43.png

# <<< Merge images >>>
montage camino2.png floor_d.png floor_f.png floor_g.png metal.png trak_g.png univ42.png univ43.png -geometry +0+0 -tile 8x1 montage.png

# <<< Convert images to 256 indexed colors >>>
s=mont_idx.pcx
convert montage.png -colors 256 $s

# <<< Create final PCX textures >>>
convert $s -crop 128x128+0+0 camino2.pcx

convert $s -crop 128x128+128+0 floor_d.pcx
convert $s -crop 128x128+256+0 floor_f.pcx
convert $s -crop 128x128+384+0 floor_g.pcx

convert $s -crop 128x128+512+0 metal.pcx

convert $s -crop 128x128+640+0 trak_g.pcx

convert $s -crop 128x128+768+0 univ42.pcx
convert $s -crop 128x128+896+0 univ43.pcx

# <<< Scale images to TGA format >>>
convert 02camino.png -resize x256 camino2.tga

convert 01tizeta_floor_d.png -resize x256 floor_d.tga
convert 01tizeta_floor_f.png -resize x256 floor_f.tga
convert 01tizeta_floor_g.png -resize x256 floor_g.tga

convert "metal plate tex.png" -resize x256 metal.tga

convert trak_tile_g.png -resize x256 trak_g.tga

convert 04univ2.png -resize x256 univ42.tga
convert 04univ3.png -resize x256 univ43.tga

# <<< Now execute texus.bat >>>
