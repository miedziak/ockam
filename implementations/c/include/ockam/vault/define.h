/**
 ********************************************************************************************************
 * @file    define.h
 * @brief   Vault configuration define values
 ********************************************************************************************************
 */

#ifndef OCKAM_VAULT_DEFINE_H_
#define OCKAM_VAULT_DEFINE_H_


/*
 ********************************************************************************************************
 *                                             Vault Defines                                            *
 ********************************************************************************************************
 */

#define OCKAM_VAULT_CFG_TPM                     0x0001000
#define OCKAM_VAULT_CFG_HOST                    0x0002000


/*
 ********************************************************************************************************
 *                                            Hardware Mfgs                                             *
 ********************************************************************************************************
 */

#define OCKAM_VAULT_CFG_TPM_MICROCHIP           0x00000100


/*
 ********************************************************************************************************
 *                                            Hardware Parts                                            *
 ********************************************************************************************************
 */

#define OCKAM_VAULT_TPM_MICROCHIP_ATECC508A      (OCKAM_VAULT_CFG_TPM | OCKAM_VAULT_CFG_TPM_MICROCHIP | 0x00000001)
#define OCKAM_VAULT_TPM_MICROCHIP_ATECC608A      (OCKAM_VAULT_CFG_TPM | OCKAM_VAULT_CFG_TPM_MICROCHIP | 0x00000002)


/*
 ********************************************************************************************************
 *                                            Crypto Libraries                                          *
 ********************************************************************************************************
 */

#define OCKAM_VAULT_HOST_MBEDCRYPTO               (OCKAM_VAULT_CFG_HOST | 0x00000001)


#endif
