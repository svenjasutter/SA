#include "PQCWrapper.hpp"

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

extern "C" {
#include "/pqc/ref/api.h"
}

PQCWrapper::PQCWrapper() {}

std::pair<std::string, std::string> PQCWrapper::createKeyPair() {
  uint8_t pk[pqcrystals_kyber768_PUBLICKEYBYTES];
  uint8_t sk[pqcrystals_kyber768_SECRETKEYBYTES];

  // Kyber expects an already allocated array of KYBER_PUBLICKEYBYTES /
  // KYBER_SECRETKEYBYTES bytes Returns 0 (success)
  int result = pqcrystals_kyber768_ref_keypair(pk, sk);

  if (result != 0) {
    throw std::runtime_error("Error in key pair generation");
  }

  std::string publicKey(pk, pk + pqcrystals_kyber768_PUBLICKEYBYTES);
  std::string secretKey(sk, sk + pqcrystals_kyber768_SECRETKEYBYTES);

  return std::make_pair(publicKey, secretKey);
}

std::pair<std::string, std::string> PQCWrapper::encrypt(
    const std::string& publicKey) {
  uint8_t ct[pqcrystals_kyber768_CIPHERTEXTBYTES];
  uint8_t ss[pqcrystals_kyber768_BYTES];

  // Kyber expects: const uint8_t *pk
  const uint8_t* pkBytes = reinterpret_cast<const uint8_t*>(publicKey.data());

  int result = pqcrystals_kyber768_ref_enc(ct, ss, pkBytes);

  if (result != 0) {
    throw std::runtime_error("Error in encryption");
  }

  std::string ciphertext(ct, ct + pqcrystals_kyber768_CIPHERTEXTBYTES);
  std::string sharedSecret(ss, ss + pqcrystals_kyber768_BYTES);

  return std::make_pair(ciphertext, sharedSecret);
}

std::string PQCWrapper::decrypt(const std::string& secretKey,
                                const std::string& ciphertext) {
  uint8_t ss[pqcrystals_kyber768_BYTES];

  // Kyber exprects: const uint8_t *ct
  const uint8_t* ctBytes = reinterpret_cast<const uint8_t*>(ciphertext.data());
  // Kyber exprects: const uint8_t *sk
  const uint8_t* skBytes = reinterpret_cast<const uint8_t*>(secretKey.data());

  int result = pqcrystals_kyber768_ref_dec(ss, ctBytes, skBytes);

  if (result != 0) {
    throw std::runtime_error("Error in decryption");
  }

  std::string decryptedMessage(ss, ss + pqcrystals_kyber768_BYTES);

  return decryptedMessage;
}

std::vector<unsigned char> PQCWrapper::aes_encrypt(
    const std::vector<unsigned char>& plaintext,
    const std::vector<unsigned char>& key) {
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  std::vector<unsigned char> ciphertext;
  int len;
  int ciphertext_len;

  assert(key.size() == 32);

  unsigned char iv[AES_BLOCK_SIZE];
  RAND_bytes(iv, AES_BLOCK_SIZE);

  EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv);

  ciphertext.resize(plaintext.size() + AES_BLOCK_SIZE);
  EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(),
                    plaintext.size());
  ciphertext_len = len;

  EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
  ciphertext_len += len;

  EVP_CIPHER_CTX_free(ctx);

  ciphertext.resize(ciphertext_len + AES_BLOCK_SIZE);
  memmove(ciphertext.data() + AES_BLOCK_SIZE, ciphertext.data(),
          ciphertext_len);
  memcpy(ciphertext.data(), iv, AES_BLOCK_SIZE);

  return ciphertext;
}

std::vector<unsigned char> PQCWrapper::aes_decrypt(
    const std::vector<unsigned char>& ciphertext,
    const std::vector<unsigned char>& key) {
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  std::vector<unsigned char> plaintext;
  int len;
  int plaintext_len;

  // AES-256
  assert(key.size() == 32);

  unsigned char iv[AES_BLOCK_SIZE];
  memcpy(iv, ciphertext.data(), AES_BLOCK_SIZE);

  EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv);
  // AES-256 in CBC mode, decryption key, IV

  plaintext.resize(ciphertext.size());
  EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                    ciphertext.data() + AES_BLOCK_SIZE,
                    ciphertext.size() - AES_BLOCK_SIZE);  // decrypt
  plaintext_len = len;

  EVP_DecryptFinal_ex(ctx, plaintext.data() + len,
                      &len);  // process remaining bytes
  plaintext_len += len;

  EVP_CIPHER_CTX_free(ctx);

  plaintext.resize(plaintext_len);

  return plaintext;
}
