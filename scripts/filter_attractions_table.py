import argparse
import csv
import re
import sys
import logging
from utils import write_csv_table

logging.basicConfig(stream = sys.stdout, level = logging.INFO)
logger = logging.getLogger()

WKT_ATTRIBUTE = "WKT"
ID_ATTRIBUTE = "name"

def cli_argument_parser():
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--in_csv_file',
                        help='Path to the input csv file.',
                        required=True,
                        type=str,
                        dest='in_csv_file')
    parser.add_argument('-a', '--attributes',
                        help='Path to the file that contains list of attributes.',
                        required=True,
                        type=str,
                        dest='attributes_file')
    parser.add_argument('-o', '--out_csv_file',
                        help='Path to the output csv file.',
                        required=True,
                        type=str,
                        dest='out_csv_file')
    args = parser.parse_args()

    return args

def read_attributes(attributes_file_name):
    attributes = []
    with open(attributes_file_name, encoding='utf-8') as attributes_file:
        reader = csv.reader(attributes_file)
        for read_row in reader:
            attributes.extend(read_row)
    return attributes

def read_table(in_csv_file_name, in_csv_attributes):
    write_rows = []
    with open(in_csv_file_name, encoding='utf-8') as in_csv_file:
        reader = csv.DictReader(in_csv_file)

        for read_row in reader:
            write_row = {}
            for attribute in in_csv_attributes:
                if attribute == 'WKT':
                    point_regex = r'POINT \((?P<long>[\d]+.*[\d]*) (?P<lat>[\d]+.*[\d]*)\)'
                    point_match = re.match(point_regex, read_row[attribute])
                    write_row['longitude'] = point_match['long']
                    write_row['latitude'] = point_match['lat']
                else:
                    write_row[attribute] = read_row[attribute]
            if write_row[ID_ATTRIBUTE]:
                write_rows.append(write_row)
    return write_rows

def filter_table(in_csv_file_name, attributes_file_name, out_csv_file_name):
    attributes = read_attributes(attributes_file_name)
    in_csv_attributes = [WKT_ATTRIBUTE] + attributes
    logger.info(f'Available attributes: {in_csv_attributes}')

    out_csv_attributes = ['longitude', 'latitude'] + attributes
    logger.info(f'Filtered attributes: {out_csv_attributes}')

    write_rows = read_table(in_csv_file_name, in_csv_attributes)
    write_csv_table(write_rows, out_csv_file_name, out_csv_attributes, logger=logger)

def main():
    args = cli_argument_parser()
    try:
        filter_table(args.in_csv_file, args.attributes_file, args.out_csv_file)
    except Exception as ex:
        logger.error(f'{str(ex)}')


if __name__ == '__main__':
    sys.exit(main() or 0)
