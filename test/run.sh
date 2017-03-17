for i in */*.vvn
do
  echo $i
  output="$(cat $i | ../vvn )"
  if [ $? -ne 0 ]
  then
    echo "nonzero exit"
    echo "$output"
  elif [ -e "$i""_expectederrors" ]
  then
    diff "$i""_expectederrors" <(echo "$output" | grep Error)
  else
    echo "$output" | grep Error
  fi
done
