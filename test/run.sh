for i in */*.vvn
do
  echo run $i
  output="$(cat $i | ../vvn )"
  if [ $? -ne 0 ]
  then
    echo "$output"
    echo "nonzero exit"
  elif [ -e "$i""_expectederrors" ]
  then
    diff "$i""_expectederrors" <(echo "$output" | grep Error)
  else
    echo "$output" | grep Error
  fi
  echo $i done
done
