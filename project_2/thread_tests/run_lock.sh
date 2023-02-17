rm result_lock.txt
make clean
make
echo 1 >>result_lock.txt
./thread_test_measurement >>result_lock.txt
echo 2 >>result_lock.txt
./thread_test_measurement >>result_lock.txt
echo 3 >>result_lock.txt
./thread_test_measurement >>result_lock.txt
echo 4 >>result_lock.txt
./thread_test_measurement >>result_lock.txt
echo 5 >>result_lock.txt
./thread_test_measurement >>result_lock.txt
echo 6 >>result_lock.txt
./thread_test_measurement >>result_lock.txt
echo 7 >>result_lock.txt
./thread_test_measurement >>result_lock.txt
echo 8 >>result_lock.txt
./thread_test_measurement >>result_lock.txt
echo 9 >>result_lock.txt
./thread_test_measurement >>result_lock.txt
echo 10 >>result_lock.txt
./thread_test_measurement >>result_lock.txt
