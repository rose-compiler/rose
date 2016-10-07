// Duff's device related code found in Mozilla.

int main(){
//Duffs device fro dbm/src/h_func.c
//
//
register unsigned int len;
register unsigned int loop;
    switch (len & (8 - 1)) {
		case 0:
			do {
                ;
				/* FALLTHROUGH */
		case 7:
				;
				/* FALLTHROUGH */
		case 6:
				;
				/* FALLTHROUGH */
		case 5:
				;
				/* FALLTHROUGH */
		case 4:
				;
				/* FALLTHROUGH */
		case 3:
				;
				/* FALLTHROUGH */
		case 2:
				;
				/* FALLTHROUGH */
		case 1:
				4;
			} while (--loop);
    }	

};
