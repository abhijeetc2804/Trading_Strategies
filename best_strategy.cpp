#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <omp.h>
#include <thread>

using namespace std;

double read_double(string i) {
    string file = "final_pnl" + i+".txt";
    cout<<file<<endl;;
    ifstream inputFile(file); // Open the input file

    if (!inputFile.is_open()) {
        cerr << "Failed to open the file." << endl;
        return 0;
    }

    double value;
    inputFile >> value; // Read the double value from the file

    if (inputFile.fail()) {
        cerr << "Failed to read the double value from the file." << endl;
        return 0;
    }

    inputFile.close();
    return value;
}

int index(vector<double> v) {
    double d = v[0];
    int ind = 0;
    for (int i = 0; i < v.size(); i++) {
    cout<<v[i]<<" "<<"sdxg"<<endl;
        if (d < v[i]) {
            d = v[i];
            ind = i;
        }
    }
    cout<<ind<<"ind"<<endl;
    return ind;
}

int main(int argc, char* argv[]) {
    // string symbol = (argv[1]);
    // string start_date = (argv[2]);
    string symbol = (argv[1]);
    string train_start_date= (argv[2]);
    string start_date = (argv[3]);
    // cout<<start_date<<" "<<train_start_date<<endl;

    #pragma omp parallel sections
    {

        {
    	 string strategy = "linear_regression1";
            string compile_command = "g++ -o ";
            string strategy_file = strategy + ".cpp";
            string executable_file = "./" + strategy;

            int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
            cout << "dsfafdsvsddacea" << endl;

            if (compile_result != 0) {
                cerr << "Compilation failed." << endl;
                return 1;
            }

            string run_command = "./" + executable_file;

            int run_result = system((run_command + " 5 2 "+" "+start_date + " "+ train_start_date ).c_str());

            if (run_result != 0) {
                cerr << "Execution failed." << endl;
                return 1;
            }
    	
    	
    	}

        #pragma omp section
        {
            string strategy = "basic_strategy1";
            string compile_command = "g++ -o ";
            string strategy_file = strategy + ".cpp";
            string executable_file = "./" + strategy;

            int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
            cout << "dsfafdsvsddacea" << endl;

            if (compile_result != 0) {
                cerr << "Compilation failed." << endl;
                return 1;
            }

            string run_command = "./" + executable_file;

            int run_result = system((run_command + " " + "7" + " " + "5" + " " + start_date).c_str());

            if (run_result != 0) {
                cerr << "Execution failed." << endl;
                return 1;
            }
        }

        #pragma omp section
        {
            string strategy = "dma1";
            string compile_command = "g++ -o ";
            string strategy_file = strategy + ".cpp";
            string executable_file = "./" + strategy;

            int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
            cout << "dsfafdsvsddacea1" << endl;

            if (compile_result != 0) {
                cerr << "Compilation failed." << endl;
                return 1;
            }

            string run_command = "./" + executable_file;

            int run_result = system((run_command + " " + "50" + " " + "5" + " " + "2" + " " + start_date).c_str());

            if (run_result != 0) {
                cerr << "Execution failed." << endl;
                return 1;
            }
        }

        #pragma omp section
        {
            string strategy = "dma++1";
            string compile_command = "g++ -o ";
            string strategy_file = strategy + ".cpp";
            string executable_file = "./" + strategy;

            int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
            cout << "dsfafdsvsddacea" << endl;

            if (compile_result != 0) {
                cerr << "Compilation failed." << endl;
                return 1;
            }

            string run_command = "./" + executable_file;

            int run_result = system((run_command + " " + "5 5 14 28 2 0.2 " + start_date).c_str());

            if (run_result != 0) {
                cerr << "Execution failed." << endl;
                return 1;
            }
        }

        #pragma omp section
        {
        	cout<<"line131"<<endl;
            string strategy = "macd1";
            string compile_command = "g++ -o ";
            string strategy_file = strategy + ".cpp";
            string executable_file = "./" + strategy;
		cout<<"_________"<<start_date<<"\n";
            int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
            cout << "dsfafdsvsddacea" << endl;

            if (compile_result != 0) {
                cerr << "Compilation failed." << endl;
                return 1;
            }
		
            string run_command = "./" + executable_file;

            int run_result = system((run_command + " 5 " + start_date).c_str());

            if (run_result != 0) {
                cerr << "Execution failed." << endl;
                return 1;
            }
        }

        #pragma omp section
        {cout<<"lin156"<<endl;
            string strategy = "rsi1";
            string compile_command = "g++ -o ";
            string strategy_file = strategy + ".cpp";
            string executable_file = "./" + strategy;

            int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
            cout << "dsfafdsvsddacea" << endl;

            if (compile_result != 0) {
                cerr << "Compilation failed." << endl;
                return 1;
            }

            string run_command = "./" + executable_file;

            int run_result = system((run_command + " 5 14 30 70 " + start_date).c_str());

            if (run_result != 0) {
                cerr << "Execution failed." << endl;
                return 1;
            }
        }

        #pragma omp section
        {
            string strategy = "adx1";
            string compile_command = "g++ -o ";
            string strategy_file = strategy + ".cpp";
            string executable_file = "./" + strategy;

            int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
            cout << "dsfafdsvsddacea" << endl;

            if (compile_result != 0) {
                cerr << "Compilation failed." << endl;
                return 1;
            }

            string run_command = "./" + executable_file;

            int run_result = system((run_command + " 5 14 25 " + start_date).c_str());

            if (run_result != 0) {
                cerr << "Execution failed." << endl;
                return 1;
            }
        }
    }

    vector<double> pnl;
    pnl.push_back(read_double("0"));
    pnl.push_back(read_double("1"));
    pnl.push_back(read_double("2"));
    pnl.push_back(read_double("3"));
    pnl.push_back(read_double("4"));
    pnl.push_back(read_double("5"));
    pnl.push_back(read_double("6"));

    int ind = index(pnl);
    cout << ind + 1 << endl;

    if (ind == 0) {
        string strategy = "basic_strategy";
        string compile_command = "g++ -o ";
        string strategy_file = strategy + ".cpp";
        string executable_file = "./" + strategy;

        int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
        cout << "dsfafdsvsddacea0" << endl;

        if (compile_result != 0) {
            cerr << "Compilation failed." << endl;
            return 1;
        }

        string run_command = "./" + executable_file;

        int run_result = system((run_command + " " + "7" + " " + "5" + " " + start_date).c_str());

        if (run_result != 0) {
            cerr << "Execution failed." << endl;
            return 1;
        }
    }

    else if (ind == 1) {
        string strategy = "dma_strategy";
        string compile_command = "g++ -o ";
        string strategy_file = strategy + ".cpp";
        string executable_file = "./" + strategy;

        int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
        cout << "dsfafdsvsddacea1" << endl;

        if (compile_result != 0) {
            cerr << "Compilation failed." << endl;
            return 1;
        }

        string run_command = "./" + executable_file;

        int run_result = system((run_command + " " + "50" + " " + "5" + " " + "2" + " " + start_date).c_str());

        if (run_result != 0) {
            cerr << "Execution failed." << endl;
            return 1;
        }
    }

    else if (ind == 2) {
    	cout<<"dsbcsabcksjandbcajsbncsjkzbc\n\n\n\n\n\n\n\n"<<endl;;
        string strategy = "dma_plus_plus_strategy";
        string compile_command = "g++ -o ";
        string strategy_file = strategy + ".cpp";
        string executable_file = "./" + strategy;

        int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
        cout << "dsfafdsvsddacea2" << endl;

        if (compile_result != 0) {
            cerr << "Compilation failed." << endl;
            return 1;
        }

        string run_command = "./" + executable_file;

        int run_result = system((run_command + " " + "5 5 14 28 2 0.2 " + start_date).c_str());

        if (run_result != 0) {
            cerr << "Execution failed." << endl;
            return 1;
        }
    }

    else if (ind == 3) {
        string strategy = "macd_strategy";
        string compile_command = "g++ -o ";
        string strategy_file = strategy + ".cpp";
        string executable_file = "./" + strategy;

        int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
        cout << "dsfafdsvsddacea" << endl;

        if (compile_result != 0) {
            cerr << "Compilation failed." << endl;
            return 1;
        }

        string run_command = "./" + executable_file;

        int run_result = system((run_command + " 5 " + start_date).c_str());

        if (run_result != 0) {
            cerr << "Execution failed." << endl;
            return 1;
        }
    }
	
    else if (ind == 4) {
        string strategy = "rsi_strategy";
        string compile_command = "g++ -o ";
        string strategy_file = strategy + ".cpp";
        string executable_file = "./" + strategy;

        int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
        cout << "dsfafdsvsddacea4" << endl;

        if (compile_result != 0) {
            cerr << "Compilation failed." << endl;
            return 1;
        }

        string run_command = "./" + executable_file;

        int run_result = system((run_command + " 5 14 30 70 " + start_date).c_str());

        if (run_result != 0) {
            cerr << "Execution failed." << endl;
            return 1;
        }
    }

    else if (ind == 5) {
    // cout<<"dsbcsabcksjandbcajsbncsjkzbc\n\n\n\n\n\n\n\n"<<endl;;
        string strategy = "adx_strategy";
        string compile_command = "g++ -o ";
        string strategy_file = strategy + ".cpp";
        string executable_file = "./" + strategy;

        int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
        cout << "dsfafdsvsddacea5" << endl;

        if (compile_result != 0) {
            cerr << "Compilation failed." << endl;
            return 1;
        }

        string run_command = "./" + executable_file;

        int run_result = system((run_command + " 5 14 25 " + start_date).c_str());

        if (run_result != 0) {
            cerr << "Execution failed." << endl;
            return 1;
        }
    }
    else if (ind == 6) {
    //cout<<"dsbcsabcksjandbcajsbncsjkzbc\n\n\n\n\n\n\n\n"<<endl;;
        string strategy = "linear_regression1";
            string compile_command = "g++ -o ";
            string strategy_file = strategy + ".cpp";
            string executable_file = "./" + strategy;

            int compile_result = system((compile_command + executable_file + " " + strategy_file).c_str());
            cout << "dsfafdsvsddacea" << endl;

            if (compile_result != 0) {
                cerr << "Compilation failed." << endl;
                return 1;
            }

            string run_command = "./" + executable_file;

            int run_result = system((run_command + " 5 2 "+" "+start_date + " "+ train_start_date ).c_str());

            if (run_result != 0) {
                cerr << "Execution failed." << endl;
                return 1;
            }
    }
    return 0;
}



