#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
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
    } else{
        cerr << "Unable to open file: " << filename << endl;
    }
    reverse(prices.begin(),prices.end());
    reverse(dates.begin(),dates.end());
    return prices;
}

void writeOrderStatistics(const vector<Trade>& trades) {
    ofstream file("order_statistics.csv");
    if (file.is_open()) {
        file << "Date,Order_dir,Quantity,Price" << endl;
        for (const auto& trade : trades) {
            file <<formatDate (trade.date) << "," << trade.direction << "," << trade.quantity << "," << to_string(trade.price) << endl;
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
    } 
    else {
        cerr << "Unable to open file: daily_cashflow.csv" << endl;
    }
}

vector<Trade> runStrategy(const vector<double>& prices, int n, int x, double p,string actual_start_date) {
    vector<Trade> trades;
    vector<double> cashflow(prices.size(), 0.0);
    int position = 0;
    double total_=0;
    int n1=prices.size();
    double sum = 0.0000;
    double sq_sum=0.0000;//sum of squares
	// cout<<n<<" "<<x<<" "<<p<<" "<<actual_start_date<<endl;
	
    int i=0;
    while(dates[i]<actual_start_date){
        sum += prices[i];
        sq_sum += prices[i]*prices[i];
        if(i>=n){
            sum -= prices[i-n];
            sq_sum -= prices[i-n]*prices[i-n];
        }
        // cout<<dates[i]<<" "<<prices[i]<<" "<<sum<<" "<<sq_sum<<" "<<position<<"\n";
        i++;
    } 
    start_idx=i;
    // cout<<sum<<" "<<sq_sum<<"\n\n\n";
    while(i<n1){
        sum += prices[i];
        sq_sum += prices[i]*prices[i];
        sum-=prices[i-n];
        sq_sum-=prices[i-n]*prices[i-n];
        
        // Calculate mean and standard deviation of past n days
        double mean = sum / n;
        double stddev = sqrt((sq_sum/n)-(mean*mean));
        
        // cout<<dates[i]<<" "<<mean<<" "<<prices[i]<<" "<<mean+p*stddev<<" "<<sum<<" "<<sq_sum<<" "<<stddev <<" "<<position<<"\n";
        
        // cout<<dates[i]<<" "<<prices[i]<<" "<<sum<<" "<<sq_sum<<" "<<mean<<" "<<stddev<<"\n";
        // Check if the current price is above or below DMA by p standard deviations
        if (prices[i] > mean + p * stddev && position < x){
            trades.push_back({dates[i], "BUY", 1, prices[i]});
            position++;
            cashflow[i] -= prices[i];
            total_ -= prices[i];
        } else if (prices[i] < mean - p * stddev && position > -x) {
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

int main(int argc, char* argv[]) {
    // Read price data from file
    vector<double> prices = readPriceData("DMA_price_data.csv");
    // Define parameters
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <n> <x> <p>" << endl;
        return 1;
    }

    // Convert command-line arguments to integers
    int n = stoi(argv[1]);
    int x = stoi(argv[2]);
    double p=  stod(argv[3]);
    string actual_start_date=argv[4];
    actual_start_date=convertDateFormat(actual_start_date);
    
    // cout<<n<<" "<<x<<" "<<p<<"\n";
    vector<Trade> trades = runStrategy(prices, n, x, p,actual_start_date);
    ofstream finalPnLFile("final_pnl1.txt");
    if (finalPnLFile.is_open()) {
        finalPnLFile<< final_PnL << endl;
        finalPnLFile.close();
    } else {
        cerr << "Unable to open file: final_pnl.txt" << endl;
    }

    // cout<<"\nDone!!!\n";
    return 0;
}
