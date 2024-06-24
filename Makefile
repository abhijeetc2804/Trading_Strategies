# Default target
.PHONY: all

# Run app.py with specified parameters based on strategy
all:
	@if [ "$(strategy)" = "BASIC" ]; then \
		python3 app.py $(strategy) $(symbol) $(n) $(x) "$(start_date)" "$(end_date)"; \
	elif [ "$(strategy)" = "DMA" ]; then \
		python3 app.py $(strategy) $(symbol) $(n) $(x) $(p) "$(start_date)" "$(end_date)"; \
	elif [ "$(strategy)" = "DMA++" ]; then \
		python3 app.py $(strategy) $(symbol) $(x) $(p) $(n) $(max_hold_days) $(c1) $(c2) "$(start_date)" "$(end_date)"; \
	elif [ "$(strategy)" = "MACD" ]; then \
		python3 app.py $(strategy) $(symbol) $(x) "$(start_date)" "$(end_date)"; \
	elif [ "$(strategy)" = "BEST_OF_ALL" ]; then \
		python3 app.py $(strategy) $(symbol) "$(start_date)" "$(end_date)"; \
	elif [ "$(strategy)" = "RSI" ]; then \
		python3 app.py $(strategy) $(symbol) $(x) $(n) $(oversold_threshold) $(overbought_threshold) "$(start_date)" "$(end_date)"; \
	elif [ "$(strategy)" = "ADX" ]; then \
		python3 app.py $(strategy) $(symbol) $(x) $(n) $(adx_threshold) "$(start_date)" "$(end_date)"; \
	elif [ "$(strategy)" = "LINEAR_REGRESSION" ]; then \
		python3 app.py $(strategy) "$(symbol)" $(x) $(p) "$(train_start_date)" "$(train_end_date)" "$(start_date)" "$(end_date)"; \
	elif [ "$(strategy)" = "PAIRS" ]; then \
		if [ -z "$(stop_loss_threshold)" ]; then \
			python3 app.py $(strategy) $(symbol1) $(symbol2) $(x) $(n) $(threshold) "$(start_date)" "$(end_date)"; \
		else \
			python3 app.py $(strategy) $(symbol1) $(symbol2) $(x) $(n) $(threshold) $(stop_loss_threshold) "$(start_date)" "$(end_date)"; \
		fi \
	elif [ "$(strategy)" = "BEST_OF_ALL" ]; then \
		make basic_strategy && make dma_strategy && make dma_plus_plus_strategy && make macd_strategy && make rsi_strategy && make adx_strategy && make pairs_strategy; \
	fi

# Strategy compilation rules
basic_strategy: basic_strategy.cpp
	g++ -o basic_strategy basic_strategy.cpp

dma_strategy: dma_strategy.cpp
	g++ -o dma_strategy dma_strategy.cpp

dma_plus_plus_strategy: dma_plus_plus_strategy.cpp
	g++ -o dma_plus_plus_strategy dma_plus_plus_strategy.cpp

macd_strategy: macd_strategy.cpp
	g++ -o macd_strategy macd_strategy.cpp

rsi_strategy: rsi_strategy.cpp
	g++ -o rsi_strategy rsi_strategy.cpp

adx_strategy: adx_strategy.cpp
	g++ -o adx_strategy adx_strategy.cpp

pairs_strategy: pairs_strategy.cpp
	g++ -o pairs_strategy pairs_strategy.cpp

# Clean rule
clean:
	rm -f basic_strategy dma_strategy dma_plus_plus_strategy macd_strategy rsi_strategy adx_strategy pairs_strategy
