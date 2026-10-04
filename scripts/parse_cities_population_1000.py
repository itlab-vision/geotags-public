import argparse
import csv
import sys
import re
import logging
from typing import TypedDict
from dataclasses import dataclass
from utils import write_csv_table

logging.basicConfig(stream=sys.stdout, level=logging.INFO)
logger = logging.getLogger()

IN_CSV_ATTRIBUTES: dict[str, str] = {'city': 'Name', 'alt_names': 'Alternate Names',
                     'country': 'Country name EN', 'coord': 'Coordinates'}
OUT_CSV_ATTRIBUTES: list[str] = ['country', 'city', 'latitude', 'longitude', 'alt_names']

@dataclass
class CityRow(TypedDict):
    id: int
    country: str
    city: str
    latitude: float
    longitude: float
    alt_names: str

def cli_argument_parser() -> argparse.Namespace:
    #pylint: disable=duplicate-code
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--in_csv_file',
                        help='Path to the input csv file.',
                        required=True,
                        type=str,
                        dest='in_csv_file')
    parser.add_argument('-o', '--out_csv_file',
                        help='Path to the output csv file.',
                        required=True,
                        type=str,
                        dest='out_csv_file')
    args = parser.parse_args()

    return args

def parse_coordinates(coordinates):
    coord_regex = r'(?P<lat>[-]*[\d]+.*[\d]*)[,]+[ ]*(?P<lng>[-]*[\d]+.*[\d]*)'
    coord_match = re.match(coord_regex, coordinates)
    lat = coord_match['lat']
    lng = coord_match['lng']
    return lat, lng

def read_table(in_csv_file_name: str, in_csv_attributes: dict[str, str]) -> list[CityRow]:
    write_rows: list[CityRow] = []
    with open(in_csv_file_name, encoding='utf-8-sig') as in_csv_file:
        csv.register_dialect('row_reader', delimiter=';')
        reader = csv.DictReader(in_csv_file, dialect='row_reader')

        for idx, read_row in enumerate(reader, start=0):
            lat, lon = parse_coordinates(
                read_row[in_csv_attributes['coord']]
            )
            write_row: CityRow = {
                'id': idx,
                'country': read_row[in_csv_attributes['country']],
                'city': read_row[in_csv_attributes['city']],
                'latitude': lat,
                'longitude': lon,
                'alt_names': read_row[in_csv_attributes['alt_names']]
            }
            write_rows.append(write_row)
    return write_rows

def main():
    args = cli_argument_parser()
    try:
        rows = read_table(args.in_csv_file, IN_CSV_ATTRIBUTES)
        sorted_rows = sorted(rows,
                             key=lambda row:(
                                row['id'], row[OUT_CSV_ATTRIBUTES[0]],
                                row[OUT_CSV_ATTRIBUTES[1]]), reverse=False)

        logger.info(f'Writing to CSV file: {args.out_csv_file}')
        write_csv_table(sorted_rows, args.out_csv_file,
                        ['id'] + OUT_CSV_ATTRIBUTES, add_count_line=True)
        logger.info(f'Successfully wrote {len(sorted_rows)} rows to CSV')
    except Exception as ex:
        logger.error(f'{str(ex)}')


if __name__ == '__main__':
    sys.exit(main() or 0)
