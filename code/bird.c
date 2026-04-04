#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    BIRD_APPLICATION,
    BIRD_S,
    BIRD_K,
    BIRD_B,
    BIRD_T,
    BIRD_VARIBALE
} BirdType;

typedef struct Bird {
    BirdType bird_type;
    char name;
    int number;
    struct Bird* left;
    struct Bird* right;
} Bird;

#define NO_NUMBER -1
#define NO_NAME 0
#define NO_CHILD NULL

Bird* create_bird(BirdType bird_type, char name, int number, Bird* left, Bird* right) {
    Bird* bird = (Bird*) malloc(sizeof(Bird));

    bird->bird_type = bird_type;
    bird->name = name;
    bird->number = number;
    bird->left = left;
    bird->right = right;

    return bird;
}

void free_bird(Bird* bird) {
    if (!bird) {
        return;
    }

    free_bird(bird->left);
    free_bird(bird->right);
    free(bird);
}

Bird* copy_bird(Bird* bird) {
    if (!bird) {
        return NULL;
    }

    return create_bird(bird->bird_type, bird->name, bird->number, copy_bird(bird->left), copy_bird(bird->right));
}

bool is_variable(char c) {
    return 'a' <= c && c <= 'z';
}

bool is_digit(char c) {
    return '0' <= c && c <= '9';
}

int char_to_int(char c) {
    return c - '0';
}

Bird* parse_expr(const char** p);
Bird* parse_term(const char** p);
Bird* parse_bird(const char** p);

Bird* parse_expr(const char** p) {
    Bird* term = parse_term(p);

    if (!term) {
        return NULL;
    }

    // parse as many terms as possible and join them with application
    while (**p != '\0' && **p != ')') {
        term = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, term, parse_term(p));
    }

    return term;
}

Bird* parse_term(const char** p) {
    if (**p == '\0' || **p == ')') {
        return NULL;
    }

    // parse sub expression (x)
    if (**p == '(') {
        (*p)++;
        Bird* expr = parse_expr(p);
        if (**p != ')') {
            fprintf(stderr, "Missing closing parenthesis.\n");
            return NULL;
        }
        (*p)++;
        return expr;
    }

    return parse_bird(p);
}

Bird* parse_bird(const char** p) {
    char name;
    int number = NO_NUMBER;

    name = **p;
    (*p)++;
    
    if (is_digit(**p)) {
        number = char_to_int(**p);
        (*p)++;
    }

    // parse variable bird
    if (is_variable(name)) {
        return create_bird(BIRD_VARIBALE, name, number, NO_CHILD, NO_CHILD);
    }
    
    // starling Sxyz -> xz(yz)
    if (name == 'S' && number == NO_NUMBER) {
        return create_bird(BIRD_S, name, NO_NUMBER, NO_CHILD, NO_CHILD);
    }

    // kestrel Kxy -> x
    if (name == 'K' && number == NO_NUMBER) {
        return create_bird(BIRD_K, name, NO_NUMBER, NO_CHILD, NO_CHILD);
    }

    // idiot Ix -> x
    if (name == 'I' && number == NO_NUMBER) {
        const char* bird = "SKK";
        return parse_expr(&bird);
    }

    // mockingbird Mx -> xx
    if (name == 'M' && number == NO_NUMBER) {
        const char* bird = "SII";
        return parse_expr(&bird);
    }

    // warbler Wxy -> xyy
    if (name == 'W' && number == NO_NUMBER) {
        const char* bird = "C(BM(BBT))";
        return parse_expr(&bird);
    }

    // lark Lxy -> w(yy)
    if (name == 'L' && number == NO_NUMBER) {
        const char* bird = "CBM";
        return parse_expr(&bird);
    }

    // compositor birds

    // bluebird Bxyz -> x(yz)
    if (name == 'B' && number == NO_NUMBER) {
        return create_bird(BIRD_B, name, number, NO_CHILD, NO_CHILD);
        // const char* bird = "S(KS)K";
        // return parse_expr(&bird);
    }

    // blackbird B1xyzw -> x(yzw)
    if (name == 'B' && number == 1) {
        const char* bird = "BBB";
        return parse_expr(&bird);
    }

    // bunting B2xyzwv -> x(yzwv)
    if (name == 'B' && number == 2) {
        const char* bird = "BB(BBB)";
        return parse_expr(&bird);
    }

    // becard B3xyzwv -> x(y(zw))
    if (name == 'B' && number == 3) {
        const char* bird = "B(BB)B";
        return parse_expr(&bird);
    }

    // dove Dxyzw -> xy(zw)
    if (name == 'D' && number == NO_NUMBER) {
        const char* bird = "BB";
        return parse_expr(&bird);
    }

    // dickcissel D1xyzwv -> xyz(wv)
    if (name == 'D' && number == 1) {
        const char* bird = "B(BB)";
        return parse_expr(&bird);
    }

    // dovekie D2xyzwv -> x(yz)(wv)
    if (name == 'D' && number == 2) {
        const char* bird = "BB(BB)";
        return parse_expr(&bird);
    }

    // eagle Exyzwv -> xy(zwv)
    if (name == 'E' && number == NO_NUMBER) {
        const char* bird = "B(BBB)";
        return parse_expr(&bird);
    }

    // permuting birds

    // cardinal Cxyz -> 
    if (name == 'C') {
        const char* bird = "S(BBS)(KK)";
        return parse_expr(&bird);
    }

    // thrush Txy -> yx
    if (name == 'T') {
        return create_bird(BIRD_T, name, number, NO_CHILD, NO_CHILD);
        // const char* bird = "CI";
        // return parse_expr(&bird);
    }

    fprintf(stderr, "Unknown bird %c\n", name);
    return NULL;
}


