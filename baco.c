/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                      ___           ___           ___        *
 *       _____         /\  \         /\__\         /\  \       *
 *      /::\  \       /::\  \       /:/  /        /::\  \      *
 *     /:/\:\  \     /:/\:\  \     /:/  /        /:/\:\  \     *
 *    /:/ /::\__\   /:/ /::\  \   /:/  /  ___   /:/  \:\  \    *
 *   /:/_/:/\:|__| /:/_/:/\:\__\ /:/__/  /\__\ /:/__/ \:\__\   *
 *   \:\/:/ /:/  / \:\/:/  \/__/ \:\  \ /:/  / \:\  \ /:/  /   *
 *    \::/_/:/  /   \::/__/       \:\  /:/  /   \:\  /:/  /    *
 *     \:\/:/  /     \:\  \        \:\/:/  /     \:\/:/  /     *
 *      \::/  /       \:\__\        \::/  /       \::/  /      *
 *       \/__/         \/__/         \/__/         \/__/       *
 *                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* The following enumeration is necessary for the command-line options.
Whenever a new type is added, it must be inserted here to work as a
command-line option. Also note that the first variable in the enumeration
must be left unchanged.
-----------------------------------------------------------------------------*/
enum commands {
    FIRST, BCD, BIN, CO1, CO2, DEC, FLT, HEX, MES, OCT, ROM
};

/* The following string arrays specify all the names that can be entered from
the command line for a given type. For example, for the type 'bin' can be
entered equivalently also 'BIN', 'binary', etc..
Note that other names can be freely added to the list without any problem.
-----------------------------------------------------------------------------*/
struct codify {
    unsigned id;
    unsigned signf;
    unsigned signt;
    unsigned decimal;
    const char name[10][40];
};

const struct codify code[] = {

        {.id = 0},
        {.id = BCD, .signf = 0, .signt = 0, .decimal = 0, .name = {"Binary Coded Decimal", "bcd", "BCD"}},
        {.id = BIN, .signf = 1, .signt = 1, .decimal = 1, .name = {"Binary Base", "bin", "BIN", "binary", "BINARY","2"}},
        {.id = CO1, .signf = 0, .signt = 1, .decimal = 0, .name = {"Ones' Complement", "c1", "C1", "co1", "CO1"}},
        {.id = CO2, .signf = 0, .signt = 1, .decimal = 0, .name = {"Two's Complement", "c2", "C2", "co2", "CO2"}},
        {.id = DEC, .signf = 1, .signt = 1, .decimal = 1, .name = {"Decimal Base", "dec", "DEC", "decimal", "DECIMAL","10"}},
        {.id = FLT, .signf = 1, .signt = 1, .decimal = 1, .name = {"Floating Point", "flt", "FLT"}},
        {.id = HEX, .signf = 1, .signt = 1, .decimal = 1, .name = {"Hexadecimal Base", "hex", "HEX", "hexadecimal","HEXADECIMAL", "16"}},
        {.id = MES, .signf = 0, .signt = 1, .decimal = 0, .name = {"Signed Magnitude Representation", "ms", "MS", "mes","MES"}},
        {.id = OCT, .signf = 1, .signt = 1, .decimal = 1, .name = {"Octal Base", "oct", "OCT", "octal", "OCTAL", "8"}},
        {.id = ROM, .signf = 0, .signt = 0, .decimal = 0, .name = {"Roman Numerals", "rom", "ROM", "roman", "ROMAN"}}

};

/* PRECISION - Determines the accuracy of the conversion from numbers in base
decimal (of the set R+) to base X in the function 'dec_to_rad()'.
-----------------------------------------------------------------------------*/
#define PRECISION (20)

/* SCRAP - Value required in the "optarg_define()" function to differentiate
the return value of baseX from the others. It is recommended not to change
this value. If necessary, take into account that SCRAP must necessarily take
a value greater than the last variable in the previous enumeration.
-----------------------------------------------------------------------------*/
#define SCRAP (100)

/* VERSION - String containing the name and version of this program.
-----------------------------------------------------------------------------*/
#define VERSION "BACO Base Converter 2.2"

/* Libraries
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <getopt.h>

/* Execution functions
-----------------------------------------------------------------------------*/
const char *conversion(unsigned, unsigned, char *, char *);

int format_scan(const char *, unsigned, unsigned, const char *);

int optarg_define(const char *);

void print_help(const char *);

/* To decimal conversion functions
-----------------------------------------------------------------------------*/
long double bcd_to_dec(const char *);

