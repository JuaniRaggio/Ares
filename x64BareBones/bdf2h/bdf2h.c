//==============================================================================
// bdf2h.c
//==============================================================================
// Bitmap Distribution Format (BDF) stores a bitmap font. This will convert it t
// o a C header file.
// Uso:
// ./bdf2h < input.bdf > out.h
//==============================================================================


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


typedef struct {
	int bBox_width;		// boundingBox_width
	int bBox_height;	// boundingBox_height
	int bBox_xos;		// boundingBox_xos
	int bBox_yos;		// boundingBox_yos
	int nChars;
} t_bdf_data;

typedef struct{
	int flag_hex;
	char *fontName;
	FILE *in;
	FILE *out;
} t_settings;

typedef struct {
	int flag_h;
	char *out_fname;
	char *in_fname;
}
t_parse_args;

void process_bdf(t_settings settings);
t_parse_args parse_args(int argc, char *argv[], t_settings *settings);


//==============================================================================
// TO-DO: agrecar --compact flag para que meta todo un char en el mismo renglon.
//==============================================================================

int main(int argc, char *argv[]) {
	t_settings settings = {0, NULL, NULL, NULL};
	t_parse_args p_args;

	p_args = parse_args(argc, argv, &settings);

	if(p_args.flag_h) {
		printf("Usage: bdf2h -i <in_bdf_font> -o <out_filename> [option(s)]\n");
		printf("Converts a font in bdf format to bitmap C array format.\n");
		printf("-h --help\tThis help text.\n");
		printf("-i <filename>\tInput font file in bdf format.\n");
		printf("-o <filename>\tOutput file to generate (default = stdout).\n");
		printf("--hex\t\tElements in C array output are in hex (default = binary).\n");
		printf("-n <fontName>\tFont name used to write array (default = input filename without extension).\n");
		return 0;
	}

	if(p_args.in_fname != NULL) {
		settings.in = fopen(p_args.in_fname, "r");
		if(settings.in == NULL) {
			fprintf(stderr, "Error opening input file %s.\n", p_args.in_fname);
			return 0;
		}
	}
	else {
		fprintf(stderr, "Use -i <filename> to specify font to convert.\n");
		return 0;
	}

	if(settings.fontName == NULL) {
		settings.fontName = strtok(p_args.in_fname, ".");
	}

	if(p_args.out_fname != NULL) {
		settings.out = fopen(p_args.out_fname, "w");
		if(settings.out == NULL) {
			fprintf(stderr, "Error opening output file %s.\n", p_args.out_fname);
			return 0;
		}
	} else {
		settings.out = stdout;
	}

	process_bdf(settings);
	return 0;
}


//==============================================================================
//
//==============================================================================

t_parse_args parse_args(int argc, char *argv[], t_settings *settings) {
	int i;
	t_parse_args p_args = {
		0,		// int flag_h
		NULL,	// char *out_filename
		NULL	// char *in_filename
	}; 	

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			p_args.flag_h = 1;
		} else if (strcmp(argv[i], "--hex") == 0) {
			settings->flag_hex = 1;
		} else if (strcmp(argv[i], "-o") == 0) {
			if (i + 1 < argc) {
				p_args.out_fname = argv[i + 1];
				i++; // Skip filename in next iteration.
			} else {
				fprintf(stderr, "Error: -o flag requires a filename.\n");
				exit(-1);
			}
		} else if (strcmp(argv[i], "-i") == 0) {
			if (i + 1 < argc) {
				p_args.in_fname = argv[i + 1];
				i++; // Skip filename in next iteration.
			} else {
				fprintf(stderr, "Error: -i flag requires a filename.\n");
				exit(-1);
			}
		} else if (strcmp(argv[i], "-n") == 0) {
			if (i + 1 < argc) {
				settings->fontName = argv[i + 1];
				i++; // Skip filename in next iteration.
			} else {
				fprintf(stderr, "Error: -f flag requires a fontName.\n");
				exit(-1);
			}
		} else {
			printf("Unknown argument: %s\n", argv[i]);
		}
	}
	return p_args;
}


//==============================================================================
// info | defines struct and font info
//==============================================================================
// Argumentos:
// -- out = output stream.
// -- settings = structure with configurations.
// -- t_bdf_data bdf_data
//==============================================================================

void write_bdf_data(t_settings settings, t_bdf_data bdf_data) {

	fprintf(settings.out, "// Bitmap font info struct def.\n");
	fprintf(settings.out, "typedef struct {\n");
	fprintf(settings.out, "\tint width;\n");
	fprintf(settings.out, "\tint height;\n");
	fprintf(settings.out, "\tint nChars;\n");
	fprintf(settings.out, "} bmp_font_inf;\n\n");

	fprintf(settings.out, "bmp_font_inf %s_inf = { %d, %d, %d };\n\n", 
		settings.fontName, bdf_data.bBox_width, bdf_data.bBox_height, 
		bdf_data.nChars);
}


