#include <string>
#include <string>
#include <vector>

int main() {

	std::string inp = " 32'-7 5/16";

	std::vector<std::string> parsed;
	std::string tmp;
    
    // set starting condition
    bool is_prev_num = false;
    if (isdigit(inp[0]) && inp[0] != ' ') {
         is_prev_num = true;
    }
	
	auto itt = inp.begin();
	char c;
	
	while ( itt != inp.end() ) {
	    c = *itt;
		if (isdigit(c) == is_prev_num) {
		    if( c != ' ' && c != '-') {
		        tmp += c;
		        // if on last iteration then force push back
		        if ( itt == inp.end() - 1) {
		            parsed.push_back(tmp);
		        }
		    }
		} else {
			is_prev_num = false;
			if( c != ' ' && c != '-') {
			    parsed.push_back(tmp);
			    tmp = c;  // starts new tmp
			}
		}
		if (isdigit(c)) is_prev_num = true;
		itt++;
		
	}
	
	for ( auto p : parsed) {
	    std::cout << p << "\n";
	}
}
