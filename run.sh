./obj/bandwidth                    >| ./results/bandwidth_resuls.txt
./obj/context                      >| ./results/context_switch_process_results.txt
./obj/context_thread               >| ./results/context_switch_thread_results.txt
./obj/measurement_overhead         >| ./results/measurement.txt
./obj/procedure_call               >| ./results/call.txt
./obj/ram_access_time              >| ./results/ram_acc.txt
./obj/syscall    ./benchUsenix90.ps  >| ./results/syscall.txt
/usr/bin/time ./obj/page_fault ./benchUsenix90.ps  >| ./results/pagefault.txt


