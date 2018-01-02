
set -x
make

#dataset="1307M 2532M 38M  674M"
dataset="38M"

for data in $dataset
do
cat /home/data/OpenCL/input-$data  > input &
./histogram
diff 0656066.out /home/data/OpenCL/ReferenceOutput/output-$data
ret=$?

if [ $ret -eq 0 ]
then
    echo "PASS"
fi
done



