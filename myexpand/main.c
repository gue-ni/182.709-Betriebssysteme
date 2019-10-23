/*
	@author: Jakob G. Maier 11809618
	@date: 18.10.19
	@details:
	@brief:
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  

void usage(char *myprog)
{
	fprintf(stderr, "Usage: %s [-t tabstop] [-o outfile] [file...]\n", myprog);
	exit(EXIT_FAILURE);
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

	//if (optind > argc-2){
	//	inputfile = argv[optind];
	//}

	FILE *out_fp;
	FILE *in_fp;

	if (outfile){
		out_fp = fopen(outfile, "w");
	} else {
		out_fp = stdout;
	}

	if (inputfile){
		in_fp = fopen(inputfile, "r");
	} else {
		in_fp = stdin;
	}

	int next_c;

	if (in_fp) {
		// feof, fileno both work
		if (isatty(feof(in_fp)) && !inputfile){  // esit if no inputfile and empty stdin
			return EXIT_FAILURE;
		}

		while ((c = getc(in_fp)) != EOF){ // programm hängt hier
			if (c == '\\'){
				if ((next_c = getc(in_fp)) == 't'){
					for (int i = 0; i < tabstop; ++i)
					{
						fputc(' ', out_fp);
					}
				} else {
					fputc(c, out_fp);
					ungetc(next_c, in_fp);
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


