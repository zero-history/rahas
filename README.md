# Rahas

Rahas is zero-knowledge programming language from Confidential Integer Processing (CIP) of 
Approximate Short Integer Solution Problem. Rahas supports three variable types; integer,
confidential integers, and public keys as follows.


| Operations and Conditionals       | Integer        | Confidential Integer | public keys |
|-----------------------------------|----------------|----------------------|-------------|
| Set                               | #SET_INT       | #SET_CINT            | #SET_PK     |
| Set according inputs' public key  |                | #SET_CINTFINPK       | -           |
| Set according outputs' public key |                | #SET_CINTFOUTPK      | -           |
| Addition                          | #ADD_INT       | #ADD_CINT            | -           |
| Subtraction                       | #SUB_INT       | #SUB_CINT            | -           |
| Multiplication                    | #MUL_INT       | #MUL_CINT            | -           |
| Division                          | #DIV_INT       | #DIV_CINT            | -           |
| Unsigned Division                 | -              | #DIVU_INT            | -           |
| If Equal                          | #IF_EQ_INT     | #IF_EQ_CINT          | #IF_EQ_PK   |
| If Not Equal                      | #IF_NEQ_INT    | #IF_NEQ_CINT         | #IF_NEQ_INT |
| If Less than or Equal             | #IF_LEQ_INT    | #IF_LEQ_CINT         | -           |
| If Greater than or Equal          | #IF_GEQ_INT    | #IF_GEQ_CINT         | -           |
| Copy                              | #CPY_INT       | #CPY_CINT            | -           |
| While Equal                       | #WHILE_EQ_INT  | #WHILE_EQ_CINT       | -           |
| While Not Equal                   | #WHILE_NEQ_INT | #WHILE_NEQ_CINT      | -           |
| While Less than or Equal          | #WHILE_LEQ_INT | #WHILE_LEQ_CINT      | -           |
| While Greater than or Equal       | #WHILE_GEQ_INT | #WHILE_GEQ_CINT      | -           |


### Building and Testing

`mkdir build`

`cmake ..`

`make`

`./tests`

### How to generate the bytecode, compile, and execute codes

The following C code shows how to build and run Rahas contracts without any coins inputs or outputs.
More code samples can be found in src/lang/tests_lang.hpp.

```c
    // Nonce for the cryptographic context object
    uint8_t seed[SEED_BYTES];

    // Generate the cryptographic context object
    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    // Generate secret keys
    uint8_t key1[MASKKEY_BYTES];
    uint8_t key2[MASKKEY_BYTES];
    uint8_t key3[MASKKEY_BYTES];
    uint8_t key4[MASKKEY_BYTES];
    RAND_bytes(key1, MASKKEY_BYTES);
    RAND_bytes(key2, MASKKEY_BYTES);
    RAND_bytes(key3, MASKKEY_BYTES);
    RAND_bytes(key4, MASKKEY_BYTES);

    // Custom range of integers that should be less than L which can be 32 or 56.
    uint8_t L1 = 25;

    // Allocate external variables for confidential integers or public keys
    var_ct *extra_gen;
    extra_gen = (var_ct *) malloc(4 * sizeof(var_ct));

    // Set extra variables with confidential integers
    extra_gen[0].type = TYPE_CINT;
    extra_gen[1].type = TYPE_CINT;
    extra_gen[2].type = TYPE_CINT;
    extra_gen[3].type = TYPE_CINT;
    in_ct_gen(&ctx, &extra_gen[0].data, 100, key1, 0, nullptr);
    in_ct_gen(&ctx, &extra_gen[1].data, 700, key2, 0, nullptr);
    in_ct_gen(&ctx, &extra_gen[2].data, 1000, key3, 0, nullptr);
    in_ct_gen(&ctx, &extra_gen[3].data, 600, key4, 0, nullptr);

    // Code path
    char fp_path[] = "../test_samples/test_while.zksc";
    // Byte code path
    char zk_path[] = "../test_samples/test.zk";

    // Byte code
    command_set cmds;
    // Turn code into a byte code
    CHECK(pre_parser(&cmds, fp_path, extra_gen) == 1);
    // Compile the byte code and generate zero-knowledge proofs (in File zk_path) 
    // without coin inputs or outputs
    CHECK(parser_recursive(&ctx, L1,  zk_path, &cmds, 0, nullptr, 0, nullptr, 4, extra_gen) == 1);
    // Execute the byte code along with the zero-knowledge proofs
    CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, 0, nullptr, 0, nullptr) == 1);
    
    // Free the byte code
    free(cmds.set);

    // Free external variables
    free(extra_gen);
```


