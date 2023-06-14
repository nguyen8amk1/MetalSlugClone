#pragma once 

#include<iostream>
#include<sstream>
#include<string>

namespace MetalSlug {
	
template<typename T>
void appendToStream(std::ostringstream& oss, const T& arg) {
	oss << arg;
}

template<typename T, typename... Args>
void appendToStream(std::ostringstream& oss, const T& arg, const Args&... args) {
	oss << arg << ' ';
	appendToStream(oss, args...);
}

template<typename... Args>
std::string print(const Args&... args) {
	std::ostringstream oss;
	appendToStream(oss, args...);
	return oss.str();
}


}