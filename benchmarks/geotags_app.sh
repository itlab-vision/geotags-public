#!/bin/bash

path_to_geotags=$1
num_of_iter=$2

if [[ -z $path_to_geotags ]]
then
    echo "Path to repo is not defined"
    return
fi

if [[ -z $num_of_iter ]]
then
    num_of_iter=100
    echo "Number of iterations is not defined, setting to $num_of_iter"
fi

source ${path_to_geotags}/benchmarks/utils.sh

app_path="${path_to_geotags}/build/bin/geotags_app"
echo "App path: $app_path"
metrics_script_path="${path_to_geotags}/benchmarks/calculate_metrics.py"
echo "Metrics script path: $metrics_script_path"
test_data_path="${path_to_geotags}/test_data"
echo "Path to test data: $test_data_path"
image_path="${test_data_path}/images/IMG_20221109_162842.jpg"
echo "Path to image: $image_path"

databases=(
    "cities_with_a_population_1000"
    "russian_cities"
    "world_cities"
)

methods=(
    "haversine"
    "haversine_approx"
)

search_types=(
    "linear"
    "grid"
    "grid_binary"
)

clean_tmp_files tmp.txt times_exif.txt

for db in ${databases[@]}; do
    for method in ${methods[@]}; do
        for search_type in ${search_types[@]}; do
            clean_tmp_files "times_reading_db_from_$db.txt" \
                            "times_distance_${method}_${search_type}_from_$db.txt"
            if [[ $search_type == "grid" || $search_type == "grid_binary" ]]; then
                clean_tmp_files "times_reading_location_segments_db_from_$db.txt"
            fi

            if [[ $search_type == "grid" || $search_type == "grid_binary" ]]; then
                tmp_files=("times_exif.txt" \
                           "times_reading_db_from_$db.txt" \
                           "times_reading_location_segments_db_from_$db.txt" \
                           "times_distance_${method}_${search_type}_from_$db.txt")
            else
                tmp_files=("times_exif.txt" \
                           "times_reading_db_from_$db.txt" \
                           "times_distance_${method}_${search_type}_from_$db.txt")
            fi

            run_command="${app_path} -i=${image_path} -s -t=csv_fb -d=${method} -st=${search_type} -f=${test_data_path}/cities_dbs/${db}.csv | grep \"Time of\" | grep -Eo '[+-]?[0-9]*\.?[0-9]+([eE][+-]?[0-9]+)?'"

            run_iterations "$run_command" "$num_of_iter" "${tmp_files[@]}"
        done
    done
done


echo -e "\nOverall (all runs):"
print_stats "$metrics_script_path" "times_exif.txt" "Parsing exif meta"

for db in ${databases[@]}; do
    echo -e "\nDatabase \`${db}\` results:"
    
    print_stats "$metrics_script_path" "times_reading_db_from_$db.txt" "Reading database \`$db\`"
    
    if [[ -f "times_reading_location_segments_db_from_$db.txt" ]]; then
        print_stats "$metrics_script_path" "times_reading_location_segments_db_from_$db.txt" "Reading location segments database \`$db\`"
    fi

    for method in ${methods[@]}; do
        for search_type in ${search_types[@]}; do
            print_stats "$metrics_script_path" "times_distance_${method}_${search_type}_from_$db.txt" \
                          "Calculating with \`$method\` metric and \`$search_type\` search on database \`$db\`"
        done
    done
    
done