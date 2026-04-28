import sys
import argparse
import logging
import traceback
import requests
from utils import write_csv_table
from bs4 import BeautifulSoup

logging.basicConfig(format='[ %(levelname)s ] %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

def cli_argument_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--save_path',
                        help='Path to save .csv file with coordinates.',
                        required=False,
                        default='russian_cities.csv',
                        type=str,
                        dest='save_path')
    args = parser.parse_args()

    return args

def parsing_coordinates():
    resp = requests.get(
        "https://time-in.ru/coordinates/russia",
        timeout=1)

    html = resp.text

    soup = BeautifulSoup(html, "html.parser")

    ll = list(soup.stripped_strings)
    l = ll[22:]
    parsed = []

    for elem in l:
        if elem == 'Координаты в регионах России':
            break
        parsed.append(elem)

    cities = parsed[::2]
    coords = parsed[1::2]
    diction = []

    for idx, (city, coord) in enumerate(zip(cities, coords), start=0):
        latitude, longitude = coord.split(',')
        diction.append(
            {'id': idx, 'country': 'Россия', 'city': city,
            'latitude': float(latitude), 'longitude': float(longitude)})
    return diction

def main():
    logger.info('Parsing command line arguments')
    args = cli_argument_parser()
    try:
        logger.info('Parsing coordinates from https://time-in.ru/coordinates/russia')
        diction = parsing_coordinates()
        write_csv_table(diction, args.save_path, logger=logger)
    except Exception:
        logger.error(traceback.format_exc())
        sys.exit(1)


if __name__ == '__main__':
    sys.exit(main() or 0)
