/*
	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
	@date: 22019-11-17 
	@brief: A simple program to expand \t to spaces
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void usage(char *myprog)
{
	fprintf(stderr, "Usage: %s [-t tabstop] [-o outfile] [file...]\n", myprog);
	exit(EXIT_FAILURE);
}

FILE *opn_str(char *f_name, int fd, const char *mode)
{
	if (f_name){
		return fopen(f_name, mode);

	} else {
		return fdopen(fd, mode);
	}
}

int main(int argc, char *argv[])
{
	int c;
	char *outfile = NULL;
	char *inputfile = NULL;
	int tabstop = 8;

	// reads in command line arguments
	while( (c = getopt(argc, argv, "t:o:h")) != -1 ){
		switch( c ){
			case 't':
				tabstop = (int) strtol(optarg, NULL, 0);
				break;

			case 'o':
				outfile = optarg;
				break;

			case 'h':
				usage(argv[0]);
				exit(EXIT_FAILURE);
				break;

			default:
				usage(argv[0]);
				exit(EXIT_FAILURE);
				break;
		}
	}

	if (optind > argc-2){
		inputfile = argv[optind];
	}

	FILE *out_fp;
	FILE *in_fp;

	// opens filename if exists, otherwise STDIN or STDOUT
	out_fp = opn_str(outfile, STDOUT_FILENO, "w");
	in_fp = opn_str(inputfile, STDIN_FILENO, "r");

	if (in_fp) {

		while ( (c = fgetc(in_fp) ) != EOF ){
			if ( c == '\t' ){
				for (int i = 0; i < tabstop; ++i){
					fputc(' ', out_fp);
				}
			} else {
				fputc(c, out_fp);
			}
		}
		fclose(in_fp);
		fclose(out_fp);
	}
	return EXIT_SUCCESS;
}