//==============================================================================
// write_char_comment | outputs 1 line of 1 bitmap character
//==============================================================================

void write_char_line_comment(char c, t_settings settings) {
	int i = 8;

	while(i > 0) {
		i--;
		(c & 0x01 << i)? fputc('#', settings.out) : fputc('-', settings.out);
	}
}

//==============================================================================
// write_char_line | outputs 1 line of 1 bitmap character
//==============================================================================

void write_char_line_data(char c, t_settings settings) {
	int i;

	if(settings.flag_hex) {
		unsigned char rev_c = 0;

		for(i = 0; i < 8; i++) {
			if(c & 0x01 << i)
				rev_c |= 0x01 << (7 - i);
		}
		fprintf(settings.out, "0x%02x", rev_c);
	} else {
		fprintf(settings.out, "0b");
		for(i = 0; i < 8; i++) {
			(c & 0x01 << i)? fputc('1', settings.out) : fputc('0', settings.out);
		}
	}
}


//==============================================================================
// write_char | output based on font info
//==============================================================================

void write_char(t_settings settings, unsigned char *bitmap, int fontwidth, 
	int fontheight, int fontyoffset, int charheight,
	int charyoffset) {

	int x;
	int y;
	int c;

	// Rows from the top of bounding box is the top of character.
	int yoffset = fontheight - charheight + (fontyoffset - charyoffset);

	for (y = 0; y < fontheight; y++) {
		fputc('\t', settings.out);
		for (x = 0; x < fontwidth; x += 8) {

			// If current row is above or below the bitmap, output a blank row.
			if(y < yoffset || y > yoffset + charheight)
				c = 0;
			else
				c = bitmap[(y - yoffset) * ((fontwidth + 7) / 8) + x / 8];

			write_char_line_data(c, settings);

			if(y < fontheight - 1)
				fprintf(settings.out, ",");

			fprintf(settings.out, "\t// ");
			write_char_line_comment(c, settings);
		}
		fputc('\n', settings.out);
	}
}


//==============================================================================
//
//==============================================================================
// Argummentos:
// -- p = hex char 0-9 a-f A-F
//==============================================================================

int hexChar2int(char *p) {
	if (*p <= '9')
		return *p - '0';
	else if (*p <= 'F')
		return *p - 'A' + 10;
	else
		return *p - 'a' + 10;
}


//==============================================================================
// write_check_errors | validate input file
//==============================================================================
// Arguments:
// -- bdf_data = struct with font info.
// Returns:
// -- error count.
//==============================================================================

int write_check_errors(t_bdf_data bdf_data) {
	int errVal = 0;

	if (bdf_data.bBox_width <= 0 || bdf_data.bBox_height <= 0) {
		fprintf(stderr, "Boundingbox size error. { w, h } = { %d, %d }\n", 
			bdf_data.bBox_width, bdf_data.bBox_height);
		errVal++;
	}
	if (bdf_data.nChars <= 0) {
		fprintf(stderr, "Number of chars = %d\n", bdf_data.nChars);
		errVal++;
	}

	return errVal;
}


//==============================================================================
//
//==============================================================================

t_bdf_data get_bdf_data(FILE *bdf) {

	char buff[1024];
	char *s, *p;

	t_bdf_data  bdf_data = {
		0,	// bBox_width;
		0,	// bBox_height;
		0,	// bBox_xos;
		0,	// bBox_yos;
		0,	// nChars;
	};
	
	while (1) {
		if (fgets(buff, sizeof(buff), bdf) == NULL)
			break;// EOF.

		s = strtok(buff, " \t\n\r");
		if (s == NULL)
			break;// Empty line.

		if (!strcasecmp(s, "FONTBOUNDINGBOX")) {
			p = strtok(NULL, " \t\n\r");
			bdf_data.bBox_width = atoi(p);
			p = strtok(NULL, " \t\n\r");
			bdf_data.bBox_height = atoi(p);
			p = strtok(NULL, " \t\n\r");
			bdf_data.bBox_xos = atoi(p);
			p = strtok(NULL, " \t\n\r");
			bdf_data.bBox_yos = atoi(p);
		} else if (!strcasecmp(s, "CHARS")) {
			p = strtok(NULL, " \t\n\r");
			bdf_data.nChars = atoi(p);
			break;
		}
	}
	return bdf_data;
}


//==============================================================================
// get_write_char | parses font and writes to output file
//==============================================================================

