#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
using namespace std;


time_t convert_to_time(const string& dateString) {
    // Assuming dateString is in "YYYY-MM-DD" format
    tm tm = {};
  //  cout<<dateString<<endl;
    istringstream ss(dateString);
    ss >> get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) {
        cerr << "Error parsing date: " << dateString << endl;
        // Return some default value or throw an exception
    }
    time_t time = mktime(&tm);
    if (time == -1) {
        cerr << "Error converting date to std::time_t: " << dateString << endl;
        // Return some default value or throw an exception
    }
    return time;
}


// Define a structure to hold stock data
struct StockData {
    int position=0;
    vector<double> close;
    vector<double> close_1;
    vector<double> open_1;
    vector<double> vwap_1;
    vector<double> low_1;
    vector<double> high_1;
    vector<double> trades_1;
    vector<double> open;
};

// Function to read CSV data (mock implementation for testing)
vector<vector<string>> readCSV(const string& filename, const vector<int>& columns) {
    ifstream file(filename);
    vector<vector<string>> data;

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return data;
    }

    string line;
    while (getline(file, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;
        int colIndex = 0;
        while (getline(ss, cell, ',')) {
            if (find(columns.begin(), columns.end(), colIndex) != columns.end()) {
                row.push_back(cell);
            }
            colIndex++;
        }
        data.push_back(row);
    }

    file.close();
    return data;
}

// Function to transpose a matrix
vector<vector<double>> transpose(vector<vector<double>>& X) {
    int row = X.size();
    int col = X[0].size();
    vector<vector<double>> Xt(col, vector<double>(row));
    for (int i = 0; i < col; i++) {
        for (int j = 0; j < row; j++) {
            Xt[i][j] = X[j][i];
        }
    }
    return Xt;
}

// Function to calculate the inverse of a matrix
vector<vector<double>> inverseMatrix(const vector<vector<double>>& A) {
    int n = A.size();
    vector<vector<double>> augmented(n, vector<double>(2 * n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            augmented[i][j] = A[i][j];
            augmented[i][j + n] = (i == j) ? 1.0 : 0.0;
        }
    }
    for (int i = 0; i < n; ++i) {
        int pivot = i;
        for (int j = i + 1; j < n; ++j) {
            if (abs(augmented[j][i]) > abs(augmented[pivot][i])) {
                pivot = j;
            }
        }
        if (pivot != i) {
            swap(augmented[i], augmented[pivot]);
        }
        double divisor = augmented[i][i];
        for (int j = 0; j < 2 * n; ++j) {
            augmented[i][j] /= divisor;
        }
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                double factor = augmented[j][i];
                for (int k = 0; k < 2 * n; ++k) {
                    augmented[j][k] -= factor * augmented[i][k];
                }
            }
        }
    }
    vector<vector<double>> inverse(n, vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            inverse[i][j] = augmented[i][j + n];
        }
    }
    return inverse;
}

// Function to multiply two matrices
vector<vector<double>> multiplyMatrices(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    int m = A.size();
    int n = A[0].size();
    int p = B[0].size();
    vector<vector<double>> result(m, vector<double>(p));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < p; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < n; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}

// Function to solve for coefficients using linear regression
vector<double> matrix_solver(vector<vector<double>>& X, vector<double>& vec) {
    int n = vec.size();
    vector<vector<double>> Y(n, vector<double>(1));
    for (int i = 0; i < n; ++i) {
        Y[i][0] = vec[i];
    }

    vector<vector<double>> Xt = transpose(X);
    vector<vector<double>> XtX = multiplyMatrices(Xt, X);
    vector<vector<double>> XtY = multiplyMatrices(Xt, Y);
    vector<vector<double>> XtX_1 = inverseMatrix(XtX);
  //  print(XtX);
  //  print(XtX_1);
    vector<vector<double>> a,b;
    a=multiplyMatrices(XtX,XtX_1);
  //  print(a);
   // a=inverseMatrix({{1,0,0},{0,1,1},{0,0,1}});
    //b=multiplyMatrices(a,{{1,0,0},{0,1,1},{0,0,1}});
   // print(a);
    //print(b);
   // cout<<a[0].size()<<endl;
   // print(XtX_1);
   // print(multiplyMatrices(XtX,XtX_1));
    vector<vector<double>> XtX_1XtY = multiplyMatrices(XtX_1, XtY);
    vector<double> res;
    for (int i = 0; i < XtX_1XtY.size(); i++) {
        res.push_back(XtX_1XtY[i][0]);
    }
    return res;
}

