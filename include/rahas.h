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

#ifndef ZKSC_RAHAS_H
#define ZKSC_RAHAS_H

#include <cinttypes>
#include <boost/multiprecision/cpp_int.hpp>
#include <sqlite3.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include "input.h"
#include "algo.h"
#include "parser.h"


using namespace boost::multiprecision;
using namespace std;


# if !defined(ZKSC_GNUC_PREREQ)
#  if defined(__GNUC__) && defined(__GNUC_MINOR__)
#   define ZKSC_GNUC_PREREQ(_maj, _min) \
 ((__GNUC__<<16)+__GNUC_MINOR__>=((_maj)<<16)+(_min))
#  else
#   define ZKSC_GNUC_PREREQ(_maj,_min) 0
#  endif
# endif

# if (!defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L))
#  if ZKSC_GNUC_PREREQ(2,7)
#   define ZKSC_INLINE __inline__
#  elif (defined(_MSC_VER))
#   define ZKSC_INLINE __inline
#  else
#   define ZKSC_INLINE
#  endif
# else
#  define ZKSC_INLINE inline
# endif

#ifndef ZKSC_API
# if defined(_WIN32)
#  ifdef ZKSC_BUILD
#   define ZKSC_API __declspec(dllexport)
#  else
#   define ZKSC_API
#  endif
# elif defined(__GNUC__) && defined(ZKSC_BUILD)
#  define ZKSC_API __attribute__ ((visibility ("default")))
# else
#  define ZKSC_API
# endif
#endif

# if defined(__GNUC__) && ZKSC_GNUC_PREREQ(3, 4)
#  define ZKSC_UNUSED __attribute__((unused))
# else
#  define ZKSC_UNUSED
# endif

# if defined(__GNUC__) && ZKSC_GNUC_PREREQ(3, 4)
#  define ZKSC_WARN_UNUSED_RESULT __attribute__ ((__warn_unused_result__))
# else
#  define ZKSC_WARN_UNUSED_RESULT
# endif

# if !defined(ZKSC_BUILD) && defined(__GNUC__) && ZKSC_GNUC_PREREQ(3, 4)
#  define ZKSC_ARG_NONNULL(_x)  __attribute__ ((__nonnull__(_x)))
# else
#  define ZKSC_ARG_NONNULL(_x)
# endif


context_t init(uint8_t seed[SEED_BYTES]);

/* functions fro commitments */
void com_set_zero(com_t *c);
void com_set(com_t *c, com_t *b);
void com_add(com_t *c, com_t *a, com_t *b);
void com_sub(com_t *c, com_t *a, com_t *b);
int com_compare(com_t *c, com_t *b);

/* functions for confidential inputs */
void in_ct_gen(context_t *ctx, in_ct *out, zint_t v, uint8_t key[MASKKEY_BYTES],
               uint8_t is_set_pk, uint8_t pk[PKID]);
void in_c_copy(in_ct *out, in_ct *in);
void in_e_copy(in_et *out, in_et *in);

void in_ct_copy(in_ct *out, in_ct *in);
void in_et_copy(in_et *out, in_et *in);
void in_ct_to_et(in_et *out, in_ct *in);

/* functions for confidential coins */
void coin_ct_init(coin_ct *coin, uint8_t range);
void coin_ct_free(coin_ct *coin);
void coin_et_init(coin_et *coin, uint8_t range);
void coin_et_free(coin_et *coin);
int coin_gen(context_t *ctx, uint8_t range, coin_ct *coin, uint32_t v, uint8_t key[MASKKEY_BYTES]);
int coin_ver(context_t *ctx, coin_et *coin);

int get_coin_size(coin_ct *coin);
int get_coin_size_et(coin_et *coin);
void coin_to_bytes(uint8_t bytes[], coin_ct *coin);
void bytes_to_coin(coin_et *coin, uint8_t bytes[]);
void bytes_to_coin_ct(coin_ct *coin, uint8_t bytes[]);

