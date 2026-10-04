import argparse
import csv
import sys
import logging
from typing import Any
from utils import write_csv_table

logging.basicConfig(stream=sys.stdout, level=logging.INFO)
logger = logging.getLogger()

CSV_ATTRIBUTES: dict[str, str] = {'country' : 'country', 'city' : 'city',
              'lat' : 'latitude', 'lng' : 'longitude'}

def cli_argument_parser() -> argparse.Namespace:
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

def read_table(in_csv_file_name: str, csv_attributes: dict[str, str]) -> list[dict[str, Any]]:
    rows = []
    with open(in_csv_file_name, encoding='utf-8') as in_csv_file:
        reader = csv.DictReader(in_csv_file)
        for idx, read_row in enumerate(reader, start=0):
            write_row = {}
            write_row['id'] = str(idx)
            for in_key, out_key in csv_attributes.items():
                write_row[out_key] = read_row[in_key]
            rows.append(write_row)

    return rows

def main():
    args = cli_argument_parser()
    try:
        rows = read_table(args.in_csv_file, CSV_ATTRIBUTES)
        fields = list(CSV_ATTRIBUTES.values())
        sort_rows = sorted(rows,
                             key=lambda row:(row['id'], row[fields[0]], row[fields[1]]),
                             reverse=False)

        logger.info(f'Writing to CSV file: {args.out_csv_file}')
        write_csv_table(sort_rows, args.out_csv_file,
                        ['id'] + fields, add_count_line=True)
        logger.info(f'Successfully wrote {len(sort_rows)} rows to CSV')

    except Exception as ex:
        logger.error(f'{str(ex)}')


if __name__ == '__main__':
    sys.exit(main() or 0)
