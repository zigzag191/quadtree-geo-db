create or replace function benchmark_query(measured_query text) returns int as
$$
declare
    start_time timestamptz;
    end_time timestamptz;
begin
    start_time = clock_timestamp();
    execute (measured_query);
    end_time = clock_timestamp();
    return 1000 * (extract(epoch from end_time) - extract(epoch from start_time));
end;
$$ language plpgsql;
