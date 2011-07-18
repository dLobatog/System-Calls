#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
* filter <filter> <binaryfile> <binaryfileoutput>
* 
* It prints some products depending on the filter used to a binary file output
*
*/

// Definition of the structure of each record
typedef struct {
	char product[52];
	float weight;
	int reference, stock;
} Record;

// Prints records
void recPrint(const Record *rec) {
	printf("%s\t%f\t%d\t%d\n", rec->product, rec->weight, rec->reference, rec->stock);
}

// Writes record to a binary file. File descriptor must be passed as argument.
ssize_t recWriteBin(int fh, Record *rec) {
	size_t size = sizeof(Record);
	ssize_t written;
	int stock;

	printf("Writing Record: ");
	recPrint(rec);

	if ((written = write(fh, rec, size))!=sizeof(Record)) {
		perror("write error");
	}
	return written;
}

// Prints records to a binary file depending on the filter chosen by the user
void recFilter(Record *rec,int size,const char *filter, const char *outputfile, int fh) {
	int stock;
	float total=size;
	stock = rec->stock;
	if (stock>=0 && stock<100) {
		if (strcmp(filter,"L") == 0) {
			recWriteBin(fh,rec);
		}
	}
	if (stock>=100 && stock<500) {
		if (strcmp(filter,"M") == 0) {
			recWriteBin(fh,rec);

		}
	}
	if (stock>=500) {
		if (strcmp(filter,"H") == 0) {
			recWriteBin(fh,rec);
		}
	}

}

// 'Batch' process. Reads each of the records passed as argument and executes recFilter to write only the proper ones on the binaryfile.
void recFilterBatch(Record *rec, int size, const char *filter, const char *outputfile, int fh) {
	while(size-->0) {
		recFilter(rec, size, filter, outputfile, fh);
		rec++;

	}
}

int readBinary(const char *filter, const char *filename, const char *outputfile) {
	char product[52];
	float weight;
	int reference, stock;
	int nlines=0;
	int i=0;
	char line [86];
	int fh;
	// reads binary file and counts lines
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
	close(fh);
	// creates binary file
	if((fh = creat(outputfile,0666)) < 0) {
		perror("Error when creating binary file");
		return 1;
	}
	recFilterBatch(recs, nlines, filter, outputfile, fh);
	close(fh);
	return 0;

}

// reads the output file in order to ensure everything went ok
int testRead(const char *filename) {
	int fh;

	if((fh = open(filename,O_RDONLY)) < 0) {
		perror("Error opening binary file");
		return 1;
	} else {
		Record rec;
		while(read(fh, &rec, sizeof(Record))>0) {
			printf("Read Record: ");
			recPrint(&rec);
		}
		close(fh);
		return 0;
	}
}


int main(int argc, char *argv[]) {
	if (argc==4) {
		if((strcmp(argv[1],"H") == 0) || (strcmp(argv[1],"M") == 0) || (strcmp(argv[1],"L") == 0)) {
			printf("Filter will be %s \nBinary file input will be %s \nBinary file output will be %s \n", argv[1],argv[2],argv[3]);
			readBinary(argv[1],argv[2],argv[3]);
			testRead(argv[3]);
		} else {
			printf("Invalid filter\n");
			return 1;
		}
	} else {
		printf("Wrong number of parameters\n");
		return 1;
	}
	return 0;
}

