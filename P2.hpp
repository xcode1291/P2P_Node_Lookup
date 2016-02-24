#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <utility> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
using namespace std;

void removeQuote(string& str) {
  str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
}

void trim(string& s)
{
   size_t p = s.find_first_not_of(" \t");
	 s.erase(0, p);

   p = s.find_last_not_of(" \t");
   if (string::npos != p)
      s.erase(p+1);
}

vector<string> split(string input, char delimit) {
  vector<string> tokens;
  string token;
  for (int i = 0; i < input.size(); i++) {
    if (input[i] != delimit) {
      token.push_back(input[i]);
    } else {
      trim(token);
      if (token.size() > 0) {
	tokens.push_back(token);
      }
      token.clear();
    }
  }
  trim(token);
  if (token.size() > 0) {
    tokens.push_back(token);
  }
  return tokens;
}

bool validateIpAddress(const string &ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    return result != 0;
}


vector<string> getParameters(string input) {
	vector<string> result;
  string delimiter = ",";
  size_t pos = 0;
  string token;
  while ((pos = input.find(delimiter)) != std::string::npos) {
    token = input.substr(0, pos);
    removeQuote(token);
		trim(token);
    result.push_back(token);
    input.erase(0, pos + delimiter.length());
  }
	removeQuote(input);
  trim(input);
	result.push_back(input);
  return result;
}

unsigned int processString(string item) {
  while (item.size() < 4) {
    item.push_back((char)0);
  }
return (unsigned int)item[0] << 24 | (unsigned int) item[1] << 16 | 
         (unsigned int)item[2] << 8 |  (unsigned int) item[3]; 
}

unsigned int reverseBit(unsigned int v) {
  v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
  v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
  v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
  v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
  v = ( v >> 16             ) | ( v               << 16);
  return v;
}

unsigned int hashFunc(string input) {
  vector<string> tokens;
  int i;
  for (i = 0; i < input.size() / 4 ; i++) {
    tokens.push_back(input.substr(i*4, 4));
  }
  if (input.size() > i * 4) {
    tokens.push_back(input.substr(i*4, input.size() - i*4));
  }  
  unsigned int result = 0;
  for (int iter = 0; iter < tokens.size(); iter++) {
    unsigned int curr = processString(tokens[iter]);
    if ((iter+1) % 2) {
      curr = reverseBit(curr);
    }
    result ^= curr;
  }
  return result;
}

unsigned int ipToInt(string ip) {
  stringstream s(ip);
  unsigned int a, b, c, d; //to store the 4 ints
  char ch; //to temporarily store the '.'
  s >> a >> ch >> b >> ch >> c >> ch >> d;
  return (a << 24) + (b << 16) + (c << 8) +  d; 
}

class PeerToPeer {

public:
  bool addNode(string nodeIp);
  bool removeNode(string nodeIp);
  bool insertItem(string mynodeIp, string itemName);
  bool deleteItem(string mynodeIp, string itemName);
  bool find(string mynodeIp, string itemName);
  vector<string> nodeTable;
  vector<unsigned int> nodeHash;
  map<unsigned int, vector<pair<string, string> > > musicTable; 
  map<unsigned int, string> hashToIpMap;
  long long successor(long long v);
  long long finger(long long current, int i);
}; 
