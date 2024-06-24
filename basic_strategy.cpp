#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> 
#include<algorithm>
#include <iomanip>
using namespace std;

struct Trade {
    string date;
    string direction;
    int quantity;
    double price;
};

vector<string> dates;
int position = 0;
int start_idx;
double final_PnL=0.0;

string formatDate(const string& input_date_string) {
    tm date_struct = {};
    istringstream input_stream(input_date_string);
    input_stream >> get_time(&date_struct, "%Y-%m-%d");

    ostringstream output_stream;
    output_stream << put_time(&date_struct, "%d/%m/%Y");

    return output_stream.str();
}

vector<double> readPriceData(const string& filename) {
    vector<double> prices;
    ifstream file(filename);
    if (file.is_open()){
        string line;
        getline(file, line); // Skip header line
        while (getline(file, line)){
            istringstream iss(line);
            string token;
            // Read the date (first column)
            getline(iss, token, ',');
            dates.push_back(token);
            // Skip columns until reaching the closing price column
            for (int i = 0; i < 6; ++i){
                getline(iss, token, ',');
            }
            // Read the closing price (8th column)
            getline(iss, token, ',');
            prices.push_back(stod(token)); // Convert closing price to double and add to vector
        }
        file.close();
    } else {
        cerr << "Unable to open file: " << filename << endl;
    }
    
    reverse(prices.begin(),prices.end());
    reverse(dates.begin(),dates.end());
    // cout << "Number of prices read: " << prices.size() << endl;
    // cout << "Prices: \n";
    // for (int i = 0; i < prices.size(); ++i) {
    //     cout <<dates[i]<<" "<<prices[i] << "\n";
    // }
    // cout << endl;
    return prices;
}


void writeOrderStatistics(const vector<Trade>& trades){
    ofstream file("order_statistics.csv");
    if (file.is_open()) {
        file << "Date,Order_dir,Quantity,Price" << endl;
        for (const auto& trade : trades) {
            file << formatDate(trade.date) << "," << trade.direction << "," << trade.quantity << "," << trade.price << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open file: order_statistics.csv" << endl;
    }
}

void writeDailyCashFlow(const vector<double>& cashflow) {
    ofstream file("daily_cashflow.csv");
    if (file.is_open()) {
        file << "Date,Cashflow" << endl;
        double cumulative_cashflow = 0.0;
        for (size_t i = start_idx; i < cashflow.size(); ++i) {
            cumulative_cashflow += cashflow[i];
            file <<formatDate(dates[i])<< "," << cumulative_cashflow << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open file: daily_cashflow.csv" << endl;
    }
}

vector<Trade> runStrategy(const vector<double>& prices, int n, int x,string actual_start_date) {
    vector<Trade> trades;
    vector<double> cashflow(prices.size(), 0.0);
    double total_=0;
    int n1=prices.size();
    vector<int> state(n1,0);//state[i]=1/-1/0 if BUY/SELL/SKIP on ith day    
    
    // cout<<n<<" "<<x<<" "<<position<<"\n";

    int inc_streak=0,dec_streak=0;
    int i=0;
    // cout<<dates[i]<<" "<<prices[i]<<" "<<state[i]<<" "<<inc_streak<<" "<<dec_streak<<"\n";
    i=1;
    while(dates[i]<actual_start_date){
        // cout<<formatDate(dates[i])<<" \n";
        if(prices[i]>prices[i-1]){
            inc_streak++;
            dec_streak=0;
        }
        else if (prices[i]<prices[i-1]){
            inc_streak=0;
            dec_streak++;
        }
        else{
            inc_streak=0;
            dec_streak=0;
        }
        i++;
    }
    // cout<<(dates[i])<<"\n\n\n";
    start_idx=i;
    
    while(i<n1){
        if(prices[i]>prices[i-1]){
            inc_streak++;
            dec_streak=0;
        }
        else if (prices[i]<prices[i-1]){
            inc_streak=0;
            dec_streak++;
        }
        else{
            inc_streak=0;
            dec_streak=0;
        }
        if(inc_streak>=n && position<x){
            state[i]=1;
            position++;
        }
        else if(dec_streak>=n && position>(-1)*x){
            state[i]=-1;
            position--;
        }
        i++;
    }
    // cout<<position<<"\n";
    for (int i =start_idx; i < n1; ++i){
        if (state[i]==1) {
            trades.push_back({dates[i], "BUY", 1, prices[i]});
            cashflow[i] -= prices[i];
            total_ -=prices[i];
        } else if (state[i]==-1) {
            trades.push_back({dates[i], "SELL", 1, prices[i]});
            cashflow[i] += prices[i];
            total_ +=prices[i];
        }
    }
    
    //Squaring off on the last trading day
    if (position > 0){
        // trades.push_back({dates[n1 - 1], "SELL", position, prices[n1 - 1]});
        // cashflow[n1 - 1] += position * prices[n1 - 1];
        final_PnL= total_ + position * prices[n1 - 1];
        position = 0;
    } else if (position < 0) {
        // trades.push_back({dates[n1 - 1], "BUY", -position, prices[n1 - 1]});
        // cashflow[n1 - 1] -= abs(position) * prices[n1 - 1];
        final_PnL= total_ - abs(position) * prices[n1 - 1];
        position = 0;
    }
    else{
        final_PnL= cashflow[n1 - 1];
    }
    writeOrderStatistics(trades);
    writeDailyCashFlow(cashflow);
    return trades;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <n> <x> <actual_start_date>" << endl;
        return 1;
    }

    // Convert command-line arguments to integers
    int n = stoi(argv[1]);
    int x = stoi(argv[2]);
    string actual_start_date=(argv[3]);
    // cout<<actual_start_date<<"\n\n";
    // Read price data from file
    vector<double> prices = readPriceData("price_data.csv");

    // Run strategy
    vector<Trade> trades = runStrategy(prices, n, x, actual_start_date);

    // Write final profit/loss to file
    ofstream finalPnLFile("final_pnl.txt");
    if (finalPnLFile.is_open()) {
        finalPnLFile<< to_string(final_PnL) << endl;
        finalPnLFile.close();
    } else {
        cerr << "Unable to open file: final_pnl.txt" << endl;
    }

    return 0;
}
