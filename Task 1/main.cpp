#include <iostream>
#include <fstream>

#include <sstream>
#include <cstring>




#include <cmath>
using namespace std;

const int TABLE_SIZE = 1500;

// Stock price information
struct StockPrice
{
    string date;
    double open;
    double high;
    double low;
    double close;
    double volume;
    double adj_close;
};

// Stock information
struct Stock
{
    string name;
    string wkn;
    string abbreviation;
    StockPrice prices[30];
};



// Hash table class
class HashTable
{
public:
    HashTable()
    {
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            table_name[i] = NULL;
            table_abbr[i] = NULL;
        }
    }

    ~HashTable()
    {
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            if (table_name[i] != NULL)
            {
                delete table_name[i];
            }
            if (table_abbr[i] != NULL)
            {
                delete table_abbr[i];
            }
        }
    }
// Tables of pointers to stocks, one for name and one for abbreviation
    Stock* table_name[TABLE_SIZE];
    Stock* table_abbr[TABLE_SIZE];

// Hash function to map a string key (name or abbreviation) to an integer index in the table
    int hash_function_name(string key)
    {
        int hash_val = 0;
        for (int i = 0; i < key.length(); i++)
        {
            hash_val = (hash_val * 31 + key[i]) % TABLE_SIZE;
        }
        return hash_val;
    }

    int hash_function_abbr(string key)
    {
        int hash_val = 0;
        for (int i = 0; i < key.length(); i++)
        {
            hash_val = (hash_val * 31 + key[i]) % TABLE_SIZE;
        }
        return hash_val;
    }

// Name table using quadratic probing
    int find_empty_slot_name(string name)
    {
        int index = hash_function_name(name);
        int i = 0;
        int count = 0;
        while (table_name[index] != NULL)
        {
            i++;
            index = (index + i * i) % TABLE_SIZE;
            count++;
            if (count >= TABLE_SIZE)
            {
                cout << "The table is full" << endl;
                return -1;
            }
        }
        return index;
    }

// Abbreviation table using quadratic probing
    int find_empty_slot_abbr(string abbreviation)
    {
        int index = hash_function_abbr(abbreviation);
        int i = 0;
        while (table_abbr[index] != NULL)
        {
            i++;
            index = (index + i * i) % TABLE_SIZE;
        }
        return index;
    }

// Find the index of a stock in the table name
    int find_index_by_name(string name)
    {
        int index = hash_function_name(name);
        int i = 0;
        while (table_name[index] != NULL && table_name[index]->name != name)
        {
            i++;
            index = (index + i * i) % TABLE_SIZE;
        }
        if (table_name[index] == NULL)
        {
            return -1;
        }
        else
        {
            return index;
        }
    }

// Find the index of a stock in the table abbreviation
    int find_index_by_abbr(string abbreviation)
    {
        int index = hash_function_abbr(abbreviation);
        int i = 0;
        while (table_abbr[index] != NULL && table_abbr[index]->abbreviation != abbreviation)
        {
            i++;
            index = (index + i * i) % TABLE_SIZE;
        }
        if (table_abbr[index] == NULL)
        {
            return -1;
        }
        else
        {
            return index;
        }
    }


// Add stock to both tables
    void add_stock(string name, string wkn, string abbreviation)
    {
        Stock* stock = new Stock;
        stock->name = name;
        stock->wkn = wkn;
        stock->abbreviation = abbreviation;
        for (int i = 0; i < 30; i++)
        {
            stock->prices[i].date = "";
            stock->prices[i].open = 0.0;
            stock->prices[i].high = 0.0;
            stock->prices[i].low = 0.0;
            stock->prices[i].close = 0.0;
            stock->prices[i].volume = 0.0;
            stock->prices[i].adj_close = 0.0;
        }
        int index_name = find_empty_slot_name(name);
        int index_abbr = find_empty_slot_abbr(abbreviation);
        table_name[index_name] = stock;
        table_abbr[index_abbr] = stock;

        cout << "Added stock with indexes " << index_name << " and " << index_abbr << "." << endl;
    }




