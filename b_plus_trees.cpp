#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
using namespace std;

struct node
{
    vector<long long int> id;
    vector<node *> child;
    vector<long long int> terminal;
    node *sequence;
    bool leaf;
};

class B_plus
{
private:
    // Variables
    node *root;
    vector<long long int> data;
    int n;

    // Methods
    node *createNode()
    {
        node *new_node = new node;
        new_node->sequence = NULL;
        new_node->leaf = true;
        return new_node;
    }

public:
    // Constructor
    B_plus(node *new_root = NULL, int N = 1)
    {
        root = new_root;
        n = N;
    }

    // Split Node
    void split(node *curr, vector<node *> trace, long long int id, long long int data_index, node *child = NULL)
    {
        long long int new_id = 0;  // New ID to be inserted into parent
        int m = (int)floor(n / 2); // Middle Index

        // Insertion Location
        vector<long long int>::iterator upper = upper_bound(curr->id.begin(), curr->id.end(), id);
        int index = (upper - curr->id.begin());

        // New Node after split
        node *split_node = createNode();

        // Insert New Node to find split point
        curr->id.insert(upper, id);
        new_id = curr->id.at(m + 1);

        if (curr->leaf == false || data_index == -1)
        {
            split_node->leaf = false;
            curr->child.insert(curr->child.begin() + index + 1, child);
            ++m; // Incase of Non-Leaf Node (don't include inserted ID)
        }
        else
        {
            curr->terminal.insert(curr->terminal.begin() + index, data_index);
        }

        // Copy ID and Child pointers to split node
        for (int i = m + 1; i < curr->id.size(); ++i)
        {
            split_node->id.push_back(curr->id[i]);
            if (curr->leaf == false || data_index == -1)
            {
                split_node->child.push_back(curr->child[i]);
            }
            else
            {
                split_node->terminal.push_back(curr->terminal[i]);
            }
        }

        if (curr->leaf == false || data_index == -1)
        {
            split_node->child.push_back(curr->child.at(curr->child.size() - 1)); // Pointer at (N + 1)
            curr->child.erase(curr->child.begin() + m + 1, curr->child.end());   // Erase old data from Old Node
            curr->id.erase(curr->id.begin() + m, curr->id.end());
        }
        else
        {
            curr->terminal.erase(curr->terminal.begin() + m + 1, curr->terminal.end()); // Erase old data from Old Node
            split_node->sequence = curr->sequence;                                      // Sequence Pointer for new Node
            curr->sequence = split_node;                                                // Sequence Pointer for old Node
            curr->id.erase(curr->id.begin() + m + 1, curr->id.end());
        }

        // If any effect on immediate parent, carry on same procedure
        if (trace.size() > 0)
        {
            node *parent = trace.at(trace.size() - 1);
            trace.pop_back();
            if (parent->id.size() == n)
            {
                split(parent, trace, new_id, -1, split_node);
            }
            else
            {
                vector<long long int>::iterator parent_upper = upper_bound(parent->id.begin(), parent->id.end(), new_id);
                int parent_index = (parent_upper - parent->id.begin());
                parent->id.insert(parent_upper, new_id);
                if (parent_index < 0 || parent_index >= parent->id.size())
                {
                    parent->child.push_back(split_node);
                }
                else
                {
                    parent->child.insert(parent->child.begin() + parent_index + 1, split_node);
                }
            }
        }
        else
        {
            node *new_root = createNode();
            new_root->leaf = false;
            new_root->id.push_back(new_id);
            new_root->child.push_back(curr);
            new_root->child.push_back(split_node);
            root = new_root;
        }

        return;
    }

    // Insert Operation
    void insert(long long int id)
    {
        node *ptr;
        data.push_back(id);
        long long int data_index = data.size() - 1;

        if (root == NULL)
        {
            ptr = createNode();
            ptr->id.push_back(id);
            ptr->terminal.push_back(data_index);
            root = ptr;
        }
        else
        {
            ptr = root;
            vector<node *> trace;
            vector<long long int>::iterator upper;
            int pos = 0;
            while (ptr->leaf == false)
            {
                trace.push_back(ptr);
                upper = upper_bound(ptr->id.begin(), ptr->id.end(), id);
                pos = (upper - ptr->id.begin());
                ptr = ptr->child[pos];
            }
            // ptr variable will now contain the leaf node, where we need to insert
            // The trace vector will have the pointers to the nodes encountered on the way to the leaf

            if (ptr->id.size() == n)
            {
                split(ptr, trace, id, data_index);
            }
            else
            {
                upper = upper_bound(ptr->id.begin(), ptr->id.end(), id);
                int index = (upper - ptr->id.begin());
                ptr->id.insert(upper, id);
                ptr->terminal.insert(ptr->terminal.begin() + index, data_index);
            }
        }
    }

    // Search/Find Operation
    void find(long long int id)
    {
        bool flag = true;
        if (this->root == NULL)
        {
            flag = false;
        }
        else
        {
            node *ptr = root;
            vector<long long int>::iterator upper;
            int pos = 0;
            while (ptr->leaf == false)
            {
                upper = upper_bound(ptr->id.begin(), ptr->id.end(), id);
                pos = (upper - ptr->id.begin());
                ptr = ptr->child[pos];
            }
            flag = binary_search(ptr->id.begin(), ptr->id.end(), id);
        }

        if (flag)
        {
            cout << "YES" << endl;
        }
        else
        {
            cout << "NO" << endl;
        }
    }