// Function to calculate coefficients using linear regression
vector<double> Coefficients(vector<vector<double>>& data) {
    vector<double> coefficients(8, 0.0);
    vector<double> Y = data[0];
    vector<vector<double>> X;
    int n = data[0].size();
    for (int i = 0; i < n; i++) {
        vector<double> aux(8, 1); // Initialize with 1
        for (int j = 1; j < 8; j++) {
            aux[j] = data[j][i];
        }
        X.push_back(aux);
    }
    coefficients = matrix_solver(X, Y);
    return coefficients;
}

// Function to predict the closing price at time t
double close_t(vector<double>& data, vector<double>& coefficients) {
    double c = 0;
    for (int i = 0; i < 8; i++) {
        c += data[i] * coefficients[i];
    }
    return c;
}

// Function to calculate the percentage change between predicted and actual prices
double percentage(double predicted_price, double actual_price) {
    return 100 * (predicted_price - actual_price) / actual_price;
}


vector<string> reverseVector(vector<string>& vec) {
    int n = vec.size();
    for (int i = 0; i < n / 2; ++i) {
        string temp = vec[i];
        vec[i] = vec[n - i - 1];
        vec[n - i - 1] = temp;
    }
    return vec;
}
void write_ordered_statistics(vector<vector<string>>& data,const string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return;
    }

    // Write column headers
    file << "Date,Order_dir,Quantity,Price\n";

    // Write data
    for (size_t row = 0; row < data[0].size(); ++row) {
        for (size_t col = 0; col < data.size(); ++col) {
            file << data[col][row]; // Write element
            if (col != data.size() - 1) {
                file << ","; // Separate columns with comma
            }
        }
        file << "\n"; // Newline after each row
    }

    file.close();
}


void write_daily_cash_flow(vector<vector<string>>& data,const string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return;
    }

    // Write column headers
    file << "Date,Cashflow\n";
    vector<string>reverse;
    for(size_t col=0; col<data.size(); col++){
        reverse=reverseVector(data[col]);
    	data[col]=reverse;
    }
    // Write data
    for (size_t row = 0; row < data[0].size(); ++row) {
        for (size_t col = 0; col < data.size(); ++col) {
            file << data[col][row]; // Write element
            if (col != data.size() - 1) {
                file << ","; // Separate columns with comma
            }
        }
        file << "\n"; // Newline after each row
    }

    file.close();
}



void write(vector<vector<double>>&data,vector<double>&coefficients,vector<string>&date_vector,int x,double p){
  int position=0;
  int n=data[0].size();
  int i=n-1 ;
  double daily_cashflow=0;
  vector<vector<string>> cash_flow(2,vector<string>(n));
  vector<vector<string>>transactions(4,vector<string>());
  
  while (i>=0){
   // cout<<i<<endl;
    
    vector<double>aux;
    aux.push_back(1);
    for (int j=1;j<8;j++){
   //   cout<<j<<endl;
    //  print2(data);
      aux.push_back(data[j][i]);
    }
    double actual_price=data[0][i];
    double predicted_price=close_t(aux,coefficients);
    double percent =percentage(predicted_price,actual_price);
    
   // cout<<net_profit<<endl;
   // cout<<percent<<" "<<actual_price<<" "<<predicted_price<<endl;
    if (percent>=p && position<x){
      //buy_stock();
      string formattedDate = date_vector[i].substr(8, 2) + "/" + date_vector[i].substr(5, 2) + "/" + date_vector[i].substr(0, 4);
   //   cout<<formattedDate<<" "<<percent<<" "<<p<<endl;
      transactions[0].push_back(formattedDate);
      transactions[1].push_back("BUY");
      transactions[2].push_back("1");
      transactions[3].push_back(to_string(actual_price));
      daily_cashflow-=actual_price;
    //   cout<<date_vector[i]<<" "<<percent<<" "<<p<<endl;
      position+=1 ;
    }
    else if (percent<=-p && position>-x){
     // sell_stock();
     string formattedDate = date_vector[i].substr(8, 2) + "/" + date_vector[i].substr(5, 2) + "/" + date_vector[i].substr(0, 4);
   //   cout<<formattedDate<<" "<<percent<<" "<<p<<endl;
      transactions[0].push_back(formattedDate);
      transactions[1].push_back("SELL");
      transactions[2].push_back("1");
      transactions[3].push_back(to_string(actual_price));
      daily_cashflow+=actual_price;
      position-=1;
    }
    cash_flow[0][i]=date_vector[i];
    string a =to_string(daily_cashflow);
    cash_flow[1][i]=a;
    i-=1;
  }
 // print1(cash_flow);
 // cout<<data[0][0]<<" "<<position<<endl;
  double net_profit=data[0][0]*position;
  double NP= stod(cash_flow[1][0]);
  net_profit+=NP;
  write_daily_cash_flow(cash_flow,"daily_cashflow.csv");
  write_ordered_statistics(transactions,"order_statistics.csv");
  ofstream outputFile("final_pnl.txt");
  string np=to_string(net_profit);
  outputFile << np;
  outputFile.close();
  return ;
}



