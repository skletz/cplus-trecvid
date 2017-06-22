#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	mosh.sh
# A script to move master shot files with less than x seconds.
#
# Inputs: 1) CSV file with master shots less than x seconds
#
# Each line indicate a master shot entry in the following format:
# <srvid,filename,basename,videoID,shotID,startframe,endframe,fps,width,height,duration,too-small>
#
# Enable globstar option (shopt -s globstar)
#
# @author skletz
# @version 1.0 12/06/17
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

USAGE="A script to list master shot files with less than x seconds.
Usage: `basename $0` [-infile] [-outfile] [-srv]
    -h    Shows help
    -i    CSV file of master shots less than x seconds
    -d    Destination Directory ...
    -s    Source Directory ...

 Examples:
    bash `basename $0` -i ../testdata/wasted-master-shots.csv -s ../testdata/shots -d ../testdata/shots-l1s"

INFILE=""
DESTDIR=""
SRCDIR=""

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts i:d:s:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    i)  INFILE=$OPTARG ;;
    d)  DESTDIR=$OPTARG ;;
    s)  SRCDIR=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

printf "%-30s %s\n" "input file :"  "$INFILE"
printf "%-30s %s\n" "source directory :"  "$DESTDIR"
printf "%-30s %s\n" "destination directory :"  "$SRCDIR"


# enable globstar options (shopt -s globstar)
file=$INFILE
filename=$(basename "$file")
#echo "filename=$filename"
dirname="$(basename "$(dirname "$file")")"
#echo "dirname=$dirname"

#iterate throug csv file (msb file)
IFS=","
[ ! -f $file ] && { echo "$file file not found"; exit 99; }
while read srvid filename basename
do
  mv $SRCDIR/$filename $DESTDIR/
done < $file
