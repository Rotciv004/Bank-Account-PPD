// Bank.cpp
#include "Bank.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <memory>

using namespace std;

// Constructorul de conturi
Bank::Account::Account(int i, long long b)
    : id(i), balance(b), mtx() {}

// Constructor pentru banca
Bank::Bank(size_t n_accounts, long long init_balance, Mode m)
    : mode(m)
{
    accounts.reserve(n_accounts);

    for (size_t i = 0; i < n_accounts; ++i)
        accounts.push_back(make_shared<Account>(static_cast<int>(i), init_balance));
    
    cout << "[BANK] Created " << n_accounts << " accounts with " 
         << (mode == Mode::FineGrained ? "fine-grained" : "coarse-grained") 
         << " locking.\n";
}

// geter pentru numarul de contrui
size_t Bank::size() const 
{
    return accounts.size();
}

// functia care simuleaza un transfer ==> true (a reusit) / false (nu a reusit)
bool Bank::transfer(int from, int to, long long amount) 
{
	// ne asiguram sa nu se transfere bani in acelasi cont
    if (from == to) 
        return false;

	// verificarea modului in care functioneaza banca
    if (mode == Mode::CoarseGrained) 
    {
        // blocam cu mutex global toata tranzactiile din banca
        lock_guard<mutex> g(global_mutex);

        // ne asigram sa nu luam o suma mai mare decat exista in cont
        if (accounts[from]->balance < amount) 
            return false;

        // relaizarea transferului propriu zis
        accounts[from]->balance -= amount;
        accounts[to]->balance += amount;

        return true;
    }
    else 
    {
        // id-ul cel mai mic in a si cel mai mare in b
        int a = min(from, to);
        int b = max(from, to);

		// blocam cele doua conturi implicate in tranzactie, in ordine crescatoare dupa id pt deadlock
        lock(accounts[a]->mtx, accounts[b]->mtx);
        lock_guard<mutex> lock1(accounts[a]->mtx, adopt_lock);
        lock_guard<mutex> lock2(accounts[b]->mtx, adopt_lock);

        // ne asiguram sa nu luam o suma mai mare decat ne permite contul
        if (accounts[from]->balance < amount) 
            return false;

		// realizarea transferului propriu zis
        accounts[from]->balance -= amount;
        accounts[to]->balance += amount;

        return true;
    }
}

// functia care returneaza soldul total al bancii
long long Bank::total_balance() const 
{
	// tinem cont de modul in care functioneaza banca
    if (mode == Mode::CoarseGrained) 
    {
        // mutex global pe toata banca
        lock_guard<mutex> g(global_mutex);
        long long s = 0;

		// calcularea prorpiu zisa a soldului total
        for (const auto& acc : accounts) 
            s += acc->balance;

        return s;
    }
    else 
    {
		// creerea unui vector cu locuri unice pentru mutex-urile fiecarui cont
        vector<unique_lock<mutex>> locks;
        locks.reserve(accounts.size());

        for (size_t i = 0; i < accounts.size(); ++i)
            locks.emplace_back(accounts[i]->mtx);

        long long s = 0;

        for (const auto& acc : accounts) s += acc->balance;

        return s;
    }
}

/*functia care returneaza un string cu soldurile tuturor conturilor
    Pentru DEBUGGING*/
string Bank::dump_balances() const 
{
    ostringstream oss;

    if (mode == Mode::CoarseGrained) 
    {
        lock_guard<mutex> g(global_mutex);

        for (const auto& acc : accounts)
            oss << "[" << acc->id << ":" << acc->balance << "] ";
    }
    else 
    {
        vector<unique_lock<mutex>> locks;
        locks.reserve(accounts.size());

        for (size_t i = 0; i < accounts.size(); ++i)
            locks.emplace_back(accounts[i]->mtx);

        for (const auto& acc : accounts)
            oss << "[" << acc->id << ":" << acc->balance << "] ";
    }

    return oss.str();
}
