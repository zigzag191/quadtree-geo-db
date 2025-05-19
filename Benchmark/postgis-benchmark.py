import psycopg2
import sys
import os
import csv
import random

SCHEMA = "map_benchmark"


def load_map_data(osm_file_path):
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


def get_number_of_nodes(cursor) -> int:
    cursor.execute(f"select count(*) from {SCHEMA}.planet_osm_point")
    points = cursor.fetchone()[0]
    cursor.execute(f"select sum(ST_NumPoints(way)) from {SCHEMA}.planet_osm_line")
    points_in_multilines = cursor.fetchone()[0]
    return points + points_in_multilines


def benchmark(cursor) -> float:
    cursor.execute(f"""
        SELECT 
            ST_XMin(ST_Extent(way)),
            ST_YMin(ST_Extent(way)),
            ST_XMax(ST_Extent(way)),
            ST_YMax(ST_Extent(way))
        FROM (
            SELECT way FROM {SCHEMA}.planet_osm_point
            UNION ALL
            SELECT way FROM {SCHEMA}.planet_osm_line
        ) AS all_ways
    """)
    min_x, min_y, max_x, max_y = cursor.fetchone()

    iterations = 10
    total_time = 0.0

    for _ in range(iterations):
        width = random.uniform(0.01, 1) * (max_x - min_x)
        height = random.uniform(0.01, 1) * (max_y - min_y)

        x1 = random.uniform(min_x, max_x - width)
        y1 = random.uniform(min_y, max_y - height)
        x2 = x1 + width
        y2 = y1 + height

        point_query = f"""
            SELECT * FROM {SCHEMA}.planet_osm_point
            WHERE way && ST_MakeEnvelope({x1}, {y1}, {x2}, {y2}, 3857)
        """

        line_query = f"""
            SELECT * FROM {SCHEMA}.planet_osm_line
            WHERE way && ST_MakeEnvelope({x1}, {y1}, {x2}, {y2}, 3857)
        """

        cursor.execute(f"SELECT benchmark_query($${point_query}$$)")
        point_time = cursor.fetchone()[0]

        cursor.execute(f"SELECT benchmark_query($${line_query}$$)")
        line_time = cursor.fetchone()[0]

        total_time += point_time + line_time

    return total_time / iterations


def main() -> None:
    with open("postgis-benchmark-result-csv", "w", newline="") as benchmark_result_file:
        csv_writer = csv.writer(benchmark_result_file, delimiter=" ")

        connection = psycopg2.connect(dbname="gis", user="gis", password="gis", host="localhost", port="5432")
        connection.autocommit = True
        cursor = connection.cursor()

        for osm_file_path in sys.argv[1:]:
            cursor.execute(f"create schema if not exists {SCHEMA}")
            load_map_data(osm_file_path)

            cursor.execute(f"update {SCHEMA}.planet_osm_point set way=mercator_projection(way)")
            cursor.execute(f"update {SCHEMA}.planet_osm_line set way=mercator_projection(way)")

            cursor.execute(f"create index if not exists ix_point on {SCHEMA}.planet_osm_point using gist(way)")
            cursor.execute(f"create index if not exists ix_line on {SCHEMA}.planet_osm_line using gist(way)")

            csv_writer.writerow([osm_file_path, get_number_of_nodes(cursor), round(benchmark(cursor), 5)])

            cursor.execute(f"drop schema if exists {SCHEMA} cascade")

        cursor.close()
        connection.close()


if __name__ == "__main__":
    main()
