/*
 * bench.c
 * Copyright (C) 2014 c9s <yoanlin93@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "../include/zwebrap.h"
#include "zwebrap_classes.h"
#include "ztrie_bench.h"

unsigned long unixtime() {
    struct timeval tp;
    if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
        return tp.tv_sec;
    }
    return 0;
}

double microtime() {
    struct timeval tp;
    long sec = 0L;
    double msec = 0.0;
    if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
        msec = (double) (tp.tv_usec / MICRO_IN_SEC);
        sec = tp.tv_sec;
        if (msec >= 1.0)
            msec -= (long) msec;
        return sec + msec;
    }
    return 0;
}

void bench_start(bench *b) {
    b->start = microtime();
}

void bench_stop(bench *b) {
    b->end = microtime();
}

double bench_iteration_speed(bench *b) {
    return (b->N * b->R) / (b->end - b->start);
}

double bench_duration(bench *b) {
    return (b->end - b->start);
}

void bench_print_summary(bench *b) {
    printf("%ld runs, ", b->R);
    printf("%ld iterations each run, ", b->N);
    printf("finished in %lf seconds\n", bench_duration(b) );
    printf("%.2f i/sec\n", bench_iteration_speed(b) );
}

/**
 * Combine multiple benchmark result into one measure entry.
 *
 * bench_append_csv("benchmark.csv", 3, &b1, &b2)
 */
void bench_append_csv(char *filename, int countOfB, ...) {
    FILE *fp = fopen(filename, "a+");
    if(!fp) {
        return;
    }

    unsigned long ts = unixtime();
    fprintf(fp, "%ld", ts);


    int i;
    bench * b;
    va_list vl;
    va_start(vl,countOfB);
    for (i=0 ; i < countOfB ; i++) {
        b = va_arg(vl, bench*);
        fprintf(fp, ",%.2f", bench_iteration_speed(b) );
    }
    va_end(vl);

    fprintf(fp, "\n");
    fclose(fp);
}



