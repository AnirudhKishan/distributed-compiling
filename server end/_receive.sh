# calculating the number of files that should have been returned

noFiles=0;
while IFS=$' ' read -r -a myArray
do
	noFiles=$((noFiles+1))
done < _config

# calculated the number of files that should have been returned

ctFiles=0	#keeping track of the number of files received vs the number of files that "should" have been returned

# receiving the files

while [ $ctFiles -lt $noFiles ]
do
	echo "Waiting for connection ..."	
	nc -l $pingPort
	echo -e "Got connection!\n"

	echo "Receiving files ..."
	nc -l $basicPort > rcvd.tzr.gz
	echo "Received files ..."

	tar -xzvf rcvd.tzr.gz
	cp toSend/* rcvd/
	echo "Un-Compressed the received files"

	rm -rf toSend rcvd.tzr.gz
	
	ctFiles=$(ls rcvd/ | wc -l)
done

echo -e "\nReceived all the files"

# received the files