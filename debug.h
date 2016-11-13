#ifndef DEBUG_H
#define DEBUG_H

#define NO_OP do {} while (0)

#ifdef DEBUG
#  define DUMPI(x) fprintf(stderr, "%s = %d\n", #x, x)
#  define DUMPS(x) fprintf(stderr, "%s = %s\n", #x, x)
#else
#  define DUMPI(x) NO_OP
#  define DUMPS(x) NO_OP
#endif // DEBUG

#endif // DEBUG_H
