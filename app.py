import subprocess
import sys
import jugaad_data as jd
from jugaad_data.nse import stock_df
import pandas as pd
from datetime import datetime, timedelta

def best_of_all(symbol,start_date1,end_date1):
    strategy_files = {
        "BASIC": "basic_strategy",
        "DMA": "dma_strategy",
        "DMA++": "dma_plus_plus_strategy",
        "MACD": "macd_strategy",
        "RSI": "rsi_strategy",
        "ADX": "adx_strategy",
        "PAIRS": "mean_reverting_pairs"
    }
    


    start_date=start_date1
    end_date=end_date1
    strategy="BASIC"
    n=7
    start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
    delta = timedelta(days=2*int(n)+10)
    new_start_date=(start_date-delta)
    new_start_date = new_start_date.strftime('%d/%m/%Y')
    
    # start_date = start_date.strftime('%d/%m/%Y')
    fetch_data_to_csv3(strategy,symbol, new_start_date, end_date)

        
    start_date=start_date1
    end_date=end_date1        
    strategy="DMA"
    n=50
    start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
    delta = timedelta(days=2*int(n)+10)
    new_start_date=(start_date-delta)
    new_start_date = new_start_date.strftime('%d/%m/%Y')
    # start_date = start_date.strftime('%d/%m/%Y')
    fetch_data_to_csv3(strategy,symbol, new_start_date, end_date)

        
    
        
    start_date=start_date1
    end_date=end_date1    
    n=14
    strategy="DMA++"
    start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
    delta = timedelta(days=3*int(n)+10)
    new_start_date=(start_date-delta)
    new_start_date = new_start_date.strftime('%d/%m/%Y')
    # start_date = start_date.strftime('%d/%m/%Y')
    fetch_data_to_csv3(strategy,symbol, new_start_date, end_date)

    
    
    
    
    start_date=start_date1
    end_date=end_date1
   # n=14
    strategy="MACD"
    start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
    delta = timedelta(days=100)
    new_start_date=(start_date-delta)
    new_start_date = new_start_date.strftime('%d/%m/%Y')
    #print("------",actual_start_date,"\n\n\n\n\n")
    #start_date = start_date.strftime('%d/%m/%Y')
    
    #new_start_date = new_start_date.strftime('%Y-%m-%d')
    
    fetch_data_to_csv3(strategy,symbol, new_start_date, end_date)
        
        
    
    start_date=start_date1
    end_date=end_date1   
    n=14
    strategy="RSI"
    print(start_date)
    start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
    delta = timedelta(days=3*int(n)+10)
    new_start_date=(start_date-delta)
    new_start_date = new_start_date.strftime('%d/%m/%Y')
    fetch_data_to_csv3(strategy,symbol, new_start_date, end_date)
      
        
        
        
        
    start_date=start_date1
    end_date=end_date1
    n=14
    strategy="ADX"
    start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
    delta = timedelta(days=3*int(n)+10)
    new_start_date=(start_date-delta)
    new_start_date = new_start_date.strftime('%d/%m/%Y')
    
    fetch_data_to_csv3(strategy,symbol, new_start_date, end_date)
    
    
    
    start_date=start_date1
    end_date=end_date1
    strategy="LINEAR_REGRESSION"
    train_start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
    delta = timedelta(days=372)
   # print(train_start_date)
    new_train_start_date=(train_start_date-delta)
    new_train_start_date=new_train_start_date.strftime('%d/%m/%Y')
    train_end_date = datetime.strptime(end_date, "%d/%m/%Y").date()
    delta = timedelta(days=365)
    new_train_end_date=(train_end_date-delta)
    new_train_end_date = new_train_end_date.strftime('%d/%m/%Y')
    print(new_train_start_date,new_train_end_date,"vhjj")
    fetch_data_to_csv_train(symbol, new_train_start_date, new_train_end_date)
    start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
    delta = timedelta(days=7)
    new_start_date=(start_date-delta)
    new_start_date = new_start_date.strftime('%d/%m/%Y')
    fetch_data_to_csv3(strategy,symbol, new_start_date, end_date)	
    start_date=start_date1
    train_start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
    delta = timedelta(days=365)
    new_train_start_date=(train_start_date-delta)
    new_train_start_date=new_train_start_date.strftime('%d/%m/%Y')
    
    print(start_date,new_train_start_date,"dfd")
    start_date=start_date1
    subprocess.run(["g++", "-o", "best_strategy", "best_strategy.cpp"])
    subprocess.run(["./best_strategy", symbol,str(new_train_start_date),str(start_date)])
        

