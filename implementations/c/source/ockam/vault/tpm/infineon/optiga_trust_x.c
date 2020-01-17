/**
 ********************************************************************************************************
 * @file    optiga_trust_x.c
 * @brief   Ockam Vault Implementation for the Optiga Trust X
 ********************************************************************************************************
 */

/*
 ********************************************************************************************************
 *                                             INCLUDE FILES                                            *
 ********************************************************************************************************
 */

#include <ockam/define.h>
#include <ockam/error.h>

#include <ockam/kal.h>
#include <ockam/memory.h>
#include <ockam/vault.h>
#include <ockam/vault/tpm.h>
#include <ockam/vault/tpm/infineon.h>

#include <optiga/optiga_crypt.h>
#include <optiga/optiga_util.h>

#if !defined(OCKAM_VAULT_CONFIG_FILE)
#error "Error: Ockam Vault Config File Missing"
#else
#include OCKAM_VAULT_CONFIG_FILE
#endif


/*
 ********************************************************************************************************
 *                                                DEFINES                                               *
 ********************************************************************************************************
 */

#define OPTIGA_TRUST_X_RAND_NUM_SIZE_MAX            1024u       /* Data sheet unclear about max size. Limit for now   */

#define OPTIGA_TRUST_X_NUM_KEYS                        2u       /* Only support one static and one ephemeral to start */
#define OPTIGA_TRUST_X_PUB_KEY_SIZE                   64u       /* Keys are NIST P256 (64 bytes)                      */
#define OPTIGA_TRUST_X_PUB_KEY_STATIC                  0u       /* Offset for global public key array                 */
#define OPTIGA_TRUST_X_PUB_KEY_EPHEMERAL               1u       /* Offset for global public key array                 */

#define OPTIGA_TRUST_X_PUB_KEY_STATIC_OFFSET    (OPTIGA_TRUST_X_PUB_KEY_STATIC * OPTIGA_TRUST_X_PUB_KEY_SIZE)
#define OPTIGA_TRUST_X_PUB_KEY_EPHEMERAL_OFFSET (OPTIGA_TRUST_X_PUB_KEY_EPHEMERAL * OPTIGA_TRUST_X_PUB_KEY_SIZE)

#define OPTIGA_TRUST_X_KEY_SLOT_STATIC              OPTIGA_KEY_STORE_ID_E0F0
#define OPTIGA_TRUST_X_KEY_SLOT_EPHEMERAL           OPTIGA_KEY_STORE_ID_E0F1

#define OPTIGA_TRUST_X_PMS_SIZE                        32u      /* Pre-master secret should always be 32 bytes        */
#define OPTIGA_TRUST_X_SHA256_DIGEST_SIZE              32u      /* SHA256 digest always 32 bytes                      */

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


/**
 *******************************************************************************
 * @struct  OPTIGA_TRUST_X_PEER_PUBLIC_KEY_s
 * @brief
 *******************************************************************************
 */

#pragma pack(1)
typedef struct {
    uint8_t bit_string_format;
    uint8_t remaining_length;
    uint8_t reserved_0;
    uint8_t compression_format;
    uint8_t public_key[OPTIGA_TRUST_X_PUB_KEY_SIZE];
} OPTIGA_TRUST_X_PEER_PUBLIC_KEY_s;
#pragma pack()


/*
 ********************************************************************************************************
 *                                            INLINE FUNCTIONS                                          *
 ********************************************************************************************************
 */

/*
 ********************************************************************************************************
 *                                            GLOBAL VARIABLES                                          *
 ********************************************************************************************************
 */


uint8_t g_optiga_trust_x_pub_keys[OPTIGA_TRUST_X_NUM_KEYS * OPTIGA_TRUST_X_PUB_KEY_SIZE];

