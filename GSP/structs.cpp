#include <bits/stdc++.h>
using namespace std;

struct Transaction
{
    int date;
    vector<string>items;

    Transaction(){}


    static Transaction readTransaction()
    {
        int nItems;
        Transaction input;
        cout << "Enter the date of the transaction: ";
        cin >> input.date;
        cout << "Enter the number of items purchased: ";
        cin >> nItems;
        for(int i = 0; i < nItems; i++)
        {
            input.items.emplace_back();
            cout << "Enter item #" << i + 1 << ": ";
            cin >> input.items.back();
        }

        sort(input.items.begin(), input.items.end());
        return input;
    }

    bool operator<(const Transaction& other)const{
        return date < other.date;
    }
};

struct Sequence
{
    vector<vector<string>>events;

    string getSequence()
    {
        string sequence = "<";
        for(auto& event : events)
        {
            sequence += "(";
            for(auto& item : event)
                sequence += item + ", ";
            sequence.pop_back(), sequence.pop_back();
            sequence += ")";
        }
        sequence += ">";
        return sequence;
    }

    bool operator==(const Sequence& other)const{
        return events == other.events;
    }

    static Sequence makeSequence(vector<Transaction>& transactions)
    {
        Sequence sequence;
        for(auto& transaction : transactions)
            sequence.events.push_back(transaction.items);
        return sequence;
    }

    static Sequence readSequence()
    {
        cout << "Enter the sequence: ";
        string sequenceString;
        cin >> sequenceString;
        Sequence currentSequence;
        string current;
        for (int i = 1; i < (int)sequenceString.size() - 1; i++) {
            if (sequenceString[i] == '(')
                currentSequence.events.emplace_back();
            else if (sequenceString[i] == ',' || sequenceString[i] == ')')
                currentSequence.events.back().push_back(current), current.clear();
            else
                current += sequenceString[i];
        }
        return currentSequence;
    }
};

struct Itemset
{
    int support = 0;
    Sequence itemset;

    void getSupport(unordered_map<int, Sequence>& sequenceData)
    {
        support = 0;
        for(auto& sequence : sequenceData)
        {
            int curEvent = 0;
            for(auto& event: sequence.second.events)
            {
                int curItem = 0;
                for(auto& item : event)
                {
                    if(item == itemset.events[curEvent][curItem])
                    {
                        curItem++;
                        if(curItem == itemset.events[curEvent].size())
                        {
                            curEvent++;
                            break;
                        }
                    }
                }
                if(curEvent == itemset.events.size())
                {
                    support++;
                    break;
                }
            }
        }
    }

    bool operator==(const Itemset& other)const{
        return itemset == other.itemset;
    }

};

struct GSP
{
    int minSupport;
    unordered_map<int, Sequence>sequenceData;
    map<int, vector<Itemset>>frequentItemsets;

    GSP(int minSupport, const unordered_map<int, Sequence> &sequenceData) : minSupport(minSupport),
                                                                            sequenceData(sequenceData)
    {
        int k = 1;
        while(1)
        {
            if(k == 1)
                generate1Itemsets();
            else if(k == 2 && !frequentItemsets[1].empty())
                generate2Itemsets();
            else if(!frequentItemsets[k - 1].empty())
                generateKItemsets(k, frequentItemsets[k - 1]);
            else
                break;
            k++;
        }
    }

    vector<Itemset>generate(Itemset& A)
    {
        vector<Itemset>possible;
        possible.emplace_back();
        // A = {{ignored}, {event}, {event}}
        // B = {{event}, {event}, {ignored}}
        // joined = {{ignored}, {event}, {event}, {ignored}}

        for(int i = 0; i < A.itemset.events.size(); i++)
        {
            vector<Itemset>newPossible;
            do{
                for(auto& last:possible)
                {
                    newPossible.emplace_back();
                    newPossible.back().itemset = last.itemset;
                    newPossible.back().itemset.events.push_back(A.itemset.events[i]);
                }
            }while(next_permutation(A.itemset.events[i].begin(), A.itemset.events[i].end()));
            possible = newPossible;
        }
        return possible;
    }

    bool canJoin(Itemset& A, Itemset& B)
    {
        vector<Itemset>possibleA = generate(A), possibleB = generate(B);

        for(auto& it : possibleB)
        {
            it.itemset.events.back().pop_back();
            if(it.itemset.events.back().empty())
                it.itemset.events.pop_back();
        }

        for(auto& itemsetA : possibleA)
        {
            itemsetA.itemset.events[0].erase(itemsetA.itemset.events[0].begin());
            if(itemsetA.itemset.events[0].empty())
                itemsetA.itemset.events.erase(itemsetA.itemset.events.begin());

            for(auto& itemsetB : possibleB)
            {
                if(itemsetA == itemsetB)
                    return true;
            }
        }
        return false;
    }

    Itemset join(Itemset& A, Itemset& B)
    {
        Itemset joined = A;
        if(B.itemset.events.back().size() == 1)
            joined.itemset.events.push_back(B.itemset.events.back());
        else
            joined.itemset.events.back() = B.itemset.events.back();
        return joined;
    }

    void generate1Itemsets()
    {
        set<string>distinctItems;
        for(auto& sequence : sequenceData)
            for(auto& event : sequence.second.events)
                for(auto& item : event)
                    distinctItems.insert(item);

        Itemset current;
        for(auto& item : distinctItems)
        {
            current.itemset.events = {{item}};
            current.getSupport(sequenceData);

            if(current.support >= minSupport)
                frequentItemsets[1].push_back(current);
        }
    }

    void generate2Itemsets()
    {
        Itemset current;
        for(auto& item1 : frequentItemsets[1])
        {
            for(auto& item2 : frequentItemsets[1])
            {
                string& a = item1.itemset.events[0][0], b = item2.itemset.events[0][0];
                current.itemset.events = {{a}, {b}};
                current.getSupport(sequenceData);

                if(current.support >= minSupport)
                    frequentItemsets[2].push_back(current);

                if(b > a) // valid for non-temporal
                {
                    current.itemset.events = {{a, b}};
                    current.getSupport(sequenceData);
                    if(current.support >= minSupport)
                        frequentItemsets[2].push_back(current);
                }

            }
        }
    }

    void generateKItemsets(int k, vector<Itemset>& itemsets)
    {
        for(int i = 0; i < itemsets.size(); i++)
        {
            for(int j = 0; j < itemsets.size(); j++)
            {
                if(i == j)
                    continue;

                Itemset current;
                if(canJoin(itemsets[i], itemsets[j]))
                {
                    current = join(itemsets[i], itemsets[j]);
                    current.getSupport(sequenceData);
                    if(current.support >= minSupport)
                        frequentItemsets[k].push_back(current);
                }
            }
        }
    }

    void printFrequentItemsets()
    {
        for(auto& it : frequentItemsets)
        {
            if(it.second.empty())
                continue;

            set<string>distinct;
            cout << "Frequent itemsets of size " << it.first << ":\n";
            for(auto& it2 : it.second)
                distinct.insert(it2.itemset.getSequence());

            for(auto& itemset : distinct)
                cout << itemset << '\n';
            cout << '\n';
        }
    }
};