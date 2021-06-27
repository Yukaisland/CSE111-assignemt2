#!/bin/bash

#######################################################################
#
# Copyright (C) 2020-2021 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

echo ""
echo "CSE111 Assignment $1"
echo ""
date
echo ""
make -s bounds | tee grade.out
echo ""

str=`cat grade.out | grep 'Passed' |  awk '{print $2}'`
pct=`cat grade.out | grep 'Passed:' | grep '\%' | sed 's/\%//' | awk -F "/" '{print $2}' | awk -F " " '{print $2}'`
total=0
if [ -z $pct ]; then
  pct=0
  printf "%30s:  %5s     %5.1f%% of 50%%\n" "Tests" $str "n/a"
else
  total=`echo "scale=2; $pct * 0.5" | bc -l`
  printf "%30s:  %5s     %5.1f%% of 50%%\n" "Tests" $str $total 
fi

lines=`cat grade.out | grep ' lines' |  awk '{print $2}'`
functions=`cat grade.out | grep ' functions' |  awk '{print $2}'`
rm grade.out

ccov=0
cstr="n/a"
if [ $pct = "100.0" ]; then
  cstr="<100%"
  if [ $lines = "100.0%" ]; then
    if [ $functions = "100.0%" ]; then
      ccov=40
      cstr="100%"
    fi
  fi
fi
printf "%30s:  %5s     %5.1f%% of 40%%\n" "Code Coverage" $cstr $ccov

ccode=0
cstr="yes"
valg=0
vstr="n/a"
if [ ! -s make.out ]
then
  (( ccode = 5 ))
  cstr="none"
  > valgrind.out
  flags="--track-origins=yes --leak-check=full --show-leak-kinds=all"
  valgrind $flags ./bounds 1>/dev/null 2>>valgrind.out

  valg=`grep 'ERROR SUMMARY' valgrind.out | grep -v 'ERROR SUMMARY: 0' | \
    awk 'BEGIN {sum=0} {sum += $4} END { print sum }'`

  valw=`grep 'Warning: ' valgrind.out | wc -l`

  (( valg += valw ))
fi
printf "%30s:   %4s     %5.1f%% of  5%%\n" "Compiler Warnings" $cstr $ccode 

errors=0
if (( valg > 0 ))
then
  vstr="$valg"
fi

if (( ccode == 5 && valg == 0 ))
then
  (( errors = 5 ))
  vstr="none"
fi
printf "%30s:   %4s     %5.1f%% of  5%%\n" "Memory Problems" $vstr $errors

total=`echo "scale=2; $total + $ccode + $ccov + $errors" | bc -l`
printf "\n%30s: %5.1f%%\n\n" "Total" $total 

if (( valg > 0 ))
then
  grep 'ERROR SUMMARY' valgrind.out | grep -v 'ERROR SUMMARY: 0'
  echo ""
  echo "See valgrind.out for details"
  echo ""
fi
