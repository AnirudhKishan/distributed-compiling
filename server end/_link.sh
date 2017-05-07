command="gcc -o a.out"
while IFS=$' ' read -r -a myArray
do
	command="$command rcvd/${myArray[0]}"
done < _config

$command
