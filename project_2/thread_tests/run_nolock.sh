rm result_nolock.txt
make clean
make
echo 1 >>result_nolock.txt
./thread_test_measurement >>result_nolock.txt
echo 2 >>result_nolock.txt
./thread_test_measurement >>result_nolock.txt
echo 3 >>result_nolock.txt
./thread_test_measurement >>result_nolock.txt
echo 4 >>result_nolock.txt
./thread_test_measurement >>result_nolock.txt
echo 5 >>result_nolock.txt
./thread_test_measurement >>result_nolock.txt
echo 6 >>result_nolock.txt
./thread_test_measurement >>result_nolock.txt
echo 7 >>result_nolock.txt
./thread_test_measurement >>result_nolock.txt
echo 8 >>result_nolock.txt
./thread_test_measurement >>result_nolock.txt
echo 9 >>result_nolock.txt
./thread_test_measurement >>result_nolock.txt
echo 10 >>result_nolock.txt
./thread_test_measurement >>result_nolock.txt