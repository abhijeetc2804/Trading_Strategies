#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> 
#include<algorithm>
#include<iomanip>
#include<cmath>

using namespace std;

struct Trade {
    string date;
    string direction;
    int quantity;
    double price;
};

vector<string> dates;
vector<double> prices1;
vector<double> prices2; 
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

void readPriceData(const string& filename,int temp){
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
            if(temp==1 || true){
            dates.push_back(token);}
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
    // cout<<dates.size()<< " "<<prices.size()<<"\n";
    // for(int i=0;i<dates.size();i++){
    //     cout<<"("<<dates[i]<<")\n";
    // }
    // cout<<"\n\n";
    if(temp==1){
        prices1=prices;
    }
    else if(temp==2){
        prices2=prices;
    }
}


void writeOrderStatistics2(const vector<Trade>& trades1,const vector<Trade>& trades2){
    ofstream file1("order_statistics_1.csv");
    if (file1.is_open()) {
        file1 << "Date,Order_dir,Quantity,Price" << endl;
        for (const auto& trade : trades1) {
            file1 << formatDate( trade.date) << "," << trade.direction << "," << trade.quantity << "," << to_string(trade.price)<< endl;
        }
        file1.close();
    } else {
        cerr << "Unable to open file: order_statistics_1.csv" << endl;
    }

    ofstream file2("order_statistics_2.csv");
    if (file2.is_open()) {
        file2 << "Date,Order_dir,Quantity,Price" << endl;
        for (const auto& trade : trades2) {
            file2 << formatDate( trade.date) << "," << trade.direction << "," << trade.quantity << "," << to_string(trade.price) << endl;
        }
        file2.close();
    } else {
        cerr << "Unable to open file: order_statistics_2.csv" << endl;
    }
}

void writeDailyCashFlow(const vector<double>& cashflow) {
    ofstream file("daily_cashflow.csv");
    if (file.is_open()) {
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

void runStrategy(int threshold,int  n, int x,string actual_start_date) {
    vector<Trade> trades1,trades2;
    vector<double> cashflow(prices1.size(), 0.0);
    double total_=0;
    int position1=0;//position2 will always be -position1
    int n1=prices1.size();    
    double sum=0;
    double sq_sum=0;
    vector<double> spread;
    for(int i =0 ; i < n1 ; i++){
        spread.push_back(prices1[i]-prices2[i]);
    }
    int i=0;
    while(dates[i]<actual_start_date){
        sum += spread[i];
        sq_sum += spread[i]*spread[i];
        if(i>=n){
            sum -= spread[i-n];
            sq_sum -= spread[i-n]*spread[i-n];
        }
        i++;
    }
    start_idx=i;
    while(i<n1){
        sum += spread[i];
        sq_sum += spread[i]*spread[i];
        sum -= spread[i-n];
        sq_sum -= spread[i-n]*spread[i-n];
        
        double mean=sum/n;
        double stddev=sqrt((sq_sum/n)-(mean*mean));

        double z_score=(spread[i]-mean)/(stddev);
        
        cout<<dates[i]<<" "<<sum<<" "<<sq_sum<<" "<<mean<<" "<<stddev<<" "<<z_score<<"\n";

        if(z_score>threshold && position1>-x){
            trades1.push_back({dates[i], "SELL", 1, prices1[i]});
            cashflow[i] += prices1[i];
            total_ += prices1[i];

            trades2.push_back({dates[i], "BUY", 1, prices2[i]});
            cashflow[i] -= prices2[i];
            total_ -= prices2[i];
            position1--;
        }
        else if(z_score<(-1)*threshold && position1<x){
            trades1.push_back({dates[i], "BUY", 1, prices1[i]});
            cashflow[i] -= prices1[i];
            total_ -=prices1[i];

            trades2.push_back({dates[i], "SELL", 1, prices2[i]});
            cashflow[i] += prices2[i];
            total_ +=prices2[i];
            position1++;
            // cout<<"["<<dates[i]<<"]\n";
        }
        i++;
    }
    //Squaring off on the last trading day
    if (position1 > 0){
        // trades.push_back({dates[n1 - 1], "SELL", position, prices1[n1 - 1]});
        // cashflow[n1 - 1] += position * prices1[n1 - 1];
        final_PnL= total_ + position1 * prices1[n1 - 1] - position1 * prices2[n1 - 1];
        position = 0;
    } else if (position1 < 0) {
        // trades.push_back({dates[n1 - 1], "BUY", -position, prices1[n1 - 1]});
        // cashflow[n1 - 1] -= abs(position) * prices1[n1 - 1];
        final_PnL= total_ - abs(position1) * prices1[n1 - 1] + abs(position1) * prices2[n1 - 1];
        position = 0;
    }
    else{
        final_PnL= total_;
    }
    writeOrderStatistics2(trades1,trades2);
    writeDailyCashFlow(cashflow);
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        cerr << "Incorrect usage" << endl;
        return 1;
    }

    // Convert command-line arguments to integers
    string symbol1= (argv[1]);
    string symbol2= (argv[2]);
    int x= stoi(argv[3]);
    int n= stoi(argv[4]);
    int threshold= stoi(argv[5]);
    string actual_start_date= (argv[6]);
    
    readPriceData("price_data1.csv",1);
    readPriceData("price_data2.csv",2);

    // Run strategy
    runStrategy(threshold, n, x, actual_start_date);

    // Write final profit/loss to file
    ofstream finalPnLFile("final_pnl.txt");
    if (finalPnLFile.is_open()) {
        finalPnLFile<< to_string(final_PnL) << endl;
        finalPnLFile.close();
    } else {
        cerr << "Unable to open file: final_pnl.txt" << endl;
    }
    // cout<<"Done!!!\n";
    return 0;
}


