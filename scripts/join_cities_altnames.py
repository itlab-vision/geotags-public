import argparse
import csv
from dataclasses import dataclass
import sys
import logging
import time
from sklearn.neighbors import BallTree
import numpy as np
from rapidfuzz import fuzz
import utils

logging.basicConfig(format='[ %(levelname)s ] %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)


def cli_argument_parser():
    #pylint: disable=duplicate-code
    parser = argparse.ArgumentParser(
        description='Merge alternative city names based on coordinates and name matching'
    )
    parser.add_argument('-c', '--cities',
                        help='Cities database (in CSV format)',
                        required=True,
                        type=str,
                        dest='cities')
    parser.add_argument('-a', '--alt_names',
                        help='Database with alternative names of cities (in CSV format)',
                        required=True,
                        type=str,
                        dest='alt_names')
    parser.add_argument('-o', '--output',
                        help='Output database (in CSV format)',
                        required=True,
                        type=str,
                        dest='output')
    parser.add_argument('-d', '--max_distance',
                        type=float,
                        default=5,
                        help='Max distance between coordinates in km (default: 5)',
                        dest='max_distance')
    parser.add_argument('-f', '--fuzzy_threshold',
                        type=int,
                        default=90,
                        help='Fuzzy match threshold for city names (default: 90)',
                        dest='fuzzy_threshold')
    return parser.parse_args()


@dataclass
class MatchConfig:
    radius_km: float
    fuzzy_threshold: int

@dataclass
class CityRecord:
    city: str
    alt_names: list
    lat: float
    lon: float

    def __post_init__(self):
        self.norm_city = utils.normalize_text(self.city)


class GlobalIndex:
    def __init__(self, records):
        self.records = records
        coords = np.radians([(r.lat, r.lon) for r in records])
        self.tree = BallTree(coords, metric="haversine")

    def __getitem__(self, idx):
        return self.records[idx]

    def query(self, lat, lon, k=1):
        point = np.radians([[lat, lon]])
        dist, ind = self.tree.query(point, k=k)
        return dist[0], ind[0]


def load_alt_db(path, sep):
    logger.info(f"Loading alt DB: {path}")
    records = []

    with open(path, encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter=sep)

        for row in reader:
            city = row["city"]
            lat = float(row["latitude"])
            lon = float(row["longitude"])
            alt = utils.split_clean(row.get("alt_names", ""))

            records.append(CityRecord(city, alt, lat, lon))

    logger.info(f"Loaded {len(records)} cities with altnames")
    return GlobalIndex(records)


def find_match(index, lat, lon, city_name, config):
    norm_city = utils.normalize_text(city_name)

    dists_rad, indices = index.query(lat, lon, k=5)

    best_score = -1
    best_alt = []

    for dist_rad, idx in zip(dists_rad, indices):
        dist_km = dist_rad * utils.EARTH_R
        candidate = index[idx]

        if dist_km > config.radius_km:
            continue

        if norm_city == candidate.norm_city:
            return True, candidate.alt_names

        name_score = fuzz.token_sort_ratio(norm_city, candidate.norm_city)
        if name_score > best_score:
            best_score = name_score
            best_alt = candidate.alt_names

    if best_score >= config.fuzzy_threshold:
        return True, best_alt

    return False, []


def process_row(row, index, config):
    city = row["city"]
    lat = float(row["latitude"])
    lon = float(row["longitude"])

    curr_alt_list = utils.split_clean(row.get("alt_names", ""))

    matched, found_alt = find_match(index, lat, lon, city, config)

    if matched:
        row["alt_names"] = utils.join_unique(curr_alt_list + found_alt)
        return True, bool(found_alt)

    row["alt_names"] = utils.join_unique(curr_alt_list)
    return False, False


def merge(cities_path, index, output_path, sep, config):
    stats = {
        "matched": 0,
        "matched_with_alt": 0,
        "unmatched": 0,
    }

    rows_out = []

    with open(cities_path, encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter=sep)
        fieldnames = list(reader.fieldnames or [])

        if "alt_names" not in fieldnames:
            fieldnames.append("alt_names")

        logger.info(f"Loading cities DB: {cities_path}")
        for row in reader:
            matched, has_alt = process_row(row, index, config)

            if matched:
                stats["matched"] += 1
                if has_alt:
                    stats["matched_with_alt"] += 1
            else:
                stats["unmatched"] += 1
            rows_out.append(row)

    utils.write_csv_table(rows_out, output_path, fieldnames, logger=logger)

    logger.info(f"Matched cities: {stats["matched"]}")
    logger.info(f"Matched cities with altnames: {stats["matched_with_alt"]}")
    logger.info(f"Unmatched: {stats["unmatched"]}")


def main():
    args = cli_argument_parser()
    try:
        t0   = time.time()

        config = MatchConfig(
            radius_km=args.max_distance,
            fuzzy_threshold=args.fuzzy_threshold,
        )

        index = load_alt_db(args.alt_names, utils.CSV_SEPARATOR)

        merge(
            args.cities,
            index,
            args.output,
            utils.CSV_SEPARATOR,
            config
        )

        logger.info(f"Done in {time.time() - t0:.2f}s")
    except Exception as ex:
        logger.exception(f'Unexpected while joining cities and alternative names: {str(ex)}')



if __name__ == '__main__':
    sys.exit(main() or 0)