OPTIGA_TRUST_X_PEER_PUBLIC_KEY_s g_optiga_trust_x_peer_pub_key =
{
    0x03,                                                       /* Bit string format                                  */
    0x42,                                                       /* Remaining length                                   */
    0x00,                                                       /* Unused bits                                        */
    0x04,                                                       /* Compression format                                 */
    {0}                                                         /* Initialize public key value to 0s                  */
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

/*
 ********************************************************************************************************
 ********************************************************************************************************
 *                                         OCKAM_VAULT_CFG_INIT
 ********************************************************************************************************
 ********************************************************************************************************
 */

#if(OCKAM_VAULT_CFG_INIT & OCKAM_VAULT_TPM_INFINEON_OPTIGA_TRUST_X)

/*
 ********************************************************************************************************
 *                                         ockam_vault_tpm_init()
 ********************************************************************************************************
 */

OCKAM_ERR ockam_vault_tpm_init(void *p_arg)
{
    OCKAM_ERR ret_val = OCKAM_ERR_NONE;
    int32_t status = (int32_t) OPTIGA_LIB_ERROR;
    VAULT_INFINEON_CFG_s *p_optiga_trust_x_cfg = 0;


    do
    {
        if(p_arg == 0) {                                        /* Ensure the p_arg value is not null                 */
            ret_val = OCKAM_ERR_INVALID_PARAM;
            break;
        }

        p_optiga_trust_x_cfg = (VAULT_INFINEON_CFG_s*) p_arg;   /* Grab vault configuration for the Optiga Trust X    */

        if(p_optiga_trust_x_cfg->iface == VAULT_INFINEON_IFACE_I2C) {
            status = optiga_util_open_application(p_optiga_trust_x_cfg->iface_cfg);
            if(OPTIGA_LIB_SUCCESS != status)
            {
                ret_val = OCKAM_ERR_VAULT_TPM_INIT_FAIL;
                break;
            }
        } else {                                                /* Only supporting I2C at the moment                  */
            ret_val = OCKAM_ERR_VAULT_TPM_UNSUPPORTED_IFACE;
            break;
        }
    } while(0);

    return ret_val;
}


/*
 ********************************************************************************************************
 *                                          ockam_vault_tpm_free()
 ********************************************************************************************************
 */

OCKAM_ERR ockam_vault_tpm_free (void)
{
   return OCKAM_ERR_NONE;
}


#endif                                                          /* OCKAM_VAULT_CFG_INIT                               */


/*
 ********************************************************************************************************
 ********************************************************************************************************
 *                                         OCKAM_VAULT_CFG_RAND
 ********************************************************************************************************
 ********************************************************************************************************
 */

#if(OCKAM_VAULT_CFG_RAND == OCKAM_VAULT_TPM_INFINEON_OPTIGA_TRUST_X)


/*
 ********************************************************************************************************
 *                                        ockam_vault_tpm_random()
 ********************************************************************************************************
 */

OCKAM_ERR ockam_vault_tpm_random(uint8_t *p_rand_num, uint32_t rand_num_size)
{
    OCKAM_ERR ret_val = OCKAM_ERR_NONE;
    optiga_lib_status_t status = OPTIGA_LIB_SUCCESS;


    do {
        if((rand_num_size <= 0) ||                              /* Make sure the expected size matches the buffer     */
           (rand_num_size > OPTIGA_TRUST_X_RAND_NUM_SIZE_MAX)) {
            ret_val = OCKAM_ERR_VAULT_SIZE_MISMATCH;
            break;
        }

        status = optiga_crypt_random(OPTIGA_RNG_TYPE_TRNG,      /* Generate a random number on the Optiga Trust X. Ok */
                                      p_rand_num,               /* to cast to 16-bit due to size check.               */
                                      (uint16_t) rand_num_size);
        if(status != OPTIGA_LIB_SUCCESS) {
            ret_val = OCKAM_ERR_VAULT_TPM_RAND_FAIL;
        }
    } while (0);

    return ret_val;
}

#endif                                                          /* OCKAM_VAULT_CFG_RAND                               */


/*
 ********************************************************************************************************
 ********************************************************************************************************
 *                                      OCKAM_VAULT_CFG_KEY_ECDH
 ********************************************************************************************************
 ********************************************************************************************************
 */

#if(OCKAM_VAULT_CFG_KEY_ECDH == OCKAM_VAULT_TPM_INFINEON_OPTIGA_TRUST_X)


/*
 ********************************************************************************************************
 *                                        ockam_vault_tpm_key_gen()
 ********************************************************************************************************
 */

OCKAM_ERR ockam_vault_tpm_key_gen(OCKAM_VAULT_KEY_e key_type)
{
    OCKAM_ERR ret_val = OCKAM_ERR_NONE;
    optiga_lib_status_t status = OPTIGA_LIB_SUCCESS;
    uint16_t pub_key_len = OPTIGA_TRUST_X_PUB_KEY_SIZE;
    optiga_key_id_t key_id;


    do
    {
        if(key_type == OCKAM_VAULT_KEY_STATIC) {
            key_id = OPTIGA_TRUST_X_KEY_SLOT_STATIC;
        } else if(key_type == OCKAM_VAULT_KEY_EPHEMERAL) {
            key_id = OPTIGA_TRUST_X_KEY_SLOT_EPHEMERAL;
        } else {
            ret_val = OCKAM_ERR_INVALID_PARAM;
            break;
        }

        status = optiga_crypt_ecc_generate_keypair(OPTIGA_ECC_NIST_P_256,
                                                   (OPTIGA_KEY_USAGE_KEY_AGREEMENT | OPTIGA_KEY_USAGE_AUTHENTICATION),
                                                   0,
                                                   &key_id,
                                                   &g_optiga_trust_x_pub_keys[OPTIGA_TRUST_X_PUB_KEY_STATIC_OFFSET],
                                                   &pub_key_len);
        if(status != OPTIGA_LIB_SUCCESS) {
            ret_val = OCKAM_ERR_VAULT_TPM_KEY_FAIL;
            break;
        }
    } while(0);

    return ret_val;
}


/*
 ********************************************************************************************************
 *                                        ockam_vault_tpm_key_get_pub()
 ********************************************************************************************************
 */

OCKAM_ERR ockam_vault_tpm_key_get_pub(OCKAM_VAULT_KEY_e key_type,
                                      uint8_t *p_pub_key, uint32_t pub_key_size)
{
    OCKAM_ERR ret_val = OCKAM_ERR_NONE;


    do
    {
        if(p_pub_key == 0) {                                    /* Ensure the buffer isn't null                       */
            ret_val = OCKAM_ERR_INVALID_PARAM;
            break;
        }

        if(pub_key_size != OPTIGA_TRUST_X_PUB_KEY_SIZE) {
            ret_val = OCKAM_ERR_VAULT_SIZE_MISMATCH;
            break;
        }

        switch(key_type) {
            case OCKAM_VAULT_KEY_STATIC:                        /* Get the static public key                          */
                ret_val = ockam_mem_copy(p_pub_key,
                                         &g_optiga_trust_x_pub_keys[OPTIGA_TRUST_X_PUB_KEY_STATIC_OFFSET],
                                         pub_key_size);
                break;

            case OCKAM_VAULT_KEY_EPHEMERAL:                     /* Get the generated ephemeral public key             */
                ret_val = ockam_mem_copy(p_pub_key,
                                         &g_optiga_trust_x_pub_keys[OPTIGA_TRUST_X_PUB_KEY_EPHEMERAL_OFFSET],
                                         pub_key_size);
                break;

            default:
                ret_val = OCKAM_ERR_INVALID_PARAM;
                break;
        }
    } while (0);

    return ret_val;
}


/**
 ********************************************************************************************************
 *                                        ockam_vault_tpm_ecdh()
 ********************************************************************************************************
 */

OCKAM_ERR ockam_vault_tpm_ecdh(OCKAM_VAULT_KEY_e key_type,
                               uint8_t *p_pub_key, uint32_t pub_key_size,
                               uint8_t *p_pms, uint32_t pms_size)
{
    OCKAM_ERR ret_val = OCKAM_ERR_NONE;
    optiga_lib_status_t status = OPTIGA_LIB_SUCCESS;
    public_key_from_host_t optiga_pub_key;
    optiga_key_id_t key_id;


    do {
        if((p_pub_key == 0) ||                                  /* Ensure the buffers are not null                    */
           (p_pms == 0))
        {
            ret_val = OCKAM_ERR_INVALID_PARAM;
            break;
        }

        if((pub_key_size != OPTIGA_TRUST_X_PUB_KEY_SIZE) ||     /* Validate the size of the buffers passed in         */
           (pms_size != OPTIGA_TRUST_X_PMS_SIZE))
        {
            ret_val = OCKAM_ERR_VAULT_SIZE_MISMATCH;
            break;
        }

        ockam_mem_copy(&(g_optiga_trust_x_peer_pub_key.public_key),
                       p_pub_key,
                       pub_key_size);
                                                                /* Configure the public key from host structure for   */
        optiga_pub_key.curve = OPTIGA_ECC_NIST_P_256;           /* the ECDH operation.                                */
        optiga_pub_key.length = sizeof(OPTIGA_TRUST_X_PEER_PUBLIC_KEY_s);
        optiga_pub_key.public_key = (uint8_t*) &g_optiga_trust_x_peer_pub_key;

        if(key_type == OCKAM_VAULT_KEY_STATIC) {
            key_id = OPTIGA_TRUST_X_KEY_SLOT_STATIC;
        } else if(key_type == OCKAM_VAULT_KEY_EPHEMERAL) {
            key_id = OPTIGA_TRUST_X_KEY_SLOT_EPHEMERAL;
        } else {
            ret_val = OCKAM_ERR_INVALID_PARAM;
            break;
        }

        status = optiga_crypt_ecdh(key_id,
                                   &optiga_pub_key,
                                   TRUE,
                                   p_pms);
        if(status != OPTIGA_LIB_SUCCESS) {
            ret_val = OCKAM_ERR_VAULT_TPM_KEY_FAIL;
            break;
        }
    } while (0);

    return ret_val;
}

#endif                                                          /* OCKAM_VAULT_CFG_KEY_ECDH                           */


/*
 ********************************************************************************************************
 ********************************************************************************************************
 *                                        OCKAM_VAULT_CFG_SHA256
 ********************************************************************************************************
 ********************************************************************************************************
 */

#if(OCKAM_VAULT_CFG_SHA256 == OCKAM_VAULT_TPM_INFINEON_OPTIGA_TRUST_X)


/**
 ********************************************************************************************************
 *                                       ockam_vault_tpm_sha256()
 ********************************************************************************************************
 */

OCKAM_ERR ockam_vault_tpm_sha256(uint8_t *p_msg, uint16_t msg_size,
                                 uint8_t *p_digest, uint8_t digest_size)
{
    OCKAM_ERR ret_val = OCKAM_ERR_NONE;
    OCKAM_ERR t_ret_val = OCKAM_ERR_NONE;
    optiga_lib_status_t status = OPTIGA_LIB_SUCCESS;
    optiga_hash_context_t hash_context;
    uint8_t *p_hash_context_buf = 0;
    hash_data_from_host_t hash_data_host;


    do {
        if(digest_size != OPTIGA_TRUST_X_SHA256_DIGEST_SIZE) {
            ret_val = OCKAM_ERR_VAULT_TPM_SHA256_FAIL;
            break;
        }

        do {
            ret_val = ockam_mem_alloc((void**) &p_hash_context_buf,
                                      msg_size);
            if(ret_val != OCKAM_ERR_NONE) {
                break;
            }

            hash_context.hash_algo             = OPTIGA_HASH_TYPE_SHA_256;
            hash_context.context_buffer        = p_hash_context_buf;
            hash_context.context_buffer_length = msg_size;

            status = optiga_crypt_hash_start(&hash_context);
            if(status != OPTIGA_LIB_SUCCESS) {
                ret_val = OCKAM_ERR_VAULT_TPM_SHA256_FAIL;
                break;
            }

            hash_data_host.buffer = p_msg;
            hash_data_host.length = msg_size;

            status = optiga_crypt_hash_update(&hash_context,
                                              OPTIGA_CRYPT_HOST_DATA,
                                              &hash_data_host);
            if(status != OPTIGA_LIB_SUCCESS) {
                ret_val = OCKAM_ERR_VAULT_TPM_SHA256_FAIL;
                break;
            }

            status = optiga_crypt_hash_finalize(&hash_context,
                                                p_digest);
            if(status != OPTIGA_LIB_SUCCESS) {
                ret_val = OCKAM_ERR_VAULT_TPM_SHA256_FAIL;
                break;
            }
        } while(0);

        t_ret_val = ockam_mem_free(p_hash_context_buf);         /* Always attempt to free the context buffer          */
        if(ret_val == OCKAM_ERR_NONE) {                         /* Only save the free return status if there are no   */
            ret_val = t_ret_val;                                /* errors up to this point.                           */
        }
    } while(0);

    return ret_val;
}

#endif                                                          /* OCKAM_VAULT_CFG_SHA256                             */


/*
 ********************************************************************************************************
 ********************************************************************************************************
 *                                         OCKAM_VAULT_CFG_HKDF
 ********************************************************************************************************
 ********************************************************************************************************
 */

#if(OCKAM_VAULT_CFG_HKDF == OCKAM_VAULT_TPM_INFINEON_OPTIGA_TRUST_X)
#error "Error: OCKAM_VAULT_CFG_HKDF invalid for INFINEON OPTIGA TRUST X"
#endif                                                          /* OCKAM_VAULT_CFG_HKDF                               */


/*
 ********************************************************************************************************
 ********************************************************************************************************
 *                                       OCKAM_VAULT_CFG_AES_GCM
 ********************************************************************************************************
 ********************************************************************************************************
 */

#if(OCKAM_VAULT_CFG_AES_GCM == OCKAM_VAULT_TPM_INFINEON_OPTIGA_TRUST_X)
#error "Error: OCKAM_VAULT_CFG_AES_GCM invalid for INFINEON OPTIGA TRUST X"
#endif                                                          /* OCKAM_VAULT_CFG_AES_GCM                            */

