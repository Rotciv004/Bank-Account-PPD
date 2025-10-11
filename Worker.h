// Worker.h
// functia care ruleaza fiecare thread

#ifndef WORKER_H
#define WORKER_H

#include <atomic>
#include "Bank.h"

void worker_thread
(
    Bank& bank,
    int thread_id,
    int ops,
    int max_amount,
    std::atomic<long long>& successful,
    std::atomic<long long>& failed
);

#endif