long double co1_to_dec(const char *);

long double co2_to_dec(const char *);

long double mes_to_dec(const char *);

long double rad_to_dec(const char *, unsigned);

long double rom_to_dec(const char *);

/* From decimal conversion functions
-----------------------------------------------------------------------------*/
const char *dec_to_bcd(long double, char *);

const char *dec_to_co1(long double, char *);

const char *dec_to_co2(long double, char *);

const char *dec_to_flt(long double, char *);

const char *dec_to_mes(long double, char *);

const char *dec_to_rad(long double, unsigned, char *);

const char *dec_to_rom(long double);

/* Auxiliary functions
-----------------------------------------------------------------------------*/
const char *binary_sum1(char *);

const char *bit_number(char *, unsigned, unsigned);

char *c1_converter(const char *);

int check_base(const char *, unsigned);

const char *remove_symbols(char *);

/* Main
-----------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
    unsigned from = 0, to = 0, bit = 0;

    const struct option long_options[] =
            {
                    {"help",    0, NULL, 'h'},
                    {"version", 0, NULL, 'v'},
                    {"bit",     1, NULL, 'b'},
                    {"from",    1, NULL, 'b'},
                    {"to",      1, NULL, 't'},
                    {NULL,      0, NULL, 0}
            };

    unsigned c, opt;

    while ((c = getopt_long(argc, argv, "hvb:f:t:", long_options, NULL)) != -1) {
        if (optarg != NULL && c != 'b') {
            /* Check if optarg is a valid codify */
            if (!(opt = optarg_define(optarg))) {
                fprintf(stderr, "'%s' is not a valid option.\n", optarg);
                exit(EXIT_FAILURE);
            }

            /* If the entered radix is not between 2 and 36
             * print an error message and exit */
            if (opt == SCRAP) {
                fprintf(stderr, "Insert a radix between 1 and 36.\n");
                exit(EXIT_FAILURE);
            }
        }

        switch (c) {
            case 'f':
                from = opt;
                break;

            case 't':
                to = opt;
                break;

            case 'b':
                bit = atoi(optarg);
                break;

            case 'h':
                print_help(argv[0]);
                exit(EXIT_SUCCESS);

            case 'v':
                printf("%s\n", VERSION);
                exit(EXIT_SUCCESS);

            case '?':
                exit(EXIT_FAILURE);
        }
    }

    /* Check that the entered string contains valid characters */
    if (format_scan(argv[optind], from, to, argv[0]))
        exit(EXIT_FAILURE);

    char *val = malloc(1024);

    if (!val) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }

    if (!conversion(from, to, argv[optind], val))
        exit(EXIT_FAILURE);

    printf("%s\n", val);

    free(val);

    exit(EXIT_SUCCESS);
}


/*=============================================================================
 * EXECUTION FUNCTIONS
=============================================================================*/

/* CONVERSION - Perform the conversions by calling the appropriate functions.
-----------------------------------------------------------------------------*/
const char *conversion(unsigned from, unsigned to, char *str, char *val) {
    long double x;

    switch (from) {
        case BCD: {
            if ((x = bcd_to_dec(str)) == -1) {
                fprintf(stderr, "BCD codify is not correct.\n");
                return NULL;
            }

            break;
        }

        case BIN:
            x = rad_to_dec(str, 2);
            break;

        case CO1:
            x = co1_to_dec(str);
            break;

        case CO2:
            x = co2_to_dec(str);
            break;

        case DEC:
            x = atof(str);
            break;

        case MES:
            x = mes_to_dec(str);
            break;

        case ROM: {
            if (!(x = rom_to_dec(str)))
                return NULL;

            break;
        }

        default: {
            /* Unary base */
            if (from - SCRAP == 1) {
                if (strrchr(str, '.') || strrchr(str, '-')) {
                    fprintf(stderr, "Unary numeral system admits only natural numbers.\n");
                    exit(EXIT_FAILURE);
                }

                x = strlen(str);

                /* This assignment only serves to run the precision within the
                 * following to-DEC printf: it is assigned the value ROM (Roman
                 * Numerals) as the unary base only accepts positive integers. */
                from = ROM;
            }

            /* Other numerical bases */
            else {
                x = rad_to_dec(str, from - SCRAP);

                /* This assignment only serves to run the precision within the following
                 * to-DEC printf: the given the BIN value as the base X accepts any value */
                from = BIN;
            }
        }
    }

    switch (to) {
        case BCD:
            return dec_to_bcd(x, val);
        case BIN:
            return dec_to_rad(x, 2, val);
        case CO1:
            return dec_to_co1(x, val);
        case CO2:
            return dec_to_co2(x, val);
        case DEC:
            return dec_to_rad(x, 10, val);
        case FLT:
            return dec_to_flt(x, val);
        case MES:
            return dec_to_mes(x, val);
        case ROM:
            return dec_to_rom(x);

        default : {
            /* Unary base */
            if (to - SCRAP == 1) {
                if (strrchr(str, '.') || strrchr(str, '-')) {
                    fprintf(stderr, "Unary numeral system admits only natural numbers.\n");
                    return NULL;
                }

                for (unsigned i = 0; i < x; i++)
                    val[i] = '0';

                val[(int) x] = '\0';

                return val;
            }

            /* Other numerical bases */
            return dec_to_rad(x, to - SCRAP, val);
        }
    }
}

