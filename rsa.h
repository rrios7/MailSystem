#include <algorithm>
#include "list.h"

namespace RSA {

const static int p = 107;
const static int q = 97;

int modPow(int x, int n, int m) {
  if (n == 0)
    return 1 % m;
  int u = modPow(x, n / 2, m);
  u = (u * u) % m;
  if (n % 2 == 1)
    u = (u * x) % m;
  return u;
}

List<std::pair<int, int>> generateKeys() {
  int n, phi, d, e;
  n = p * q;
  phi = (p - 1) * (q - 1);

  for (e = 2; e < phi; ++e)
    if (std::__gcd(e, phi) == 1 and std::__gcd(e, n) == 1)
      break;

  d = 2;
  int counter = 0;

  while (true) {
    if ((d * e) % phi == 1) {
      if (counter > 0)
        break;
      counter += 1;
    }
    ++d;
  }
  return List<std::pair<int, int>>{{e, n}, {d, n}};
}

int cipher(char c, std::pair<int, int> publicKey) {
  return modPow(c, publicKey.first, publicKey.second);
}

char decipher(int c, std::pair<int, int> privateKey) {
  return static_cast<char>(modPow(c, privateKey.first, privateKey.second));
}

std::string encrypt(const std::string& txt, std::pair<int, int> publicKey) {
  std::string encrypted;
  for (char c : txt) {
    std::string number = std::to_string(cipher(c, publicKey));
    encrypted += std::to_string(number.length()) + number;
  }
  return encrypted;
}

std::string decrypt(const std::string& txt, std::pair<int, int> privateKey) {
  std::string decrypted;

  for (size_t i = 0; i < txt.size(); ++i) {
    int charLength = txt[i] - '0';
    std::string container;
    while (charLength--) {
      container += txt[++i];
    }
    int c = stoi(container);
    decrypted += decipher(c, privateKey);
  }
  return decrypted;
}

}  // namespace RSA
