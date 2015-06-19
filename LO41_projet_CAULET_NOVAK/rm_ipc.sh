#/bin/bash

list=$(ipcs -q)
nb=$(echo $list | wc -w)
seuil=14
id=$(echo $list | cut -d ' ' -f $seuil)
i=1
((nb = nb - $seuil ))
((nb = nb / 6 ))
id=$(echo $list | cut -d ' ' -f $seuil)
ipcrm -q $id

while [ $i -le $nb ]
do
	((seuil = seuil + 6 ))
	id=$(echo $list | cut -d ' ' -f $seuil)
	((i = i + 1))
	ipcrm -q $id
done