/* FORMAT_SCAN - Checks that the format of the entered number respects the
 * format required to perform the conversion requested by the user.
-----------------------------------------------------------------------------*/
int format_scan(const char *num, unsigned from, unsigned to, const char *name) {
    int error = 0, decimal = 0, sign = -1;

    /* "from" (source) or "to" (destination) are empty */
    if (!from || !to) {
        fprintf(stderr, "Usage: conv -f <CODIFY> -t <CODIFY> <NUMBER>\n"
                        "Use «%s --help » for more informations.\n", name);
        return 1;
    }

    /* "from" (source) or "to" (destination) are the same */
    if (from == to) {
        fprintf(stderr, "Source and destination are the same.\n");
        return 1;
    }

    for (unsigned i = 0; i < strlen(num); i++) {
        if (num[i] == '-')
            sign = i;

        else if (num[i] == '.')
            decimal++;

        else if (isdigit(num[i]) && isalpha(num[i]))
            error++;
    }

    switch (error) {
        /* No errors found */
        case 0:
            break;

        /* There are errors */
        default:
            fprintf(stderr, "The codify is not correct.\n");
            return 1;
    }

    switch (decimal) {
        /* No decimal point */
        case 0:
            break;

        /* Decimal point is present */
        case 1: {
            for (unsigned i = 0; i < (sizeof(code) / sizeof(struct codify)); i++)
                if (from == code[i].id || to == code[i].id)
                    if (!code[i].decimal) {
                        fprintf(stderr, "%s accepts only integer.\n", code[i].name[0]);
                        return 1;
                    }
            break;
        }

        /* Multiple points are present */
        default :
            fprintf(stderr, "The codify is not correct.\n");
            return 1;
    }

    switch (sign) {
        /* No minus */
        case -1:
            break;

        /* Minus is present */
        case 0: {
            for (unsigned i = 0; i < (sizeof(code) / sizeof(struct codify)); i++) {
                if (from == code[i].id)
                    if (!code[i].signf) {
                        fprintf(stderr, "%s accepts only positive numbers.\n", code[i].name[0]);
                        return 1;
                    }

                if (to == code[i].id)
                    if (!code[i].signt) {
                        fprintf(stderr, "%s accepts only positive numbers.\n", code[i].name[0]);
                        return 1;
                    }
            }

            break;
        }

        /* Multiple minus are present */
        default:
            fprintf(stderr, "The codify is not correct.\n");
            return 1;
    }

    /* Check whether the number is correct for the indicated base */
    switch (from) {
        case BIN:
        case CO1:
        case CO2:
        case MES:

            if (check_base(num, 2)) return 1;
            break;

        case BCD:
        case DEC:
        case ROM:

            break;

        default :

            if (check_base(num, from - SCRAP)) return 1;
    }

    return 0;
}

