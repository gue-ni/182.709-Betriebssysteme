/*
    @author: Jakob G. Maier
    @date: 2019-11-19
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

unsigned char dict(unsigned char n)
{
    if (n < 26)
        return n + 65; 

    if (n < 52)
        return n + 71;

    if (n < 62)
        return n - 4;

    if (n == 0x3E)
        return 0x2B;

    return 0x2F; 
}

unsigned char rdict(unsigned char n)
{
    if (n == 0x3D)
        return 0x00;

    if (0x40 < n && n < 0x5B)
        return n - 65;

    if (0x60 < n && n < 0x7B)
        return n - 71;

    if (0x2F < n && n < 0x3A)
        return n + 4; 

    if (n == 0x2B)
        return 0x3E; 

    if (n == 0x2F)
        return 0x3F;

    return 0xFF;
}

void b64_decode(unsigned char *in, unsigned char *out, size_t size)
{
    for (int i = 0; i < size; i++)
    {   
        in[i] = rdict(in[i]);
    }
       
    out[0] = (unsigned char) ((in[0] << 2) | (in[1] >> 4)); 
    out[1] = (unsigned char) ((in[1] << 4) | (in[2] >> 2)); 
    out[2] = (unsigned char) (((in[2] << 6) & 0xC0) | in[3]);
}


void b64_encode(unsigned char *in, unsigned char *out, size_t size)
{
    memset(in + size, 0x00, 3 - size);

    out[0] =  dict( in[0] >> 2 );
    out[1] =  dict( ( ( in[0] & 0x03 ) << 4 ) | ( ( in[1] & 0xF0 ) >> 4) );
    out[2] =  size > 1 ? dict( ( ( in[1] & 0x0F ) << 2 ) | ( ( in[2] & 0xC0 ) >> 6) ) : (unsigned char) '=';
    out[3] =  size > 2 ? dict( in[2] & 0x3F )                                         : (unsigned char) '=';
}

int main(int argc, char *argv[])
{
    int encode = 1;
    int lbr = -1;
    int c;

    while ( (c = getopt(argc, argv, "del:")) != -1)
    {
        switch (c)
        {
            case 'd':
                encode = !encode;
                break;
    
            case 'e':
                break;

            case 'l':
                lbr = atoi(optarg);
                break;

            default:
                fprintf(stderr, "Usage: %s [ -d | -e ]\n", argv[0]);
                exit(0);
                break;                
            }
    }

    size_t n_in, n_out;

    if (encode)
    {
        n_in  = 3;
        n_out = 4;
    } else {
        n_in  = 4;
        n_out = 3;
    }
    
    unsigned char in[n_in], out[n_out];

    FILE *input = stdin;
    
    int l = 0;

    size_t n;
    while( (n = fread(in, 1, sizeof(in), input)) > 0)
    {
        if (encode)
        {
            b64_encode(in, out, n);
        } else {
            b64_decode(in, out, n);
        }

        fwrite(out, 1, sizeof(out), stdout);
        
        if (++l == lbr && lbr != -1)
        {
            printf("\n");
            l = 0;
        }
    }
    printf("\n");
    return EXIT_SUCCESS;
}
    

