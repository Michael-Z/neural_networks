#!/bin/bash

list=`ls *.png`

for i in $list
do
    convert $i -resize 25% "${i}_copy.png"
done
