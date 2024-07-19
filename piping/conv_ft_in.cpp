#include <iostream>
#include <string>
#include <vector>

int main() {

	std::string inp = "32'-7 5/16";

	std::vector<std::string> parsed;
	std::string tmp;

	bool is_prev_num = false;

	for ( auto c : inp ) {

		if is_prev_num == isdigit(c) {
			if ( c != " " || c != "" ) { 
				tmp += c;
			}
		} else {
			parsed.push_back(tmp);
			std::cout "\n" << tmp;
			tmp = c;  // starts new tmp
		}
	}
	
	return 0;

}
