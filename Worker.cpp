// Worker.cpp
#include "Worker.h"

#include <random>
#include <iostream>

using namespace std;

void worker_thread
(
    Bank& bank,
    int thread_id,
    int ops,
    int max_amount,
    atomic<long long>& successful,
    atomic<long long>& failed
)
{
    cout << "[WORKER " << thread_id << "] Starting work - " << ops << " operations to perform.\n";
    
    // seed local reproductibilitate sau perofrmata
    mt19937_64 rng(random_device{}() + thread_id);
    uniform_int_distribution<int> acc_dist(0, static_cast<int>(bank.size()) - 1); // creere distributii aleatoare pentru alegerea conturilor
    uniform_int_distribution<int> amount_dist(1, max_amount); // creere  distributie aleatoare pentru sume

    int local_successful = 0;
    int local_failed = 0;
    int progress_interval = ops / 4; // 25%, 50%, 75%, 100%

    if (progress_interval == 0) 
        progress_interval = ops;

    for (int i = 0; i < ops; ++i) 
    {
        int from = acc_dist(rng);
        int to = acc_dist(rng);

        while (to == from) 
			to = acc_dist(rng); // asiguram ca nu sunt egale

        long long amount = amount_dist(rng);

        if (bank.transfer(from, to, amount))
        {
            ++successful;
            ++local_successful;
        }
        else
        {
            ++failed;
            ++local_failed;
        }

        // Progress report
        if ((i + 1) % progress_interval == 0)
        {
            int percent = ((i + 1) * 100) / ops;
            cout << "[WORKER " << thread_id << "] Progress: " << percent 
                 << "% (" << (i + 1) << "/" << ops << " ops)"
                 << " - Local stats: " << local_successful << " success, " 
                 << local_failed << " failed\n";
        }
    }
    
    cout << "[WORKER " << thread_id << "] Completed! Total: " << local_successful 
         << " successful, " << local_failed << " failed transfers.\n";
}
