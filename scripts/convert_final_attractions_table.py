from pathlib import Path
import argparse
import pandas as pd


def sanitize_cell(value: str) -> str:
    if not isinstance(value, str):
        return value

    value = value.replace(";", ",")     # Replace semicolons with commas to avoid CSV issues
    value = value.strip()               # Remove leading and trailing whitespace
    value = " ".join(value.split())     # Replace multiple spaces with a single space

    return value


def cli_argument_parser() -> argparse.Namespace:
    #pylint: disable=duplicate-code
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--in_csv_file',
                        help='Path to the input xlsx file.',
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


def read_table(path: Path) -> pd.DataFrame:
    if path.suffix.lower() == ".xlsx":
        return pd.read_excel(path, dtype=str)

    return pd.read_csv(path, sep=";", dtype=str, keep_default_na=False)


def main():
    args = cli_argument_parser()

    input_path = Path(args.in_csv_file)
    output_csv = Path(args.out_csv_file)
    output_xlsx = output_csv.with_suffix(".xlsx")

    # [1] Read table
    df = read_table(input_path)
    df = df.fillna("")

    # [2] Sanitize cells
    df = df.map(sanitize_cell)

    # [3.1] Save Excel
    df.to_excel(output_xlsx, index=False)

    # [3.2] Save CSV
    rows_count = len(df)
    csv_body = df.to_csv(sep=";", index=False)
    with open(output_csv, "w", encoding="utf-8-sig", newline="") as f:
        f.write(f"{rows_count}\n")
        f.write(csv_body)

if __name__ == "__main__":
    main()