int main()
{
    zhashx_t *n = zhashx_new ();

    char *route_data = (char *) malloc (sizeof (char) * 20);
    sprintf (route_data, "%s", "Route Data");

    MEASURE(tree_compile)
    zhashx_insert (n, "/foo/bar/baz", NULL);
    zhashx_insert (n, "/foo/bar/qux", NULL);
    zhashx_insert (n, "/foo/bar/quux", NULL);
    zhashx_insert (n, "/foo/bar/corge", NULL);
    zhashx_insert (n, "/foo/bar/grault", NULL);
    zhashx_insert (n, "/foo/bar/garply", NULL);
    zhashx_insert (n, "/foo/baz/bar", NULL);
    zhashx_insert (n, "/foo/baz/qux", NULL);
    zhashx_insert (n, "/foo/baz/quux", NULL);
    zhashx_insert (n, "/foo/baz/corge", NULL);
    zhashx_insert (n, "/foo/baz/grault", NULL);
    zhashx_insert (n, "/foo/baz/garply", NULL);
    zhashx_insert (n, "/foo/qux/bar", NULL);
    zhashx_insert (n, "/foo/qux/baz", NULL);
    zhashx_insert (n, "/foo/qux/quux", NULL);
    zhashx_insert (n, "/foo/qux/corge", NULL);
    zhashx_insert (n, "/foo/qux/grault", NULL);
    zhashx_insert (n, "/foo/qux/garply", NULL);
    zhashx_insert (n, "/foo/quux/bar", NULL);
    zhashx_insert (n, "/foo/quux/baz", NULL);
    zhashx_insert (n, "/foo/quux/qux", NULL);
    zhashx_insert (n, "/foo/quux/corge", NULL);
    zhashx_insert (n, "/foo/quux/grault", NULL);
    zhashx_insert (n, "/foo/quux/garply", NULL);
    zhashx_insert (n, "/foo/corge/bar", NULL);
    zhashx_insert (n, "/foo/corge/baz", NULL);
    zhashx_insert (n, "/foo/corge/qux", NULL);
    zhashx_insert (n, "/foo/corge/quux", NULL);
    zhashx_insert (n, "/foo/corge/grault", NULL);
    zhashx_insert (n, "/foo/corge/garply", NULL);
    zhashx_insert (n, "/foo/grault/bar", NULL);
    zhashx_insert (n, "/foo/grault/baz", NULL);
    zhashx_insert (n, "/foo/grault/qux", NULL);
    zhashx_insert (n, "/foo/grault/quux", NULL);
    zhashx_insert (n, "/foo/grault/corge", NULL);
    zhashx_insert (n, "/foo/grault/garply", NULL);
    zhashx_insert (n, "/foo/garply/bar", NULL);
    zhashx_insert (n, "/foo/garply/baz", NULL);
    zhashx_insert (n, "/foo/garply/qux", NULL);
    zhashx_insert (n, "/foo/garply/quux", NULL);
    zhashx_insert (n, "/foo/garply/corge", NULL);
    zhashx_insert (n, "/foo/garply/grault", NULL);
    zhashx_insert (n, "/bar/foo/baz", NULL);
    zhashx_insert (n, "/bar/foo/qux", NULL);
    zhashx_insert (n, "/bar/foo/quux", NULL);
    zhashx_insert (n, "/bar/foo/corge", NULL);
    zhashx_insert (n, "/bar/foo/grault", NULL);
    zhashx_insert (n, "/bar/foo/garply", NULL);
    zhashx_insert (n, "/bar/baz/foo", NULL);
    zhashx_insert (n, "/bar/baz/qux", NULL);
    zhashx_insert (n, "/bar/baz/quux", NULL);
    zhashx_insert (n, "/bar/baz/corge", NULL);
    zhashx_insert (n, "/bar/baz/grault", NULL);
    zhashx_insert (n, "/bar/baz/garply", NULL);
    zhashx_insert (n, "/bar/qux/foo", NULL);
    zhashx_insert (n, "/bar/qux/baz", NULL);
    zhashx_insert (n, "/bar/qux/quux", NULL);
    zhashx_insert (n, "/bar/qux/corge", NULL);
    zhashx_insert (n, "/bar/qux/grault", NULL);
    zhashx_insert (n, "/bar/qux/garply", NULL);
    zhashx_insert (n, "/bar/quux/foo", NULL);
    zhashx_insert (n, "/bar/quux/baz", NULL);
    zhashx_insert (n, "/bar/quux/qux", NULL);
    zhashx_insert (n, "/bar/quux/corge", NULL);
    zhashx_insert (n, "/bar/quux/grault", NULL);
    zhashx_insert (n, "/bar/quux/garply", NULL);
    zhashx_insert (n, "/bar/corge/foo", NULL);
    zhashx_insert (n, "/bar/corge/baz", NULL);
    zhashx_insert (n, "/bar/corge/qux", NULL);
    zhashx_insert (n, "/bar/corge/quux", NULL);
    zhashx_insert (n, "/bar/corge/grault", NULL);
    zhashx_insert (n, "/bar/corge/garply", NULL);
    zhashx_insert (n, "/bar/grault/foo", NULL);
    zhashx_insert (n, "/bar/grault/baz", NULL);
    zhashx_insert (n, "/bar/grault/qux", NULL);
    zhashx_insert (n, "/bar/grault/quux", NULL);
    zhashx_insert (n, "/bar/grault/corge", NULL);
    zhashx_insert (n, "/bar/grault/garply", NULL);
    zhashx_insert (n, "/bar/garply/foo", NULL);
    zhashx_insert (n, "/bar/garply/baz", NULL);
    zhashx_insert (n, "/bar/garply/qux", NULL);
    zhashx_insert (n, "/bar/garply/quux", NULL);
    zhashx_insert (n, "/bar/garply/corge", NULL);
    zhashx_insert (n, "/bar/garply/grault", NULL);
    zhashx_insert (n, "/baz/foo/bar", NULL);
    zhashx_insert (n, "/baz/foo/qux", NULL);
    zhashx_insert (n, "/baz/foo/quux", NULL);
    zhashx_insert (n, "/baz/foo/corge", NULL);
    zhashx_insert (n, "/baz/foo/grault", NULL);
    zhashx_insert (n, "/baz/foo/garply", NULL);
    zhashx_insert (n, "/baz/bar/foo", NULL);
    zhashx_insert (n, "/baz/bar/qux", NULL);
    zhashx_insert (n, "/baz/bar/quux", NULL);
    zhashx_insert (n, "/baz/bar/corge", NULL);
    zhashx_insert (n, "/baz/bar/grault", NULL);
    zhashx_insert (n, "/baz/bar/garply", NULL);
    zhashx_insert (n, "/baz/qux/foo", NULL);
    zhashx_insert (n, "/baz/qux/bar", NULL);
    zhashx_insert (n, "/baz/qux/quux", NULL);
    zhashx_insert (n, "/baz/qux/corge", NULL);
    zhashx_insert (n, "/baz/qux/grault", NULL);
    zhashx_insert (n, "/baz/qux/garply", NULL);
    zhashx_insert (n, "/baz/quux/foo", NULL);
    zhashx_insert (n, "/baz/quux/bar", NULL);
    zhashx_insert (n, "/baz/quux/qux", NULL);
    zhashx_insert (n, "/baz/quux/corge", NULL);
    zhashx_insert (n, "/baz/quux/grault", NULL);
    zhashx_insert (n, "/baz/quux/garply", NULL);
    zhashx_insert (n, "/baz/corge/foo", NULL);
    zhashx_insert (n, "/baz/corge/bar", NULL);
    zhashx_insert (n, "/baz/corge/qux", NULL);
    zhashx_insert (n, "/baz/corge/quux", NULL);
    zhashx_insert (n, "/baz/corge/grault", NULL);
    zhashx_insert (n, "/baz/corge/garply", NULL);
    zhashx_insert (n, "/baz/grault/foo", NULL);
    zhashx_insert (n, "/baz/grault/bar", NULL);
    zhashx_insert (n, "/baz/grault/qux", NULL);
    zhashx_insert (n, "/baz/grault/quux", NULL);
    zhashx_insert (n, "/baz/grault/corge", NULL);
    zhashx_insert (n, "/baz/grault/garply", NULL);
    zhashx_insert (n, "/baz/garply/foo", NULL);
    zhashx_insert (n, "/baz/garply/bar", NULL);
    zhashx_insert (n, "/baz/garply/qux", NULL);
    zhashx_insert (n, "/baz/garply/quux", NULL);
    zhashx_insert (n, "/baz/garply/corge", NULL);
    zhashx_insert (n, "/baz/garply/grault", NULL);
    zhashx_insert (n, "/qux/foo/bar", NULL);
    zhashx_insert (n, "/qux/foo/baz", NULL);
    zhashx_insert (n, "/qux/foo/quux", NULL);
    zhashx_insert (n, "/qux/foo/corge", NULL);
    zhashx_insert (n, "/qux/foo/grault", NULL);
    zhashx_insert (n, "/qux/foo/garply", NULL);
    zhashx_insert (n, "/qux/bar/foo", NULL);
    zhashx_insert (n, "/qux/bar/baz", NULL);
    zhashx_insert (n, "/qux/bar/quux", NULL);
    zhashx_insert (n, "/qux/bar/corge", route_data);
    zhashx_insert (n, "/qux/bar/grault", NULL);
    zhashx_insert (n, "/qux/bar/garply", NULL);
    zhashx_insert (n, "/qux/baz/foo", NULL);
    zhashx_insert (n, "/qux/baz/bar", NULL);
    zhashx_insert (n, "/qux/baz/quux", NULL);
    zhashx_insert (n, "/qux/baz/corge", NULL);
    zhashx_insert (n, "/qux/baz/grault", NULL);
    zhashx_insert (n, "/qux/baz/garply", NULL);
    zhashx_insert (n, "/qux/quux/foo", NULL);
    zhashx_insert (n, "/qux/quux/bar", NULL);
    zhashx_insert (n, "/qux/quux/baz", NULL);
    zhashx_insert (n, "/qux/quux/corge", NULL);
    zhashx_insert (n, "/qux/quux/grault", NULL);
    zhashx_insert (n, "/qux/quux/garply", NULL);
    zhashx_insert (n, "/qux/corge/foo", NULL);
    zhashx_insert (n, "/qux/corge/bar", NULL);
    zhashx_insert (n, "/qux/corge/baz", NULL);
    zhashx_insert (n, "/qux/corge/quux", NULL);
    zhashx_insert (n, "/qux/corge/grault", NULL);
    zhashx_insert (n, "/qux/corge/garply", NULL);
    zhashx_insert (n, "/qux/grault/foo", NULL);
    zhashx_insert (n, "/qux/grault/bar", NULL);
    zhashx_insert (n, "/qux/grault/baz", NULL);
    zhashx_insert (n, "/qux/grault/quux", NULL);
    zhashx_insert (n, "/qux/grault/corge", NULL);
    zhashx_insert (n, "/qux/grault/garply", NULL);
    zhashx_insert (n, "/qux/garply/foo", NULL);
    zhashx_insert (n, "/qux/garply/bar", NULL);
    zhashx_insert (n, "/qux/garply/baz", NULL);
    zhashx_insert (n, "/qux/garply/quux", NULL);
    zhashx_insert (n, "/qux/garply/corge", NULL);
    zhashx_insert (n, "/qux/garply/grault", NULL);
    zhashx_insert (n, "/quux/foo/bar", NULL);
    zhashx_insert (n, "/quux/foo/baz", NULL);
    zhashx_insert (n, "/quux/foo/qux", NULL);
    zhashx_insert (n, "/quux/foo/corge", NULL);
    zhashx_insert (n, "/quux/foo/grault", NULL);
    zhashx_insert (n, "/quux/foo/garply", NULL);
    zhashx_insert (n, "/quux/bar/foo", NULL);
    zhashx_insert (n, "/quux/bar/baz", NULL);
    zhashx_insert (n, "/quux/bar/qux", NULL);
    zhashx_insert (n, "/quux/bar/corge", NULL);
    zhashx_insert (n, "/quux/bar/grault", NULL);
    zhashx_insert (n, "/quux/bar/garply", NULL);
    zhashx_insert (n, "/quux/baz/foo", NULL);
    zhashx_insert (n, "/quux/baz/bar", NULL);
    zhashx_insert (n, "/quux/baz/qux", NULL);
    zhashx_insert (n, "/quux/baz/corge", NULL);
    zhashx_insert (n, "/quux/baz/grault", NULL);
    zhashx_insert (n, "/quux/baz/garply", NULL);
    zhashx_insert (n, "/quux/qux/foo", NULL);
    zhashx_insert (n, "/quux/qux/bar", NULL);
    zhashx_insert (n, "/quux/qux/baz", NULL);
    zhashx_insert (n, "/quux/qux/corge", NULL);
    zhashx_insert (n, "/quux/qux/grault", NULL);
    zhashx_insert (n, "/quux/qux/garply", NULL);
    zhashx_insert (n, "/quux/corge/foo", NULL);
    zhashx_insert (n, "/quux/corge/bar", NULL);
    zhashx_insert (n, "/quux/corge/baz", NULL);
    zhashx_insert (n, "/quux/corge/qux", NULL);
    zhashx_insert (n, "/quux/corge/grault", NULL);
    zhashx_insert (n, "/quux/corge/garply", NULL);
    zhashx_insert (n, "/quux/grault/foo", NULL);
    zhashx_insert (n, "/quux/grault/bar", NULL);
    zhashx_insert (n, "/quux/grault/baz", NULL);
    zhashx_insert (n, "/quux/grault/qux", NULL);
    zhashx_insert (n, "/quux/grault/corge", NULL);
    zhashx_insert (n, "/quux/grault/garply", NULL);
    zhashx_insert (n, "/quux/garply/foo", NULL);
    zhashx_insert (n, "/quux/garply/bar", NULL);
    zhashx_insert (n, "/quux/garply/baz", NULL);
    zhashx_insert (n, "/quux/garply/qux", NULL);
    zhashx_insert (n, "/quux/garply/corge", NULL);
    zhashx_insert (n, "/quux/garply/grault", NULL);
    zhashx_insert (n, "/corge/foo/bar", NULL);
    zhashx_insert (n, "/corge/foo/baz", NULL);
    zhashx_insert (n, "/corge/foo/qux", NULL);
    zhashx_insert (n, "/corge/foo/quux", NULL);
    zhashx_insert (n, "/corge/foo/grault", NULL);
    zhashx_insert (n, "/corge/foo/garply", NULL);
    zhashx_insert (n, "/corge/bar/foo", NULL);
    zhashx_insert (n, "/corge/bar/baz", NULL);
    zhashx_insert (n, "/corge/bar/qux", NULL);
    zhashx_insert (n, "/corge/bar/quux", NULL);
    zhashx_insert (n, "/corge/bar/grault", NULL);
    zhashx_insert (n, "/corge/bar/garply", NULL);
    zhashx_insert (n, "/corge/baz/foo", NULL);
    zhashx_insert (n, "/corge/baz/bar", NULL);
    zhashx_insert (n, "/corge/baz/qux", NULL);
    zhashx_insert (n, "/corge/baz/quux", NULL);
    zhashx_insert (n, "/corge/baz/grault", NULL);
    zhashx_insert (n, "/corge/baz/garply", NULL);
    zhashx_insert (n, "/corge/qux/foo", NULL);
    zhashx_insert (n, "/corge/qux/bar", NULL);
    zhashx_insert (n, "/corge/qux/baz", NULL);
    zhashx_insert (n, "/corge/qux/quux", NULL);
    zhashx_insert (n, "/corge/qux/grault", NULL);
    zhashx_insert (n, "/corge/qux/garply", NULL);
    zhashx_insert (n, "/corge/quux/foo", NULL);
    zhashx_insert (n, "/corge/quux/bar", NULL);
    zhashx_insert (n, "/corge/quux/baz", NULL);
    zhashx_insert (n, "/corge/quux/qux", NULL);
    zhashx_insert (n, "/corge/quux/grault", NULL);
    zhashx_insert (n, "/corge/quux/garply", NULL);
    zhashx_insert (n, "/corge/grault/foo", NULL);
    zhashx_insert (n, "/corge/grault/bar", NULL);
    zhashx_insert (n, "/corge/grault/baz", NULL);
    zhashx_insert (n, "/corge/grault/qux", NULL);
    zhashx_insert (n, "/corge/grault/quux", NULL);
    zhashx_insert (n, "/corge/grault/garply", NULL);
    zhashx_insert (n, "/corge/garply/foo", NULL);
    zhashx_insert (n, "/corge/garply/bar", NULL);
    zhashx_insert (n, "/corge/garply/baz", NULL);
    zhashx_insert (n, "/corge/garply/qux", NULL);
    zhashx_insert (n, "/corge/garply/quux", NULL);
    zhashx_insert (n, "/corge/garply/grault", NULL);
    zhashx_insert (n, "/grault/foo/bar", NULL);
    zhashx_insert (n, "/grault/foo/baz", NULL);
    zhashx_insert (n, "/grault/foo/qux", NULL);
    zhashx_insert (n, "/grault/foo/quux", NULL);
    zhashx_insert (n, "/grault/foo/corge", NULL);
    zhashx_insert (n, "/grault/foo/garply", NULL);
    zhashx_insert (n, "/grault/bar/foo", NULL);
    zhashx_insert (n, "/grault/bar/baz", NULL);
    zhashx_insert (n, "/grault/bar/qux", NULL);
    zhashx_insert (n, "/grault/bar/quux", NULL);
    zhashx_insert (n, "/grault/bar/corge", NULL);
    zhashx_insert (n, "/grault/bar/garply", NULL);
    zhashx_insert (n, "/grault/baz/foo", NULL);
    zhashx_insert (n, "/grault/baz/bar", NULL);
    zhashx_insert (n, "/grault/baz/qux", NULL);
    zhashx_insert (n, "/grault/baz/quux", NULL);
    zhashx_insert (n, "/grault/baz/corge", NULL);
    zhashx_insert (n, "/grault/baz/garply", NULL);
    zhashx_insert (n, "/grault/qux/foo", NULL);
    zhashx_insert (n, "/grault/qux/bar", NULL);
    zhashx_insert (n, "/grault/qux/baz", NULL);
    zhashx_insert (n, "/grault/qux/quux", NULL);
    zhashx_insert (n, "/grault/qux/corge", NULL);
    zhashx_insert (n, "/grault/qux/garply", NULL);
    zhashx_insert (n, "/grault/quux/foo", NULL);
    zhashx_insert (n, "/grault/quux/bar", NULL);
    zhashx_insert (n, "/grault/quux/baz", NULL);
    zhashx_insert (n, "/grault/quux/qux", NULL);
    zhashx_insert (n, "/grault/quux/corge", NULL);
    zhashx_insert (n, "/grault/quux/garply", NULL);
    zhashx_insert (n, "/grault/corge/foo", NULL);
    zhashx_insert (n, "/grault/corge/bar", NULL);
    zhashx_insert (n, "/grault/corge/baz", NULL);
    zhashx_insert (n, "/grault/corge/qux", NULL);
    zhashx_insert (n, "/grault/corge/quux", NULL);
    zhashx_insert (n, "/grault/corge/garply", NULL);
    zhashx_insert (n, "/grault/garply/foo", NULL);
    zhashx_insert (n, "/grault/garply/bar", NULL);
    zhashx_insert (n, "/grault/garply/baz", NULL);
    zhashx_insert (n, "/grault/garply/qux", NULL);
    zhashx_insert (n, "/grault/garply/quux", NULL);
    zhashx_insert (n, "/grault/garply/corge", NULL);
    zhashx_insert (n, "/garply/foo/bar", NULL);
    zhashx_insert (n, "/garply/foo/baz", NULL);
    zhashx_insert (n, "/garply/foo/qux", NULL);
    zhashx_insert (n, "/garply/foo/quux", NULL);
    zhashx_insert (n, "/garply/foo/corge", NULL);
    zhashx_insert (n, "/garply/foo/grault", NULL);
    zhashx_insert (n, "/garply/bar/foo", NULL);
    zhashx_insert (n, "/garply/bar/baz", NULL);
    zhashx_insert (n, "/garply/bar/qux", NULL);
    zhashx_insert (n, "/garply/bar/quux", NULL);
    zhashx_insert (n, "/garply/bar/corge", NULL);
    zhashx_insert (n, "/garply/bar/grault", NULL);
    zhashx_insert (n, "/garply/baz/foo", NULL);
    zhashx_insert (n, "/garply/baz/bar", NULL);
    zhashx_insert (n, "/garply/baz/qux", NULL);
    zhashx_insert (n, "/garply/baz/quux", NULL);
    zhashx_insert (n, "/garply/baz/corge", NULL);
    zhashx_insert (n, "/garply/baz/grault", NULL);
    zhashx_insert (n, "/garply/qux/foo", NULL);
    zhashx_insert (n, "/garply/qux/bar", NULL);
    zhashx_insert (n, "/garply/qux/baz", NULL);
    zhashx_insert (n, "/garply/qux/quux", NULL);
    zhashx_insert (n, "/garply/qux/corge", NULL);
    zhashx_insert (n, "/garply/qux/grault", NULL);
    zhashx_insert (n, "/garply/quux/foo", NULL);
    zhashx_insert (n, "/garply/quux/bar", NULL);
    zhashx_insert (n, "/garply/quux/baz", NULL);
    zhashx_insert (n, "/garply/quux/qux", NULL);
    zhashx_insert (n, "/garply/quux/corge", NULL);
    zhashx_insert (n, "/garply/quux/grault", NULL);
    zhashx_insert (n, "/garply/corge/foo", NULL);
    zhashx_insert (n, "/garply/corge/bar", NULL);
    zhashx_insert (n, "/garply/corge/baz", NULL);
    zhashx_insert (n, "/garply/corge/qux", NULL);
    zhashx_insert (n, "/garply/corge/quux", NULL);
    zhashx_insert (n, "/garply/corge/grault", NULL);
    zhashx_insert (n, "/garply/grault/foo", NULL);
    zhashx_insert (n, "/garply/grault/bar", NULL);
    zhashx_insert (n, "/garply/grault/baz", NULL);
    zhashx_insert (n, "/garply/grault/qux", NULL);
    zhashx_insert (n, "/garply/grault/quux", NULL);
    zhashx_insert (n, "/garply/grault/corge", NULL);
    END_MEASURE(tree_compile)
    BENCHMARK_SUMMARY(tree_compile);

    void *match = NULL;
    BENCHMARK(match_str)
    match = zhashx_lookup (n , "/qux/bar/corge");
    END_BENCHMARK(match_str)
    BENCHMARK_SUMMARY(match_str);
    assert (match);
    zhashx_destroy (&n);

    return 0;
}

