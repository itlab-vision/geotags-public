#!/bin/bash

path_to_geotags=$1
num_of_iter=$2

if [[ -z $path_to_geotags ]]; then
    echo "Path to repo is not defined"
    exit 1
fi

if [[ -z $num_of_iter ]]; then
    num_of_iter=100
    echo "Number of iterations is not defined, setting to $num_of_iter"
fi

source ${path_to_geotags}/benchmarks/utils.sh
app_path="${path_to_geotags}/build/bin/search_districts_app"
echo "App path: $app_path"

metrics_script_path="${path_to_geotags}/benchmarks/calculate_metrics.py"
echo "Metrics script path: $metrics_script_path"

test_data_path="${path_to_geotags}/test_data"
echo "Path to test data: $test_data_path"

image_path="${test_data_path}/images/IMG_20221109_162842.jpg"
echo "Path to image: $image_path"

declare -A db_paths=(
    ["russian_admin_units"]="${test_data_path}/admin_units_dbs/russian_admin_units.csv"
    ["russian_million_plus_cities"]="${test_data_path}/cities_dbs/russian_million_plus_cities.csv"
)

clean_tmp_files tmp.txt times_exif.txt

for db in "${!db_paths[@]}"; do
    db_file="${db_paths[$db]}"

    clean_tmp_files \
        "times_reading_db_from_${db}.txt" \
        "times_search_db_from_${db}.txt"

    tmp_files=(
        "times_exif.txt"
        "times_reading_db_from_${db}.txt"
        "times_search_db_from_${db}.txt"
    )

    run_command="${app_path} -i=${image_path} -d=${db_file} -t=csv_fb | grep 'Time of' | grep -Eo '[+-]?[0-9]*\.?[0-9]+([eE][+-]?[0-9]+)?'"

    run_iterations "$run_command" "$num_of_iter" "${tmp_files[@]}"
done

echo -e "\nOverall (all runs):"
print_stats "$metrics_script_path" "times_exif.txt" "Parsing exif meta"

for db in "${!db_paths[@]}"; do
    echo -e "\nResults for db \`${db}\`:"

    print_stats "$metrics_script_path" \
        "times_reading_db_from_${db}.txt" \
        "Reading districts"

    print_stats "$metrics_script_path" \
        "times_search_db_from_${db}.txt" \
        "Searching nearest district"
done

