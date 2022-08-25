/**********************************************************************
    CIP_SIS (Confidential Integer Processing)
    Copyright (C) 2022  Jayamine Alupotha

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ZKSC_UTIL_H
#define ZKSC_UTIL_H

typedef struct {
    void (*fn)(const char *text, void *data);
    const void *data;
} zksc_callback;


#ifdef DETERMINISTIC
#define SQLITE3_FAILURE(msg, error, db) do { \
    fprintf(stderr, "%s\n %s\n", msg, error); \
    sqlite3_free(error); \
    sqlite3_close(db); \
    abort(); \
} while(0);
#else
#define SQLITE3_FAILURE(msg, error, db) do { \
    fprintf(stderr, "%s:%d: %s (%s)\n", __FILE__, __LINE__, msg, error); \
    sqlite3_free(error); \
    sqlite3_close(db); \
    abort(); \
} while(0)
#endif

#ifdef DETERMINISTIC
#define SQLITE3_NOFREE_FAILURE(msg, error, db, stmt) do { \
    fprintf(stderr, "%s\n %s  (%s)\n", msg, error); \
    sqlite3_finalize(stmt); \
    sqlite3_close(db); \
    abort(); \
} while(0);
#else
#define SQLITE3_NOFREE_FAILURE(msg, error, db, stmt) do { \
    fprintf(stderr, "%s:%d: %s (%s)\n", __FILE__, __LINE__, msg, error); \
    sqlite3_finalize(stmt); \
    sqlite3_close(db); \
    abort(); \
} while(0)
#endif

#ifdef DETERMINISTIC
#define SQLITE3_ROLLBACK_FAILURE(msg, error, db, stmt) do { \
    fprintf(stderr, "%s\n %s  (%s)\n", msg, error);   \
    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);  \
    sqlite3_finalize(stmt); \
    sqlite3_close(db); \
    abort(); \
} while(0);
#else
#define SQLITE3_ROLLBACK_FAILURE(msg, error, db, stmt) do { \
    fprintf(stderr, "%s:%d: %s (%s)\n", __FILE__, __LINE__, msg, error); \
    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);  \
    sqlite3_finalize(stmt); \
    sqlite3_close(db); \
    abort(); \
} while(0)
#endif

#ifdef DETERMINISTIC
#define TEST_FAILURE(msg) do { \
    fprintf(stderr, "%s\n", msg); \
    abort(); \
} while(0);
#else
#define TEST_FAILURE(msg) do { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, msg); \
    abort(); \
} while(0)
#endif

#if __GNUC_PREREQ(3, 0)
#define EXPECT(x, c) __builtin_expect((x),(c))
#else
#define EXPECT(x,c) (x)
#endif

#ifdef DETERMINISTIC
#define SQLITE3_CHECK(cond) do { \
    if (EXPECT(!(cond), 0)) { \
        SQLITE3_FAILURE("sqlite3 database operation failed"); \
    } \
} while(0)
#else
#define SQLITE3_CHECK(cond, error, db) do { \
    if (EXPECT(!(cond), 0)) { \
        SQLITE3_FAILURE("sqlite3 database operation failed: " #cond, error, db); \
    } \
} while(0)
#endif

#ifdef DETERMINISTIC
#define SQLITE3_NOFREE_CHECK(cond, error, db, stmt) do { \
    if (EXPECT(!(cond), 0)) { \
        SQLITE3_NOFREE_FAILURE("sqlite3 database operation failed: " #cond, error, db, stmt); \
    } \
} while(0)
#else
#define SQLITE3_NOFREE_CHECK(cond, error, db, stmt) do { \
    if (EXPECT(!(cond), 0)) { \
        SQLITE3_NOFREE_FAILURE("sqlite3 database operation failed: " #cond, error, db, stmt); \
    } \
} while(0)
#endif

#ifdef DETERMINISTIC
#define SQLITE3_ROLLBACK_CHECK(cond, error, db, stmt) do { \
    if (EXPECT(!(cond), 0)) { \
        SQLITE3_ROLLBACK_FAILURE("sqlite3 database operation failed: " #cond, error, db, stmt); \
    } \
} while(0)
#else
#define SQLITE3_ROLLBACK_CHECK(cond, error, db, stmt) do { \
    if (EXPECT(!(cond), 0)) { \
        SQLITE3_ROLLBACK_FAILURE("sqlite3 database operation failed: " #cond, error, db, stmt); \
    } \
} while(0)
#endif

#ifdef DETERMINISTIC
#define CHECK(cond) do { \
    if (EXPECT(!(cond), 0)) { \
        TEST_FAILURE("test condition failed"); \
    } \
} while(0)
#else
#define CHECK(cond) do { \
    if (EXPECT(!(cond), 0)) { \
        TEST_FAILURE("test condition failed: " #cond); \
    } \
} while(0)
#endif

#if defined(_WIN32) || defined(WIN32)
# define I64FORMAT "I64d"
# define I64uFORMAT "I64u"
#else
# define I64FORMAT "lld"
# define I64uFORMAT "llu"
#endif


#ifdef DETERMINISTIC
#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif
#else
#if (DEBUG_MODE==1)
# define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif
#endif

#endif /* ZKSC_UTIL_H */
