import argparse
import csv
import sys
import logging
from utils import write_csv_table

logging.basicConfig(stream=sys.stdout, level=logging.INFO)
logger = logging.getLogger()

CSV_ATTRIBUTES = {'country' : 'country', 'city' : 'city',
              'lat' : 'latitude', 'lng' : 'longitude'}

def cli_argument_parser():
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--in_csv_file',
                        help='Input csv file name downloaded from '
                             'https://simplemaps.com/data/world-cities',
                        required=True,
                        type=str,
                        dest='in_csv_file')
    parser.add_argument('-o', '--out_csv_file',
                        help='Output csv file name',
                        required=False,
                        type=str,
                        default='out.csv',
                        dest="out_csv_file")
    args = parser.parse_args()

    return args

def read_table(in_csv_file_name, csv_attributes):
    rows = []
    with open(in_csv_file_name, encoding='utf-8') as in_csv_file:
        reader = csv.DictReader(in_csv_file)
        for idx, read_row in enumerate(reader, start=0):
            write_row = {}
            write_row['id'] = idx
            for in_key, out_key in csv_attributes.items():
                write_row[out_key] = read_row[in_key]
            rows.append(write_row)

    return rows

def main():
    args = cli_argument_parser()
    try:
        rows = read_table(args.in_csv_file, CSV_ATTRIBUTES)
        fields = list(CSV_ATTRIBUTES.values())
        write_csv_table(rows, args.out_csv_file,
                        ['id'] + fields, logger=logger,
                        sort_key=lambda row:(row['id'], row[fields[0]], row[fields[1]]))
    except Exception as ex:
        logger.error(f'{str(ex)}')


if __name__ == '__main__':
    sys.exit(main() or 0)
