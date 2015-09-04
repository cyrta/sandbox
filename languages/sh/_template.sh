#!/usr/bin/env bash


# example from
#  http://bsdpants.blogspot.de/2007/02/option-ize-your-shell-scripts.html


USAGE='usage: '`basename $0`' [-13ds] [-o out-file] filename'
snmpver=2c
sync=0
debug=0


while [[ ${1:0:1} = '-' ]] ; do
  N=1
  L=${#1}

  while [[ $N -lt $L ]] ; do
    case ${1:$N:1} in
       'd') debug=1 ;;
       's') sync=1 ;;
       '1') snmpver=1 ;;
       '3') snmpver=3 ;;
       'o') if [[ $N -ne $(($L-1)) || ! -n ${2} ]] ; then
              echo $USAGE
              exit 1
            fi
            outfile=${2}
            shift ;;
       *) echo $USAGE
          exit 1 ;;
    esac
    N=$(($N+1))
  done
  shift
done

if [[ ! -n ${1} ]] ; then
  echo $USAGE
  exit 1
fi

infile=$1
echo -n "snmpver:$snmpver debug:$debug sync:$sync "
echo "outfile:$outfile infile:$infile"