/* OPTARG_DEFINE - Defines the type of conversion.
-----------------------------------------------------------------------------*/
int optarg_define(const char *type) {
    /* BCD */
    for (unsigned i = 0; i < (sizeof code[BCD].name / sizeof(const char **)); i++)
        if (!strcmp(type, code[BCD].name[i]))
            return BCD;

    /* Binary */
    for (unsigned i = 0; i < (sizeof code[BIN].name / sizeof(const char **)); i++)
        if (!strcmp(type, code[BIN].name[i]))
            return BIN;

    /* Ones' Complement */
    for (unsigned i = 0; i < (sizeof code[CO1].name / sizeof(const char **)); i++)
        if (!strcmp(type, code[CO1].name[i]))
            return CO1;

    /* Two's complement */
    for (unsigned i = 0; i < (sizeof code[CO2].name / sizeof(const char **)); i++)
        if (!strcmp(type, code[CO2].name[i]))
            return CO2;

    /* Decimal */
    for (unsigned i = 0; i < (sizeof code[DEC].name / sizeof(const char **)); i++)
        if (!strcmp(type, code[DEC].name[i]))
            return DEC;

    /* Floating Point */
    for (unsigned i = 0; i < (sizeof code[FLT].name / sizeof(const char **)); i++)
        if (!strcmp(type, code[FLT].name[i]))
            return FLT;

    /* Hexadecimal */
    for (unsigned i = 0; i < (sizeof code[HEX].name / sizeof(const char **)); i++)
        if (!strcmp(type, code[HEX].name[i]))
            return SCRAP + 16;

    /* Signed Magnitude Representation */
    for (unsigned i = 0; i < (sizeof code[MES].name / sizeof(const char **)); i++)
        if (!strcmp(type, code[MES].name[i]))
            return MES;

    /* Roman */
    for (unsigned i = 0; i < (sizeof code[ROM].name / sizeof(const char **)); i++)
        if (!strcmp(type, code[ROM].name[i]))
            return ROM;

    /* Octal */
    for (unsigned i = 0; i < (sizeof code[OCT].name / sizeof(const char **)); i++)
        if (!strcmp(type, code[OCT].name[i]))
            return SCRAP + 8;

    /* Base X */
    if (!strncmp(type, "base", 4) || !strncmp(type, "BASE", 4)) {
        int base = atoi(strrchr(type, 'e') + 1);

        if (base > 0 && base < 37)
            return SCRAP + base;

        return SCRAP;
    }

    return 0;
}

/* PRINT_HELP - Show help message.
-----------------------------------------------------------------------------*/
void print_help(const char *name) {
    printf(
            "%s\n"
            "Radix and numerical codes converter\n\n"
            "Usage: %s -f <CODIFY> -t <CODIFY> <NUMBER>\n\n"

            "Options:\n\n"

            " -f, --from            Source encoding\n"
            " -t, --to              Destination encoding\n"
            " -b  --bit             Number of bit/digit\n"
            " -h, --help            Show this help message and exit\n"
            " -v, --version         Show version and exit\n\n"

            "Codifies:\n\n"

            " BASEX                 Generic Base\n"
            " BCD                   Binary Coded Decimal\n"
            " BIN                   Binary Base\n"
            " CO1                   Ones' Complement\n"
            " CO2                   Two's Complement\n"
            " DEC                   Decimal Base\n"
            " HEX                   Hexadecimal Base\n"
            " MES                   Signed Magnitude Representation\n"
            " OCT                   Octal Base\n"
            " ROM                   Roman Numerals\n\n"

            "Examples:\n"
            " %s -f dec -t bin 18.05          It converts from base 10 to base 2\n"
            " %s -f bin -t base15 1010011010  It converts from base 2 to base 15\n\n"

            "To enter a negative number type: -- <NUMBER>\n"
            "For example, to enter the number -5 type: -- -5\n\n"

            "Report bugs to <norisgit@gmail.com>\n"

            , VERSION, name, name, name);
}


/*=============================================================================
 * TO DECIMAL CONVERSION FUNCTIONS
=============================================================================*/

