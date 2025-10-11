// Bank.h
// Clasa Bank si structuri ajutataore

#ifndef BANK_H
#define BANK_H

#include <vector>
#include <mutex>
#include <string>
#include <memory>

using namespace std;

// Clasa de modelare a bancii si a conturilor ei aferente
class Bank 
{
public:
	// Modurile de functionare a bancii ==> granularitate fina / grosiera
    enum class Mode 
    { 
        FineGrained, CoarseGrained 
    };

	// Constructor pentru banca cu n conturi, fiecare cu un sold initial
    Bank(size_t n_accounts, long long init_balance, Mode m);

	// Geter pentru numarul de conturi
    size_t size() const;

	/* Functia care simuleaza un transfer intre doua conturi
        True ==> tranzactie reusita
        False ==> tranzactie nereusita*/
    bool transfer(int from, int to, long long amount);

	// Functia care calculeaza soldul total al bancii
    long long total_balance() const;

	/* Functia care returneaza un string cu soldurile tuturor conturilor
        Ne ajuta la DEBUGGING*/
    string dump_balances() const;

private:

    struct Account 
    {
        int id;
        long long balance;
		mutable mutex mtx; // mutex pentru fiecare conts
        Account(int i, long long b);
    };

	vector<shared_ptr<Account>> accounts; // schimbare la shared_ptr pentru siguranta in threaduri si usurinta in copiere

    Mode mode;
	mutable mutex global_mutex; // mutex global pentru modul grosier CoarseGrained
};

#endif
