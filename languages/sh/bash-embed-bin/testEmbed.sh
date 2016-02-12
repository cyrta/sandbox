
#!/bin/bash

set -eu

trap 'rm -f hello' SIGINT
rm -f hello

uudecode $0  	#	apt-get install sharutils

./hello

rm -f hello
exit
