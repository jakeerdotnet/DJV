#!/bin/sh

dpiList="32 48 64 96 120 144 168 192 216 240 264 288"

for svg in djvIcon.svg
do
    for dpi in ${dpiList}
    do
        python ../Util/svg-objects-export-master/svg-objects-export.py -f -P "FILE" -d $dpi"DPI/" --extra "--export-dpi $dpi" $svg
    done
done

