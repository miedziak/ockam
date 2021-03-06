/**
 ********************************************************************************************************
 * @file    test_vault.h
 * @brief   Common test functions for vault
 ********************************************************************************************************
 */

/*
 ********************************************************************************************************
 *                                             INCLUDE FILES                                            *
 ********************************************************************************************************
 */

#include <stdint.h>

#include <ockam/log.h>
#include <ockam/vault.h>


/*
 ********************************************************************************************************
 *                                                DEFINES                                               *
 ********************************************************************************************************
 */

#define TEST_VAULT_NO_TEST_CASE                     0xFF


/*
 ********************************************************************************************************
 *                                               CONSTANTS                                              *
 ********************************************************************************************************
 */

/*
 ********************************************************************************************************
 *                                               DATA TYPES                                             *
 ********************************************************************************************************
 */

/*
 ********************************************************************************************************
 *                                          FUNCTION PROTOTYPES                                         *
 ********************************************************************************************************
 */

/*
 ********************************************************************************************************
 *                                            GLOBAL VARIABLES                                          *
 ********************************************************************************************************
 */

/*
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS                                           *
 ********************************************************************************************************
 */

/*
 ********************************************************************************************************
 *                                            LOCAL FUNCTIONS                                           *
 ********************************************************************************************************
 */

void test_vault_random(void);
void test_vault_key_ecdh(OCKAM_VAULT_EC_e ec, uint8_t load_keys);
void test_vault_sha256(void);
void test_vault_hkdf(void);
void test_vault_aes_gcm(void);

void test_vault_print(OCKAM_LOG_e level, char* p_module, uint32_t test_case, char* p_msg);
void test_vault_print_array(OCKAM_LOG_e level, char* p_module, char* p_label, uint8_t* p_array, uint32_t size);