### Variable Initiation

This a simple programming language, and variables are denoted as `VARX` 
when `X` can be an integer of 15-bits. For example,

```python
#SET_INT    VAR0    100
#SET_CINT   VAR1   ETR0
#SET_PK     VAR2   ETR1
```

For the above settings, we need to generate external variables inside the C code as follows.

```c
    var_ct *extra_gen;
    extra_gen = (var_ct *) malloc(2 * sizeof(var_ct));

    // Set externals
    extra_gen[0].type = TYPE_CINT;
    extra_gen[1].type = TYPE_PK;
    // Confidential Integer for ETR0
    in_ct_gen(&ctx, &extra_gen[0].data, percentage, key1, 0, nullptr);
    // Public Key for ETR1
    extra_gen[1].data.is_set_pk = 1;
    memset(extra_gen[1].data.pk, 'a', PKID);
```

### IF Conditions

```python
#SET_INT    VAR0    100
#SET_INT    VAR2    600
#SET_INT    VAR1    1000
#IF_NEQ_INT VAR0   VAR2
    #ADD_INT    VAR2 VAR2 VAR0
    #IF_NEQ_INT VAR0   VAR2
        #ADD_INT    VAR2 VAR2 VAR0
    #ELSE
        #SUB_INT  VAR2 VAR2 VAR0
    #END_IF
#ELSE
    #SUB_INT  VAR2 VAR2 VAR0
#END_IF
#SET_PK     VAR3   ETR1
#SET_CINT   VAR0   ETR0
#SET_CINT   VAR2   ETR0
#IF_EQ_CINT VAR0   VAR2
    #ADD_CINT    VAR2 VAR2 VAR0
    #IF_NEQ_CINT VAR0   VAR2
        #ADD_CINT    VAR2 VAR2 VAR0
    #ELSE
        #SUB_CINT  VAR2 VAR2 VAR0
    #END_IF
#ELSE
    #SUB_CINT  VAR2 VAR2 VAR0
#END_IF
#RETURN_OK
```

### WHILE loops

```python
#SET_INT    VAR0    100
#SET_INT    VAR1    600
#SET_INT    VAR2    1000
#WHILE_NEQ_INT VAR1   VAR2
    #ADD_INT    VAR1 VAR1 VAR0
#END_WHILE
#SET_CINT    VAR3    ETR0
#SET_CINT    VAR4    ETR1
#SET_CINT    VAR5    ETR2
#WHILE_NEQ_CINT VAR4   VAR5
    #ADD_CINT    VAR4 VAR4 VAR3
#END_WHILE

#SET_INT  VAR6    500
#SET_CINT VAR7    ETR2
#WHILE_GEQ_CINT   VAR4    VAR7
    #SUB_CINT   VAR4   VAR4   VAR3
    #WHILE_GEQ_INT   VAR1    VAR6
        #SUB_INT   VAR1  VAR1   VAR0
    #END_WHILE
    #ADD_INT VAR1 VAR1  VAR0
#END_WHILE
#ADD_INT VAR1 VAR1  VAR0
#IF_EQ_INT  VAR1    VAR6
    #ADD_CINT VAR4 VAR4  VAR3
    #IF_EQ_CINT  VAR4   VAR7
        #RETURN_OK
    #END_IF
#END_IF
#RETURN_ERROR
```




