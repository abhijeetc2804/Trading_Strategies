#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> 
#include<algorithm>
#include<cmath>
#include <ctime> 
#include <queue>

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
    if(temp==1){
        prices1=prices;
    }
    else if(temp==2){
        prices2=prices;
    }
}


void writeOrderStatistics2(const vector<Trade>& trades1,const vector<Trade>& trades2){
    ofstream file1("order_statistics1.csv");
    if (file1.is_open()) {
        file1 << "Date,Direction,Quantity,Price" << endl;
        for (const auto& trade : trades1) {
            file1 << trade.date << "," << trade.direction << "," << trade.quantity << "," << trade.price << endl;
        }
        file1.close();
    } else {
        cerr << "Unable to open file: order_statistics.csv" << endl;
    }

    ofstream file2("order_statistics2.csv");
    if (file2.is_open()) {
        file2 << "Date,Direction,Quantity,Price" << endl;
        for (const auto& trade : trades2) {
            file2 << trade.date << "," << trade.direction << "," << trade.quantity << "," << trade.price << endl;
        }
        file2.close();
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
            file <<dates[i]<< "," << cumulative_cashflow << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open file: daily_cashflow.csv" << endl;
    }
}

void runStrategy(int threshold,int stop_loss_threshold,int  n, int x,string actual_start_date) {
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
        // cout<<dates[i]<<" "<<spread[i]<<" "<<sum<<" "<<sq_sum<<"\n";
        i++;
    }
    queue<vector<double>>q1;
    start_idx=i;
    while(i<n1){
        sum += spread[i];
        sq_sum += spread[i]*spread[i];
        sum -= spread[i-n];
        sq_sum -= spread[i-n]*spread[i-n];
        
        double mean=sum/n;
        double stddev=sqrt((sq_sum/n)-(mean*mean));

        double z_score=(spread[i]-mean)/(stddev);
        
       // cout<<dates[i]<<" "<<sum<<" "<<sq_sum<<" "<<mean<<" "<<stddev<<" "<<z_score<<"\n";

        if(z_score>threshold){
            int pos=0;
            vector<double>v={mean,stddev,-1,double(i)};
            q1.push(v);
           // trades1.push_back({dates[i], "SELL", 1, prices1[i]});
            // cout<<pos<<" "<<total_<<" "<<prices1[i]<<" " <<prices2[i]<<" "<<i<<endl;
            pos-=1;
            cashflow[i] += prices1[i];
            total_ += prices1[i];

            //trades2.push_back({dates[i], "BUY", 1, prices1[i]});
            cashflow[i] -= prices2[i];
            total_ -= prices2[i];

            
            queue<vector<double>>q2;
            while (!q1.empty()){
            	vector<double>v(4,0.0);
            	v[0]=q1.front()[0];
            	v[1]=q1.front()[1];
            	v[2]=q1.front()[2];
            	v[3]=q1.front()[3];
            	q1.pop();
            	double z_past=(spread[i]-v[0])/(v[1]);
            	if (z_past>stop_loss_threshold || z_past<-stop_loss_threshold){
            		if (v[2]>0){
            			//trades1.push_back({dates[v[3]], "SELL", 1, prices1[i]});
            			// cout<<pos<<" "<<total_<<" "<<prices1[i]<<" " <<prices2[i]<<"hi "<<i<<" "<<v[2]<<endl;
            			pos-=1;
            			cashflow[i] += prices1[i];
            			total_ += prices1[i];

            			//trades2.push_back({dates[v[3]], "BUY", 1, prices1[i]});
            			cashflow[i] -= prices2[i];
            			total_ -= prices2[i];
            		}
            		else{
            			//trades1.push_back({dates[v[3]], "BUY", 1, prices1[i]});
            			// cout<<pos<<" "<<total_<<" "<<prices1[i]<<" " <<prices2[i]<<"hi "<<i<<" "<<v[2]<<endl;
            			pos+=1;
            			cashflow[i] -= prices1[i];
            			total_ -= prices1[i];

            			//trades2.push_back({dates[v[3]], "SELL", 1, prices1[i]});
            			cashflow[i] += prices2[i];
            			total_ += prices2[i];
            		
            		}
            	
            	}
            	else{
            		
            		q2.push(v);
            	
            	}
            	
            }
            while (!q2.empty()){
            	vector<double>v(4,0.0);
            	v=q2.front();
            	q2.pop();
            	q1.push(v);
            }
            if (pos>0 && position1>-x){
            trades1.push_back({dates[i], "SELL", pos, prices1[i]});
            trades2.push_back({dates[i], "BUY", pos, prices1[i]});
            position1-=pos;
            }
            else if(pos<0 && position1<x){
            	trades1.push_back({dates[i], "BUY", -pos, prices1[i]});
            trades2.push_back({dates[i], "SELL", -pos, prices1[i]});
            position1+=-pos;
            
            }
        }
        else if(z_score<(-1)*threshold ){
           int pos=0;
            vector<double>v={mean,stddev,1,double(i)};
            q1.push(v);
           // trades1.push_back({dates[i], "SELL", 1, prices1[i]});
            pos+=1;
            cashflow[i] -= prices1[i];
            total_ -= prices1[i];

            //trades2.push_back({dates[i], "BUY", 1, prices1[i]});
            cashflow[i] += prices2[i];
            total_ += prices2[i];

            
            queue<vector<double>>q2;
            while (!q1.empty()){
            	vector<double>v(4,0.0);
            	v[0]=q1.front()[0];
            	v[1]=q1.front()[1];
            	v[2]=q1.front()[2];
            	v[3]=q1.front()[3];
            	q1.pop();
            	double z_past=(spread[i]-v[0])/(v[1]);
            	if (z_past>stop_loss_threshold || z_past<-stop_loss_threshold){
            		if (v[2]>0){
            			//trades1.push_back({dates[v[3]], "SELL", 1, prices1[i]});
            			// cout<<pos<<" "<<total_<<" "<<prices1[i]<<" " <<prices2[i]<<" hi"<<i<<" "<<v[2]<<endl;
            			pos-=1;
            			cashflow[i] += prices1[i];
            			total_ += prices1[i];

            			//trades2.push_back({dates[v[3]], "BUY", 1, prices1[i]});
            			cashflow[i] -= prices2[i];
            			total_ -= prices2[i];
            		}
            		else{
            			//trades1.push_back({dates[v[3]], "BUY", 1, prices1[i]});
            			// cout<<pos<<" "<<total_<<" "<<prices1[i]<<" " <<prices2[i]<<" hi"<<i<<" "<<v[2]<<endl;
            			pos+=1;
            			cashflow[i] -= prices1[i];
            			total_ -= prices1[i];

            			//trades2.push_back({dates[v[3]], "SELL", 1, prices1[i]});
            			cashflow[i] += prices2[i];
            			total_ += prices2[i];
            		
            		}
            	
            	}
            	else{
            		
            		q2.push(v);
            	
            	}
            	
            }
            while (!q2.empty()){
            	vector<double>v(4,0.0);
            	v=q2.front();
            	q2.pop();
            	q1.push(v);
            
            }
            if (pos>0 && position1>-x){
            trades1.push_back({dates[i], "SELL", pos, prices1[i]});
            trades2.push_back({dates[i], "BUY", pos, prices1[i]});
            position1-=pos;
            }
            else if(pos<0 && position1<x){
            	trades1.push_back({dates[i], "BUY", -pos, prices1[i]});
            trades2.push_back({dates[i], "SELL", -pos, prices1[i]});
            position1+=-pos;
            
            }
        }
        i++;
    }
    
    if (position1 > 0){
        // trades.push_back({dates[n1 - 1], "SELL", position, prices1[n1 - 1]});
        // cashflow[n1 - 1] += position * prices1[n1 - 1];
        // cout<<total_<<"  "<<position1<<" " <<prices1[n1-1]<<" " <<prices2[n1-1]<<"hiiiiiii"<<endl;;
        
        final_PnL= total_ + position1 * prices1[n1 - 1] - position1 * prices2[n1 - 1];
        position = 0;
    } else if (position1 < 0) {
        // trades.push_back({dates[n1 - 1], "BUY", -position, prices1[n1 - 1]});
        // cashflow[n1 - 1] -= abs(position) * prices1[n1 - 1];
        // cout<<total_<<"  "<<position1<<" " <<prices1[n1-1]<<" " <<prices2[n1-1]<<"hiiiiiiii"<<endl;;
        final_PnL= total_ - abs(position1) * prices1[n1 - 1] + abs(position1) * prices2[n1 - 1];
        position = 0;
    }
    else{
    	//  cout<<total_<<"  "<<position1<<" " <<prices1[n1-1]<<" " <<prices2[n1-1]<<"hiiiiii"<<endl;;
        final_PnL= total_;
    }
    
    writeOrderStatistics2(trades1,trades2);
    writeDailyCashFlow(cashflow);
    
    
   } 
    
 int main(int argc, char* argv[]) {
    if (argc != 8) {
        cerr << "Incorrect usage" << endl;
        return 1;
    }
	// cout<<"HEree\n\n\n";
    // Convert command-line arguments to integers
    string symbol1= (argv[1]);
    string symbol2= (argv[2]);
    int x= stoi(argv[3]);
    int n= stoi(argv[4]);
    int threshold= stoi(argv[5]);
    int stop_loss_threshold=stoi(argv[6]);
    string actual_start_date= (argv[7]);
    //cout<<actual_start_date<<"\n\n\n\n";
    readPriceData("price_data1.csv",1);
    readPriceData("price_data2.csv",2);

    // Run strategy
    runStrategy(threshold,stop_loss_threshold, n, x, actual_start_date);

    // Write final profit/loss to file
    ofstream finalPnLFile("final_pnl.txt");
    if (finalPnLFile.is_open()) {
        finalPnLFile<< final_PnL << endl;
        finalPnLFile.close();
    } else {
        cerr << "Unable to open file: final_pnl.txt" << endl;
    }
    cout<<"Done!!!\n";
    return 0;
}   
    
    
    