/* BCD_TO_DEC - Converts a BCD-encoded number to a decimal number.
 * Returns -1 in the event of an error (if the BCD encoding is incorrect).
-----------------------------------------------------------------------------*/
long double bcd_to_dec(const char *bcd) {
    unsigned len = strlen(bcd);
    long double dec = 0;

    /* If any numbers are missing in the encoding, it returns an error: remember
     * that BCD encoding provides four bits to represent each decimal digit. */
    if (len % 4 != 0)
        return -1;

    /* The index i scans each bit of the number encoded in BCD. The index j defines
     * the significance of the bits. Initially is set to 10^(len/4-1) which specifies
     * whether the four bits being analysed correspond to units, tens, hundreds, etc.
     * For example if the number encoded in BCD is 000100100011 (i.e. 123) then len=3
     * and therefore j is initially set to j=10^(3-1)=100. In fact the most significant
     * bit of 123 belongs to the hundreds. This value is then multiplied by the
     * respective BCD encoding (via if). The cycle continues by dividing j by 10, so
     * that the significance of the next four bits is correct. This is done until the
     * bits are finished.
     */
    for (unsigned i = 0, j = powl(10, len / 4 - 1); i < len; i += 4, j /= 10) {
        if (bcd[i] == '0' && bcd[i + 1] == '0' && bcd[i + 2] == '0' && bcd[i + 3] == '1')
            dec += 1 * j;
        else if (bcd[i] == '0' && bcd[i + 1] == '0' && bcd[i + 2] == '1' && bcd[i + 3] == '0')
            dec += 2 * j;
        else if (bcd[i] == '0' && bcd[i + 1] == '0' && bcd[i + 2] == '1' && bcd[i + 3] == '1')
            dec += 3 * j;
        else if (bcd[i] == '0' && bcd[i + 1] == '1' && bcd[i + 2] == '0' && bcd[i + 3] == '0')
            dec += 4 * j;
        else if (bcd[i] == '0' && bcd[i + 1] == '1' && bcd[i + 2] == '0' && bcd[i + 3] == '1')
            dec += 5 * j;
        else if (bcd[i] == '0' && bcd[i + 1] == '1' && bcd[i + 2] == '1' && bcd[i + 3] == '0')
            dec += 6 * j;
        else if (bcd[i] == '0' && bcd[i + 1] == '1' && bcd[i + 2] == '1' && bcd[i + 3] == '1')
            dec += 7 * j;
        else if (bcd[i] == '1' && bcd[i + 1] == '0' && bcd[i + 2] == '0' && bcd[i + 3] == '0')
            dec += 8 * j;
        else if (bcd[i] == '1' && bcd[i + 1] == '0' && bcd[i + 2] == '0' && bcd[i + 3] == '1')
            dec += 9 * j;
        else
            return -1;
    }

    return dec;
}

/* C1_TO_DEC - Converts a binary ones' complement number to decimal.
 * It doesn't check if the passed number is actually binary: you must therefore
 * perform this check before calling the function.
-----------------------------------------------------------------------------*/
long double co1_to_dec(const char *c1) {
    /* If the number starts with 1 then is negative */
    if (c1[0] == '1')

        /* I perform the ones' complement of the number then
         * I convert from binary to decimal, and I put the
         * minus sign because it is negative */
        return rad_to_dec(c1_converter(c1), 2) * -1;

    /* If the number starts with 0 then this is positive,
     * and I simply convert it from binary to decimal */
    return rad_to_dec(c1, 2);
}

/* C2_TO_DEC - Converts a binary two's complement number to decimal.
Needs a function that converts from binary to ones' complement. It does not
check that the number passed is actually binary: you must therefore perform
this check before calling the function.
-----------------------------------------------------------------------------*/
long double co2_to_dec(const char *c2) {
    /* If the number starts with 1 then is negative */
    if (c2[0] == '1')
        return co1_to_dec(c2) - 1;

    /* If the number starts with 0 then this is positive,
     * and I simply convert it from binary to decimal */
    return rad_to_dec(c2, 2);
}

/* MES_TO_DEC - Converts signed magnitude representation binary number into a
 * decimal number. It does not check that the number passed is binary: you must
 * therefore perform this action before calling the function.
-----------------------------------------------------------------------------*/
long double mes_to_dec(const char *ms) {
    char *m = malloc(strlen(ms));

    if (!m) {
        fprintf(stderr, "Memory allocation error.\n");
        return -1;
    }

    strcpy(m, ms);

    /* Set the first bit to 0 to perform
     * a correct conversion to decimal */
    m[0] = '0';

    /* Convert the number to decimal */
    long double dec = rad_to_dec(m, 2);

    free(m);

    /* If the first digit of the number in SMR is 1 then
     * the number is negative (I multiply it by -1) */
    if (ms[0] == '1')
        return dec * -1;

    /* If the first digit is 0 then the number is
     * positive, and I return it without changing it */
    return dec;
}

