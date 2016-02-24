#include "P2.hpp"
// global instance for p2p system
PeerToPeer sys;
bool PeerToPeer::addNode(string nodeIp) {
  for (int i = 0; i < nodeTable.size(); i++) {
    if (nodeIp == nodeTable[i]) {
      cout << "# addNode(\""<< nodeIp <<"\"); returns failed\n";
      cout << "	can't add node " << nodeIp << " since it's already"
           << " existed in the system. add node failed\n";
      return true;
    }
  }   
  nodeTable.push_back(nodeIp);
  hashToIpMap[ipToInt(nodeIp)] = nodeIp;
  nodeHash.push_back(ipToInt(nodeIp));
  sort(nodeHash.begin(), nodeHash.end());
  cout << "# addNode(\""<< nodeIp <<"\"); returns success\n";
  unsigned int successorOfNode = (unsigned int) (successor(ipToInt(nodeIp) % (1ull << 32)));
  if (successorOfNode != ipToInt(nodeIp) || musicTable.find(successorOfNode) != musicTable.end()) {
  	vector<pair<string, string> > tmp;
	vector<pair<string, string> >::iterator iter;
	vector<vector<pair<string, string> >::iterator> needToDeleted;
	for (iter = musicTable[successorOfNode].begin(); iter != musicTable[successorOfNode].end(); iter++) {
	  if ((unsigned int) (successor(hashFunc(iter->second)) % (1ull << 32))
	       == ipToInt(nodeIp)) {
				tmp.push_back(*iter);
				needToDeleted.push_back(iter);
			}
		}
		for (int i = 0; i < needToDeleted.size(); i++) {
		  musicTable[successorOfNode].erase(needToDeleted[i]);
		}
		musicTable[ipToInt(nodeIp)] = tmp;
  }	
  return true;
}

bool PeerToPeer::removeNode(string nodeIp) {
  vector<string>::iterator it;
  for (it = nodeTable.begin(); it != nodeTable.end(); it++) {
    if (*it == nodeIp) {
      break;
    }
  }
  if (it == nodeTable.end()) {
    cout << "# removeNode(\"" << nodeIp << "\"); return failed\n";
    cout << "	remove node " << nodeIp << "failed since it's not existed.\n";
    return true;
  }
  nodeTable.erase(it);
  cout << "# removeNode(\"" << nodeIp << "\"); return success\n";
  unsigned int hashCode = ipToInt(nodeIp);
  unsigned int successorVal = (unsigned int) (successor(hashCode) % (1ull << 32));
  vector<unsigned int>::iterator iter;
  for (iter = nodeHash.begin(); iter != nodeHash.end(); iter++) {
    if (*iter == hashCode) 
      break;
  } 
  nodeHash.erase(iter);
  if (musicTable.find(hashCode) != musicTable.end()) {
    vector<pair<string, string> > items = musicTable[hashCode];
    if (musicTable.find(successorVal) == musicTable.end()) {
      musicTable[successorVal] = items;
    } else {
      for (int i = 0; i < items.size(); i++) {
	musicTable[successorVal].push_back(items[i]);
      }
    }
  }
  hashToIpMap.erase(hashToIpMap.find(ipToInt(nodeIp)));
  return true;
}

bool PeerToPeer::insertItem(string mynodeIp, string itemName) {
  cout << "# insertItem(\"" << mynodeIp << "\", \"" << itemName << "\"); "
       << "return success\n";
  unsigned int itemHash = hashFunc(itemName);
  unsigned int successorVal = (unsigned int)(successor(itemHash) % (1ull << 32));
	if (musicTable.find(successorVal) == musicTable.end()) {
    vector<pair<string, string> > tmp;
    musicTable[successorVal] = tmp;
  }  
  musicTable[successorVal].push_back(make_pair(mynodeIp,itemName));
  return true;
}

