/**
********************************************************************************************************
 * @file        test_atecc508a.c
 * @brief
 ********************************************************************************************************
 */

/*
 ********************************************************************************************************
 *                                             INCLUDE FILES                                            *
 ********************************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <ockam/define.h>
#include <ockam/error.h>
#include <ockam/vault.h>

#include <test_vault.h>


/*
 ********************************************************************************************************
 *                                                DEFINES                                               *
 ********************************************************************************************************
 */

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

OCKAM_VAULT_CFG_s vault_cfg =
{
    .p_tpm                       = 0,
    .p_host                      = 0,
    OCKAM_VAULT_EC_CURVE25519
};


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


/**
 ********************************************************************************************************
 *                                             main()
 *
 * @brief   Main point of entry for mbedcrypto test
 *
 ********************************************************************************************************
 */

void main (void)
{
    OCKAM_ERR err;
    uint8_t i;


    /* ---------- */
    /* Vault Init */
    /* ---------- */

    err = ockam_vault_init((void*) &vault_cfg);                 /* Initialize vault                                   */

    if(err != OCKAM_ERR_NONE) {                                 /* Ensure it initialized before proceeding, otherwise */
        test_vault_print(OCKAM_LOG_ERROR,                       /* don't bother trying to run any other tests         */
                         "MBEDCRYPTO",
                          0,
                         "Error: Ockam Vault Init failed");
        return;
    }

    /* ------------------------ */
    /* Random Number Generation */
    /* ------------------------ */

    test_vault_random();

    /* --------------------- */
    /* Key Generation & ECDH */
    /* --------------------- */

    test_vault_key_ecdh(vault_cfg.ec, 1);

    /* ------ */
    /* SHA256 */
    /* ------ */

    test_vault_sha256();

    /* -----*/
    /* HKDF */
    /* -----*/

    test_vault_hkdf();

    /* -------------------- */
    /* AES GCM Calculations */
    /* -------------------- */

    test_vault_aes_gcm();

    return;
}

