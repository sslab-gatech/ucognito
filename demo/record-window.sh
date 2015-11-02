#!/bin/bash -x

INFO=$(xwininfo -frame)

WIN_GEO=$(echo $INFO | grep -oEe 'geometry [0-9]+x[0-9]+' | grep -oEe '[0-9]+x[0-9]+')
WIN_XY=$(echo $INFO | grep -oEe 'Corners:\s+\+[0-9]+\+[0-9]+' | grep -oEe '[0-9]+\+[0-9]+' | sed 's/+/,/')
FPS="30"

INRES="$WIN_GEO"
OUTRES="1280x720"

OUTPUT_WIDTH=$(echo $OUTRES | cut -d'x' -f 1)
OUTPUT_HEIGHT=$(echo $OUTRES | cut -d'x' -f 2)

ffmpeg -f x11grab -s "$INRES" -r $FPS -i :0.0+$WIN_XY -f alsa \
-ac 2 -i default -vcodec libx264 -acodec libmp3lame -ab 128k \
-ar 44100 -threads 0 \
-vf "scale=min($OUTPUT_WIDTH/iw\,$OUTPUT_HEIGHT/ih)*iw:-1 , pad=$OUTPUT_WIDTH:$OUTPUT_HEIGHT:max((ow-iw)/2\,0):max((oh-ih)/2\,0)" \
-f flv $1
