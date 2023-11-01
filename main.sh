 
#!/bin/bash

directory=$1

if [ ! -d "$directory" ]; then
  exit 1
fi
echo $2

function iterate() {
  local dir="$1"

  for file in "$dir"/*; do
    if [ -f "$file" ]; then
    	#value=$(<"$file")
    	res=`cat "$file" | grep -E "^[A-Z][a-zA-Z0-9 ,]*(\.){1}$" | grep -v -E "si[ ]*," | grep -v -E "n[pb]"`
    	if [ -z "$res" ]; then
    		echo "$file">>"$2"
	fi
     
    fi

    if [ -d "$file" ]; then
      iterate "$file"
    fi
  done
}

iterate "$directory"




