#!/bin/bash
# Convert interim format to desired JSON format
sed 's/'"'"'/"/g;s/u"/"/g;s/"{/{/g;s/}"/}/g;s/PartialOrder/Pattern/g' $1 > $2