bool PeerToPeer::deleteItem(string mynodeIp, string itemName) {
  unsigned int itemHash = hashFunc(itemName);
  unsigned int successorVal = (unsigned int)(successor(itemHash) % (1ull << 32));
  vector<pair<string, string> >::iterator it;
  for (it = musicTable[successorVal].begin(); it != musicTable[successorVal].end(); it++) {
    if (it->second == itemName) break;
  }
  if (it->first != mynodeIp) {
    cout << "# deleteItem(\"" + mynodeIp + "\", \"" + itemName + "\"); return failed\n";
    cout << "  " << mynodeIp << " is not the owner of the " << itemName << "\n";
    return true;
  }
  cout << "# deleteItem(\"" + mynodeIp + "\", \"" + itemName + "\"); return success\n";
  musicTable[successorVal].erase(it);
  return true;
}

long long PeerToPeer::successor(long long v) {
  unsigned int left = (unsigned int) (v % (1ull << 32));
  unsigned int successor = nodeHash[0];
  for (int i = 0; i < nodeHash.size(); i++) {
    if (left < nodeHash[i]) {
      successor = nodeHash[i];
      break;
    }  
  }
  v = v - left + successor;
  if (successor < left) {
    v +=  (1ull << 32); 
  }
  return v;
}

long long  PeerToPeer::finger(long long current, int i) {
  return current + (1ull << i);
}

bool PeerToPeer::find(string mynodeIp, string itemName) {
  long long target = (long long) hashFunc(itemName);
  long long currentHash = (long long)ipToInt(mynodeIp);
  if (currentHash > target) {
    long long offset = 0x1ll << 32;
    target = target + offset;
  }
  vector<string> seq;
  seq.push_back(mynodeIp);
  vector<int> steps;
  bool result = false;
  int iter = 0;
  while (true && iter < 100) {
    iter++ ;
    if ((currentHash <= target && target <= successor(currentHash))) {
      unsigned int successorHash = (successor(currentHash) % (1ull << 32));
      seq.push_back(hashToIpMap[successorHash]);
      if (musicTable.find(successorHash) != musicTable.end()) {
	for (int songIndex = 0; songIndex < musicTable[successorHash].size();
		songIndex++ ) {
	    if (musicTable[successorHash][songIndex].second == itemName) {
	      result = true;
	    }
	 }
      } 
      break;
    }
    currentHash =  successor(currentHash);
    unsigned int successorHash = (currentHash % (1ull << 32));
    seq.push_back(hashToIpMap[successorHash]);
    int i;
    for (i = 0; i <= 31; i++) {
      if (finger(currentHash, i) > target) {
	break;
      }
    }
    steps.push_back(i-1);
    currentHash = finger(currentHash, i-1); 
  }
  cout << "# find(\"" << mynodeIp << "\", \"" << itemName << "\"); ";
  if (result) {
    cout << "returns success\n";
  } else {
    cout << "returns failed\n";
  } 
  cout <<"  \"" << itemName << "\" hashed to 0x" << hex << hashFunc(itemName)<<"\n";
  cout <<"  " << seq[0] << " -> " << seq[1] << " (successor)";
  for (int i = 2; i < seq.size(); i++ ) {
    cout << " --> " << seq[i] << " (finger table entry j=" << dec << steps[i-2] << ")";
  }
  cout << "\n";
  return true;
}

