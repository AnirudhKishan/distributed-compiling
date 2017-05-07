# resetting environment

rm -rf `ls -l | awk '{print $9}' | grep -v _`

mkdir src
mkdir toSend

echo -e "Environment has been reset\n"

# environment reset

# loading parameters

source "_parameters.sh"

# parametes loaded

# waiting for the server to contact me

echo "Waiting for the server to contact"

nc -l $basicPort

echo "Server has contacted"

# server has contacted

# getting the files to compile

startMessage=$(nc -l $basicPort)

while [[ $startMessage == "< start >" ]]
do
	echo "Got < start > message"

	# get number of files for this session
	nf=$(nc -l $basicPort)

	echo -e "Got number of files for this session as: $nf\n"

	for (( i=0; i<$nf; i++ ))
	do
		if [ $i -eq 0 ]
		then
			fn=$(nc -l $basicPort)
			echo -n "$fn " >> config
			echo "Got expected compilation output file as *$fn*"
		else
			fn=$(nc -l $basicPort)
			echo -n "$fn " >> config
			echo "Got dependency filename as *$fn*"
			
			nc -l $basicPort > "src/$fn"
			echo "Got the dependency file"			
		fi
	done

	echo "" >> config	# a newline into the configuration file
	
	echo -e "\n...\n"
	
	startMessage=$(nc -l $basicPort)
done

echo -e "\nGot the < end > message\n"

echo -e "\n---Got all file's I've gotta work with!---\n"

# got all the files to compile

# compiling the files

echo -e "\nCompiling the files ..."

source "_compile.sh"

echo -e "Compiled the files!\n"

# compiles the files

# sending the compiled files

echo "Sending the compiled files..."

source "_send.sh"

echo "Sent the compiled files!"

# sent the compiled files

# sanitising the environment

rm -rf `ls -l | awk '{print $9}' | grep -v _`

# sanitised the environment

# exiting
exit 0
