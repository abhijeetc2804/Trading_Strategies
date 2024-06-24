#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> 
#include<algorithm>
#include<iomanip>

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
vector<double> high;
vector<double> low;
vector<double> prev_close;
int position = 0;
double total_=0;
int start_idx;
double alpha;
double final_PnL=0.0;

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

            getline(iss, token, ',');getline(iss, token, ',');//Skipping Series and OPEN

            getline(iss, token, ',');
            high.push_back(stod(token));
            
            getline(iss, token, ',');
            low.push_back(stod(token));

            getline(iss, token, ',');
            prev_close.push_back(stod(token));

            getline(iss, token,','); //Skipping LTP

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
    reverse(high.begin(),high.end());
    reverse(low.begin(),low.end());
    reverse(prev_close.begin(),prev_close.end());

    
    return prices;
}

void writeOrderStatistics(const vector<Trade>& trades){
    ofstream file("order_statistics.csv");
    if (file.is_open()) {
        file << "Date,Order_dir,Quantity,Price" << endl;
        for (const auto& trade : trades) {
            file << formatDate(trade.date) << "," << trade.direction << "," << trade.quantity << "," << to_string(trade.price) << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open file: order_statistics.csv" << endl;
    }
}

void writeDailyCashFlow(const vector<double>& cashflow) {
    ofstream file("daily_cashflow.csv");
    if (file.is_open()){
        file << "Date,Cashflow" << endl;
        double cumulative_cashflow = 0.0;
        for (size_t i = start_idx; i < cashflow.size(); ++i) {
            cumulative_cashflow += cashflow[i];
            file <<formatDate(dates[i])<< "," << to_string(cumulative_cashflow) << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open file: daily_cashflow.csv" << endl;
    }
}

vector<Trade> runStrategy(const vector<double>& prices, int n, int adx_threshold,int x,string actual_start_date) {
    vector<Trade> trades;
    vector<double> cashflow(prices.size(), 0.0);
    int n1=prices.size();
    alpha=double(2.0)/(double(n+1));
    int i=1;
    while(dates[i]<actual_start_date){
        i++;
    }
    start_idx=i;
    double dX;
    double TR=max(max(high[i]-low[i],abs(high[i]-prices[i])),abs(low[i]-prices[i]));
    double ATR=TR;
    double DMp=0.0;
    DMp=max(DMp,(high[i]-high[i-1]));
    double DMn=0.0;
    DMn=max(DMn,(low[i]-low[i-1]));

    double DIp=DMp/ATR;
    double DIn=DMn/ATR;

    double ADX;
    if(DIp!=0 ||DIn!=0){
        dX=(double(DIp-DIn)/double(DIp+DIn))*(double(100));
        ADX=dX;
    }
    else{
        dX=double(0);
        ADX=double(0);
    }
    while(i<n1){
        TR =max(max(high[i]-low[i],abs(high[i]-prices[i])),abs(low[i]-prices[i]));
        ATR += alpha*(TR-ATR);
        DMn=0.0;
        DMn=max(DMn,(low[i]-low[i-1]));
        DMp=0.0;
        DMp=max(DMp,(high[i]-high[i-1]));
        double temp1=DMn/ATR;
        double temp2=DMp/ATR;
        DIn += alpha*(temp1-DIn);
        DIp += alpha*(temp2-DIp);
        if(DIp!=0 || DIn!=0){
            dX=(double(DIp-DIn)/double(DIp+DIn))*(double(100));
        }        
        else{
            dX=0;
        }
        ADX += alpha*(dX-ADX);
        bool check=true;
        if(DIp==double(0) && DIn==double(0)){
            check=false;
        }
        if(check && (position<x) && (ADX>adx_threshold)){
            trades.push_back({dates[i], "BUY", 1, prices[i]});
            cashflow[i] -= prices[i];
            total_ -=prices[i];
            position++;
        }
        else if(check && (position>-x) && (ADX<adx_threshold)){
            trades.push_back({dates[i], "SELL", 1, prices[i]});
            cashflow[i] += prices[i];
            total_ +=prices[i];
            position--;
        }
        i++;
    }       

    //Squaring off on the last trading day
    if (position > 0){
        final_PnL= total_ + position * prices[n1 - 1];
        position = 0;
    } else if (position < 0) {
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
    if (argc != 5) {
        cerr << "Incorrect usage!!\n" << endl;
        return 1;
    }

    // Convert command-line arguments to integers
    int x=stoi(argv[1]);
    int n=stoi(argv[2]);
    int adx_threshold=stoi(argv[3]);
    string actual_start_date=(argv[4]);
    
    // Read price data from file
    vector<double> prices = readPriceData("price_data.csv");
    // cout<<dates.size()<<" "<<prices.size()<<"\n";
    // cout<<dates[0]<<" "<<dates.back()<<"\n";
    // Run strategy
    vector<Trade> trades = runStrategy(prices, n,adx_threshold, x, actual_start_date);
    // cout<<start_idx<<" "<<dates[start_idx]<<"\n";
    // Write final profit/loss to file
    ofstream finalPnLFile("final_pnl.txt");
    if (finalPnLFile.is_open()) {
        finalPnLFile<< to_string (final_PnL) << endl;
        finalPnLFile.close();
    } else {
        cerr << "Unable to open file: final_pnl.txt" << endl;
    }
    cout<<"Done!!!\n";
    return 0;
}
