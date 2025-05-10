#!/bin/bash
cd bin
# Create a test output directory with timestamp
TIMESTAMP=$(date +"%Y-%m-%d_%H-%M-%S")
TEST_DIR="../tests/run_$TIMESTAMP"
mkdir -p "$TEST_DIR"

OUTFILE="$TEST_DIR/test_results.csv"
DOC_IDS=(1 2 3 4 5 6 7 8 9 10)   # Replace with actual IDs
KEYWORDS=("war" "science" "artificial" "data")  # Replace with real keywords
PROCESSES=(1 2 4 8 16 32)

echo "command,arg,processes,time_ms" > "$OUTFILE"

# --- Test -s (search) ---
for keyword in "${KEYWORDS[@]}"; do
    for p in "${PROCESSES[@]}"; do
        echo "Running: -s \"$keyword\" with $p processes"
        start=$(date +%s%3N)
        ./dclient -s "$keyword" "$p" > "$TEST_DIR/search_${keyword}_${p}.out"
        end=$(date +%s%3N)
        elapsed=$((end - start))
        echo "search,$keyword,$p,$elapsed" >> "$OUTFILE"
    done
done

# --- Test -c (consult) ---
for id in "${DOC_IDS[@]}"; do
    echo "Running: -c $id"
    start=$(date +%s%3N)
    ./dclient -c "$id" > "$TEST_DIR/consult_${id}.out"
    end=$(date +%s%3N)
    elapsed=$((end - start))
    echo "consult,$id,1,$elapsed" >> "$OUTFILE"
done

echo "✅ All tests complete. Results and outputs saved in $TEST_DIR"