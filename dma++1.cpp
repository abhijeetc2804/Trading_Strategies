#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include<queue>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;
string convertDateFormat(const std::string& inputDate) {
    std::stringstream ss(inputDate);
    std::tm tm = {};

    // Parse the input string into a time structure
    ss >> std::get_time(&tm, "%d/%m/%Y");

    // Format the date in "YYYY-MM-DD" format
    std::stringstream formattedDate;
    formattedDate << std::put_time(&tm, "%Y-%m-%d");

    return formattedDate.str();
}
struct Trade {
    string date;
    string direction;
    int quantity;
    double price;
};

vector<string> dates;
int position = 0;
int start_idx;
double final_PnL = 0.0;

string formatDate(const string& input_date_string) {
    tm date_struct = {};
    istringstream input_stream(input_date_string);
    input_stream >> get_time(&date_struct, "%Y-%m-%d");

    ostringstream output_stream;
    output_stream << put_time(&date_struct, "%d/%m/%Y");

    return output_stream.str();
}

vector<double> readPriceData(const string &filename){
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

void writeOrderStatistics(const vector<Trade> &trades) {
    ofstream file("order_statistics.csv");
    if (file.is_open()) {
        file << "Date,Order_dir,Quantity,Price" << endl;
        for (const auto &trade : trades) {
            file << formatDate(trade.date) << "," << trade.direction << "," << trade.quantity << "," << to_string(trade.price) << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open file: order_statistics.csv" << endl;
    }
}

void writeDailyCashFlow(const vector<double> &cashflow) {
    ofstream file("daily_cashflow.csv");
    if (file.is_open()) {
        file << "Date,Cashflow" << endl;
        double cumulative_cashflow = 0.0;
        for (size_t i = 0; i < cashflow.size(); ++i) {
            cumulative_cashflow += cashflow[i];
            file << formatDate(dates[i]) << "," << to_string(cumulative_cashflow) << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open file: daily_cashflow.csv" << endl;
    }
}

vector<Trade> runStrategy(const vector<double> &prices, int n, int x, double p, int maxHoldDays, double c1, double c2, string actual_start_date) {
    vector<Trade> trades;
    vector<double> cashflow(prices.size(), 0.0);
    int position = 0;
    int n1 = prices.size();

    double sum = 0.0;
    double sq_sum = 0.0; // sum of squares
    double abs_prc_change=0.0;

    int i = 1;
    
    while (dates[i] < actual_start_date) {
        sum += prices[i];
        sq_sum += prices[i] * prices[i];
        abs_prc_change += abs(prices[i]-prices[i-1]);
        if (i >= n) {
            sum -= prices[i - n];
            sq_sum -= prices[i - n] * prices[i - n];
        }
        if(i>=n+1){
            abs_prc_change-=abs(prices[i-n]-prices[i-n-1]);
        }
        // cout << dates[i] << " " << prices[i] << " " << sum << " " << sq_sum << " " << position << "\n";
        i++;
    }
    start_idx = i;

    queue<pair<int,char>> upcoming_trade;
    double curr_SF=0.5;
    double curr_AMA=prices[start_idx];
    double total_=0;
    while (i < n1) {
        sum += prices[i];
        sq_sum += prices[i] * prices[i];
        sum -= prices[i - n];
        sq_sum -= prices[i - n] * prices[i - n];
        abs_prc_change += abs(prices[i]-prices[i-1]);
        abs_prc_change-=abs(prices[i-n]-prices[i-n-1]);
        

        char state='X';
        if(abs_prc_change==0.0){
            // cout<<"YOOO\n\n\n";
            // cout<<dates[i]<<"\n";
            int stock_to_sell_buy=0;
            if(!upcoming_trade.empty() && upcoming_trade.front().first==i){
                state = upcoming_trade.front().second;
                upcoming_trade.pop();
                stock_to_sell_buy++;
            }
            if(stock_to_sell_buy!=0){
                if(state=='S'){
                    trades.push_back({dates[i], "SELL", stock_to_sell_buy, prices[i]});
                    position--;
                    cashflow[i] += prices[i];
                    total_ += prices[i];
                }
                else if(state=='B'){
                    trades.push_back({dates[i], "BUY", stock_to_sell_buy, prices[i]});
                    position++;
                    cashflow[i] -= prices[i];
                    total_ -= prices[i];
                }
            }
            i++;
            continue;
        }
        // Calculate efficiency ratio (ER)
        double ER = (prices[i] - prices[i - n]) / (abs_prc_change);
        ER=abs(ER);

        // Calculate smoothing factor (SF)
        if(i>start_idx){
            curr_SF = curr_SF + c1 * ( (((2*ER/(1 + c2)) - 1)/(2 * ER / (1 + c2) + 1)) - curr_SF);
        }
        // Calculate Adaptive Moving Average (AMA)
        if(i>start_idx){
            curr_AMA = curr_AMA + curr_SF * (prices[i] - curr_AMA);
        }
        
        // cout << dates[i]<<" "<<curr_AMA*(1.0 - (p / 100.0))<<" "<<curr_AMA*(1.0 + (p / 100.0))<<" " << prices[i] << " " << sum << " " << sq_sum << " " << abs_prc_change << " " << position << " " << ER << " " << curr_SF << " " << curr_AMA << "\n";
        
        bool buy_through_AMA = (prices[i]>curr_AMA*(1+(p/100)));
        bool sell_through_AMA = (prices[i]<curr_AMA*(1-(p/100)));
        
        bool deadline=false;
        if(!upcoming_trade.empty()){
            if(upcoming_trade.front().first==i){
                deadline=true;
            }
        }
        // cout<<sum<<" "<<sq_sum<<" "<<abs_prc_change<<" "<<deadline<<" "<<buy_through_AMA<<" "<<sell_through_AMA<<"\n";
        if(deadline){
            state=upcoming_trade.front().second;//state=='S' means sell today, state='B' means buy today
            if(state=='S'){
                if(buy_through_AMA){
                    upcoming_trade.pop();
                    upcoming_trade.push({i+maxHoldDays,'S'});
                }
                else if(sell_through_AMA){
                    upcoming_trade.pop();
                    trades.push_back({dates[i], "SELL", 1, prices[i]});
                    position--;
                    cashflow[i] += prices[i];
                    total_ += prices[i];
                }
                else {
                    upcoming_trade.pop();
                    trades.push_back({dates[i], "SELL", 1, prices[i]});
                    position--;
                    cashflow[i] += prices[i];
                    total_ += prices[i];
                }
            }
            else if(state=='B'){
                if(sell_through_AMA){
                    upcoming_trade.pop();
                    upcoming_trade.push({i+maxHoldDays,'B'});
                }
                else if(buy_through_AMA){
                    upcoming_trade.pop();
                    trades.push_back({dates[i], "BUY", 1, prices[i]});
                    position++;
                    cashflow[i] -= prices[i];
                    total_ -= prices[i];
                }
                else{
                    upcoming_trade.pop();
                    trades.push_back({dates[i], "BUY", 1, prices[i]});
                    position++;
                    cashflow[i] -= prices[i];
                    total_ -= prices[i];
                }
            }
        }
        else{
            if(sell_through_AMA && position>-x){
                trades.push_back({dates[i], "SELL", 1, prices[i]});
                position--;
                cashflow[i] += prices[i];
                total_ += prices[i];
                if(upcoming_trade.empty()){
                    upcoming_trade.push({i+maxHoldDays,'B'});
                }
                else if(upcoming_trade.front().second=='B'){
                    upcoming_trade.push({i+maxHoldDays,'B'});
                }
                else{
                    upcoming_trade.pop();
                }
            }
            else if(buy_through_AMA && position<x){
                trades.push_back({dates[i], "BUY", 1, prices[i]});
                position++;
                cashflow[i] -= prices[i];
                total_ -= prices[i];
                if(upcoming_trade.empty()){
                    upcoming_trade.push({i+maxHoldDays,'S'});
                }
                else if(upcoming_trade.front().second=='S'){
                    upcoming_trade.push({i+maxHoldDays,'S'});
                }
                else{
                    upcoming_trade.pop();
                }
            }
        }
        // Implement stop-loss: Close positions after maxHoldDays
        i++;
    }


    // Squaring off on the last trading day
    if (position > 0) {
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
        final_PnL= total_;
    }

    // Write order statistics and daily cashflow to files
    writeOrderStatistics(trades); 
    writeDailyCashFlow(cashflow);
    return trades;
}

int main(int argc, char *argv[]) {
    // Read price data from file
    vector<double> prices = readPriceData("DMA++_price_data.csv");
    
    // Define parameters
    if (argc != 8) {
        cerr << "Usage: " << argv[0] << " <n> <x> <p> <maxHoldDays> <c1> <c2> <actual_start_date>" << endl;
        return 1;
    }

    // Convert command-line arguments to integers
    int x = stoi(argv[1]);
    double p = stoi(argv[2]);
    int n = stod(argv[3]);
    int maxHoldDays = stoi(argv[4]);
    double c1 = stod(argv[5]);
    double c2 = stod(argv[6]);
    string actual_start_date = argv[7];

    // cout << "n: " << n << ", x: " << x << ", p: " << p << ", Max Hold Days: " << maxHoldDays << ", c1: " << c1 << ", c2: " << c2 << ", Actual Start Date: " << actual_start_date << endl;
actual_start_date=convertDateFormat(actual_start_date);
    vector<Trade> trades = runStrategy(prices, n, x, p, maxHoldDays, c1, c2, actual_start_date);

    ofstream finalPnLFile("final_pnl2.txt");
    if(finalPnLFile.is_open()) {
        finalPnLFile << to_string(final_PnL) << endl;
        finalPnLFile.close();
    }
    else{
        cerr << "Unable to open file: final_pnl.txt" << endl;
    }
    cout<<"Done !!!\n";
    return 0;
}
