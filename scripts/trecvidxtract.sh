#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	trecvidxtract.sh
# A script for extracting content features from the TREVid dataset.
# This script uses the descriptor extraction tool of the vretbox.
#
# @author skletz
# @version 1.0 09/06/17
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


USAGE="A script for extracting content features from the TREVid dataset using vretbox.
Usage: `basename $0` [-infile] [-outfile] [-srv]
    -h    Shows help
    -i    Video-File <only tested with .MP4 files>
    -c    Config-File
    -o    The path to the output directory
    -s    Server-ID <s1,s2 ...> for the identification of the output files on different servers
 Examples:
    bash `basename $0` -i ../testdata/shots/test.mp4 -c ../testdata/config/test.ini -o ../testdata/features -s s1"

INFILE=""
CONFIGFILE=""
OUTFILE=""
SRV=""

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts i:c:o:s:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    i)  INFILE=$OPTARG ;;
    c)  CONFIGFILE=$OPTARG ;;
    o)  OUTFILE=$OPTARG ;;
    s)  SRV=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

printf "%-20s %s\n" "input file :"  "$INFILE"
printf "%-20s %s\n" "config file :"   "$CONFIGFILE"
printf "%-20s %s\n" "output file :"   "$OUTFILE"
printf "%-20s %s\n" "server id :"    "$SRV"

#Default
BIN="builds/linux/bin"
PROG="progvretbox.1.0"

if [ "$(uname)" == "Darwin" ]; then
  echo "Target: Darwin"
  BIN="../builds/mac/bin"
  PROG="progvretbox.1.0"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  echo "Target: Linux"
  BIN="../builds/linux/bin"
  PROG="progvretbox.1.0"
else
  echo "Target: Windows?"
  BIN="../builds/win/bin"
  PROG="vretbox.exe"
fi

IN_NAME=$(basename "$INFILE")
OUT_NAME=$(basename "$OUTFILE")

echo -e "srvid:\t$SRV\t$BIN/$PROG --config $CONFIGFILE -i "$INFILE" $OUTFILE/${IN_NAME%.*}.bin"
$BIN/$PROG --config "$CONFIGFILE" -i "$INFILE" "$OUTFILE/${IN_NAME%.*}.bin"