/* RAD_TO_DEC - Converts a number whatever base to decimal.
-----------------------------------------------------------------------------*/
long double rad_to_dec(const char *num, unsigned base) {
    long double dec = 0;
    unsigned sign = 0;
    char num_int[100], num_dec[100];

    if (num[0] == '-') {
        sign++;
        num++;
    }

    /* If there is a decimal part I separate it from the integer part */
    if (strchr(num, '.') != NULL) {
        strcpy(num_dec, strchr(num, '.') + 1);
        unsigned len = strlen(num) - strlen(num_dec) - 1;
        strncpy(num_int, num, len);
        num_int[len] = '\0';
    }

    /* If there is no decimal part I save only the integer part */
    else
        strcpy(num_int, num);

    /* Convert the integer part to base X */
    for (unsigned i = strlen(num_int), j = 0; i > 0; i--)
        if (isdigit(num_int[i - 1]))
            dec += ((num_int[i - 1] - '0') * powl(base, j++));
        else
            dec += ((toupper(num_int[i - 1]) - 'A' + 10) * powl(base, j++));

    /* Convert decimal part to decimal */
    if (strchr(num, '.') != NULL)
        for (int i = 0, j = 0; i < strlen(num_dec); i++)
            if (isdigit(num_dec[i]))
                dec += ((num_dec[i] - '0') * powl(base, --j));
            else
                dec += ((toupper(num_dec[i]) - 'A' + 10) * powl(base, --j));

    if (sign)
        return -1 * dec;

    return dec;
}

/* ROM_TO_DEC - Converts from Roman numeration system to decimal.
-----------------------------------------------------------------------------*/
long double rom_to_dec(const char *rom) {
    /* Assign a priority to each symbol, increasing in value, from 1 to 7: if
     * the character has a lower priority than the one to its left I sum it up,
     * otherwise I subtract it. This is cycled for each character of the string,
     * starting with the rightmost (last) character. */

    enum {
        no, pi, pv, px, pl, pc, pd, pm
    };
    long double dec = 0;
    unsigned priority = 0;

    for (unsigned i = strlen(rom); i > 0; i--)
        switch (toupper(rom[i - 1])) {
            case 'I':
                if (priority <= pi)
                    dec += 1;
                else
                    dec -= 1;

                priority = pi;
                break;

            case 'V':
                if (priority <= pv)
                    dec += 5;
                else
                    dec -= 5;

                priority = pv;
                break;

            case 'X':
                if (priority <= px)
                    dec += 10;
                else
                    dec -= 10;

                priority = px;
                break;

            case 'L':
                if (priority <= pl)
                    dec += 50;
                else
                    dec -= 50;

                priority = pl;
                break;

            case 'C':
                if (priority <= pc)
                    dec += 100;
                else
                    dec -= 100;

                priority = pc;
                break;

            case 'D':
                if (priority <= pd)
                    dec += 500;
                else
                    dec -= 500;

                priority = pd;
                break;

            case 'M':
                if (priority <= pm)
                    dec += 1000;
                else
                    dec -= 1000;

                priority = pm;
                break;

            default :
                fprintf(stderr, "The codify is not correct.\n");
                return 0;
        }

    return dec;
}


/*=============================================================================
FROM DECIMAL CONVERSION FUNCTIONS
=============================================================================*/

/* DEC_TO_BCD - Converts from decimal (positive integer) to BCD encoding.
-----------------------------------------------------------------------------*/
const char *dec_to_bcd(long double dec, char *bcd) {
    char str[128];

    sprintf(str, "%.0Lf", dec);

    for (unsigned i = 0; i < strlen(str); i++)
        switch (str[i]) {
            case '0':
                strcat(bcd, "0000");
                break;
            case '1':
                strcat(bcd, "0001");
                break;
            case '2':
                strcat(bcd, "0010");
                break;
            case '3':
                strcat(bcd, "0011");
                break;
            case '4':
                strcat(bcd, "0100");
                break;
            case '5':
                strcat(bcd, "0101");
                break;
            case '6':
                strcat(bcd, "0110");
                break;
            case '7':
                strcat(bcd, "0111");
                break;
            case '8':
                strcat(bcd, "1000");
                break;
            case '9':
                strcat(bcd, "1001");
                break;
            default :
                fprintf(stderr, "Unhandled exception.\n");
                return NULL;
        }

    return bcd;
}

/* DEC_TO_CO1 - Convert from decimal to ones' complement.
-----------------------------------------------------------------------------*/
const char *dec_to_co1(long double dec, char *co1) {
    char *val = malloc(128);

    if (!val) {
        fprintf(stderr, "Memory allocation error.\n");
        return NULL;
    }

    /* Convert to binary */
    strcpy(co1, dec_to_rad(dec < 0 ? -1 * dec : dec, 2, val));

    if (dec < 0) {
        /* Perform the ones' complement */
        strcpy(co1, c1_converter(co1));

        /* Add '1' in front of the number */
        unsigned len = strlen(co1);
        for (unsigned i = 0; i < len; i++)
            co1[len - i] = co1[len - i - 1];

        co1[0] = '1';
        co1[len + 1] = '\0';
    } else {
        /* If the number is positive I add
         * a zero to the binary number */
        sprintf(val, "0%s", co1);
        strcpy(co1, val);
    }

    free(val);

    return co1;
}

