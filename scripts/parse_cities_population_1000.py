import argparse
import csv
import sys
import re
import logging
from utils import write_csv_table

logging.basicConfig(stream=sys.stdout, level=logging.INFO)
logger = logging.getLogger()

IN_CSV_ATTRIBUTES = {'city': 'Name', 'alt_names': 'Alternate Names',
                     'country': 'Country name EN', 'coord': 'Coordinates'}
OUT_CSV_ATTRIBUTES = ['country', 'city', 'latitude', 'longitude', 'alt_names']

def cli_argument_parser():
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

def read_table(in_csv_file_name, in_csv_attributes):
    write_rows = []
    with open(in_csv_file_name, encoding='utf-8-sig') as in_csv_file:
        csv.register_dialect('row_reader', delimiter=';')
        reader = csv.DictReader(in_csv_file, dialect='row_reader')

        for idx, read_row in enumerate(reader, start=0):
            write_row = {}
            write_row['id'] = idx
            write_row['country'] = read_row[in_csv_attributes['country']]
            write_row['city'] = read_row[in_csv_attributes['city']]
            write_row['latitude'], write_row['longitude'] = parse_coordinates(
                read_row[in_csv_attributes['coord']])
            write_row['alt_names'] = read_row[in_csv_attributes['alt_names']]
            write_rows.append(write_row)
    return write_rows

def main():
    args = cli_argument_parser()
    try:
        rows = read_table(args.in_csv_file, IN_CSV_ATTRIBUTES)
        write_csv_table(rows, args.out_csv_file,
                        ['id'] + OUT_CSV_ATTRIBUTES, logger=logger,
                        sort_key=lambda row:(row['id'], row[OUT_CSV_ATTRIBUTES[0]],
                                                row[OUT_CSV_ATTRIBUTES[1]]))
    except Exception as ex:
        logger.error(f'{str(ex)}')


if __name__ == '__main__':
    sys.exit(main() or 0)