void coin_ct_to_et(coin_et *out, coin_ct *in);

/* functions for stateless confidential transaction metadata */

void metadata_to_bytes(uint8_t bytes[], com_t *E);
void bytes_to_metadata(com_t *E, uint8_t bytes[]);

void tx_metadata_gen(context_t *ctx, uint8_t metadata[], int supply,
                     uint16_t inlen, coin_ct in[],
                     uint16_t outlen, coin_ct out[]);
int tx_metadata_ver_before(context_t *ctx, uint8_t metadata[], int supply,
                           uint16_t inlen, coin_et in[],
                           uint16_t outlen, coin_et out[]);
int tx_metadata_ver_after(context_t *ctx, com_t *tmp, int supply, uint8_t metadata[]) ;

/* functions for smart contract */

/**
 * Read string code into a linked list of commands.
 * @param cmds - command list (no need to initialize)
 * @param zksc_contract_path - string code
 * @param extra - external compiling objects
 * @return 1 - successful or error codes
 */
int pre_parser(command_set *cmds, char *zksc_contract_path, var_ct extra[]);


/**
 * Compile the commands and generate zero-knowledge proofs.
 * @param ctx - context object for cryptographic functions
 * @param range - range of coins
 * @param cmds - command list (no need to initialize)
 * @param zk_proof_path - file path for proofs
 * @param inlen - number of input coins
 * @param in - input coins
 * @param outlen - number of output coins
 * @param out - output coins
 * @param extralen - number of externals
 * @param extra - externals including the opening
 * @return 1 - successful or error codes
 */
int parser_recursive(context_t *ctx, uint8_t range,
                     char *zk_proof_path,
                     command_set *cmds,
                     uint8_t inlen, coin_ct in[],
                     uint8_t outlen, coin_ct out[],
                     uint8_t extralen, var_ct extra[]);

/**
 * Execute the commands and verifies zero-knowledge proofs.
 * @param ctx - context object for cryptographic functions
 * @param range - range of coins
 * @param zk_proof_path - file path for proofs
 * @param cmds - command list (no need to initialize)
 * @param inlen - number of input coins
 * @param in - input coins
 * @param outlen - number of output coins
 * @param out - output coins
 * @return 1 - successful or error codes
 */
int parser_recursive_ver(context_t *ctx, uint8_t range,
                         char *zk_proof_path,
                         command_set *cmds,
                         uint8_t inlen, coin_et in[],
                         uint8_t outlen, coin_et out[]);

/**
 * Compile the commands and generate zero-knowledge proofs.
 * @param ctx - context object for cryptographic functions
 * @param range - range of coins
 * @param zk_proof - opened FILE for proofs
 * @param cmds - command list (no need to initialize)
 * @param inlen - number of input coins
 * @param in - input coins
 * @param outlen - number of output coins
 * @param out - output coins
 * @param extralen - number of externals
 * @param extra - externals including the opening
 * @return 1 - successful or error codes
 */
int compiler(context_t *ctx, uint8_t range,
             FILE * zk_proof,
             command_set *cmds,
             uint8_t inlen, coin_ct in[],
             uint8_t outlen, coin_ct out[],
             uint8_t extralen, var_ct extra[]);

/**
 * Execute the commands and verifies zero-knowledge proofs.
 * @param ctx - context object for cryptographic functions
 * @param range - range of coins
 * @param zk_proof - opened FILE for proofs
 * @param cmds - command list (no need to initialize)
 * @param inlen - number of input coins
 * @param in - input coins
 * @param outlen - number of output coins
 * @param out - output coins
 * @return 1 - successful or error codes
 */
int executor(context_t *ctx, uint8_t range,
             FILE * zk_proof,
             command_set *cmds,
             uint8_t inlen, coin_et in[],
             uint8_t outlen, coin_et out[]);


void poly_print(poly *a, int len);

#endif //ZKSC_RAHAS_H