void print_bird_(Bird* bird, bool is_right_term) {
    if (!bird) {
        return;
    }
    if (bird->bird_type == BIRD_APPLICATION) {
        if (is_right_term) {
            printf("(");
        }
        print_bird_(bird->left, false);
        print_bird_(bird->right, true);
        if (is_right_term) {
            printf(")");
        }
    } else {
        printf("%c", bird->name);
        if (bird->number != NO_NUMBER) {
            printf("%c", bird->number);
        }
    }
}

void print_bird(Bird* bird) {
    print_bird_(bird, false);
    printf("\n");
    return;
}

bool reduce_bird(Bird** bird_ptr) {
    Bird* bird = *bird_ptr;

    if (!bird || bird->bird_type != BIRD_APPLICATION) {
        return false;
    }

    // K reduction Kxy -> x
    if (
        bird->left->bird_type == BIRD_APPLICATION &&
        bird->left->left->bird_type == BIRD_K
    ) {
        Bird* x = copy_bird(bird->left->right);
        free_bird(bird);
        *bird_ptr = x;
        return true;
    }

    // S reduction Sxyz -> xz(yz)
    if (
        bird->left->bird_type == BIRD_APPLICATION &&
        bird->left->left->bird_type == BIRD_APPLICATION &&
        bird->left->left->left->bird_type == BIRD_S
    ) {
        Bird* x = copy_bird(bird->left->left->right);
        Bird* y = copy_bird(bird->left->right);
        Bird* z = copy_bird(bird->right);
        Bird* z_copy = copy_bird(z);

        free_bird(bird);

        Bird* xz = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, x, z);
        Bird* yz = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, y, z_copy);
        *bird_ptr = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, xz, yz);
        return true;
    }

    // B reduction Bxyz -> x(yz)
    if (
        bird->left->bird_type == BIRD_APPLICATION &&
        bird->left->left->bird_type == BIRD_APPLICATION &&
        bird->left->left->left->bird_type == BIRD_B
    ) {
        Bird* x = copy_bird(bird->left->left->right);
        Bird* y = copy_bird(bird->left->right);
        Bird* z = copy_bird(bird->right);

        free_bird(bird);

        Bird* yz = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, y, z);
        *bird_ptr = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, x, yz);
        return true;
    }

    // T reduction Txy -> yz
    if (
        bird->left->bird_type == BIRD_APPLICATION &&
        bird->left->left->bird_type == BIRD_T
    ) {
        Bird* x = copy_bird(bird->left->right);
        Bird* y = copy_bird(bird->right);

        free_bird(bird);

        *bird_ptr = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, y, x);
        return true;
    }

    return reduce_bird(&bird->left) || reduce_bird(&bird->right);
}

bool contains_variable(Bird* bird, char name, int number) {
    if (!bird) {
        return false;
    }

    if (bird->bird_type == BIRD_VARIBALE && bird->name == name && bird->number == number) {
        return true;
    }

    return contains_variable(bird->left, name, number) || contains_variable(bird->right, name, number);
}

