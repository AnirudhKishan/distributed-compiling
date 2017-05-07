function increment
{
	if [ $1 -eq $2 ]
	then
		echo 0
	else
		echo $(($1+1))
	fi
}
