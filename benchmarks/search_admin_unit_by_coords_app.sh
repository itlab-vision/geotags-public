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

APP_PATH="${path_to_geotags}/build/bin/search_admin_unit_by_coords_app"
METRICS_SCRIPT="${path_to_geotags}/benchmarks/calculate_metrics.py"
TEST_DATA_PATH="${path_to_geotags}/test_data"

echo "App path: ${APP_PATH}"
echo "Metrics script path: ${METRICS_SCRIPT}"
echo "Path to test data: ${TEST_DATA_PATH}"

# -----------------------------------------------------------------------------
# Coordinates
# -----------------------------------------------------------------------------

# Linear: Chita
LINEAR_LAT=52.03
LINEAR_LON=113.5

# Neighbors: Vologda region (27). Near to Kirov region (81)
NEIGHBORS_LAT=59.184452
NEIGHBORS_LON=45.948694

# -----------------------------------------------------------------------------
# Databases
# -----------------------------------------------------------------------------

declare -A ADMIN_UNITS_DB=(
    ["russian_admin_units"]="${TEST_DATA_PATH}/admin_units_dbs/russian_admin_units.csv"
    ["russian_admin_units_simplify_1km"]="${TEST_DATA_PATH}/admin_units_dbs/russian_admin_units_simplify_1km.csv"
)

NEIGHBORS_DB="${TEST_DATA_PATH}/admin_units_dbs/russian_admin_units_neighbors.csv"

CITIES_DB="${TEST_DATA_PATH}/admin_units_and_cities_dbs/rus_cities_admins.csv"

# -----------------------------------------------------------------------------
# Configurations
# -----------------------------------------------------------------------------

READER_TYPES=(
    "csv"
    "csv_fb"
)

NEIGHBORS_READER_TYPES=(
    "csv"
    "bin"
)

# -----------------------------------------------------------------------------
# Helpers
# -----------------------------------------------------------------------------

get_result_files() {
    local db="$1"
    local search_type="$2"
    local reader="$3"
    local neighbors_reader="$4"

    local db_safe="${db//\//_}"
    local base

    if [[ "$search_type" == "linear" ]]; then
        base="times_${db_safe}_linear_${reader}"
    else
        base="times_${db_safe}_neighbors_${reader}_${neighbors_reader}"
    fi

    local reading_file="${base}_reading.txt"
    local search_file="${base}_search.txt"

    if [[ "$search_type" == "neighbors" ]]; then
        local neighbors_file="${base}_neighbors.txt"
        echo "$reading_file $neighbors_file $search_file"
    else
        echo "$reading_file $search_file"
    fi
}

clean_result_files() {
    local db="$1"
    local search_type="$2"
    local reader="$3"
    local neighbors_reader="$4"

    read -r -a files <<< \
        "$(get_result_files "$db" "$search_type" "$reader" "$neighbors_reader")"

    clean_tmp_files "${files[@]}"
}

run_experiment() {
    local db_name="$1"
    local db_file="$2"
    local reader="$3"
    local neighbors_reader="$4"
    local search_type="$5"

    local lat
    local lon

    if [[ "$search_type" == "linear" ]]; then
        lat="$LINEAR_LAT"
        lon="$LINEAR_LON"
    else
        lat="$NEIGHBORS_LAT"
        lon="$NEIGHBORS_LON"
    fi

    local cmd="${APP_PATH} \
        -lat=${lat} \
        -lon=${lon} \
        -df=${db_file} \
        -drt=${reader} \
        -st=${search_type} \
        -lf=${CITIES_DB}"

    if [[ "$search_type" == "neighbors" ]]; then
    cmd="${cmd} \
        -dnf=${NEIGHBORS_DB} \
        -dnrt=${neighbors_reader}"
    fi

    cmd="${cmd} \
        | grep 'Time of' \
        | grep -Eo '[+-]?[0-9]*\\.?[0-9]+([eE][+-]?[0-9]+)?'"

    read -r -a result_files <<< \
        "$(get_result_files "$db_name" "$search_type" "$reader" "$neighbors_reader")"

    run_iterations \
        "$cmd" \
        "$num_of_iter" \
        "${result_files[@]}"
}

