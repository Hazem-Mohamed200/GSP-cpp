#include <bits/stdc++.h>
#include "structs.cpp"
using namespace std;

unordered_map<int, Sequence> getSequenceData(unordered_map<int, vector<Transaction>>& customerTransactions)
{
    unordered_map<int, Sequence>sequenceData;
    for(auto& customer : customerTransactions)
    {
        sort(customer.second.begin(), customer.second.end());
        sequenceData[customer.first] = Sequence::makeSequence(customer.second);
    }
    return sequenceData;
}

int main()
{
    cout << "Transactional data(1) or sequential data(2): ";

    int choice;
    cin >> choice;

    unordered_map<int, Sequence>sequenceData;

    if (choice == 1) {
        int nTransactions;
        cout << "Enter the number of transactions: ";
        cin >> nTransactions;

        // customer ID
        unordered_map<int, vector<Transaction>> customerTransactions;

        for(int i = 0, customerID; i < nTransactions; i++)
        {
            cout << "Enter the ID of the customer who made transaction #" << i + 1 << ": ";
            cin >> customerID;
            customerTransactions[customerID].push_back(Transaction::readTransaction());
        }

        sequenceData = getSequenceData(customerTransactions);
    }
    else {
        int nSequences;
        cout << "Enter the number of sequences: ";
        cin >> nSequences;

        for (int i = 0, sequenceID; i < nSequences; i++) {
            cout << "Enter the ID of the sequence #" << i + 1 << ": ";
            cin >> sequenceID;
            sequenceData[sequenceID] = Sequence::readSequence();
        }
    }

    int minSupport;
    cout << "Enter the minimum support: ";
    cin >> minSupport;

    GSP gsp(minSupport, sequenceData);
    gsp.printFrequentItemsets();
}