# creating an icon file from one 1024x1024 image

export d="myicons.iconset"
mkdir "$d"
for i in 16 32 128 256 512; do
  sips -z $i $i --out "$d/icon_${i}x${i}.png" "$1"
  export j=$((2*$i))
  sips -z $j $j --out "$d/icon_${i}x${i}@2x.png" "$1"
done
unset j

iconutil -c icns "$d" && rm -r "$DIR" && echo Ok.
unset d
