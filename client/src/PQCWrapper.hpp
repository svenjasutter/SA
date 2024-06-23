#ifndef PQCWRAPPER_HPP
#define PQCWRAPPER_HPP
#include <string>
#include <vector>

class PQCWrapper {
 public:
  PQCWrapper();

  std::pair<std::string, std::string> createKeyPair();

  std::pair<std::string, std::string> encrypt(const std::string& publicKey);

  std::string decrypt(const std::string& secretKey,
                      const std::string& ciphertext);

  std::vector<unsigned char> aes_encrypt(
      const std::vector<unsigned char>& plaintext,
      const std::vector<unsigned char>& key);

  std::vector<unsigned char> aes_decrypt(
      const std::vector<unsigned char>& ciphertext,
      const std::vector<unsigned char>& key);
};

#endif  // PQCWRAPPER_HPP
