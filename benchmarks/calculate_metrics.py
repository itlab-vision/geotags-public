import argparse
import sys
import logging


logging.basicConfig(format='%(message)s', stream = sys.stdout, level = logging.INFO)
logger = logging.getLogger()


def cli_argument_parser():
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--in_txt_file',
                        help='Path to the input txt file.',
                        required=True,
                        type=str,
                        dest='in_txt_file')
    parser.add_argument('-d', '--description',
                        help='Description for the metrics',
                        default='')
    args = parser.parse_args()

    return args


def get_metrics(filepath):
    with open(filepath, 'r', encoding='utf-8') as file:
        lines = [float(line.strip()) for line in file]
    if not lines:
        return None, None, None
    return sum(lines) / len(lines), max(lines), min(lines)


def main():
    args = cli_argument_parser()
    try:
        txt_file = args.in_txt_file
        description = args.description
        mean_metric, max_metric, min_metric = get_metrics(txt_file)

        if description:
            logger.info(f"\t{description}:")

        if mean_metric is None:
            logger.info("\t\tNo data in file\n")
        else:
            logger.info(f"\t\tMean: {round(mean_metric, 8)}")
            logger.info(f"\t\tMax: {round(max_metric, 8)}")
            logger.info(f"\t\tMin: {round(min_metric, 8)}")

    except Exception as ex:
        logger.error(f'{str(ex)}')


if __name__ == '__main__':
    sys.exit(main() or 0)