def fetch_data_to_csv3(strategy,symbol, start_date, end_date):
    # Convert start_date and end_date strings to datetime objects
    start_date = datetime.strptime(start_date, "%d/%m/%Y")
    end_date = datetime.strptime(end_date, "%d/%m/%Y")
    
    # Call the stock_df function with the adjusted date formats
    data = stock_df(symbol, start_date, end_date, series="EQ")
    
    # Save data to CSV file
    data.to_csv(f"{strategy}_price_data.csv", index=False)
    
    return data	

def fetch_data_to_csv_train(symbol, start_date, end_date):
    # Convert start_date and end_date strings to datetime objects
    start_date = datetime.strptime(start_date, "%d/%m/%Y")
    end_date = datetime.strptime(end_date, "%d/%m/%Y")
    
    # Call the stock_df function with the adjusted date formats
    data = stock_df(symbol, start_date, end_date, series="EQ")
    
    # Save data to CSV file
    data.to_csv("price_data_train.csv", index=False)
    
    return data

def fetch_data_to_csv(symbol, start_date, end_date):
    # Convert start_date and end_date strings to datetime objects
    start_date = datetime.strptime(start_date, "%d/%m/%Y")
    end_date = datetime.strptime(end_date, "%d/%m/%Y")
    
    # Call the stock_df function with the adjusted date formats
    data = stock_df(symbol, start_date, end_date, series="EQ")
    data=data.drop_duplicates()
    # Save data to CSV file
    data.to_csv("price_data.csv", index=False)
    return data

def fetch_data_to_csv2(symbol1,symbol2,start_date,end_date,filename1,filename2):
    # Convert start_date and end_date strings to datetime objects
    start_date = datetime.strptime(start_date, "%d/%m/%Y")
    end_date = datetime.strptime(end_date, "%d/%m/%Y")
    
    # Call the stock_df function with the adjusted date formats
    data = stock_df(symbol1, start_date, end_date, series="EQ")
    data=data.drop_duplicates()

    # Save data to CSV file
    data.to_csv(filename1, index=False)

    # Call the stock_df function with the adjusted date formats
    data = stock_df(symbol2, start_date, end_date, series="EQ")
    data=data.drop_duplicates()
    # Save data to CSV file
    data.to_csv(filename2, index=False)
    
    return data

def write_data_to_csv(filename, data):
    data.to_csv(filename,index=False)

