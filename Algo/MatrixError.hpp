#ifndef _MATRIX_ERROR_H
#define _MATRIX_ERROR_H

#include <iostream>
#include <cstdlib>

#define E_MAT_INVALID_DIMENSION 1
#define E_VEC_INVALID_DIMENSION 2
#define E_MAT_ALLOC 3
#define E_VEC_ALLOC 4
#define E_DEREF 99

#define INFO 0
#define WARN 1
#define SEVERE 2
#define CRITICAL 3

#define SAFE_UINT(X) static_cast<uint64_t>(X)
#define SAFE_INT(X) static_cast<int64_t>(X)

#define merror(X, S) switch(S) { \
		case INFO: fprintf(stderr, "[*] %s\n", X); \
		case WARN: fprintf(stderr, "[!] %s\n", X); \
		case SEVERE: fprintf(stderr, "[!!] %s\n", X); \
		case CRITICAL: fprintf(stderr, "[CRITICAL] %s\n", X); \
		default: fprintf(stderr, "[?] %s\n", X); \
	}

#define G_TRY(X) try { \
		X; \
	} \
	catch (const std::exception& ex) { \
		merror(ex.what(), SEVERE) \
	}

#define DEREF_TRY(X) try { \
		X; \
	} \
	catch (const std::out_of_range& ex) { \
		merror(ex.what(), CRITICAL) \
	}

#define ALLOC_TRY(X) try { \
		X; \
	} \
	catch (const std::bad_alloc& ex) { \
		merror(ex.what(), CRITICAL) \
	}

#define BAD_TRY(X) try { \
		X; \
	} \
	catch (const std::bad_exception& ex) { \
		merror(ex.what(), WARN) \
	}

#define OVERFLOW_TRY(X) try { \
		X; \
	} \
	catch (const std::overflow_error& ex) { \
		merror(ex.what(), WARN) \
	}

#endif