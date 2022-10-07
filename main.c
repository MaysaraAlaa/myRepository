#include <stdio.h>
// Yes, no <stdlib.h>. That's intended !

void go_south_east(int *lat, int *lon)// Pass in the current address of the latitude and the longitude
{
	*lat = *lat - 1; // Decrease what's inside the latitude's address which is (latitude)
	*lon = *lon + 1; // Increase what's inside the longitude's address which is (longitude)
} /* Using the function here will define it before the main() so it is waiting for arguments & will display an error message if I don't give it
   * arguments when I call it */

int main()
{
 	int latitude = 32;    // Those local variables can't be changed or used outside the main function !!
	int longitude = -64; // Having the same variable names inside and outsid a function, it will always use the insider one !!
	
	go_south_east(&latitude, &longitude); // Pass in the addresses of (latitude, longitude) to pointer variables (lat, lon)
	
	printf("Avast! Now at: [%i, %i]\n", latitude, longitude); // Print their values after it's been changed internally !
	
	return 0;
}