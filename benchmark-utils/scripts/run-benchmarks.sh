#!/bin/bash
export LC_ALL=C.UTF-8

# 假設腳本從儲存庫的主資料夾運行

#####################################################################################
# 常量:

# 輸出文件
FILE_OUT="./benchmark-utils/results/test_revise_cache_16g_no-measure.csv"

# 執行設置
STABILIIZER_EXEC="./stabilizer"
MEDUSA_EXEC="../MEDUSA/MEDUSA"

STABILIIZER_BASE_OPT="-i"
MEDUSA_BASE_OPT="-i"

# 基準測試目錄
BENCH_NO_MEASURE="./benchmarks/no-measure/"

# 測量設置
REPS=1
TIMEOUT="timeout 1h"   # 無超時: ""

# 輸出設置
SEP=","
NO_RUN="---"
ERROR="Error"
TO="TO"

#####################################################################################
# 函數:

# 使用STABILIIZER執行檔模擬單個電路。將結果保存在給定變數中。
sim_file_STABILIIZER() {
    local var_time_name="$1"
    local var_mem_name="$2"
    local var_fail_name="$3"
    
    local fail="${!var_fail_name}"
    local time_sum=0.0
    local mem_sum=0.0
    local output
    local time_cur
    local mem_cur

    if [[ $fail = false ]]; then
        for i in $(eval echo "{1..$REPS}"); do
            output=$($TIMEOUT $STABILIIZER_EXEC $STABILIIZER_run_opt <$file 2>&1 | grep -v "^ *'")
            if [[ -z $output ]]; then
                time_avg=$TO
                mem_avg=$TO
                fail=true
                break;
            elif grep -q "Time" <<< "$output"; then
                time_cur=$(echo "$output" | grep -oP '(?<=Time=)[0-9.eE+-]+' | awk '{printf "%.4f", $1}')
                mem_cur=$(echo "$output" | grep -oP '(?<=Peak Memory Usage=)[0-9]+' | awk '{printf "%.2f", $1 / 1024}')

                time_sum=$(echo "scale=4; $time_sum + $time_cur" | bc)
                mem_sum=$(echo "scale=2; $mem_sum + $mem_cur" | bc)
            else
                time_avg=$ERROR
                mem_avg=$ERROR
                fail=true
                break;
            fi
        done

        if [[ $fail = false ]]; then
            time_avg=$(echo "scale=4; $time_sum / $REPS.0" | bc)
            mem_avg=$(echo "scale=2; $mem_sum / $REPS.0" | bc)
        fi
    else
        time_avg=$NO_RUN
        mem_avg=$NO_RUN
    fi

    eval "$var_time_name"=$time_avg
    eval "$var_mem_name"=$mem_avg
    eval "$var_fail_name"=$fail
}

# 使用MEDUSA執行檔模擬單個電路。將結果保存在給定變數中。
sim_file_MEDUSA() {
    local var_time_name="$1"
    local var_mem_name="$2"
    local var_fail_name="$3"
    
    local fail="${!var_fail_name}"
    local time_sum=0.0
    local mem_sum=0.0
    local output
    local time_cur
    local mem_cur

    if [[ $fail = false ]]; then
        for i in $(eval echo "{1..$REPS}"); do
            output=$($TIMEOUT $MEDUSA_EXEC $MEDUSA_run_opt <$file 2>&1 | grep -v "^ *'")
            if [[ -z $output ]]; then
                time_avg=$TO
                mem_avg=$TO
                fail=true
                break;
            elif grep -q "Time" <<< "$output"; then
                time_cur=$(echo "$output" | grep -oP '(?<=Time=)[0-9.eE+-]+' | awk '{printf "%.4f", $1}')
                mem_cur=$(echo "$output" | grep -oP '(?<=Peak Memory Usage=)[0-9]+' | awk '{printf "%.2f", $1 / 1024}')

                time_sum=$(echo "scale=4; $time_sum + $time_cur" | bc)
                mem_sum=$(echo "scale=2; $mem_sum + $mem_cur" | bc)
            else
                time_avg=$ERROR
                mem_avg=$ERROR
                fail=true
                break;
            fi
        done

        if [[ $fail = false ]]; then
            time_avg=$(echo "scale=4; $time_sum / $REPS.0" | bc)
            mem_avg=$(echo "scale=2; $mem_sum / $REPS.0" | bc)
        fi
    else
        time_avg=$NO_RUN
        mem_avg=$NO_RUN
    fi

    eval "$var_time_name"=$time_avg
    eval "$var_mem_name"=$mem_avg
    eval "$var_fail_name"=$fail
}

