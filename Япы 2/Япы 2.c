#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAXL 10

static char letters[MAXL];
static int  nletters;
static int  digit[MAXL];
static int  leading[MAXL];
static int  weight[MAXL];

static int  order[MAXL];
static int  abs_weight[MAXL];

static long long contrib[MAXL][10];

static void parse(const char* s)
{
    nletters = 0;
    int sign = 1;
    int first = 1;

    const char* p = s;
    while (*p) {
        if (*p == '+' || *p == '=') {
            sign = (*p == '=') ? -1 : 1;
            first = 1;
            p++;
            continue;
        }

        if (*p >= 'A' && *p <= 'Z') {
            int len = 0;
            const char* tmp = p;
            while (*tmp >= 'A' && *tmp <= 'Z') {
                len++;
                tmp++;
            }

            int w = 1;
            for (int i = len - 1; i >= 0; --i) {
                char c = p[i];
                int idx = -1;
                for (int j = 0; j < nletters; ++j) {
                    if (letters[j] == c) { idx = j; break; }
                }
                if (idx < 0) {
                    idx = nletters++;
                    letters[idx] = c;
                    leading[idx] = 0;
                    weight[idx] = 0;
                }

                if (i == 0 && first) {
                    leading[idx] = 1;
                    first = 0;
                }

                weight[idx] += sign * w;
                w *= 10;
            }
            p += len;
        }
        else {
            p++;
        }
    }
}

static int solve(int idx, int used, long long sum, long long max_remaining)
{
    if (idx == nletters) return sum == 0;

    if (llabs(sum) > max_remaining) {
        return 0;
    }

    int real_idx = order[idx];

    int start_d = 0;
    if (leading[real_idx]) start_d = 1;

    for (int d = start_d; d <= 9; ++d) {
        if (used & (1 << d)) continue;
        if (d == 0 && leading[real_idx]) continue;

        digit[real_idx] = d;

        long long w_contrib = contrib[real_idx][d];

        long long next_max = max_remaining - 9LL * abs_weight[real_idx];

        if (solve(idx + 1, used | (1 << d), sum + w_contrib, next_max)) {
            return 1;
        }
    }
    return 0;
}

static void print_answer(const char* s)
{
    for (const char* p = s; *p; ++p) {
        char c = *p;
        if (c >= 'A' && c <= 'Z') {
            for (int i = 0; i < nletters; ++i) {
                if (letters[i] == c) { putchar('0' + digit[i]); break; }
            }
        }
        else {
            putchar(c);
        }
    }
    putchar('\n');
}

static void prepare_optimization2(void)
{
    for (int i = 0; i < nletters; ++i) {
        order[i] = i;
        abs_weight[i] = abs(weight[i]);
    }

    for (int i = 0; i < nletters - 1; ++i) {
        for (int j = 0; j < nletters - 1 - i; ++j) {
            if (abs_weight[order[j]] < abs_weight[order[j + 1]]) {
                int temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }
}

static void prepare_optimization3(void)
{
    for (int i = 0; i < nletters; ++i) {
        for (int d = 0; d <= 9; ++d) {
            contrib[i][d] = (long long)d * weight[i];
        }
    }
}

int main(void)
{
    char raw[256];
    char clean[256];

    if (!fgets(raw, sizeof(raw), stdin)) return 1;

    int k = 0;
    for (int i = 0; raw[i] != '\0' && raw[i] != '\n' && raw[i] != '\r'; ++i) {
        char c = raw[i];
        if ((c >= 'A' && c <= 'Z') || c == '+' || c == '=') {
            clean[k++] = c;
        }
    }
    clean[k] = '\0';

    parse(clean);

    prepare_optimization2();
    prepare_optimization3(); 

    long long initial_max = 0;
    for (int i = 0; i < nletters; ++i) {
        initial_max += 9LL * abs_weight[i];
    }

    if (solve(0, 0, 0, initial_max)) {
        print_answer(clean);
    }
    else {
        printf("No solution\n");
    }

    return 0;
}