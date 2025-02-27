
# <<< Download and unpack texturepak.zip >>>

# <<< Scale images >>>
convert "metal plate tex.png" -resize x256 metal.png

# <<< Merge images >>>
montage 02camino.png 01tizeta_floor_d.png 01tizeta_floor_f.png 01tizeta_floor_g.png metal.png trak_tile_g.png 04univ2.png 04univ3.png -geometry +0+0 -tile 8x1 large.png

# <<< Do conversion to indexed color >>>
large=large.pcx
convert large.png -colors 256 $large

# <<< Create large textures >>>
convert $large -crop 256x256+0+0 camino2.pcx

convert $large -crop 256x256+256+0 floor_d.pcx
convert $large -crop 256x256+512+0 floor_f.pcx
convert $large -crop 256x256+768+0 floor_g.pcx

convert $large -crop 256x256+1024+0 metal.pcx

convert $large -crop 256x256+1280+0 trak_g.pcx

convert $large -crop 256x256+1536+0 univ42.pcx
convert $large -crop 256x256+1792+0 univ43.pcx

# <<< Generate small textures... >>>

# <<< Scale images >>>
convert 02camino.png -resize x128 s_camino.png

convert 01tizeta_floor_d.png -resize x128 s_floord.png
convert 01tizeta_floor_f.png -resize x128 s_floorf.png
convert 01tizeta_floor_g.png -resize x128 s_floorg.png

convert "metal plate tex.png" -resize x128 s_metal.png

convert trak_tile_g.png -resize x128 s_trakg.png

convert 04univ2.png -resize x128 s_univ2.png
convert 04univ3.png -resize x128 s_univ3.png

# <<< Merge images >>>
montage s_camino.png s_floord.png s_floorf.png s_floorg.png s_metal.png s_trakg.png s_univ2.png s_univ3.png -geometry +0+0 -tile 8x1 tiny.png

# <<< Do conversion to indexed color >>>
tiny=tiny.pcx
convert tiny.png -colors 256 $tiny

# <<< Create small textures >>>
convert $tiny -crop 128x128+0+0 s_camino.pcx

convert $tiny -crop 128x128+128+0 s_floord.pcx
convert $tiny -crop 128x128+256+0 s_floorf.pcx
convert $tiny -crop 128x128+384+0 s_floorg.pcx

convert $tiny -crop 128x128+512+0 s_metal.pcx

convert $tiny -crop 128x128+640+0 s_trakg.pcx

convert $tiny -crop 128x128+768+0 s_univ2.pcx
convert $tiny -crop 128x128+896+0 s_univ3.pcx


# <<< All done >>>
