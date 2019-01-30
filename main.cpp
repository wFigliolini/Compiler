
#include "R0/R0Test.h"

int main(int argc, char* argv) {
	int status = RTest();
	if ( status = 0 ) {
		return 0;
	}
	else {
		std::cout << "Tests Failed with code " << status << std::endl;
	}
}