print_experiment_stats() {
    local db="$1"
    local search_type="$2"
    local reader="$3"
    local neighbors_reader="$4"

    echo

    if [[ "$search_type" == "linear" ]]; then
        echo "Results:"
        echo "  db=${db}"
        echo "  reader=${reader}"
        echo "  search=linear"
    else
        echo "Results:"
        echo "  db=${db}"
        echo "  reader=${reader}"
        echo "  neighbors_reader=${neighbors_reader}"
        echo "  search=neighbors"
    fi

     read -r -a files <<< \
        "$(get_result_files "$db" "$search_type" "$reader" "$neighbors_reader")"

    local reading_file="${files[0]}"

    if [[ "$search_type" == "linear" ]]; then
        local search_file="${files[1]}"
    else
        local neighbors_file="${files[1]}"
        local search_file="${files[2]}"
    fi

    echo

    if [[ -f "$reading_file" ]]; then
        print_stats "$METRICS_SCRIPT" "$reading_file" "Reading districts"
    else
        echo "Reading districts: no data"
    fi

    if [[ "$search_type" == "neighbors" ]]; then
        echo

        if [[ -f "$neighbors_file" ]]; then
            print_stats \
                "$METRICS_SCRIPT" \
                "$neighbors_file" \
                "Reading district neighbors"
        else
            echo "Reading district neighbors: no data"
        fi
    fi

    if [[ -f "$search_file" ]]; then
        print_stats "$METRICS_SCRIPT" "$search_file" "Searching district"
    else
        echo "Searching district: no data"
    fi
}

# -----------------------------------------------------------------------------
# Cleanup old results
# -----------------------------------------------------------------------------

for db_name in "${!ADMIN_UNITS_DB[@]}"; do

    for reader in "${READER_TYPES[@]}"; do

        clean_result_files \
            "$db_name" \
            "linear" \
            "$reader" \
            ""

    done

    for reader in "${READER_TYPES[@]}"; do
        for neighbors_reader in "${NEIGHBORS_READER_TYPES[@]}"; do

            clean_result_files \
                "$db_name" \
                "neighbors" \
                "$reader" \
                "$neighbors_reader"

        done
    done
done

# -----------------------------------------------------------------------------
# LINEAR SEARCH BENCHMARK
# -----------------------------------------------------------------------------

for db_name in "${!ADMIN_UNITS_DB[@]}"; do

    db_file="${ADMIN_UNITS_DB[$db_name]}"

    for reader in "${READER_TYPES[@]}"; do

        run_experiment \
            "$db_name" \
            "$db_file" \
            "$reader" \
            "" \
            "linear"

    done
done

# -----------------------------------------------------------------------------
# NEIGHBORS SEARCH BENCHMARK
# -----------------------------------------------------------------------------

for db_name in "${!ADMIN_UNITS_DB[@]}"; do

    db_file="${ADMIN_UNITS_DB[$db_name]}"

    for reader in "${READER_TYPES[@]}"; do
        for neighbors_reader in "${NEIGHBORS_READER_TYPES[@]}"; do

            run_experiment \
                "$db_name" \
                "$db_file" \
                "$reader" \
                "$neighbors_reader" \
                "neighbors"

        done
    done
done


# -----------------------------------------------------------------------------
# Print statistics
# -----------------------------------------------------------------------------
echo
echo "=================================================="
echo "OVERALL STATISTICS"
echo "=================================================="

# -----------------------------------------------------------------------------
# LINEAR RESULTS
# -----------------------------------------------------------------------------

echo
echo "LINEAR SEARCH RESULTS"

for db_name in "${!ADMIN_UNITS_DB[@]}"; do

    for reader in "${READER_TYPES[@]}"; do

        print_experiment_stats \
            "$db_name" \
            "linear" \
            "$reader" \
            ""

    done
done

# -----------------------------------------------------------------------------
# NEIGHBORS RESULTS
# -----------------------------------------------------------------------------

echo
echo "NEIGHBORS SEARCH RESULTS"

for db_name in "${!ADMIN_UNITS_DB[@]}"; do

    for reader in "${READER_TYPES[@]}"; do
        for neighbors_reader in "${NEIGHBORS_READER_TYPES[@]}"; do

            print_experiment_stats \
                "$db_name" \
                "neighbors" \
                "$reader" \
                "$neighbors_reader"

        done
    done
done

