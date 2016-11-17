#ifndef DEBUG_H
#define DEBUG_H

#define NO_OP do {} while (0)

#ifdef DEBUG
#  define DUMPI(x) fprintf(stderr, "%s = %d\n", #x, x)
#  define DUMPU(x) fprintf(stderr, "%s = %lu\n", #x, x)
#  define DUMPS(x) fprintf(stderr, "%s = %s\n", #x, x)
#  define LOG(x) fprintf(stderr, "%s\n", x)
#else
#  define DUMPI(x) NO_OP
#  define DUMPU(x) NO_OP
#  define DUMPS(x) NO_OP
#  define LOG(x) NO_OP
#endif // DEBUG

#endif // DEBUG_H
