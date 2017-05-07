ct=0
while IFS=$' ' read -r -a myArray
do
	ct=0
	command="gcc -c"
	for i in ${myArray[@]}
	do
		if [ $ct -eq 0 ]
			then
			command=$command" -o toSend/$i"
		fi

		if [ $ct -eq 1 ]
			then
			command=$command" src/$i"
		fi

		ct=$((ct+1))
	done
	echo $command
	$command
done < config