    // Count Operation
    void id_count(long long int id)
    {
        long long int counter = 0;
        long long int current_counter = 0;
        if (this->root == NULL)
        {
            ;
        }
        else
        {
            node *ptr = root;
            vector<long long int>::iterator upper;
            int pos = 0;
            while (ptr->leaf == false)
            {
                upper = upper_bound(ptr->id.begin(), ptr->id.end(), id);
                pos = (upper - ptr->id.begin());
                ptr = ptr->child[pos];
            }

            counter += count(ptr->id.begin(), ptr->id.end(), id);

            ptr = ptr->sequence;
            while(ptr != NULL && ptr->id[0] <= id && ptr->sequence != NULL)
            {
                current_counter = count(ptr->id.begin(), ptr->id.end(), id);
                counter += current_counter;
                ptr = ptr->sequence;
            }

            current_counter = count(data.begin(), data.end(), id);
            if(ptr != NULL && ptr->id[0] <= id)
            {
                counter += count(ptr->id.begin(), ptr->id.end(), id);
            }

            if(counter != current_counter)
            {
                counter = current_counter;
            }
        }
        cout << counter << endl;
    }

    // Range Operation
    void range(long long int x, long long int y)
    {
        long long int count = 0;
        if (this->root == NULL)
        {
            ;
        }
        else
        {
            if (y < x)
            {
                cout << "x < y is required"
                     << "x: " << x << "; y: " << y << endl;
                return;
            }

            node *ptr1 = root;
            node *ptr2 = root;
            vector<long long int>::iterator upper;
            int pos = 0;
            vector<long long int>::iterator x_lower;
            vector<long long int>::iterator y_upper;
            int x_index = 0;
            int y_index = 0;

            // For x
            while (ptr1->leaf == false)
            {
                upper = upper_bound(ptr1->id.begin(), ptr1->id.end(), x);
                pos = (upper - ptr1->id.begin());
                ptr1 = ptr1->child[pos];
            }
            pos = 0;
            // For y
            while (ptr2->leaf == false)
            {
                upper = upper_bound(ptr2->id.begin(), ptr2->id.end(), y);
                pos = (upper - ptr2->id.begin());
                ptr2 = ptr2->child[pos];
            }

            x_lower = lower_bound(ptr1->id.begin(), ptr1->id.end(), x);
            y_upper = upper_bound(ptr2->id.begin(), ptr2->id.end(), y);
            x_index = x_lower - ptr1->id.begin();
            y_index = y_upper - ptr2->id.begin();

            if (ptr1 != ptr2 && (x_index < 0 || x_index == ptr1->id.size()))
            {
                if (ptr1->id.at(ptr1->id.size() - 1) == x)
                {
                    ++count;
                }
                ptr1 = ptr1->sequence;
                x_index = 0;
            }

            if (ptr1 == ptr2)
            {
                if (x_index >= 0 && y_index >= 0)
                {
                    count += y_index - x_index;
                }
                else if ((y_index < 0 || y_index == ptr2->id.size()) and x_index >= 0)
                {
                    count += ptr1->id.size() - x_index;
                }
                else
                {
                    ;
                }
            }
            else
            {
                if (y_index < 0 || y_index == ptr2->id.size())
                {
                    count += ptr2->id.size();
                    y_index = -1;
                }
                else
                {
                    --y_index;
                }
                node *ptr = ptr1;
                count += (ptr->id.size() - x_index);

                ptr = ptr->sequence;
                while (ptr != NULL && ptr != ptr2)
                {
                    count += ptr->id.size();
                    ptr = ptr->sequence;
                }

                for (int i = 0; i <= y_index; ++i)
                {
                    ++count;
                }
            }
        }
        cout << count << endl;
    }

    // Print Leaf Nodes
    void printLeaves()
    {
        node *curr = root;

        if (curr == NULL)
        {
            return;
        }

        while (curr->leaf == false)
        {
            curr = curr->child[0];
        }

        while (true)
        {
            for (int i = 0; i < curr->terminal.size(); ++i)
            {
                cout << data[curr->terminal[i]] << " ";
            }
            cout << endl;
            if (curr->sequence != NULL)
            {
                curr = curr->sequence;
            }
            else
            {
                break;
            }
        }
    }

    // Print IDs in Node
    void printNode(node *curr)
    {
        if (curr == NULL)
        {
            return;
        }

        int N = curr->id.size();
        for (int i = 0; i < N; ++i)
        {
            cout << curr->id[i] << " ";
        }
        cout << endl;
    }
};

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        cout << "USAGE: ./a.out <filename>" << endl;
        exit(1);
    }

    B_plus tree = B_plus(NULL, 3);
    ifstream input(argv[1]);
    char query[7];
    long long int x = 0;
    long long int y = 0;
    string line;

    while (getline(input, line))
    {
        istringstream inp(line);
        inp >> query;

        if (strcmp(query, "QUIT") == 0)
        {
            break;
        }
        else if (strcmp(query, "INSERT") == 0)
        {
            inp >> x;
            tree.insert(x);
        }
        else if (strcmp(query, "FIND") == 0)
        {
            inp >> x;
            tree.find(x);
        }
        else if (strcmp(query, "COUNT") == 0)
        {
            inp >> x;
            tree.id_count(x);
        }
        else if (strcmp(query, "RANGE") == 0)
        {
            inp >> x;
            inp >> y;
            tree.range(x, y);
        }
        else
        {
            cout << "Unknown Query: " << query << endl;
        }
    }

    return 0;
}