/* DEC_TO_CO2 - Convert from decimal to two's complement.
-----------------------------------------------------------------------------*/
const char *dec_to_co2(long double dec, char *co2) {
    char *val = malloc(128);

    if (!val) {
        fprintf(stderr, "Memory allocation error.\n");
        return NULL;
    }

    /* Convert to ones' complement */
    strcpy(co2, dec_to_co1(dec, val));

    free(val);

    if (dec > -1)
        return co2;

    /* Add 1 */
    return binary_sum1(co2);
}

/* DEC_TO_FLT - Convert from decimal to floating point.
-----------------------------------------------------------------------------*/
const char *dec_to_flt(long double dec, char *flt) {
    /* Save the sign of the number */
    if (dec < 0) {
        dec *= -1;
        strcpy(flt, "1");
    } else
        strcpy(flt, "0");

    char *val = malloc(128);
    char *tmp = malloc(128);

    if (!val || !tmp) {
        fprintf(stderr, "Memory allocation error.\n");
        return NULL;
    }

    sprintf(tmp, "%Lf", dec);

    /* Save the exponent in two's complement */
    unsigned len = strlen(tmp) - strlen(strchr(tmp, '.'));
    strcat(flt, dec_to_co2(len, val));

    strcat(flt, dec_to_rad(atof(remove_symbols(tmp)), 2, val));

    free(val);
    free(tmp);

    //return flt;
    return "TODO";
}

/* DEC_TO_MES - Converts from signed magnitude representation to decimal.
-----------------------------------------------------------------------------*/
const char *dec_to_mes(long double dec, char *mes) {
    char *val = malloc(128);

    /* Convert the number to binary: if it is negative I add
     * a 1 in front of the string, otherwise I add a zero */
    sprintf(mes, "%s%s", dec < 0 ? "1" : "0", dec_to_rad(dec < 0 ? -1 * dec : dec, 2, val));

    free(val);

    return mes;
}

/* DEC_TO_RAD - Convert from decimal to base X.
-----------------------------------------------------------------------------*/
const char *dec_to_rad(long double num, unsigned base, char *bin) {
    char str_int[128], str_dec[128], tmp[128];
    unsigned sign = 0;

    /* If the number is negative I increase a counter to insert the sign
     * later in the conversion string, and remove it from the number */
    if (num < 0) {
        sign++;
        num *= -1;
    }

    /* Put in "num_int" the integer part of the number
     * and in "num_int" the decimal part of the number */
    long double num_int;
    long double num_dec = modfl(num, &num_int);

    /* Convert the integer part from decimal to base X, dividing by base and
     * saving the remainder: initially the number in base X will be reversed */
    do {
        unsigned v;

        if ((v = fmodl(num_int, base)) < 10)
            sprintf(tmp, "%u", v);
        else
            sprintf(tmp, "%c", v - 10 + 'A');

        strcat(bin, tmp);

    } while ((int) (num_int /= base));

    /* If the number is negative I add a minus */
    if (sign)
        strcat(bin, "-");

    /* Reverse the number */
    unsigned len = strlen(bin);
    for (unsigned i = 0; i < len / 2; i++) {
        tmp[0] = bin[i];
        bin[i] = bin[len - i - 1];
        bin[len - i - 1] = tmp[0];
    }

    /* I convert the decimal part from decimal to base X, multiplying by base and
     * saving the first digit of the result (< base), which will then be discarded
     * at each cycle (e.g. for 1.27 is saved 1, and then it becomes 0.27) */
    if (num_dec) {
        
        /* Insert decimal point */
        strcat(bin, ".");

        for (unsigned i = 0; i < PRECISION; i++) {
            num_dec *= base;

            /* The decimal part of the number is stored in 'num_dec': I convert it to
             * an integer (the first time it will obviously be 0, later it may take
             * values less than base) and save the result in "num_int" */
            sprintf(str_int, "%Lf", num_dec);
            unsigned value = atof(str_int);

            /* The preceding instruction could be
             * alternatively replaced by the following: */
            //unsigned value = str_int[0] - '0';

            /* Save the i-th part of the decimal part of the number in base X:
            the first character of "str_int" will in fact always be < base */
            len = strlen(bin);

            if (value < 10) {
                bin[len] = str_int[0];
                bin[len + 1] = '\0';
            } else {
                char buff[3];
                sprintf(buff, "%c", (unsigned) value - 10 + 'A');
                strcat(bin, buff);
            }

            /* Save in 'num_dec' the decimal part of the decimal number */
            sprintf(str_dec, "%Lf", num_dec - value);
            num_dec = atof(str_dec);
        }
    }

    return bin;
}