// solves for a permutation bird in terms of B and T, needs a real name
Bird* big_tbone(Bird* bird, char name, int number) {
    if (!bird) {
        return NULL;
    }

    if (bird->bird_type != BIRD_APPLICATION) {
        return copy_bird(bird);
    }

    // ends in abstraction variable -> elim variable    
    if (bird->right->bird_type == BIRD_VARIBALE && bird->right->name == name && bird->right->number == number) {
        return copy_bird(bird->left);
    }

    // ends in application -> abstract with B
    // x(yz) -> Bxyz
    if (bird->right->bird_type == BIRD_APPLICATION) {
        const char* bluebird = "B";
        Bird* b = parse_expr(&bluebird);
        Bird* x = copy_bird(bird->left);
        Bird* y = copy_bird(bird->right->left);
        Bird* z = copy_bird(bird->right->right);

        Bird* bx = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, b, x);
        Bird* bxy = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, bx, y);
        Bird* bxyz = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, bxy, z);
        return bxyz;
    }

    // ends in bird -> abstract with T
    // yx -> Txy
    const char* thrush = "T";
    Bird* t = parse_bird(&thrush);
    Bird* x = copy_bird(bird->right);
    Bird* y = copy_bird(bird->left);
    Bird* tx = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, t, x);
    Bird* txy = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, tx, y);
    return txy;
}

Bird* abstract_bird(Bird* bird, char name, int number) {
    if (!bird) {
        return NULL;
    }

    // [x]x -> I
    if (bird->bird_type == BIRD_VARIBALE && bird->name == name && bird->number == number) {
        const char* new_bird = "I";
        return parse_bird(&new_bird);
    }

    // [x] E1 E1 -> S [x]E1 [x]E2
    if (bird->bird_type == BIRD_APPLICATION && contains_variable(bird, name, number)) {
        const char* starling = "S";
        Bird* s = parse_bird(&starling);
        Bird* left = abstract_bird(bird->left, name, number);
        Bird* right = abstract_bird(bird->right, name, number);
        Bird* s_left = create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, s, left);
        return create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, s_left, right);
    }

    // [x]y -> Ky
    const char* kestrel = "K";
    Bird* k = parse_bird(&kestrel);
    Bird* y = copy_bird(bird);
    return create_bird(BIRD_APPLICATION, NO_NAME, NO_NUMBER, k, y);
}

#define REQUIRED_ARGS \
    REQUIRED_STRING_ARG(expr, "expression", "Expression of birds")

#define OPTIONAL_ARGS \
    OPTIONAL_STRING_ARG(abstraction_vars, "", "-a", "abstraction variables", "Variables to abstract with bracket abstraction") \
    OPTIONAL_STRING_ARG(permutation_vars, "", "-p", "abstraction variables", "Variables to abstract with permutation abstraction")

#define BOOLEAN_ARGS \
    BOOLEAN_ARG(verbose, "-v", "Verbose output")

#include "easyargs.h"

int main(int argc, char *argv[]) {
    args_t args = make_default_args();

    if (!parse_args(argc, argv, &args)) {
        return 1;
    }

    Bird* expr = parse_expr((const char**)&args.expr);

    if (*args.abstraction_vars != '\0') {
        char* p = args.abstraction_vars;
        while (*p != '\0') {
            p++;
        }
        p--;
        Bird* abstracted;
        for (;p >= args.abstraction_vars; p--) {
            abstracted = abstract_bird(expr, *p, NO_NUMBER);
            free_bird(expr);
            expr = abstracted;
            if (args.verbose) {
                print_bird(expr);
            }
        }
    } else if (*args.permutation_vars != '\0') {
        char* p = args.permutation_vars;
        while (*p != '\0') {
            p++;
        }
        p--;
        Bird* abstracted;
        for (;p >= args.permutation_vars; p--) {
            while (contains_variable(expr, *p, NO_NUMBER)) {
                abstracted = big_tbone(expr, *p, NO_NUMBER);
                free_bird(expr);
                expr = abstracted;
                if (args.verbose) {
                    print_bird(expr);
                }
            }
        }
    } else {
        while (reduce_bird(&expr)) {
            if (args.verbose) {
                print_bird(expr);
            }
        }
    }

    if (!args.verbose) {
        print_bird(expr);
    }
    free_bird(expr);
}
