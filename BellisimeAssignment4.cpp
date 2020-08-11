`#include<iostream>
#include<string>
#include<sstream> // used for to parse string input
#include<algorithm> // used to transform text to upper case
#include<fstream> // used for file reading

/*Assignment 4: This program reads from a file and interprets the code in the progamming language BORG. This program has 
the following functionality:
START-FINISH blocks
COM - comments
VAR - for variable declartaion and instantiation
operators: ++, --, -, +, *, /, %, ^
PRINT
*/

// Macro for table size
#define TABLE_SIZE 11

using namespace std;

const static int EMPTY = -1;

// Node class: key, data, scope variables to keep track of relevant node.
class Node {
    public:
        Node();
        Node(string key, int data, int scope, Node* next = 0);
        // Getters and setters for variables
        void setData(int data);
        void setNext(Node* temp);
        void setKey(string key);
        int getData();
        Node* getNext();
        string getKey();
        // Class data members
        string key;
        int data, scope;
        Node *next;`
};

// Default constructor
Node::Node() {
    key = "";
    data = 0;
    scope = 0;
    next = 0;
}

// Paramterized constructor
Node::Node(string key, int data, int scope, Node* next) {
    this->key = key;
    this->data = data;
    this->scope = scope;
    this->next = next;
}

// Setter methods
void Node::setData(int data) {
    this->data = data;
}

void Node::setNext(Node* next) {
    this->next = next;
}

void Node::setKey(string key) {
    this->key = key;
}

// Getter methods
int Node::getData() {
    return data;
}

Node* Node::getNext() {
    return next;
}

string Node::getKey() {
    return key;
}

// Hashtable structure - hashtable uses chaining as collision resolution
class HashTable {
    public:
        Node ** table;
        HashTable();
        ~HashTable();
        int hash(string key);
        bool add(const string &key, const int& data, const int &scope);
        int lookup(const string &key, const int &scope);
        void deleteScope(const int &scope);
};

// Default constructor
HashTable::HashTable() {
    table = new Node* [TABLE_SIZE];
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = NULL;
    }
}

// Hash function - takes the ASCII value for each char in the variable name, multiplies it by the index and sums each iteration. returns the value of the hash
int HashTable::hash(string key) {
    int i = 1;
    int hash = 0;
    for (const char &c: key) {
        hash += i * int(c);
        i++;
    }
    return hash % TABLE_SIZE;
}

// add method for the HashTable class
bool HashTable::add(const string& key, const int& data, const int& scope) {
    int index = hash(key);

    // If the node is null then a new node is created and the table[index] is set to that node
    if (table[index] == NULL) {
        table[index] = new Node(key, data, scope);
        return true;
    }
    // Checks the key of the index, then checks the scope, if the scope is less than or equal to the current scope then we update the Node's data member
    else if (table[index]->getKey() == key) {
        if (table[index]->scope <= scope) {
            table[index]->setData(data);
            return true;
        }
        else {
            return false;
        }
    }
    else {
        // Logic to update variable if it is not the first in the chain
        Node* entry = table[index];
        while (entry->next != NULL) {
            if (entry->getKey() == key) {
                if (entry->scope <= scope) {
                    entry->setData(data);
                    return true;
                }
                else {
                    return false;
                }
            }
            entry = entry->next;
        }
        if (entry->getKey() == key) {
            if (entry->scope <= scope) {
                entry->setData(data);
                return true;
            }
        }
        else {
            entry->setNext(new Node(key, data, scope));
            return true;
        }
        return false;
    }
    return false;
}

// lookup searches for value in hashtable, uses key and current scope to determine whether the variable is 'accessible'. returns the value of the key, value pair
int HashTable::lookup(const string& key, const int& scope) {
    int index = hash(key);

    // Lands on index, then checks key. if the key is not the desired key, the chain is looped through until the keys are the same or not found
    Node *entry = table[index];
    if (entry != NULL) {
        while (entry != NULL) {
            if (entry->getKey() == key) {
                if (entry->scope <= scope) 
                    return entry->getData();
            }
            entry = entry->next;
        }
        return -1;
    }
    return -1;
}

// deleteScope loops through the hashtable finding values with the scope of the last code block and nulls all the node(s)
void HashTable::deleteScope(const int &scope) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table[i] != NULL) {
            Node* prev = NULL;
            Node* entry = table[i];
            while (entry != NULL) {
                if (entry->scope == scope) {
                    entry = new Node();
                }
                entry = entry->next;
            }
        }
    }
}

// Destructor for hashtable
HashTable::~HashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if(table[i] != NULL) {
            Node *prev = NULL;
            Node *entry = table[i];
            while(entry != NULL) {
                prev = entry;
                entry = entry->next;
                delete prev;
            }
        }
    } 
    delete [] table;
}

// Function prototype
void parse_string(string &input, string tokens[]);