/* DEC_TO_ROM - Converts from decimal to Roman numeration system.
-----------------------------------------------------------------------------*/
const char *dec_to_rom(long double dec) {
    /* The equivalent of 0 is the latin word "nulla" */
    if (dec == 0)
        return "NULL";

    char dec_str[128];
    sprintf(dec_str, "%Lf", dec);

    for (unsigned i = strlen(dec_str); i > 0; i--) {
        switch (dec_str[i]) {
            case '0':
                break;
            case '1':
                break;
        }
    }

    /* TODO */

    return "TODO";
}


/*=============================================================================
 * AUXILIARY FUNCTIONS
=============================================================================*/

/* BINARY_SUM1 - Add 1 to a binary number.
-----------------------------------------------------------------------------*/
const char *binary_sum1(char *bin) {
    unsigned len = strlen(bin), count = 0;
    unsigned *to_zero = malloc(len * sizeof(unsigned));

    for (unsigned i = len; i > 0; i--) {
        if (bin[i - 1] == '1')
            to_zero[count++] = i - 1;

        else {
            bin[i - 1] = '1';

            for (unsigned j = 0; j < count; j++)
                bin[to_zero[j]] = '0';

            return bin;
        }
    }

    bin[0] = '1';

    for (unsigned j = 1; j < count + 1; j++)
        bin[j] = '0';

    bin[count + 1] = '\0';

    return bin;
}

/* BIT_NUMBER - It prints only the first n bits of a string.
-----------------------------------------------------------------------------*/
const char *bit_number(char *str, unsigned bit, unsigned phase) {
    unsigned len = strlen(str);

    if (bit == 0)
        return str;

    if (len > bit && phase) {
        fprintf(stderr, "Too few bit. It requires almost %u bit.\n", len);
        return NULL;
    }

    char *str_cpy = malloc(len);
    strcpy(str_cpy, str);
    strcpy(str, "");

    for (unsigned i = 0; i < (bit - len); i++)
        strcat(str, "0");

    strcat(str, str_cpy);

    free(str_cpy);

    return str;
}

/* C1_CONVERTER - Performs the ones' complement of the binary number entered.
 * Doesn't check if the number is binary: you must therefore perform this
 * check before calling the function.
-----------------------------------------------------------------------------*/
char *c1_converter(const char *bin) {
    unsigned len = strlen(bin);
    char *c1 = malloc(len + 1);

    if (!c1) {
        fprintf(stderr, "Memory allocation error.\n");
        return NULL;
    }

    for (unsigned i = 0; i < len; i++)
        c1[i] = bin[i] == '0' ? '1' : '0';

    // TODO FREE

    return c1;
}

/* CHECK_BASE - Returns 0 if the passed string contains only the digits allowed
 * for that particular base (e.g. 0,1,2,3,4,5,6,7 in octal base or 0 and 1 in
 * binary base). Returns 1 if invalid values are contained.
-----------------------------------------------------------------------------*/
int check_base(const char *x, unsigned base) {
    unsigned count = 0, point = 0;

    /* The minus in the first position is a valid character */
    if (x[0] == '-')
        count++;

    /* Increase count at each valid character */
    for (unsigned i = 0; i < strlen(x); i++) {
        /* The character is valid if it is correct for that base */
        if (x[i] - '0' < base)
            count++;

        /* The letter must be correct for that particular base */
        else if (toupper(x[i]) >= 'A' && toupper(x[i]) <= base + 'A' - 11)
            count++;

        /* Count the points present (there may be 0 or 1) */
        else if (x[i] == '.') {
            count++;
            point++;
        }
    }

    /* The string is correct if count is equal to the length of
     * the string and if the number of decimal points is 0 or 1 */
    if (count == strlen(x) && point < 2)
        return 0;

    fprintf(stderr, "Inserted number is not in base %u.\n", base);
    return 1;
}

/* REMOVE_SYMBOLS - Removes everything other than a number.
-----------------------------------------------------------------------------*/
const char *remove_symbols(char *str) {
    for (unsigned i = 0, j = 0; i < strlen(str) + 1; i++)
        if (isdigit(str[i]) || str[i] == '\0')
            str[j++] = str[i];
    return str;
}