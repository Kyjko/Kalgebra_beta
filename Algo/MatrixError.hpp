#ifndef _MATRIX_ERROR_H
#define _MATRIX_ERROR_H

#include <iostream>
#include <cstdlib>

#define E_MAT_INVALID_DIMENSION 91
#define E_VEC_INVALID_DIMENSION 92

#define INFO 0
#define WARN 1
#define SEVERE 2
#define CRITICAL 3

#define SAFE_UINT(X) static_cast<uint64_t>(X)
#define SAFE_INT(X) static_cast<int64_t>(X)

#define merror(X, S) switch(S) { \
		case INFO: fprintf(stderr, "[*] %s\n", X); break; \
		case WARN: fprintf(stderr, "[!] %s\n", X); break; \
		case SEVERE: fprintf(stderr, "[!!] %s\n", X); break; \
		case CRITICAL: fprintf(stderr, "[CRITICAL] %s\n", X); break; \
		case E_MAT_INVALID_DIMENSION: fprintf(stderr, "[MATRIX DIMENSION] %s\n", X); break; \
		case E_VEC_INVALID_DIMENSION: fprintf(stderr, "[VECTOR DIMENSION] %s\n", X); break; \
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