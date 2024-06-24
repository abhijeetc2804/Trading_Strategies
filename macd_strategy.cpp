#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <queue>
#include <ctime>
#include <iomanip>

using namespace std;

struct Trade {
    string date;
    string direction;
    int quantity;
    double price;
};

string formatDate(const string& input_date_string) {
    tm date_struct = {};
    istringstream input_stream(input_date_string);
    input_stream >> get_time(&date_struct, "%Y-%m-%d");

    ostringstream output_stream;
    output_stream << put_time(&date_struct, "%d/%m/%Y");

    return output_stream.str();
}

vector<string> dates;
int position = 0;
int start_idx;
double final_PnL = 0.0;

vector<double> readPriceData(const string& filename) {
    vector<double> prices;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        getline(file, line); // Skip header line
        while (getline(file, line)) {
            istringstream iss(line);
            string token;
            // Read the date (first column)
            getline(iss, token, ',');
            dates.push_back(token);
            // Skip columns until reaching the closing price column
            for (int i = 0; i < 6; ++i) {
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
    reverse(prices.begin(), prices.end());
    reverse(dates.begin(), dates.end());
    return prices;
}

void writeOrderStatistics(const vector<Trade>& trades) {
    ofstream file("order_statistics.csv");
    if (file.is_open()) {
        file << "Date,Order_dir,Quantity,Price" << endl;
        for (const auto& trade : trades) {
            file << formatDate(trade.date) << "," << trade.direction << "," << trade.quantity << "," << to_string(trade.price) << endl;
        }
        file.close();
    }   
    else{
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
            file << formatDate(dates[i]) << "," << to_string(cumulative_cashflow) << endl;
        }
        final_PnL += cumulative_cashflow;
        file.close();
    } else {
        cerr << "Unable to open file: daily_cashflow.csv" << endl;
    }
}

// Function to calculate Exponential Weighted Mean (EWM)
double calculateEWM(const vector<double>& prices, int period, int currentIndex) {
    double alpha = 2.0 / (period + 1);
    int start_position=currentIndex-period+1;
    double ewm = prices[start_position];
    for (int i = start_position+1; i <= currentIndex; ++i){
        ewm = ewm + alpha * (prices[i] - ewm);
    }
    return ewm;
}

// Function to run MACD strategy
vector<Trade> runMACDStrategy(const vector<double>& prices,int x, string actual_start_date) {
    vector<Trade> trades;
    vector<double> cashflow(prices.size(), 0.0);
    double total_=0;
    int position = 0;
    int n1 = prices.size();
    int i = 0;

    while (dates[i] < actual_start_date) {
        ++i;
    }
    start_idx=i;
    double l_EWM=prices[i];
    double s_EWM=prices[i];
    double alpha_short=2.0/13.0;
    double alpha_long=2.0/27.0;
    double alpha_macd= 2.0/10.0;
    double signal=0.0;
    double macd=0.0;
    while (i < n1){
        s_EWM += alpha_short*(prices[i]-s_EWM);
        l_EWM += alpha_long*(prices[i]-l_EWM);
        macd =s_EWM-l_EWM;
        signal=alpha_macd*(macd-signal)+signal;
        // Buy or sell based on MACD and Signal Line
        if (macd>signal && position<x)  {
            trades.push_back({dates[i], "BUY", 1, prices[i]});
            position++;
            cashflow[i] -= prices[i];
            total_ -= prices[i];
        } else if (macd<signal && position>-x) {
            trades.push_back({dates[i], "SELL", 1, prices[i]});
            position--;
            cashflow[i] += prices[i];
            total_ += prices[i];
        }
        i++;
    }
    //Squaring off on the last trading day
    if (position > 0){
        // trades.push_back({dates[n1 - 1], "SELL", position, prices[n1 - 1]});
        // cashflow[n1 - 1] += position * prices[n1 - 1];
        final_PnL= final_PnL + position * prices[n1 - 1];
        position = 0;
    } else if (position < 0) {
        // trades.push_back({dates[n1 - 1], "BUY", -position, prices[n1 - 1]});
        // cashflow[n1 - 1] -= abs(position) * prices[n1 - 1];
        final_PnL= final_PnL - abs(position) * prices[n1 - 1];
        position = 0;
    }

    // Write order statistics and daily cashflow to files
    writeOrderStatistics(trades);
    writeDailyCashFlow(cashflow);
    return trades;
}

int main(int argc, char* argv[]) {
    // Read price data from file
    vector<double> prices = readPriceData("price_data.csv");

    // Define parameters
    if (argc != 3) {
        cerr << "Incorrect Usage" << endl;
        return 1;
    }
    // Convert command-line arguments to integers
    int x=stoi(argv[1]);
    string actual_start_date = argv[2];
    cout<<actual_start_date<<"\n";
    // Run MACD strategy
    vector<Trade> macdTrades = runMACDStrategy(prices,x, actual_start_date);
    ofstream finalPnLFile("final_pnl.txt");
    if (finalPnLFile.is_open()) {
        finalPnLFile << to_string(final_PnL) << endl;
        finalPnLFile.close();
    } else {
        cerr << "Unable to open file: final_pnl.txt" << endl;
    }

    return 0;
}