if __name__ == "__main__":
    if(sys.argv[1]=="BEST_OF_ALL"):
        symbol=sys.argv[2]
        start_date=sys.argv[3]
        end_date=sys.argv[4]

        
        # print("-----",start_date,end_date,"\n\n\n\n\n")
        best_of_all(symbol,str(start_date),str(end_date))

    strategy=sys.argv[1]
    symbol=sys.argv[2]
    end_date=sys.argv[-1]
    start_date=sys.argv[-2]
    # print(strategy,symbol,end_date,start_date)
    # Fetch data and save to CSV
    strategy_files = {
        "BASIC": "basic_strategy",
        "DMA": "dma_strategy",
        "DMA++": "dma_plus_plus_strategy",
        "MACD": "macd_strategy",
        "RSI": "rsi_strategy",
        "ADX": "adx_strategy",
        "PAIRS": "mean_reverting_pairs",
        "LINEAR_REGRESSION": "linear_regression"
    }

    if (strategy=="LINEAR_REGRESSION"):
        if len(sys.argv) != 9:
            print("Incorrect Usage")
            sys.exit(1)            
        strategy, symbol, x, p,train_start_date,train_end_date,start_date, end_date = sys.argv[1:]
        train_start_date = datetime.strptime(train_start_date, "%d/%m/%Y").date()
        delta = timedelta(days=7)
        new_train_start_date=(train_start_date-delta)
        new_train_start_date = new_train_start_date.strftime('%d/%m/%Y')
        fetch_data_to_csv_train(symbol, new_train_start_date, train_end_date)
        start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
        delta = timedelta(days=7)
        new_start_date=(start_date-delta)
        new_start_date = new_start_date.strftime('%d/%m/%Y')
        fetch_data_to_csv(symbol, new_start_date, end_date)
        # Compile the specified C++ program
        subprocess.run(["g++", "-o", strategy_files[strategy], strategy_files[strategy]+".cpp"])
        # Run the compiled program with parameters
        subprocess.run(["./" + strategy_files[strategy], x, p, str(start_date) ,str(train_start_date)])
        
    elif(strategy=="BASIC") : 
        if len(sys.argv) != 7:
            print("Usage: python app.py BASIC <symbol> <n> <x> <start_date> <end_date>")
            sys.exit(1)

        strategy, symbol, n, x, start_date, end_date = sys.argv[1:]
        
        start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
        delta = timedelta(days=2*int(n)+10)
        new_start_date=(start_date-delta)
        new_start_date = new_start_date.strftime('%d/%m/%Y')
        # start_date = start_date.strftime('%d/%m/%Y')
        fetch_data_to_csv(symbol, new_start_date, end_date)
        # Compile the specified C++ program
        subprocess.run(["g++", "-o", strategy_files[strategy], strategy_files[strategy]+".cpp"])
        # Run the compiled program with parameters
        subprocess.run(["./" + strategy_files[strategy], n, x, str(start_date)])
    
    elif (strategy=="DMA") : 
        if len(sys.argv) != 8:
            print("Usage: python app.py DMA <symbol> <n> <x> <p> <start_date> <end_date>")
            sys.exit(1)
        
        strategy, symbol, n, x, p, start_date, end_date = sys.argv[1:]
        
        start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
        delta = timedelta(days=2*int(n)+10)
        new_start_date=(start_date-delta)
        new_start_date = new_start_date.strftime('%d/%m/%Y')
        # start_date = start_date.strftime('%d/%m/%Y')
        fetch_data_to_csv(symbol, new_start_date, end_date)

        
        # Compile the specified C++ program
        subprocess.run(["g++", "-o", strategy_files[strategy], strategy_files[strategy]+".cpp"])
        # Run the compiled program with parameters
        subprocess.run(["./" + strategy_files[strategy], n, x, p,str(start_date)])
    
    elif (strategy=="DMA++") :
        if len(sys.argv) !=11:
            print("Usage: python app.py DMA++ <symbol> <x> <p> <n> <max_hold_days> <c1> <c2> <start_date> <end_date>")
            sys.exit(1)
        x = (sys.argv[3])
        p = (sys.argv[4])
        n = (sys.argv[5])
        max_hold_days = (sys.argv[6])
        c1 = (sys.argv[7])
        c2 = (sys.argv[8])
        
        start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
        delta = timedelta(days=3*int(n)+10)
        new_start_date=(start_date-delta)
        new_start_date = new_start_date.strftime('%d/%m/%Y')
        # start_date = start_date.strftime('%d/%m/%Y')
        fetch_data_to_csv(symbol, new_start_date, end_date)

        # Compile the specified C++ program
        subprocess.run(["g++", "-o", strategy_files[strategy], strategy_files[strategy]+".cpp"])
        # Run the compiled program with parameters
        subprocess.run(["./" + strategy_files[strategy], x, p, n, max_hold_days, c1, c2,str(start_date)])
    
    elif strategy == "MACD":
        if len(sys.argv) != 6:
            print("Usage: python app.py MACD <symbol> <x> <start_date> <end_date>")
            sys.exit(1)
        strategy, symbol, x, start_date, end_date = sys.argv[1:]
        start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
        delta = timedelta(days=100)
        new_start_date=(start_date-delta)
        new_start_date = new_start_date.strftime('%d/%m/%Y')
        # start_date = start_date.strftime('%d/%m/%Y')
        
        fetch_data_to_csv(symbol, new_start_date, end_date)
        subprocess.run(["g++", "-o", strategy_files[strategy], f"{strategy_files[strategy]}.cpp"])
        subprocess.run(["./" + strategy_files[strategy], x, str(start_date)])

    elif strategy == "RSI":
        if len(sys.argv) != 9:
            print("Usage: python app.py RSI <symbol> <x> <n> <oversold_threshold> <overbought_threshold> <start_date> <end_date>")
            sys.exit(1)
        
        strategy, symbol, x, n, oversold_threshold, overbought_threshold, start_date, end_date = sys.argv[1:]
        start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
        delta = timedelta(days=3*int(n)+10)
        new_start_date=(start_date-delta)
        new_start_date = new_start_date.strftime('%d/%m/%Y')
        
        fetch_data_to_csv(symbol, new_start_date, end_date)
        # Compile the specified C++ program
        subprocess.run(["g++", "-o", strategy_files[strategy], f"{strategy_files[strategy]}.cpp"])
        # Run the compiled program with parameters
        subprocess.run(["./" + strategy_files[strategy], x, n, oversold_threshold, overbought_threshold, str(start_date)])

    elif strategy == "ADX":
        if len(sys.argv) != 8:
            print("Usage: python app.py ADX <symbol> <x> <n> <adx_threshold> <start_date> <end_date>")
            sys.exit(1)
        strategy, symbol, x, n, adx_threshold, start_date, end_date = sys.argv[1:]

        start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
        delta = timedelta(days=3*int(n)+10)
        new_start_date=(start_date-delta)
        new_start_date = new_start_date.strftime('%d/%m/%Y')
        
        fetch_data_to_csv(symbol, new_start_date, end_date)

        # Compile the specified C++ program
        subprocess.run(["g++", "-o", strategy_files[strategy], f"{strategy_files[strategy]}.cpp"])
        # Run the compiled program with parameters
        subprocess.run(["./" + strategy_files[strategy], x, n, adx_threshold, str(start_date)])
    
    elif strategy == "PAIRS":
        if (len(sys.argv)!=9 and len(sys.argv)!=10 ):
            print("Incorrect Usage\n")
            sys.exit(1)
        if len(sys.argv) == 10:
            symbol1, symbol2, x, n, threshold,stop_loss_threshold, start_date, end_date = sys.argv[2:]
            start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
            delta = timedelta(days=3 * int(n) + 10)
            new_start_date = (start_date - delta)
            new_start_date = new_start_date.strftime('%d/%m/%Y')
            fetch_data_to_csv2(symbol1, symbol2, new_start_date, end_date, "price_data1.csv", "price_data2.csv")
        # Compile the specified C++ program
            subprocess.run(["g++", "-o", "pairs", "pairs.cpp"])
        # Run the compiled program with parameters
            #subprocess.run(["./" + "pairs", symbol1, symbol2, x, n, threshold,stop_loss_threshold, str(start_date)])
            subprocess.run(["./pairs", symbol1, symbol2, str(x), str(n), str(threshold), str(stop_loss_threshold), str(start_date)])
        elif len(sys.argv) == 9:
            symbol1, symbol2, x, n, threshold, start_date, end_date = sys.argv[2:]
            start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
            delta = timedelta(days=3 * int(n) + 10)
            new_start_date = (start_date - delta)
            new_start_date = new_start_date.strftime('%d/%m/%Y')
        
        # Fetch data for both symbols
            fetch_data_to_csv2(symbol1, symbol2, new_start_date, end_date, "price_data1.csv", "price_data2.csv")
        # Compile the specified C++ program
            subprocess.run(["g++", "-o", strategy_files[strategy], f"{strategy_files[strategy]}.cpp"])
        # Run the compiled program with parameters
            subprocess.run(["./" + strategy_files[strategy], symbol1, symbol2, x, n, threshold, str(start_date)])


