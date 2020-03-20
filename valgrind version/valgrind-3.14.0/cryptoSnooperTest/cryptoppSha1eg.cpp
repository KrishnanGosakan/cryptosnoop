#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

#include <string>
#include <iostream>
using namespace std;

int main()
{
  CryptoPP::SHA1 sha1;
  std::string source = "Hello";  //This will be randomly generated somehow
  std::string hash = "";
  CryptoPP::StringSource(source, true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hash))));
  cout<<hash<<"\n";
}