bool processInput(string line) {  
  if (line.find("addNode") != string::npos) {
    size_t start = line.find("(");
    string parameter = line.substr(start + 1, line.find(")") - start -1 );
    vector<string> paras = getParameters(parameter);
    if (paras.size() != 1) {
      cout << "wrong numer of parameters for addNode function.\n";
      return false;
    }
    if (!validateIpAddress(paras[0])) {
      cout <<"invalid IP address for addNode funcion.\n";
      return false;
    }
    return sys.addNode(paras[0]);     
  } 
 
 if (line.find("insertItem") != string::npos) {
    size_t start = line.find("(");
    string parameter = line.substr(start + 1, line.find(")") - start -1 );       
    vector<string> paras = getParameters(parameter);
    if (paras.size() != 2) {
      cout << "wrong number of parameters for insertItem function.\n";
      return false;
    } 
    if (!validateIpAddress(paras[0])) {
      cout <<"invalid IP address for insertItem funcion.\n";
      return false;
    }
    return sys.insertItem(paras[0], paras[1]);
  } 
  
 if (line.find("find") != string::npos) {
    size_t start = line.find("(");
    string parameter = line.substr(start + 1, line.find(")") - start -1 );       
    vector<string> paras = getParameters(parameter);
    if (paras.size() != 2) {
      cout << "wrong number of parameters for find function.\n";
      return false;
    } 
    if (!validateIpAddress(paras[0])) {
      cout <<"invalid IP address for find funcion.\n";
      return false;
    }
    return sys.find(paras[0], paras[1]);
  } 
 
  if (line.find("removeNode") != string::npos) {
    size_t start = line.find("(");
    string parameter = line.substr(start + 1, line.find(")") - start -1 );       
    vector<string> paras = getParameters(parameter);
    if (paras.size() != 1) {
      cout << "wrong number of parameters for removeNode function.\n";
      return false;
    } 
    if (!validateIpAddress(paras[0])) {
      cout <<"invalid IP address for removeNode funcion.\n";
      return false;
    }
    return sys.removeNode(paras[0]);  
  }
  
  if (line.find("deleteItem") != string::npos) {
    size_t start = line.find("(");
    string parameter = line.substr(start + 1, line.find(")") - start -1 ); 
    vector<string> paras = getParameters(parameter);
    if (paras.size() != 2) {
      cout << "wrong number of parameters for deleteItem function.\n";
      return false;
    } 
    if (!validateIpAddress(paras[0])) {
      cout <<"invalid IP address for deleteItem funcion.\n";
      return false;
    }
    return sys.deleteItem(paras[0], paras[1]); 
  }
  return false; 
}


int main() {
  /*processInput("addNode(\"129.210.16.48\");");
  processInput("addNode(\"129.210.16.128\");");
  processInput("insertItem(\"129.210.16.48\", \"Listen to the Music\");");
  processInput("insertItem(\"129.210.16.8\", \"time to say goodbay\");");
  processInput("find(\"129.210.16.48\", \"Listen to the music\");");
  processInput("deleteItem(\"129.210.16.148\", \"time to say goodbay\");");
  processInput("removeNode(\"129.210.16.48\", \"time to say goodbay\");");
  //cout << hex << hashFunc("Listen to the Music") << endl;
  //cout << ipToInt("129.210.16.48") << endl; 
  //cout << ipToInt("129.210.16.128") << endl; 
  //cout << ipToInt("129.210.16.32") << endl; */
  /*sys.addNode("129.210.16.48");
  sys.addNode("129.210.16.128");
  sys.insertItem("129.210.16.48", "Listen to the Music");
  sys.insertItem("129.210.16.48", "Time to Say Goodbye");
  sys.find("129.210.16.32", "Listen to the Music"); 
  sys.deleteItem("129.210.16.48", "Listen to the Music");
  sys.find("129.210.16.32", "Listen to the Music");
  sys.removeNode("129.210.16.88");*/ 


  /*sys.addNode("129.210.16.48");
  sys.addNode("129.210.16.68");
  sys.addNode("129.210.16.32");
  sys.addNode("129.210.16.18");
  sys.addNode("129.210.16.18");
  sys.addNode("129.210.16.8");
  sys.insertItem("129.210.16.48", "xyz");
  sys.insertItem("129.210.16.48", "abc");
  sys.deleteItem("129.210.16.45", "xyz");
  sys.find("129.210.16.64", "xyz"); 
  sys.deleteItem("129.210.16.48", "xyz");
  sys.find("129.210.16.64", "xyz"); 
  sys.removeNode("129.210.16.48"); 
  sys.find("129.210.16.64", "abc"); */
  string input_line;
  string total_input;
  while (cin) {
    getline(cin, input_line);
		trim(input_line);
		if (input_line.empty() || input_line.find("#") != string::npos
        || input_line == "returns success") {
      continue; // skip comments 
    }
    total_input += input_line;
    /*if (!processInput(input_line)) {
      cout << "invalid input \"" << input_line << "\" is detected. "
           << "exit the program now\n";
      break;
    } */   
  }
  vector<string> tokens = split(total_input, ';');
  for (int i = 0; i < tokens.size(); i++) {
    if (!processInput(tokens[i])) {
      cout << "invalid input \"" << input_line << "\" is detected. "
           << "exit the program now\n";
      break;
    }     
  } 
  return 0;

}
