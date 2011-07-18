#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
* statistics <binaryfile>
* 
* It gives statistics about the availability of some products stored in records in a binary file 
*
*/

// Definition of the structure of each record
typedef struct {
	char product[52];
	float weight;
	int reference, stock;
} Record;


void recShowStatistics(Record *rec,int size) {
	// Use of float is required in order to calculate percentages.
	// If stock is over 1000 it is counted as high availability
	float highAv=0;
	float mediumAv=0;
	float lowAv=0;
	float highPerc;
	float mediumPerc;
	float lowPerc;
	int stock;
	float total=size;
	// Calculate availability
	while(size-->0) {
		stock = rec->stock;
		if (stock>=0 && stock<100) {
			lowAv++;
		}
		if (stock>=100 && stock<500) {
			mediumAv++;
		}
		if (stock>=500) {
			highAv++;
		}
		rec++;
	}
	// Calculate percentages
	highPerc = (highAv/total) * 100;
	mediumPerc = (mediumAv/total) * 100;
	lowPerc = (lowAv/total) * 100;
	printf("H: %g % \nM: %g % \nL: %g % \n",highPerc,mediumPerc,lowPerc);
}

int readBinary(const char *filename) {
	char product[52];
	float weight;
	int reference, stock;
	int nlines=0;
	int i=0;
	char line [86];
	int fh;
	// reads binary file and counts the lines
	if((fh = open(filename,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
		while(read(fh, &rec, sizeof(Record))> 0) {
			nlines++;
		}
		close(fh);
	}
	Record recs[nlines];
	if((fh = open(filename,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
		//saves the records of the binary file in an array of records
		while(read(fh, &recs[i], sizeof(Record))>0) {
			i++;
		}
	}
	recShowStatistics(recs, nlines);
	close(fh);
	return 0;

}

int main(int argc, char *argv[]) {
	if (argc==2) {
		printf("Binaryfile will be %s\n", argv[1]);
	} else {
		printf("Wrong number of parameters\n");
		return 1;
	}
	readBinary(argv[1]);
	return 0;
}
