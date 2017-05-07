# compressing files to send

tar -zcvf toSend.tar.gz toSend/

echo -e "Compressed files to send\n"

# compressed files to send

# waiting for connection

nc -z $server $pingPort
status=$(echo $?)
while [[ $status -ne 0 ]]
do
	echo "Waiting for connection ..."	
	sleep $sleepTime

	nc -z $server $pingPort
	status=$(echo $?)
done

echo -e "\nGot connection!\n"

# got connection

sleep $sleepTime

# sending

echo -e "\nSending files ..."

nc $server $basicPort < toSend.tar.gz

echo "Files Sent!"

# sent
