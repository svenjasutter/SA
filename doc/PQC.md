# Post Quantum Encryption
## Round 4
On July 5, 2022, NIST announced four candidates for PQC Standardization Round 4.
(x => will be standardized)

### Public-Key Encryption / Key-Establishment Mechanisms (KEMs)
- CRYSTALS-KYBER (x)
- BIKE
- HQC
- SIKE

### Digital Signatures
- CRYSTALS-Dilithium (x)
- FALCON (x)
- SPHINCS+ (x)

### Mathematical Basis
CRYSTALS-KYBER and CRYSTALS-Dilithium are based on structured lattices.
FALCON is also lattice-based.
SPHINCS+ relies on hash functions.
BIKE and HQC are based on structured codes.
Classic McEliece is based on coding theory.
SIKE is based on isogenies, which are mathematical mappings between elliptic curves.


Source: https://www.nist.gov/news-events/news/2022/07/pqc-standardization-process-announcing-four-candidates-be-standardized-plus

## CRYSTALS-KYBER
Kyber-512 aims at security roughly equivalent to AES-128
Kyber-768 aims at security roughly equivalent to AES-192
Kyber-1024 aims at security roughly equivalent to AES-256

### Developer Recomendations
- hybrid mode: combination with "pre-quantum"
- use Kyber-768 parameter set (achieves more than 128 bits of security against all known classical and quantum attacks)

### Official Implementation

The official Kyber implementation is written in C (https://github.com/pq-crystals/kyber).
Open Quantum Safe (OQS): support the development and prototyping of quantum-resistant cryptography with an open source C library (https://openquantumsafe.org/)

### Third-party Implementations
https://pq-crystals.org/kyber/software.shtml

#### Kyber-py for comprehension
https://github.com/giacomopope/kyber-py

#### api.h

#define crypto_kem_keypair KYBER_NAMESPACE(_keypair)
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);

#define crypto_kem_enc KYBER_NAMESPACE(_enc)
int crypto_kem_enc(unsigned char *ct,
                   unsigned char *ss,
                   const unsigned char *pk);

#define crypto_kem_dec KYBER_NAMESPACE(_dec)
int crypto_kem_dec(unsigned char *ss,
                   const unsigned char *ct,
                   const unsigned char *sk);

#### Key Exchange

Recipient:

1. create_keypair() --> Public Key, Secret Key

4. uses Secret Key to decapsulate the Shared Secret from ciphertext
   decrypt_message() --> Shared Secret (ss)

Sender:

2. Uses Recipient's Public Key to encapsulate a shared secret
   encrypt_message() --> Ciphertext (ct), Shared Secret (ss)

3. Tansmission of Ciphertext to recipient