// Delete stock from both tables
    void delete_stock(string name, string abbreviation)
{
    int index_name = find_index_by_name(name);
    int index_abbr = find_index_by_abbr(abbreviation);
    if (index_name == -1 || index_abbr == -1)
    {
        cerr << "Stock not found!" << endl;
        return;
    }
    Stock* stock = table_name[index_name];
    if (stock->name != name || stock->abbreviation != abbreviation)
    {
        cerr << "Stock not found!" << endl;
        return;
    }
    table_name[index_name] = NULL;
    table_abbr[index_abbr] = NULL;
    delete stock;
    cout << "Deleted stock: " << name << " abbreviation:" << abbreviation << endl;
}


    // Import price values for a stock from a CSV file
    void import_stock_prices(string abbreviation, string filename)
    {
        int index = find_index_by_abbr(abbreviation);
        if (index == -1)
        {
            cerr << "Stock not found!" << endl;
            return;
        }
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "File not found!" << endl;
            return;
        }
        string line;
        getline(file, line); // skip header line
        int i = 0;
        while (getline(file, line) && i < 30)
        {
            if (line == "")
            {
                continue;
            }
            StockPrice price;
            stringstream ss(line);
            getline(ss, price.date, ',');
            ss >> price.open;
            ss.ignore(1);
            ss >> price.high;
            ss.ignore(1);
            ss >> price.low;
            ss.ignore(1);
            ss >> price.close;
            ss.ignore(1);
            ss >> price.volume;
            ss.ignore(1);
            ss >> price.adj_close;
            Stock* stock = table_abbr[index];
            stock->prices[i] = price;
            i++;
        }
        file.close();
        cout << "Imported " << i << " prices for " << abbreviation << endl;
    }

// Search for a stock in the table by name
    void search_stock_by_name(string key)
    {
        int index = find_index_by_name(key);
        if (index == -1)
        {
            cerr << "Stock not found!" << endl;
            return;
        }
        Stock* stock = table_name[index];
        StockPrice latest_price = stock->prices[0];
        for (int i = 1; i < 30; i++)
        {
            if (stock->prices[i].date != "")
            {
                latest_price = stock->prices[i];
            }
        }
        cout << "Index\tName\tWKN\tAbbreviation\tDate\t\tOpen\tHigh\tLow\tClose\tVolume\tAdj Close" << endl;
        cout << index << "\t";
        cout << stock->name << "\t";
        cout << stock->wkn << "\t";
        cout << stock->abbreviation << "\t\t";
        cout << latest_price.date << "\t";
        cout << latest_price.open << "\t";
        cout << latest_price.high << "\t";
        cout << latest_price.low << "\t";
        cout << latest_price.close << "\t";
        cout << latest_price.volume << "\t";
        cout << latest_price.adj_close << endl;
    }

// Search for a stock in the table by abbreviation
    void search_stock_by_abbr(string key)
    {
        int index = find_index_by_abbr(key);
        if (index == -1)
        {
            cerr << "Stock not found!" << endl;
            return;
        }
        Stock* stock = table_abbr[index];
        StockPrice latest_price = stock->prices[0];
        for (int i = 1; i < 30; i++)
        {
            if (stock->prices[i].date != "")
            {
                latest_price = stock->prices[i];
            }
        }
        cout << "Index\tName\tWKN\tAbbreviation\tDate\t\tOpen\tHigh\tLow\tClose\tVolume\tAdj Close" << endl;
        cout << index << "\t";
        cout << stock->name << "\t";
        cout << stock->wkn << "\t";
        cout << stock->abbreviation << "\t\t";
        cout << latest_price.date << "\t";
        cout << latest_price.open << "\t";
        cout << latest_price.high << "\t";
        cout << latest_price.low << "\t";
        cout << latest_price.close << "\t";
        cout << latest_price.volume << "\t";
        cout << latest_price.adj_close << endl;
    }