void get_write_char(t_bdf_data bdf_data, t_settings settings, 
	unsigned char *bitmap) {

	int i, j;
	char *s, *p;

	char buff[1024];

	int n = 0, scanline = -1;

	char charname[1024];
	int encoding;
	int bbx;
	int bby;
	int bbw;
	int bbh;
	int dwidth;

	encoding = -1;
	bbx = 0;
	bby = 0;
	bbw = 0;
	bbh = 0;
	dwidth = INT_MIN;
	strcpy(charname, "unknown character");

	while (1) {
		if (!fgets(buff, sizeof(buff), settings.in))
			break;// EOF.

		if (!(s = strtok(buff, " \t\n\r")))
			break;// Empty line.

		if (!strcasecmp(s, "STARTCHAR")) {
			p = strtok(NULL, " \t\n\r");
			strcpy(charname, p);
		} else if (!strcasecmp(s, "ENCODING")) {
			p = strtok(NULL, " \t\n\r");
			encoding = atoi(p);
		} else if (!strcasecmp(s, "DWIDTH")) {
			p = strtok(NULL, " \t\n\r");
			dwidth = atoi(p);
		} else if (!strcasecmp(s, "BBX")) {
			p = strtok(NULL, " \t\n\r");
			bbw = atoi(p);
			p = strtok(NULL, " \t\n\r");
			bbh = atoi(p);
			p = strtok(NULL, " \t\n\r");
			bbx = atoi(p);
			p = strtok(NULL, " \t\n\r");
			bby = atoi(p);
		} else if (!strcasecmp(s, "BITMAP")) {
			fprintf(settings.out, "// %03d 0x%02x '%s'\n", encoding, encoding, 
				charname);

			//fprintf(out, "// dwidth %d, bbx %d, bby %d, bbw %d, bbh %d\n",
			//dwidth, bbx, bby, bbw, bbh);

			if (n == bdf_data.nChars) {
				fprintf(stderr, 
					"Error: bdf metadata declares less chars han actually inside.\n");
				exit(-1);
			}
			if (dwidth == INT_MIN) {
				fprintf(stderr, "Char dwidth not specified.\n");
				exit(-1);
			}

			if (bbx < 0) {
				dwidth -= bbx;//	Width adjustment.
				bbx = 0;
			}
			if (bbx + bbw > dwidth)
				dwidth = bbx + bbw;

			//width_table[n] = width;// No width table, will use monospace.
			//encoding_table[n] = encoding;// Will not use an encoding table.
			n++;

			scanline = 0;
		
			memset(bitmap, 0, ((bdf_data.bBox_width + 7) / 8) * bdf_data.bBox_height);
		} else if (!strcasecmp(s, "ENDCHAR")) {

			fprintf(settings.out, "\t{\n");
			write_char(settings, bitmap, bdf_data.bBox_width, 
				bdf_data.bBox_height, bdf_data.bBox_yos, bbh, bby);
			fprintf(settings.out, "\t}");

			if(n != bdf_data.nChars)
				fprintf(settings.out, ",\n");// Coma en todos excepto ultimo.

			scanline = -1;
			dwidth = INT_MIN;
		} else {
			if (scanline >= 0) {
				p = s;
				j = 0;
				while (*p) {
					i = hexChar2int(p);
					p++;
					if (*p) {
						i = hexChar2int(p) | i * 16;
					} else {
						bitmap[j + scanline * ((bdf_data.bBox_width + 7) / 8)] = i;
						break;
					}
					bitmap[j + scanline * ((bdf_data.bBox_width + 7) / 8)] = i;
					j++;
					p++;
				}
				scanline++;
			}
		}
	}
}


//==============================================================================
//	process bdf font file.
//==============================================================================
// Recibe:
// -- bdf = stream input bdf file.
// -- out = output stream.
// -- settings = structure with program settings configured.
//==============================================================================

void process_bdf(t_settings settings) {

	t_bdf_data  bdf_data;

	//unsigned *width_table;
	//unsigned *encoding_table;
	unsigned char *bitmap;

	bdf_data = get_bdf_data(settings.in);
	write_bdf_data(settings, bdf_data);

	if(write_check_errors(bdf_data) > 0)
		exit(-1); 

	//	Allocate tables
	//width_table = malloc(bdf_data.nChars * sizeof(*width_table));
	//if (width_table == NULL) {
	//	fprintf(stderr, "Malloc allocation failed (width_table var).\n");
	//	exit(-1);
	//}

	//encoding_table = malloc(bdf_data.nChars * sizeof(*encoding_table));
	//if (encoding_table == NULL) {
	//	fprintf(stderr, "Malloc allocation failed (encoding_table var).\n");
	//	exit(-1);
	//}

	bitmap = malloc(((bdf_data.bBox_width + 7) / 8) * bdf_data.bBox_height);
	if (!bitmap) {
		fprintf(stderr, "Malloc allocation failed (bitmap var).\n");
		exit(-1);
	}

	fprintf(settings.out, "const unsigned char %s_bmp[][%d] = {\n", settings.fontName,
		bdf_data.bBox_height);// Begins array.
	get_write_char(bdf_data, settings, bitmap);
	fprintf(settings.out, "\n};\n"); // Cierre de corchete arreglo.
}





