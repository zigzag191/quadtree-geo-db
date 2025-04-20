import psycopg2
import sys
import os
import csv

SCHEMA = "map_benchmark"


def get_number_of_nodes(cursor) -> int:
    return 123


def benchmark(cursor) -> int:
    return 321


def main() -> None:
    with open("postgis-benchmark-result-csv", "w", newline="") as benchmark_result_file:
        csv_writer = csv.writer(benchmark_result_file, delimiter=" ")

        connection = psycopg2.connect(dbname="gis", user="gis", password="gis", host="localhost", port="5432")
        connection.autocommit = True
        cursor = connection.cursor()

        for osm_file_path in sys.argv[1:]:
            cursor.execute(f"create schema if not exists {SCHEMA}")
            os.system(f"osm2pgsql "
                      f"--create {osm_file_path} "
                      f"--host=localhost "
                      f"--port=5432 "
                      f"--database=gis "
                      f"--user=gis "
                      f"--password=gis "
                      f"--schema={SCHEMA} "
                      f"--style=./default.style "
                      f"--latlong")
            cursor.execute(f"update {SCHEMA}.planet_osm_point set way=mercator_projection(way)")
            cursor.execute(f"update {SCHEMA}.planet_osm_line set way=mercator_projection(way)")
            csv_writer.writerow([osm_file_path, get_number_of_nodes(cursor), benchmark(cursor)])
            cursor.execute(f"drop schema if exists {SCHEMA} cascade")

        cursor.close()
        connection.close()


if __name__ == "__main__":
    main()
