#!/bin/bash

clean_tmp_files() {
    rm -f "$@"
}

run_iterations() {
    local run_command=$1
    local num_of_iter=$2
    shift 2               # необходимо для массива tmp_files
    local tmp_files=("$@")
  
    for ((i = 1; i <= num_of_iter; i++)); do
        bash -c "$run_command" > tmp.txt

        for idx in "${!tmp_files[@]}"; do
            line=$((idx+1))
            sed -n "${line}p" tmp.txt >> "${tmp_files[idx]}"
        done
    done
}

print_stats() {
    local metrics_script_path=$1
    local file=$2
    local description=$3

    if [[ -f "$file" ]]; then
        python "$metrics_script_path" -i "$file" -d "$description"
    else
        echo -e "\tFile $file not found for $description\n"
    fi
    echo
}