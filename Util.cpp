#pragma once 

#include<iostream>
#include<sstream>
#include<string>

namespace Util {

const class MessageFormater {
	private: 
	template<typename T>
	static void appendToStream(std::ostringstream& oss, const T& arg) {
		oss << arg;
	}

	template<typename T, typename... Args>
	static void appendToStream(std::ostringstream& oss, const T& arg, const Args&... args) {
		oss << arg;
		appendToStream(oss, args...);
	}

	public: 
	template<typename... Args>
	static std::string print(const Args&... args) {
		std::ostringstream oss;
		appendToStream(oss, args...);
		return oss.str();
	}

};

}