int main() {
    // Hashtable instantiation
    HashTable table;

    // Main variable declarations
    int scope = 0;
    string inp;
    string tokens[4];

    // File input
    ifstream myFile ("BorgCode.borg");

    // Main loop
    if(myFile.is_open()) {
        while(getline(myFile, inp)) {

            // Nullifying tokens so there are no remainders from previous iterations for the logic below
            for (int i = 0; i < 4; i++) {
                tokens[i] = "";
            }

            // Obtaining tokens from file input
            parse_string(inp, tokens);

            // Converting string to upper case
            if (tokens[1] != "++" && tokens[1] != "--" && tokens[1] != "=") {
                transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::toupper);
            }

            // Checks appropriate token for input
            if (tokens[0] == "START") {

                // Increasing the scope when START is entered
                scope++;
            }
            else if (tokens[0] == "VAR") {

                // If the first token is VAR - create a variable in the hashtable
                string varName = tokens[1];
                int data;

                // Logic for setting variable = variable. 
                // If the token is not a digit, then we look up value in the hashtable
                if (!isdigit(tokens[3][0])) {
                    data = table.lookup(tokens[3], scope);
                    if (data == -1) {
                        cout << tokens[3] << " IS UNDEFINED" << endl;
                        continue;
                    }
                }

                // If the token is a digit then we just add the variable to the hashtable with the add method
                else {
                    data = stoi(tokens[3]);
                }
                table.add(varName, data, scope);
            }

            // Checks for increment operator - checks to see if value exists in hashtable and updates if it does
            else if (tokens[1] == "++") {
                string varName = tokens[0];
                int data = table.lookup(varName, scope) + 1;
                if (data == 0) {
                    cout << varName << " IS UNDEFINED" << endl;
                    continue;
                }
                table.add(varName, data, scope);
            }

            // Checks for decrement operator - checks to see if value exists in hashtable and updates if it does
            else if (tokens[1] == "--") {
                string varName = tokens[0];
                int data = table.lookup(varName, scope) - 1;
                if (data == -2) {
                    cout << varName << " IS UNDEFINED" << endl;
                    continue;
                }
                table.add(varName, data, scope);
            }

            // Updates variable after instantiation
            else if (tokens[1] == "=") {
                if (tokens[3] == "") {
                    string varName = tokens[0];

                    // Logic to check to see if operand after = is variable name
                    if (isdigit(tokens[2][0])) {
                        int data = stoi(tokens[2]);
                        table.add(varName, data, scope);
                    }
                    else {
                        int data = table.lookup(tokens[2], scope);
                        if (data == -1) {
                            table.add(varName, data, scope);
                        }
                        else {
                            cout << tokens[2] << " IS UNDEFINED" << endl;
                        }
                    }
                }
            }

            // Various print functionalities
            else if (tokens[0] == "PRINT") {

                // Variable name for output
                string varName = tokens[1];

                // Stores the value of the variable 
                int returnValue = table.lookup(varName, scope);

                // Variable to store the value of the second operand
                int value;

                // Checks to see if the second operand is a numeric - if it is not then value is updated to the value of the variable (if it)
                if (!isdigit(tokens[3][0])) {
                    value = table.lookup(tokens[3], scope);
                }
                else {
                    value = stoi(tokens[3]);
                }
                // Operator functionality for print statements
                if (returnValue != -1) {
                    if (tokens[2] != "+" && tokens[2] != "-" && tokens[2] != "*" && tokens[2] != "/" && tokens[2] != "%" && tokens[2] != "^") {
                        cout << varName << " IS " << returnValue << endl;
                    }
                    else {
                        if (tokens[2] == "+") {
                            cout << varName << tokens[2] << tokens[3] << " IS " << returnValue + value << endl;
                        }
                        else if (tokens[2] == "-") {
                            cout << varName << tokens[2] << tokens[3] << " IS " << returnValue - value << endl;
                        }
                        else if (tokens[2] == "*") {
                            cout << varName << tokens[2] << tokens[3] << " IS " << returnValue * value << endl;
                        }
                        else if (tokens[2] == "/") {
                            cout << varName << tokens[2] << tokens[3] << " IS " << returnValue / float(value) << endl;
                        }
                        else if (tokens[2] == "%") {
                            cout << varName << tokens[2] << tokens[3] << " IS " << returnValue % value << endl;
                        }
                        else if (tokens[2] == "^") {
                            int yield = 1;
                            for (int i = 0; i < stoi(tokens[3]); i++) {
                                if(stoi(tokens[3]) == 0) {
                                    yield = 1; 
                                    break;
                                }
                                if (i == 1) {
                                    yield = table.lookup(varName, scope);
                                }
                                yield *= table.lookup(varName, scope);
                            }
                            cout << varName << tokens[2] << tokens[3] << " IS " << yield << endl;
                        }
                    }
                }
                else {
                    cout << varName << " IS UNDEFINED" << endl;
                }
            }

            // Decrements scope after finish is entered - this then makes a call to the deleteScope function which nullifies any value initialized in the previous scope
            else if (tokens[0] == "FINISH") {
                table.deleteScope(scope);
                scope--;
                
                // If the scope becomes 0 again the program finishes
                if (scope == 0)
                    break;
            }
        }

        // Closing file  
        myFile.close();
    }
    system("pause");
    return 0;
}

//parse_string takes in a string and an array of strings, splits up the string passed as an argument and places it in the array (in this case the array is limited to a size of 4)
void parse_string(string &input, string tokens[]) {
    stringstream ss(input);
    string buffer;
    int i = 0;

    // Loops through buffer and places all characters before a space in one of the indexes
    while (ss.good() && i < 4) {
        ss >> tokens[i];
        if (tokens[i] == "COM") {
            break;
        }
        i++;
    }
}

/*
OUTPUT FROM BORGCODE.BORG:
BORAMIR IS 25
LEGOLAS IS 101
GANDALF IS UNDEFINED
BORAMIR*2 IS 52
GANDALF IS 49
BORAMIR IS 26
GANDALF IS UNDEFINED
LEGOLAS IS 1000
LEGOLAS IS 1000
LEGOLAS IS 999

OUTPUT FROM TEST.BORG:
X+Y IS 15
Z IS UNDEFINED
Y IS 20
Z%5 IS 0
Z IS 20
I+1414 IS 1415
I IS 0
I IS UNDEFINED
X IS 4
*/