// PRint everything
    void print_all()
    {
        cout << "Printing table_name:" << endl;
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            if (table_name[i] != NULL)
            {
                cout << "Index\tName\tWKN\tAbbreviation\tDate\t\tOpen\tHigh\tLow\tClose\tVolume\tAdj Close" << endl;
                Stock* stock = table_name[i];
                for (int j = 0; j < 30; j++)
                {
                    if (stock->prices[j].date != "")
                    {
                        cout << i << "\t";
                        cout << stock->name << "\t";
                        cout << stock->wkn << "\t";
                        cout << stock->abbreviation << "\t\t";
                        cout << stock->prices[j].date << "\t";
                        cout << stock->prices[j].open << "\t";
                        cout << stock->prices[j].high << "\t";
                        cout << stock->prices[j].low << "\t";
                        cout << stock->prices[j].close << "\t";
                        cout << stock->prices[j].volume << "\t";
                        cout << stock->prices[j].adj_close << endl;
                    }
                }
            }
        }

        cout << "Printing table_abbr:" << endl;
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            if (table_abbr[i] != NULL)
            {
                cout << "Index\tName\tWKN\tAbbreviation\tDate\t\tOpen\tHigh\tLow\tClose\tVolume\tAdj Close" << endl;
                Stock* stock = table_abbr[i];
                for (int j = 0; j < 30; j++)
                {
                    if (stock->prices[j].date != "")
                    {
                        cout << i << "\t";
                        cout << stock->name << "\t";
                        cout << stock->wkn << "\t";
                        cout << stock->abbreviation << "\t\t";
                        cout << stock->prices[j].date << "\t";
                        cout << stock->prices[j].open << "\t";
                        cout << stock->prices[j].high << "\t";
                        cout << stock->prices[j].low << "\t";
                        cout << stock->prices[j].close << "\t";
                        cout << stock->prices[j].volume << "\t";
                        cout << stock->prices[j].adj_close << endl;
                    }
                }
            }
        }
    }

// Plot the table_name: x-axis date, y-axis price
    void plot_last_30_days(Stock* stock)
    {
        cout << "Closing prices for the last 30 days of " << stock->name << ":" << endl;
        cout << endl;

        // Maximum price among the last 30 days
        int max_price = 0;
        for (int i = 0; i < 30; i++)
        {
            if (stock->prices[i].date != "")
            {
                int price = (floor(stock->prices[i].close / 10));
                if (price > max_price)
                {
                    max_price = price;
                }
            }
        }
        // Plot the prices for each row, each row = price range
        for (int i = max_price; i >= 0; i--)
        {
            cout << "   ";
            for (int j = 0; j < 30; j++)
            {
                if (stock->prices[j].date != "")
                {
                    int price = (floor(stock->prices[j].close / 10));
                    if (price >= i)
                    {
                        cout << " * "; // Print an asterisk if the price falls within the range
                    }
                    else
                    {
                        cout << "   ";
                    }
                }
            }
            cout << endl;
        }

        cout << "   ";
        for (int i = 0; i < 30; i++)
        {
            if (stock->prices[i].date != "")
            {
                cout << "---";
            }
        }
        cout << endl;


        cout << endl << endl;
    }

    // Save data in CSV file

    void save_hashtable(string filename, HashTable& hashtable)
{
    ofstream file(filename, ios::out | ios::binary);
    if (!file.is_open())
    {
        cerr << "Unable to open file " << filename << " for writing." << endl;
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (hashtable.table_name[i] != nullptr)
        {
            Stock* stock = hashtable.table_name[i];
            file.write((char*)&stock->name, sizeof(string));
            file.write((char*)&stock->wkn, sizeof(string));
            file.write((char*)&stock->abbreviation, sizeof(string));
            for (int j = 0; j < 30; j++)
            {
                StockPrice price = stock->prices[j];
                if (price.date != "")
                {
                    file.write((char*)&price.date, sizeof(string));
                    file.write((char*)&price.open, sizeof(double));
                    file.write((char*)&price.high, sizeof(double));
                    file.write((char*)&price.low, sizeof(double));
                    file.write((char*)&price.close, sizeof(double));
                    file.write((char*)&price.volume, sizeof(double));
                    file.write((char*)&price.adj_close, sizeof(double));
                }
            }
        }
    }
    file.close();
    cout << "Saved hashtable data to file " << filename << endl;
}

