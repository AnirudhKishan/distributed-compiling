# resetting the environment

rm -rf `ls -l | awk '{print $9}' | grep -v _`

mkdir rcvd

# reset the environment

# loading parameters

source "_parameters.sh"

# parametes loaded

# finding the number of clients
# and checking their existence

echo -e "Finding the number of clients and checking their existence\n"

no_hosts=0

while read -r host
do
	arr[$no_hosts]=$host

	nc -z $host $basicPort
	status=$(echo $?)

	if [[ $status -ne 0 ]]
		then
		echo -e "\nThere was a problem contacting $host\nExiting"
		exit 1
	else
		echo "Successfully contacted $host"
	fi
	
	no_hosts=$(($no_hosts+1))
done < _hosts

echo -e "Found the number of hosts as: $no_hosts\n"

# found the number of clients

# sending the files to compile

echo -e "\nSending the files to compile\n"

source "_increment.sh"

sleep $sleepTime

hostCounter=0

while IFS=$' ' read -r -a myArray
do
	echo "Sending < start > message to ${arr[$hostCounter]}"
	echo "< start >" | nc ${arr[$hostCounter]} $basicPort
	sleep $sleepTime

	echo -e "Sending number of files for this session as ${#myArray[@]}\n"
	echo "${#myArray[@]}" | nc ${arr[$hostCounter]} $basicPort
	sleep $sleepTime

	fileCounter=0

	for i in "${myArray[@]}"
	do
		if [ $fileCounter -eq 0 ]
		then
			echo "Sending expected compilation output as *$i*"
			echo "$i" | nc ${arr[$hostCounter]} $basicPort
			sleep $sleepTime
		else
			echo "Sending dependency file name as *$i*"
			echo "$i" | nc ${arr[$hostCounter]} $basicPort
			sleep $sleepTime

			echo "Sending dependency file"
			cat "_src/$i" | nc ${arr[$hostCounter]} $basicPort
			sleep $sleepTime
		fi

		fileCounter=$(( fileCounter + 1 ))		
	done

	hostCounter=$(increment $hostCounter $((no_hosts-1)))
	
	echo -e "\n...\n"
	
done < _config

for (( i=0; i<$no_hosts; i++ ))
do
	echo -e "\nSending <end> message to ${arr[$i]}\n"	# technically, this can be anything but '< start >'
	echo "<end>" | nc ${arr[$i]} $basicPort
done

# sent the files to the clients

# receiving the files

echo -e "\nWaiting the _hosts to compile and return the output ..."

source "_receive.sh"

echo -e "Received the output files\n"

# received the files

# linking the output files

echo -e "\nLinking the ouput files"

source "_link.sh"

echo "Linked the output files\n"

# linked the files

rm -rf rcvd

# exiting
exit 0