# 預期第一個參數為測試的文件路徑，第二個參數為是否為符號電路
run_benchmark_file() {
    local file=$1
    local is_loop=$2

    benchmark_name=$(echo "$file" | cut -d'/' -f4-)

    # 變更為無循環的文件進行普通模擬（如果在循環目錄中）
    if [[ $is_loop = true ]]; then
        directory=$(dirname "$file")
        filename=$(basename "$file")
        new_filename="NL_$filename"

        if [ -f "$directory/$new_filename" ]; then
            file="$directory/$new_filename"
        elif [[ $MEDUSA_fail = false ]]; then
            # MEDUSA 無法解析該電路
            echo "缺少文件 $directory/$new_filename" >&2
        fi
    fi
    sim_file_STABILIIZER "STABILIIZER_time_avg" "STABILIIZER_mem_avg" "STABILIIZER_fail"
    sim_file_MEDUSA "MEDUSA_time_avg" "MEDUSA_mem_avg" "MEDUSA_fail"

    printf "%s$SEP%s$SEP%s$SEP%s$SEP%s\n" "${benchmark_name%.qasm}" \
           "$STABILIIZER_time_avg" "$STABILIIZER_mem_avg" "$MEDUSA_time_avg" "$MEDUSA_mem_avg" \
           >> "$file_out"
}

run_benchmarks() {
    # 初始化
    benchmarks_fd=$BENCH_NO_MEASURE
    STABILIIZER_run_opt=$STABILIIZER_BASE_OPT
    MEDUSA_run_opt=$MEDUSA_BASE_OPT
    MEDUSA_run_exec=$MEDUSA_EXEC
    file_out=$FILE_OUT

    printf "%s$SEP%s$SEP%s$SEP%s$SEP%s\n" "Circuit" "STABILIIZER t" \
           "STABILIIZER mem" "MEDUSA t" "MEDUSA mem" \
           >> "$file_out"

    for folder in "$benchmarks_fd"*/; do
        folder_name=$(basename "$folder")

        # 初始化崩潰和超時的標誌
        STABILIIZER_fail=false
        MEDUSA_fail=false

        if [[ ! "$folder_name" =~ ^LP- ]]; then
            # 非循環電路
            for file in "$folder"*.qasm; do
                if [ -f "$file" ]; then
                    run_benchmark_file $file false
                fi
                # Random 和 RevLib - 模擬時間非線性增長 -> 每個基準測試等待超時（重置標誌）
                if [[ ( "$folder_name" = "Random" ) || ( "$folder_name" = "RevLib" ) || ( "$folder_name" = "Feynman" ) ]]; then
                    STABILIIZER_fail=false
                    MEDUSA_fail=false
                fi
                if [[ ( $STABILIIZER_fail = true ) && ( $MEDUSA_fail = true ) ]]; then
                    break;
                fi
            done
        else
            # 帶循環變體的電路
            for file in "$folder"[0-9]*.qasm; do
                # 避免在沒有匹配文件時進行無效迭代
                if [ -f "$file" ]; then
                    run_benchmark_file $file true
                fi
                if [[ ( $STABILIIZER_fail = true ) && ( $MEDUSA_fail = true ) ]]; then
                    break;
                fi
            done
        fi
    done
}

# 從基準測試名稱中移除 'LP-' 並正確重新排序給定的 csv 文件。
format_csv() {
    local file=$1
    local tmp_file="tmp.csv"

    sed -i '/^LP-/ s/^LP-//' "$file"
    { head -n1 "$file"; tail -n+2 "$file" | sort -t, -k1,1; } > "$tmp_file"
    mv "$tmp_file" "$file"
}

#####################################################################################
# 輸出:
run_benchmarks
format_csv $FILE_OUT
