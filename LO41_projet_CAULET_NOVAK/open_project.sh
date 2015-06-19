#/bin/bash
cd ./
list=$(ls ./ | egrep ".c$")
nb=$(echo $list | wc -w)
i=1
# open .c, split .h
while [ $i -le $nb ]
do
   var=$(echo $list | cut -d ' ' -f $i)   #  *.c
   var2=$(echo $var | cut -d '.' -f 1)    #  \{.c}
   var3=$var2".h"
   if [ $var2 != "main"  ]; then
      gnome-terminal --window --maximize --working-directory=$PWD --title="$var2" -x vim "-c split $var3" $var
   fi
   (( i++  ))
done
#open Makefile / main
gnome-terminal --window --maximize --working-directory=$PWD --title="main" -x vim "-c split ./Makefile" ./main.c

#close terminal
#pid=$(ps a | grep "bash$" | cut -c 1-5)
#kill -9 $pid
