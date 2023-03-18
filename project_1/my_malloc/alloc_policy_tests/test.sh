make clean
make
echo  "equal size"
./equal_size_allocs
echo  "large"
./large_range_rand_allocs
echo "small"
./small_range_rand_allocs
