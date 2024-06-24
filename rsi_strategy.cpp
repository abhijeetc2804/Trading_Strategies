#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> 
#include<iomanip>
#include<algorithm>

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

vector<double> readPriceData(const string& filename){
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
    return prices;
}


void writeOrderStatistics(const vector<Trade>& trades){
    ofstream file("order_statistics.csv");
    if (file.is_open()) {
        file << "Date,Order_dir,Quantity,Price" << endl;
        for (const auto& trade : trades) {
            file << formatDate( trade.date) << "," << trade.direction << "," << trade.quantity << "," << to_string(trade.price) << endl;
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

vector<Trade> runStrategy(const vector<double>& prices, int n, int x,int oversold_threshold,int overbought_threshold,string actual_start_date) {
    vector<Trade> trades;
    vector<double> cashflow(prices.size(), 0.0);
    double total_=0;
    int n1=prices.size();
    int i=1;
    double gain=0,loss=0;
    while(dates[i]<actual_start_date){
        gain += max(prices[i]-prices[i-1],(double)0.0);
        loss += max(prices[i-1]-prices[i],(double)0.0);

        if(i>=n+1){
            gain -= max(prices[i-n]-prices[i-n-1],(double)0.0);
            loss -= max(prices[i-n-1]-prices[i-n],(double)0.0);
        }
        i++;
    }
    start_idx=i;
    while(i<n1){
        gain += max(prices[i]-prices[i-1],(double)0.0);
        loss += max(prices[i-1]-prices[i],(double)0.0);
        gain -= max(prices[i-n]-prices[i-n-1],(double)0.0);
        loss -= max(prices[i-n-1]-prices[i-n],(double)0.0);

        double RS=gain/loss;
        double RSI = 100.0 - (100.0/(1+RS));

        // cout<<dates[i]<<" "<<gain<<" "<<loss<<" "<<RS<<" "<<RSI<<"\n";
        if(RSI <oversold_threshold && position<x){
            trades.push_back({dates[i], "BUY", 1, prices[i]});
            cashflow[i] -= prices[i];
            total_ -=prices[i];
            position++;
        }
        else if(RSI > overbought_threshold && position>-x){
            trades.push_back({dates[i], "SELL", 1, prices[i]});
            cashflow[i] += prices[i];
            total_ +=prices[i];
            position--;
        }
        i++;
    }
    // cout<<total_<<" "<<position<<"\n";
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
        final_PnL= total_;
    }
    writeOrderStatistics(trades);
    writeDailyCashFlow(cashflow);
    return trades;
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        cerr << "Usage: " << argv[0] << " <n> <x> <actual_start_date>" << endl;
        return 1;
    }

    // Convert command-line arguments to integers
    int x=stoi(argv[1]);
    int n=stoi(argv[2]);
    int  oversold_threshold=stoi(argv[3]);
    int overbought_threshold=stoi(argv[4]);
    string actual_start_date=(argv[5]);
    
    cout<<x<<" "<<n<<" "<<oversold_threshold<<" "<<overbought_threshold<<" "<<actual_start_date<<"\n";

    // Read price data from file
    vector<double> prices = readPriceData("price_data.csv");

    // Run strategy
    vector<Trade> trades = runStrategy(prices, n, x, oversold_threshold, overbought_threshold, actual_start_date);

    // Write final profit/loss to file
    ofstream finalPnLFile("final_pnl.txt");
    if (finalPnLFile.is_open()) {
        finalPnLFile<< to_string(final_PnL) << endl;
        finalPnLFile.close();
    } else {
        cerr << "Unable to open file: final_pnl.txt" << endl;
    }
    cout<<"Done!!!\n";
    return 0;
}