// Main function
int main(int argc, char* argv[]) {
    // Read stock data from CSV (mock data for testing)
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " x p train_start_date start_date" << std::endl;
        return 1; // Return non-zero to indicate an error
    }
    int x = std::stoi(argv[1]);
    int p = std::stoi(argv[2]);
    string train_start_date = argv[4];
    string start_date = argv[3];
   // cout<<start_date<<endl;
    
    vector<vector<string>>data=readCSV("price_data_train.csv", {0,2,3,4,7,8,13});
    // Prepare the data for linear regression
   // print(data);
    time_t ts_date=convert_to_time(train_start_date);
    int n=data.size();
    int i=n-1;
   // cout<<train_start_date<<endl;
    while(i>=0){
      time_t z =convert_to_time(data[i][0]);
      if (z<ts_date){
        i-=1;
      }
      else{
        break;
      }
    } 
    // cout<<i<<n<<endl;
   // print1(data);
    vector<string>date_vector;
    vector<vector<double>>train_data;
    vector<double>helper;
    //close 
    // cout<<"hi"<<endl;
    for(int j=1;j<=i;j++){
   // cout<<j<<" "<<i<<" "<<data[j][4]<<endl;
      double d=stod(data[j][4]);
      helper.push_back(d);
    }
   // cout<<"hi"<<endl;
    train_data.push_back(helper);
  //  cout<<train_data.size()<<endl;
    //print2(train_data);
    helper.clear();
    //close_1
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][4]);
      helper.push_back(d);
    }
    train_data.push_back(helper);
    helper.clear();
    //open_1
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][1]);
      helper.push_back(d);
    }
    train_data.push_back(helper);
    helper.clear();
    //vwap_1
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][5]);
      helper.push_back(d);
    }
    train_data.push_back(helper);
    helper.clear();
    //low_1
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][3]);
      helper.push_back(d);
    }
    train_data.push_back(helper);
    helper.clear();
    //high_1
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][2]);
      helper.push_back(d);
    }
    train_data.push_back(helper);
    helper.clear();
    //trades_1
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][6]);
      helper.push_back(d);
    }
    train_data.push_back(helper);
    helper.clear();
    //open
     for(int j=1;j<=i;j++){
      double d=stod(data[j][1]);
      helper.push_back(d);
    }
    train_data.push_back(helper);
    helper.clear();
   // print2(train_data);
    
    
    
    
    
    
    // Calculate coefficients using linear regression
    vector<double> coefficients = Coefficients(train_data);
    
    
    
    
    
    
    data=readCSV("price_data.csv", {0,2,3,4,7,8,13});
    time_t s_date=convert_to_time(start_date);
    n=data.size();
    i=n-1;
   // cout<<train_start_date<<endl;
    while(i>=0){
      time_t z =convert_to_time(data[i][0]);
      if (z<s_date){
        i-=1;
      }
      else{
        break;
      }
    }
   // i+=1;
  //  cout<<data.size()<<" "<<i<<endl;
    vector<vector<double>>real_data;

    for(int j=1;j<=i;j++){
  //  cout<<j<<" "<<i<<" "<<data[j][4]<<endl;
      date_vector.push_back(data[j][0]);
      double d=stod(data[j][4]);
      helper.push_back(d);
    }
   	
    real_data.push_back(helper);
    helper.clear();
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][4]);
      helper.push_back(d);
    }
    real_data.push_back(helper);
    helper.clear();
    //open_1
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][1]);
      helper.push_back(d);
    }
    real_data.push_back(helper);
    helper.clear();
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][5]);
      helper.push_back(d);
    }
    real_data.push_back(helper);
    helper.clear();
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][3]);
      helper.push_back(d);
    }
    real_data.push_back(helper);
    helper.clear();
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][2]);
      helper.push_back(d);
    }
    real_data.push_back(helper);
    helper.clear();
     for(int j=2;j<=i+1;j++){
      double d=stod(data[j][6]);
      helper.push_back(d);
    }
    real_data.push_back(helper);
    helper.clear();
    //open
     for(int j=1;j<=i;j++){
      double d=stod(data[j][1]);
      helper.push_back(d);
    }
    real_data.push_back(helper);
    helper.clear();
   // print2(real_data);
    
    
    
    
    write(real_data,coefficients,date_vector,x,p);
    cout<<"Done!!!\n";
    return 0;
}