/*// Load hashtable data from a CSV file
   void load_hashtable(string filename, HashTable& hashtable)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Unable to open file " << filename << " for reading." << endl;
            return;
        }

        // Read header row (ignore)
        string line;
        getline(file, line);

        // Read data rows
        while (getline(file, line))
        {
            stringstream ss(line);
            string name, wkn, abbreviation, date;
            double open, high, low, close, volume, adj_close;
            getline(ss, name, ',');
            getline(ss, wkn, ',');
            getline(ss, abbreviation, ',');
            getline(ss, date, ',');
            ss >> open;
            ss.ignore(1);
            ss >> high;
            ss.ignore(1);
            ss >> low;
            ss.ignore(1);
            ss >> close;
            ss.ignore(1);
            ss >> volume;
            ss.ignore(1);
            ss >> adj_close;
            ss.ignore(1);

            // Add the stock to the hashtable
            int index = hashtable.find_empty_slot_name(name);
            cout << "name: " << name << ", index: " << index << endl; // debugg
            if (index == -1)
            {
                cerr << "Hashtable is full, unable to add stock " << name << endl;
            }/*
            else
            {
                Stock *stock = hashtable.table_name[index];
                stock->name = name;
                stock->wkn = wkn;
                stock->abbreviation = abbreviation;
                for (int i = 0; i < 30; i++)
                {
                    if (stock->prices[i].date == "")
                    {
                        stock->prices[i].date = date;
                        stock->prices[i].open = open;
                        stock->prices[i].high = high;
                        stock->prices[i].low = low;
                        stock->prices[i].close = close;
                        stock->prices[i].volume = volume;
                        stock->prices[i].adj_close = adj_close;
                        break;
                    }
                }
            }
        }
        file.close();
        cout << "Loaded hashtable data from file " << filename << endl;
    }
*/



};

int main()
{
    HashTable table;
    bool done = false;
    while (!done)
    {
        cout << "Menu:" << endl;
        cout << "1. Add a stock" << endl;
        cout << "2. Delete a stock" << endl;
        cout << "3. Import stock prices from a CSV file" << endl;
        cout << "4. Search for a stock" << endl;
        cout << "5. Print both tables" << endl;
        cout << "6. SAVE" << endl;
        cout << "7. LOAD" << endl;
        cout << "8. Plot" << endl;
        cout << "9. Exit" << endl;
        int choice;
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            string name, wkn, abbreviation;
            cout << "Enter the name of the stock: ";
            cin >> name;
            cout << "Enter the WKN of the stock: ";
            cin >> wkn;
            cout << "Enter the abbreviation of the stock: ";
            cin >> abbreviation;
            table.add_stock(name, wkn, abbreviation);
            break;
        }
        case 2:
        {
            string key1, key2;
            cout << "Enter the name and abbreviation of the stock to delete!"<< endl;
            cout << "Name: ";
            cin >> key1;
            cout <<"Abbreviation: ";
            cin >> key2;
            table.delete_stock(key1, key2);
            break;
        }
        case 3:
        {
            string abbreviation, filename;
            cout << "Enter the abbreviation of the stock to import prices for: ";
            cin >> abbreviation;
            cout << "Enter the filename of the CSV file to import prices from: ";
            cin >> filename;
            table.import_stock_prices(abbreviation, filename);
            break;
        }
        case 4:
        {
            string key;
            int search_by;
            cout << "Search by (1=abbreviation, 2=name): ";
            cin >> search_by;
            if(search_by == 1)
            {
                cout <<"Enter the abbreviation of the stock: " << endl;
                cin >> key;
                table.search_stock_by_abbr(key);
                break;
            }
            else if(search_by == 2)
            {
                cout <<"Enter the name of the stock: " << endl;
                cin >> key;
                table.search_stock_by_name(key);
                break;            }
            else
            {
                cerr << "Invalid search option!" << endl;
            }
            break;
        }
        case 5:
        {
            table.print_all();
            break;
        }
        case 6:
        {
            string filename;
            cout <<"Enter the name of the file: " << endl;
            cin >> filename;
            table.save_hashtable(filename, table);
            break;
        }
        case 7:
        {
            string filename;
            cout <<"Enter the name of the file: " << endl;
            cin >> filename;
//            table.load_hashtable(filename, table);
            break;
        }
        case 8:
        {
            cout << "Enter the name of the stock to plot: ";
            string name;
            cin >> name;
            int index = table.find_index_by_name(name);
            if (index == -1)
            {
                cerr << "Stock not found!" << endl;
                break;
            }
            else
            {
                table.plot_last_30_days(table.table_name[index]);
                break;
            }
        }
        case 9:
        {
            done = true;
            break;
        }
        default:
        {
            cerr << "Invalid choice!" << endl;
            break;
        }
        }
    }
    return 0;
}
