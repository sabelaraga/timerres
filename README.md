# timerres
Timer resolution on Intel architectures

This project is based on the Intel's whitepaper "How to Benchmark Code Execution Times on Intel IA-32 and IA-64 Instruction Set Architectures" http://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf

Instead of using kernel space, the test is run in user space. Althought this increases the variability, it is often the case in HPC than the user cannot launch its own kernel modules.
