/*
 * Temporary mini-benchmark for ECP multiplication
 * (and comparision with GF(p) exponentation if any argument is given)
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "polarssl/config.h"

#include "polarssl/dhm.h"
#include "polarssl/ecp.h"
#include "polarssl/timing.h"
#include "polarssl/error.h"

#if !defined(POLARSSL_TIMING_C) || !defined(POLARSSL_BIGNUM_C) ||   \
    !defined(POLARSSL_DHM_C) || !defined(POLARSSL_ECP_C)

int main( int argc, char *argv[] )
{
    ((void) argc);
    ((void) argv);

    printf("POLARSSL_{TIMING,BIGNUM,DHM,ECP}_C not defined.\n");
    return( 0 );
}

#else

static int myrand( void *rng_state, unsigned char *output, size_t len )
{
    size_t use_len;
    int rnd;

    if( rng_state != NULL )
        rng_state  = NULL;

    while( len > 0 )
    {
        use_len = len;
        if( use_len > sizeof(int) )
            use_len = sizeof(int);

        rnd = rand();
        memcpy( output, &rnd, use_len );
        output += use_len;
        len -= use_len;
    }

    return( 0 );
}

static void dhm_bench_case( const char *s, const char *p,
                            const char *g, const char *x )
{
    unsigned long i;
    mpi P, G, X, R, C;

    mpi_init( &P ); mpi_init( &G ); mpi_init( &X ); mpi_init( &R );
    mpi_init( &C );

    mpi_read_string( &P, 16, p );
    mpi_read_string( &G, 16, g );
    mpi_read_string( &X, 16, x );

    printf( " MODP-%s : ", s );
    fflush( stdout );
    set_alarm( 3 );

    for( i = 1; ! alarmed; i++ )
        mpi_exp_mod( &R, &G, &X, &P, &C );

    printf( "%9lu exp/s\n", i / 3 );

    mpi_free( &P ); mpi_free( &G ); mpi_free( &X ); mpi_free( &R );
    mpi_free( &C );
}

#define MODP_1024_G \
    "205BE4D89282CE6A9488626CA9BCC1264E7C9525903F049D" \
    "D6A77315AA12B01467EB1EA21B25569BECEC276623859A8C" \
    "518D445DF43DCBA1B75D23B6BB6B96084FA3C10CA8DC1A96" \
    "77E26332152233E13F4E6BF3C5D48514BC264A35AF385463" \
    "8B8AE5E363BD6AE0B5D44875BB60A73A5E4075FB2037B833" \
    "DC382EA6DD458EAE"

#define MODP_1024_X \
    "262F6727B2E44AF61981AAAB2AA749F64FD79AD18A1FC999" \
    "2E71795130CB69F89BCFB70DB2A0F2A49A825A0AAA50715D" \
    "5D9AE98198F0522AECBF8A0A2D331AB0296110617FC685FF" \
    "7338F4201329ABDFDEAEC779E4D56CC46A0A38B18DC18DFE" \
    "640865426A7360BCCD4E4F82B3686B4804071D4B9F5582ED" \
    "E4758F170C3A1415"

#define MODP_2048_G \
    "717892E6698EB3CD62113AAE793EA6690791F89290FD83C3" \
    "A059F328C5FBD10AB25C1E8F96245836600FC7772AFB42ED" \
    "F7A809FC3D38F7794574CAC8A318D41C917EE34C81EA8838" \
    "BA6068E81ED3C7F039A83914D044AB2379B3C5C24F0FFD4E" \
    "B6642F26F1EDF015AB7829E9EDC8DCD8A4013C76682338E8" \
    "3D44B0F4E870CF3134874EAA3AC0803DA2CE30922E251B02" \
    "BDDD94137F9DFD41627AE20B7E4137BA94496A4B2AEB0354" \
    "8E1C4126A87240B43E23BB574D354EF1AD01FED5627F240C" \
    "2F4F5521D2F51DFE0702D18EED6DDF022D93554E7AB67D81" \
    "AD4C5256A6AC00DE1AD0751FDCC17F8C7136C194F48A39F5" \
    "957B831081C9058CA0C3A03C7C41F1C9"

#define MODP_2048_X \
    "AFA7186FC7A0370DE6E3DD95473C9D10C916BD186E41FB48" \
    "195D123E5CFE8D237A2A640DCE19B3DD7028C6168084A61D" \
    "4E4E218A1D98E0C2C93E2E1516F97E596B50C811CE93417B" \
    "905DA6833D1CE41838AEAA9CA6CE36E2430A244A6696E99B" \
    "54B6AAC75D4F6FF6F0568145BDBBC83F5F603D698198A8EE" \
    "2098F5DCABDD458C465E795520EDD46C3B992EAFACF24A56" \
    "263850C9A07165A0CE52B7DC71489CA67AC4A6E6176B547B" \
    "A1C233E3C7898C283E82B3E74B9056F84F7B4A10B5CE10FF" \
    "7D43107B68F2381E64C77C61474DE8CA70EA8DF935D43CD2" \
    "CE208D9F7E468F07A93A74676ED1A3C77CB1CD59E96DCA77" \
    "125DE5B672798FC943048FDBB24167B1"

#define MODP_3072_G \
    "38B2B12253661411B63E62F3FAC5BE7CF6A416F249E4A039" \
    "0884765C18F2EDF93FCC2646B888ED64B783765D576887B1" \
    "74ABB76123ECA6537DDC1F95757645F3C870CBFF2D40E40D" \
    "C2556A14115C37BE299BDCB0F07AB7AAE0D8FA6CF44A0C22" \
    "ED5368922EF028EC2B5B0AB42F858D5D780828A508C252A7" \
    "96F8CE7AA9C83D1E820F6991771AAA2286A7754FCCB3AA16" \
    "51D840EC788C5DDEAFAAC0C82EAC84EBFF5EB52699BE4020" \
    "F62607937EB168766BCC1F786F4FCDDBB66C0BCEA983AE87" \
    "5C2DD6C201FBF5C178308E64831D14F12399F21761D13BC1" \
    "20B2377FBAD73613FA7B7E443BEE3879E7EEFB0A625A0EBA" \
    "2D379E8F5EA62C91BC58A6FCDD5D7435B0E747B7B65A47BE" \
    "0CA769DE340E611A98A6DDED72E89486875F02DAE24715E8" \
    "3787A5DB53AA28D3F838E7F11EE80F0310C8597B585D6B1A" \
    "3991FBD7459ED277C34C9A4AE0AE6DC7EE5C99B14F366129" \
    "C3747617B724A2BED48F5BF3106B40E60CC3677780F1A39D" \
    "430154C2B460CC676AACC7B34F792621D743D1AC59EDBAC6"

#define MODP_3072_X \
    "4391157AD824806F07421C1545AA795E26B120A5CEE427F0" \
    "6725BB247E6B2F4C4BE052E4B240B2DD6194B37F3423FA3F" \
    "00C3DF9F4C2165662FCCC9B8DE52C19001E8D54E9053A605" \
    "24BCA6AB0698632EB5D62451987E89AF0FA63479D344E2A7" \
    "58ED8DB5D287600C85A8DD08D9E2E5587177AB6A25A76C71" \
    "5BCAA737FB7DC45E371F4DEC13B40CA9D461A99A98847E1B" \
    "D9CD1BA02DCC331B5B2256887B7F0139A9E7BEA8A6CDA318" \
    "B95E12A1CEE6A26EC070C34C270067C79DBC906295C3C9DF" \
    "2FA6D6B637B7CE81B535DB2D6E196F46ED2F92F3EE8EACE1" \
    "002349D035550A44B75E76B7A77A6A670EC13E334201B543" \
    "B1708295F33B777BD4555A032313C3388DBD5ABC5FB0FEFD" \
    "A9A98C0CC4C7E919927BDC8D46B5347A1266A4C8331C2EB0" \
    "C93DB67244BD6CE2757BA61A063DDD0DF0561A97786D1299" \
    "EBE3EAC838F420D1A0DC9D633B2F16CCE91E436C0413CE6A" \
    "F98E0273FC5C08A4EA70D6DC09A1855AFB402E02BD9F261E" \
    "863717A552F4A83D4DD5060CB70E2D4D7FFAEE912C2C4408"

static void dhm_bench( void )
{
    dhm_bench_case( "1024", POLARSSL_DHM_RFC5114_MODP_1024_P,
            MODP_1024_G, MODP_1024_X );

    dhm_bench_case( "2048", POLARSSL_DHM_RFC3526_MODP_2048_P,
            MODP_2048_G, MODP_2048_X );

    dhm_bench_case( "3072", POLARSSL_DHM_RFC3526_MODP_3072_P,
            MODP_3072_G, MODP_3072_X );
}

static void ecp_bench_case( size_t dp, const char *s, const char *m )
{
    unsigned long i;
    ecp_group grp;
    ecp_point R;
    mpi M;

    ecp_group_init( &grp ); ecp_point_init( &R ); mpi_init( &M );

    ecp_use_known_dp( &grp, dp );
    mpi_read_string( &M, 16, m );

    printf( "  ECP-%s  : ", s );
    fflush( stdout );
    set_alarm( 3 );

    for( i = 1; ! alarmed; i++ )
        ecp_mul( &grp, &R, &M, &grp.G, myrand, NULL );

    printf( "%9lu mul/s\n", i / 3 );

    ecp_group_free( &grp ); ecp_point_free( &R ); mpi_free( &M );
}

#define ECP_192_M \
    "FE61330B02915F25970C2AF8575CBB227E6B6394774E6EC8"
#define ECP_224_M \
    "EAC63628087E32F61A383DCAED57E4348081FF3637D95B904C2FD487"
#define ECP_256_M \
    "83C4106612BDDB781DD446CF2A873D78AA0620B9003789244A18F83B39B3EA63"
#define ECP_384_M \
    "3E45569675A7E2E0E2E663A11E4226FBEF0F99A96CD6E547" \
    "9ABDDF36DD9AB7278BCD8B96A0775847A6431D2AF7476CD9"
#define ECP_521_M \
    "017F540D09F24EC6B102E8E4A9F14B850442D98C68FB29A6B09B9B9D40E2A750" \
    "7F3D2D6C5B6536B607EF2BCEA4797BB3A68F0D745410EB5CFFC7FF7FB17381544E"

static void ecp_bench( void )
{
#if defined(POLARSSL_ECP_DP_SECP192R1_ENABLED)
    ecp_bench_case( POLARSSL_ECP_DP_SECP192R1, "192", ECP_192_M );
#endif
#if defined(POLARSSL_ECP_DP_SECP224R1_ENABLED)
    ecp_bench_case( POLARSSL_ECP_DP_SECP224R1, "224", ECP_224_M );
#endif
#if defined(POLARSSL_ECP_DP_SECP256R1_ENABLED)
    ecp_bench_case( POLARSSL_ECP_DP_SECP256R1, "256", ECP_256_M );
#endif
#if defined(POLARSSL_ECP_DP_SECP384R1_ENABLED)
    ecp_bench_case( POLARSSL_ECP_DP_SECP384R1, "384", ECP_384_M );
#endif
#if defined(POLARSSL_ECP_DP_SECP521R1_ENABLED)
    ecp_bench_case( POLARSSL_ECP_DP_SECP521R1, "521", ECP_521_M );
#endif
}

int main( int argc, char *argv[] )
{
    ((void) argv);
    if( argc > 1 )
    {
        dhm_bench();
    }

    ecp_bench();

    return( 0 );
}
#endif
