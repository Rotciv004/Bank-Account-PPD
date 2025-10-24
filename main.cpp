// main.cpp
#include "Bank.h"
#include "Worker.h"

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

using namespace std;

int main(int argc, char** argv) 
{
	// Parametrii impliciti
    size_t n_accounts = 20; // nr de contrui
    long long init_balance = 100000; // sold per cont
	int n_threads = 8; // nr de threaduri
	int ops_per_thread = 200000; // nr de operatii per thread
	int max_amount = 1000; // suma maxima transferata intr-o operatie

    // de schimbat la CoarseGrained pt testare
	Bank::Mode mode = Bank::Mode::FineGrained; // un mutex per cont
	//Bank::Mode mode = Bank::Mode::CoarseGrained; // un singur mutex pt toata banca


    //pt argumente din linia de comanda daca vrei sa modifici parametri impliciti
    // BankAccountPPD 20 8 200000 CoarseGrained
    if (argc >= 2) 
        n_threads = stoi(argv[1]);

    if (argc >= 3) 
        ops_per_thread = stoi(argv[2]);

    if (argc >= 4) 
        n_accounts = static_cast<size_t>(stoul(argv[3]));

    if (argc >= 5) 
    {
        string m(argv[4]);
        mode = (m == "coarse") ? Bank::Mode::CoarseGrained : Bank::Mode::FineGrained;
    }

    cout << "\n";
    cout << "BANK TRANSFER SIMULATION\n";
    cout << "Configuration:\n";
    cout << "- Accounts: " << n_accounts << "\n";
    cout << "- Initial balance per account: " << init_balance << "\n";
    cout << "- Worker threads: " << n_threads << "\n";
    cout << "- Operations per thread: " << ops_per_thread << "\n";
    cout << "- Max transfer amount: " << max_amount << "\n";
    cout << "- Locking mode: " << (mode == Bank::Mode::FineGrained ? "FineGrained" : "CoarseGrained") << "\n\n";

    cout << "main.cpp: [INIT]-> Creating bank with " << n_accounts << " accounts...\n";
    Bank bank(n_accounts, init_balance, mode);
    long long initial_total = bank.total_balance();
    cout << "main.cpp: [INIT]-> Bank initialized. Total balance: " << initial_total << "\n";

    atomic<long long> successful{ 0 }, failed{ 0 };
    vector<thread> threads;
    threads.reserve(n_threads);
	// thread de monitorizare periodica
    atomic<bool> done{ false };
    
    cout << "\nmain.cpp: [MONITOR]-> Starting monitoring thread...\n";
    thread monitor([&]() // pentru monitorizare si pentru a nu avea foaete multe mesaje pe parcurs
        {
        int checks = 0;
        while (!done.load()) 
        {
            this_thread::sleep_for(chrono::seconds(1));
            long long sum = bank.total_balance();
            ++checks;
            
            long long current_successful = successful.load();
            long long current_failed = failed.load();
            long long total_ops = current_successful + current_failed;

            if (sum != initial_total) 
            {
                // cerr pentru mesaje in timp real in caz de eroare
                cerr << "main.cpp: [MONITOR]-> INCONSISTENCY DETECTED! initial=" << initial_total
                    << " now=" << sum << " (difference=" << (sum - initial_total) << ")\n";
            }
            else 
            {
                cout << "main.cpp: [MONITOR]-> Check #" << checks << " - Balance OK: " << sum 
                     << " | Operations: " << total_ops << " (Success: " << current_successful 
                     << ", Failed: " << current_failed << ")\n";
            }
        }
        });

    // pornire temporizator
    cout << "\nmain.cpp: [START]-> Launching " << n_threads << " worker threads...\n";
    auto t0 = chrono::steady_clock::now();

	// lansare threaduri de lucru
    for (int i = 0; i < n_threads; ++i)
    {
        // DEBUGGING POINT
        cout << "\nDEBUGGING POINT: About to create thread #" << i << " ===\n";
        cout << "main.cpp: [DEBUG]-> Current bank balances:\n" << bank.dump_balances() << "\n";
        cout << "main.cpp: [DEBUG]-> Current successful operations: " << successful.load() << "\n";
        cout << "main.cpp: [DEBUG]-> Current failed operations: " << failed.load() << "\n";
        cout << "main.cpp: [DEBUG]-> Press any key to continue to next thread creation...\n";
        
        cin.get();
        
        threads.emplace_back(worker_thread, ref(bank), i, ops_per_thread, max_amount, ref(successful), ref(failed));
        cout << "main.cpp: [START]-> Worker thread #" << i << " launched.\n";
        
        cout << "main.cpp: [DEBUG]-> Thread #" << i << " created successfully.\n";
        cout << "main.cpp: [DEBUG]-> Total threads created so far: " << (i + 1) << "\n\n";
    }

    cout << "main.cpp: [START]-> All worker threads are now running!\n";

	// asteptare threaduri de lucru
    cout << "\nmain.cpp: [WAIT]-> Waiting for all worker threads to complete...\n";
    for (auto& t : threads) 
        t.join();

    cout << "main.cpp: [WAIT]-> All worker threads completed!\n";

	// oprire thread monitorizare
    done = true;
    cout << "main.cpp: [MONITOR]-> Stopping monitoring thread...\n";
	// afisare rezultat final
    monitor.join();
    cout << "main.cpp: [MONITOR]-> Monitoring thread stopped.\n";

    auto t1 = chrono::steady_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();

    cout << "\n\nFINAL RESULTS\n";
    
    long long final_total = bank.total_balance();
    long long total_ops = successful.load() + failed.load();
    double success_rate = (total_ops > 0) ? (100.0 * successful.load() / total_ops) : 0.0;

    cout << "main.cpp: [RESULT]-> Execution time: " << ms << " ms\n";
    cout << "main.cpp: [RESULT]-> Total operations: " << total_ops << "\n";
    cout << "main.cpp: [RESULT]-> Successful transfers: " << successful.load() << " (" << success_rate << "%)\n";
    cout << "main.cpp: [RESULT]-> Failed transfers: " << failed.load() << " (" << (100.0 - success_rate) << "%)\n";
    cout << "main.cpp: [RESULT]-> Throughput: " << (total_ops * 1000.0 / ms) << " operations/second\n";
    cout << "\nmain.cpp: [VERIFICATION]-> Initial total balance: " << initial_total << "\n";
    cout << "main.cpp: [VERIFICATION]-> Final total balance:   " << final_total << "\n";

    if (initial_total == final_total)
    {
        cout << "\nSUCCESS!\n";
        cout << "Total balance invariant preserved.\n";
        cout << "The bank is CONSISTENT!\n";
    }
    else 
    {
        cout << "\nERROR!\n";
        cout << "BUG DETECTED: Total balance changed by " << (final_total - initial_total) << "!\n";
        cout << "The bank is INCONSISTENT!\n";
        cout << "\nAccount balances dump:\n" << bank.dump_balances() << "\n\n";
    }

    return 0;
}
