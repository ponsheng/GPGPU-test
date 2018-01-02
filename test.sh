
#set -x
make

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

dataset="1307M 2532M 38M  674M"
#dataset="38M"

mkfifo input
for data in $dataset
do

echo -e "$GREEN$data"
echo -e "$NC"

cat /home/data/OpenCL/input-$data  > input &
./histogram
diff 0656066.out /home/data/OpenCL/ReferenceOutput/output-$data  &> /dev/null
ret=$?


if [ $ret -eq 0 ]
then
    echo -e "$RED PASS"
else
    echo -e "$RED FAIL"
fi

rm input

done
