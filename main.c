/* librarian v1.0: library usage trainer for C language. renat & nn
 * renatrafikov@gmail.com GNU General Public License (GPL) v3 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  const char* lib;
  const char* calls[15];
  int call_count;
} StructData;

extern StructData database[];
extern int atom_count;

typedef struct {
  const char* code;
  const char* required[10];
  int required_count;
} GeneratedExercise;

GeneratedExercise generate_exercise(int difficulty) {
  static char buffer[2048];
  buffer[0] = '\0';

  GeneratedExercise g;
  g.required_count = 0;

  static const char* init_lines[] = {
      "int y = x * 2;",    "char buf[32] = \"hello\";",
      "double k = 3.14;",  "int arr[5] = {1,2,3,4,5};",
      "char p = 's';",     "int i = 0;",
      "unsigned u = 100;", "float f = 2.5f;",
      "long t = 100000;",  "short flag = 1;"};
  static const int init_count = 10;

  int used_inits[10] = {0};

  int max_libs;
  int lib_range;
  int calls;

  if (difficulty < 15) {
    max_libs = 1;
    lib_range = 4;
    calls = 1;
  } else if (difficulty <= 30) {
    max_libs = 1 + rand() % 2;
    lib_range = 10;
    calls = 1;
  } else {
    max_libs = 2 + rand() % 3;
    lib_range = atom_count;
    calls = 1 + rand() % 2;
  }

  strcpy(buffer, "void test(){\n");

  int extra_lines = rand() % 4;
  for (int i = 0; i < extra_lines; i++) {
    int idx;
    do {
      idx = rand() % init_count;
    } while (used_inits[idx]);
    used_inits[idx] = 1;
    strcat(buffer, "    ");
    strcat(buffer, init_lines[idx]);
    strcat(buffer, "\n");
  }

  const char* used_calls[32];
  int used_call_count = 0;

  for (int i = 0; i < calls; i++) {
    int idx = rand() % lib_range;

    int exists = 0;
    for (int j = 0; j < g.required_count; j++)
      if (strcmp(g.required[j], database[idx].lib) == 0) exists = 1;

    if (!exists && g.required_count < max_libs)
      g.required[g.required_count++] = database[idx].lib;

    const char* call;
    int unique = 0;
    int attempts = 0;

    while (!unique && attempts < 20) {
      call = database[idx].calls[rand() % database[idx].call_count];
      unique = 1;

      for (int c = 0; c < used_call_count; c++)
        if (strcmp(used_calls[c], call) == 0) unique = 0;

      attempts++;
    }

    used_calls[used_call_count++] = call;

    strcat(buffer, "    ");
    strcat(buffer, call);
    strcat(buffer, "\n");
  }

  strcat(buffer, "}\n");
  g.code = buffer;
  return g;
}

int check_answer(const GeneratedExercise* ex, const char* user_input) {
  char copy[256];
  strcpy(copy, user_input);

  int matched[10] = {0};
  int found = 0;

  char* tok = strtok(copy, " \t\n");
  while (tok) {
    for (int i = 0; i < ex->required_count; i++) {
      if (!matched[i] && strcmp(tok, ex->required[i]) == 0) {
        matched[i] = 1;
        found++;
      }
    }
    tok = strtok(NULL, " \t\n");
  }

  return found == ex->required_count;
}

int main() {
  srand((unsigned)time(NULL));
  int score = 0;

  printf("Press q to quit\n\n");

  while (1) {
    GeneratedExercise ex = generate_exercise(score);

    printf("Which libraries are required?\n");
    printf("----------------------------------\n");
    printf("%s", ex.code);
    printf("----------------------------------\n");
    printf("Type library names (example: stdio.h stdlib.h):\n> ");

    char user[256];
    if (!fgets(user, sizeof(user), stdin)) break;
    if (user[0] == 'q') break;

    if (check_answer(&ex, user)) {
      printf("\033[1;32mCorrect!\033[0m\n");
      score++;
    } else {
      printf("\033[1;31mWrong\033[0m\nAnswer: ");
      for (int i = 0; i < ex.required_count; i++)
        printf("\033[1;32m%s\033[0m ", ex.required[i]);
      printf("\n");
      if (score > 0) score--;
    }

    printf("Score: \033[1;34m%d\033[0m\n\n", score);
  }

  return 0;
}

StructData database[] = {
    {"stdio.h",
     {"printf(\"User: %s (%d)\\n\", name, id);", "scanf(\"%d\", &value);",
      "FILE* fp = fopen(\"log.txt\", \"a\");", "fclose(fp);",
      "fputs(\"ready\\n\", fp2);", "int c = fgetc(fp3);",
      "fgets(buffer, sizeof(buffer), stdin);",
      "fprintf(stderr, \"Warning: %s\\n\", msg);", "fflush(stdout);",
      "setvbuf(fp, NULL, _IONBF, 0);", "rewind(fp);", "long pos = ftell(fp);",
      "fseek(fp, 0, SEEK_END);", "remove(\"temp.txt\");",
      "rename(\"old.txt\", \"new.txt\");"},
     15},

    {"string.h",
     {"size_t len = strlen(title);",
      "if (strcmp(user, \"admin\") == 0) access = 1;", "strncpy(dst, src, 20);",
      "strcat(path, \"/config\");", "char* token = strtok(buffer, \" \");",
      "memcpy(dstbuf, srcbuf, bytes);", "memmove(arr+1, arr, count);",
      "int diff = strncmp(a, b, 5);", "char* found = strstr(text, \"error\");",
      "memset(cache, 0, sizeof(cache));", "strncat(out, suffix, 3);",
      "char* p = strchr(line, '=');", "char* q = strrchr(path, '/');",
      "if (memcmp(a, b, 16) == 0) match = 1;", "strcpy(user_copy, username);"},
     15},

    {"stdlib.h",
     {"int* arr = malloc(n * sizeof(int));", "free(arr);",
      "double* buf = calloc(20, sizeof(double));",
      "data = realloc(data, new_size);", "int r = rand() % 10;",
      "srand(time(NULL));", "double x = atof(input);",
      "long v = strtol(numstr, NULL, 10);",
      "char* end; long h = strtoll(hex, &end, 16);", "int cmp = abs(a - b);",
      "exit(EXIT_FAILURE);", "system(\"clear\");",
      "qsort(items, count, sizeof(Item), cmp_items);",
      "div_t rdiv = div(a, b);", "char* env = getenv(\"HOME\");"},
     15},

    {"math.h",
     {"double r = sqrt(x*x + y*y);", "double a = pow(base, exp);",
      "double s = sin(angle_rad);", "double c = cos(angle_rad);",
      "double t = tan(angle_rad);", "double e = exp(value);",
      "double ln = log(value);", "double lg = log10(value);",
      "double ab = fabs(delta);", "double f = floor(val);",
      "double ce = ceil(val);", "double h = hypot(x, y);",
      "double r2 = fmod(total, 7.0);", "double r3 = round(score);",
      "double m = fmax(a, b);"},
     15},

    {"ctype.h",
     {"if (isdigit(c)) numbers++;", "if (isalpha(c)) letters++;",
      "if (isalnum(c)) valid++;", "if (isspace(*p)) p++;",
      "if (isupper(c)) caps++;", "if (islower(c)) lows++;", "c = toupper(c);",
      "d = tolower(d);", "if (isxdigit(x)) hex_count++;",
      "if (ispunct(c)) punctuation++;", "while (isspace(*s)) s++;",
      "if (iscntrl(c)) ctrl++;", "if (isprint(c)) printable++;",
      "if (isgraph(c)) graph++;", "if (isblank(c)) blanks++;"},
     15},

    {"time.h",
     {"time_t now = time(NULL);", "struct tm* t = localtime(&now);",
      "strftime(date, 32, \"%Y-%m-%d\", t);", "clock_t start = clock();",
      "double secs = difftime(t2, t1);", "struct tm* g = gmtime(&now);",
      "time(&timestamp);", "char* ct = ctime(&now);", "mktime(t);",
      "clock_t elapsed = clock() - start;",
      "struct tm tm_info = *localtime(&now);",
      "strftime(buf, 64, \"%H:%M:%S\", &tm_info);",
      "int year = t->tm_year + 1900;", "sleep(1);", "int min = t->tm_min;"},
     15},

    {"stdbool.h",
     {"bool active = true;", "bool ok = (value > 0);",
      "if (ready == false) init();", "bool visible = (opacity > 0.5);",
      "if (enabled) run();", "bool valid = (count == expected);",
      "state.running = true;", "bool empty = (len == 0);",
      "if (!online) reconnect();", "bool match = (hash1 == hash2);",
      "bool done = (progress >= 100);", "if (flag && ready) execute();",
      "bool retry = (errors < limit);", "config.active = false;",
      "bool locked = (mutex != NULL);"},
     15},

    {"limits.h",
     {"if (x > INT_MAX - step) overflow = 1;", "char bits = CHAR_BIT;",
      "if (value > SHRT_MAX) value = SHRT_MAX;", "long cap = LONG_MAX / 2;",
      "unsigned short ms = USHRT_MAX;", "int m = INT_MIN;",
      "long lm = LONG_MIN;", "if (size > UINT_MAX) return -1;",
      "unsigned long ul = ULONG_MAX;",
      "if (value < SCHAR_MIN) value = SCHAR_MIN;",
      "if (n > UCHAR_MAX) n = UCHAR_MAX;", "range = LLONG_MAX;",
      "if (d < LLONG_MIN) d = LLONG_MIN;", "limit = UINT_MAX - 10;",
      "short mn = SHRT_MIN;"},
     15},

    {"float.h",
     {"if (fabs(a - b) < FLT_EPSILON) equal = 1;",
      "double maxd = DBL_MAX / 1.5;", "float small = FLT_MIN * 2;",
      "long double eps = LDBL_EPSILON;", "double minp = DBL_TRUE_MIN;",
      "float biggest = FLT_MAX;", "if (val < DBL_MIN) val = DBL_MIN;",
      "precision -= FLT_EPSILON;", "double db = DBL_EPSILON * 10;",
      "long double mx = LDBL_MAX / 4;", "float mn = FLT_MAX - 1000;",
      "if (fabs(err) < DBL_EPSILON) stable = 1;", "double tiny = DBL_TRUE_MIN;",
      "float norm = fmin(x, FLT_MAX);", "double low = fmax(a, DBL_MIN);"},
     15},

    {"stdint.h",
     {"int32_t id = 1024;", "uint16_t code = 0xABCD;",
      "int64_t delta = end - start;", "uint8_t flags = 0xF0;",
      "uintptr_t p = (uintptr_t)ptr;", "int16_t s = INT16_MAX - 1;",
      "uint32_t mask = 0xFFFFFFFF;", "int_least8_t small = -5;",
      "uint_fast32_t fast = counter;", "intmax_t total = imax1 + imax2;",
      "uintmax_t ut = (uintmax_t)value;", "int8_t sn = (value < 0) ? -1 : 1;",
      "uint64_t ticks = (uint64_t)time(NULL);",
      "intptr_t idx = (intptr_t)array;", "uint32_t len32 = (uint32_t)len;"},
     15},

    {"errno.h",
     {"if (!fp) { int e = errno; }", "perror(\"write failed\");",
      "if (errno == EINVAL) handle_bad_arg();", "errno = 0;",
      "char* es = strerror(errno);", "if (errno == ENOMEM) cleanup();",
      "printf(\"Err %d: %s\\n\", errno, strerror(errno));",
      "if (errno != 0) return -1;", "log_error(strerror(errno));",
      "int saved = errno;", "errno = EPERM;", "if (errno == EAGAIN) retry();",
      "if (errno == ENOENT) missing = 1;", "if (errno) notify_admin(errno);",
      "errno = saved;"},
     15},

    {"assert.h",
     {"assert(ptr != NULL);", "assert(count > 0);", "assert(index < size);",
      "assert(config.enabled);", "assert(speed <= MAX_SPEED);",
      "assert(list->head != NULL);", "assert(strlen(name) < 32);",
      "assert(buffer[0] != '\\0');", "assert(fp != NULL);", "assert(id >= 0);",
      "assert(result != -1);", "assert(total <= limit);",
      "assert(mode == READ || mode == WRITE);", "assert(user.active);",
      "assert(data != old_data);"},
     15},

    {"signal.h",
     {"signal(SIGINT, handler);", "signal(SIGTERM, shutdown_server);",
      "raise(SIGUSR1);",
      "struct sigaction sa; sa.sa_handler = on_sig; sigaction(SIGINT, &sa, "
      "NULL);",
      "sigemptyset(&mask);", "sigaddset(&mask, SIGTERM);",
      "sigprocmask(SIG_BLOCK, &mask, NULL);", "kill(getpid(), SIGUSR2);",
      "sigfillset(&set);", "sigdelset(&set, SIGINT);", "sigpending(&pending);",
      "int r = sigismember(&pending, SIGUSR1);", "siginterrupt(SIGINT, 1);",
      "sa.sa_flags = SA_RESTART;", "sigaction(SIGTERM, &sa, NULL);"},
     15}

};

int atom_count = sizeof(database) / sizeof(database[0]);
