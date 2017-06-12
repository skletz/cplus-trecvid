#!/bin/bash
# ¯\_(ツ)_/¯
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#	list-too-small-shots.sh
# A script to list master shot files with less than x seconds to update
# ground truths from the TRECVid dataset.
# This script process master shot files with the following filename format.
# <videoID>_<shotID>_<startframe>-<endframe>_<fps>_<width>x<height>.mp4
# Examples can be found in the "./testdata/shots/"
#
# Inputs: 1) Path to the directory which includes all master shot files (max 3 subdirectories)
#         2) Maximal shot length in seconds
# Outputs:  1) CSV file with master shots less than x seconds
#           2) CSV file with master shots with more than x second
#           3) CSV file with all master shots containing a filter for the files with less than x second
# Each line indicate a master shot entry in the following format:
# <path,directory,filename,basename,videoID,shotID,startframe,endframe,fps,width,height,duration,too-small>
#
# Enable globstar option (shopt -s globstar)
#
# @author skletz
# @version 1.0 11/06/17
# -----------------------------------------------------------------------------
# @TODO:
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

USAGE="A script to list master shot files with less than x seconds.
Usage: `basename $0` [-infile] [-outfile] [-srv]
    -h    Shows help
    -i    Directory containing all master shots in max 3 subdirectories
    -s    Maximal shot length in seconds
    -o    Output CSV file for master shots less than x seconds
    -u    Output CSV file for master shots with more than x seconds
    -a    Output CSV file for all founded master shots
 Examples:
    bash `basename $0` -i ../testdata -s 1 -o ../testdata/waste-master-shots.csv -u ../testdata/new-master-shots.csv -a ../testdata/all-master-shots.csv"

INDIR=""
OUTFILE_WASTE=""
OUTFILE_NEW=""
OUTFILE_ALL=""
# max seconds
MAX=

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts i:s:o:u:a:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    i)  INDIR=$OPTARG ;;
    s)  MAX=$OPTARG ;;
    o)  OUTFILE_WASTE=$OPTARG ;;
    u)  OUTFILE_NEW=$OPTARG ;;
    a)  OUTFILE_ALL=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

printf "%-30s %s\n" "input directory :"  "$INDIR"
printf "%-30s %s\n" "max seconds :"  "$MAX"
printf "%-30s %s\n" "output file (smaller than):"   "$OUTFILE_WASTE"
printf "%-30s %s\n" "output file (greater than):"   "$OUTFILE_NEW"
printf "%-30s %s\n" "output file (all) :"    "$OUTFILE_ALL"


function func_getVar {
  videoid=$(echo $1 | cut -d'_' -f1 )
  shotid=$(echo $1 | cut -d'_' -f2 )
  startframe=$(echo $1 | cut -d'_' -f3 | cut -d'-' -f1)
  endframe=$(echo $1 | cut -d'_' -f3 | cut -d'-' -f2)
  fps=$(echo $1 | cut -d'_' -f4 )
  width=$(echo $1 | cut -d'_' -f5 | cut -d'x' -f1 )
  height=$(echo $1 | cut -d'_' -f5 | cut -d'x' -f2 | cut -d'.' -f1)
}

videoid=""
shotid=""
startframe=""
endframe=""
fps=""
width=""
height=""

# enable globstar options (shopt -s globstar)
# iterate over three levels
for file in $INDIR/*.mp4 $INDIR/**/*.mp4 $INDIR/**/**/*.mp4
do
  # if there is no matching file, the pattern will be printed
  # therefore we exclude it
  #if [[ $file != *"/*"* ]]; then #do something; fi

    filename=$(basename "$file")
    #echo "filename=$filename"
    dirname="$(basename "$(dirname "$file")")"
    #echo "dirname=$dirname"

    if echo $filename | grep -Eq '[0-9]+_[0-9]+_[0-9]+-[0-9]+_[0-9]+.[0-9]+_[0-9]+x[0-9]+.mp4'
    then
      echo "found file: $file"
      func_getVar $filename

      length=$(echo $endframe - $startframe | bc -l);
      duration=$(echo $length / $fps | bc -l);

      #debugging
      #echo "videoid: $videoid, shotid: $shotid, startframe: $startframe, endframe: $endframe, fps: $fps, width: $width, height: $height, duration=$duration

      line="$dirname/$filename,$dirname,$filename,${filename%.*},$videoid,$shotid,$startframe,$endframe,$fps,$width,$height"
      filter=''
      if (($(bc <<< "$duration < $MAX")))
      then
        duration=0$(echo "$duration")
        #echo "===> Shot has less than $MAX second (duration)"
        echo $line,$duration >> $OUTFILE_WASTE
        filter='x'
      else
        echo $line,$duration >> $OUTFILE_NEW
        filter=''
      fi #if smaller than max seconds

      echo $line,$duration,$filter >> $OUTFILE_ALL

    fi #if regex pattern matches

done #for each entry in directories
