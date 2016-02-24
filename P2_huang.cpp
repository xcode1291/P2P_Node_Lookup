#include <sstream>
#include <string.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

//convert integer to string
string itos(int num){
    stringstream s;
    s << num;
    return s.str();
}

string decimal_binary(int num){
    string binary = "", addZero = "";
    int a = num;
    while(a > 0){
	binary += itos(a%2);
	a = a/2;
    }
    int zeroCounts = 8 - binary.length();
    for(int i = 0; i < zeroCounts; i++){
	addZero += "0";
    }
    if(binary.size() < 8){
	binary += addZero;
    }
    int i = 0, j = binary.length()-1;
    while(i <= j){
	char temp = binary[i];
	binary[i] = binary[j];
	binary[j] = temp;
	i++;
	j--;
    }
    return binary;
}

unsigned  binary2Integer(string s){
    unsigned result = 0;
    for(int i = 0; i < s.length(); i++){
	int temp = (int)(s[i] - '0');
	result = result*2 + temp;
    } 
    return result;
}

//convert an IP address to unsigned integer
unsigned processIP(string s){
    vector<string> ip;
    int j = 0;
    while(j < s.length()){
      int i = j;
      int start = i;
      while(i < s.length()){
	if(s[i] != '.'){
	  i++;
	}else{
	  break;
	}
      }
      ip.push_back(s.substr(start, i-start));
      j = i+1;
    }
    vector<int> ips;
    for(int i = 0; i < ip.size(); i++){
      string temp = ip[i];
      int num = 0;
      for(int j = 0; j < temp.length(); j++){
	int digit = (int)(temp[j]-'0');
	num = num*10 + digit;
      } 
      ips.push_back(num);
    }  
    string binary = "";
    for(int i = 0; i < ips.size(); i++){
	binary += decimal_binary(ips[i]);
    }
    return binary2Integer(binary);
}

//cut the item name into chuncks with 4 chars in each chunck
vector<string> cutString(string item){
    vector<string> ret;  
    int i = 0;
    while(i < item.length()){
	if(i+3 < item.length()){
	    ret.push_back(item.substr(i, 4));
	    i = i+4;
	}else{
	    ret.push_back(item.substr(i, item.length()-1-i));
	}
    } 
    return ret;
}






int main(){
    cout << processIP("129.210.16.48")<< endl;
    cout << processIP("129.210.16.128") << endl;
    cout << processIP("129.210.16.32") << endl;
    cout << processIP("255.255.255.255") << endl;
    return 0;
}














