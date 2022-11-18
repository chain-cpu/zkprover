#ifndef SCALAR_HPP
#define SCALAR_HPP

#include <gmpxx.h>
#include <string>
#include "goldilocks_base_field.hpp"
#include "ffiasm/fec.hpp"
#include "utils.hpp"

using namespace std;

/* Global scalar variables */
extern mpz_class ScalarMask4;
extern mpz_class ScalarMask8;
extern mpz_class ScalarMask16;
extern mpz_class ScalarMask20;
extern mpz_class ScalarMask32;
extern mpz_class ScalarMask64;
extern mpz_class ScalarMask256;
extern mpz_class ScalarTwoTo8;
extern mpz_class ScalarTwoTo16;
extern mpz_class ScalarTwoTo18;
extern mpz_class ScalarTwoTo32;
extern mpz_class ScalarTwoTo64;
extern mpz_class ScalarTwoTo128;
extern mpz_class ScalarTwoTo192;
extern mpz_class ScalarTwoTo256;
extern mpz_class ScalarTwoTo255;
extern mpz_class ScalarTwoTo258;
extern mpz_class ScalarZero;
extern mpz_class ScalarOne;
extern mpz_class ScalarGoldilocksPrime;

/* Scalar to/from field element conversion */

inline void fe2scalar  (Goldilocks &fr, mpz_class &scalar, const Goldilocks::Element &fe)
{
    scalar = fr.toU64(fe);
}

inline void scalar2fe  (Goldilocks &fr, const mpz_class &scalar, Goldilocks::Element &fe)
{
    if ( !scalar.fits_ulong_p() )
    {
        cerr << "scalar2fe() found scalar out of u64 range:" << scalar.get_str(16) << endl;
        exitProcess();
    }
    fe = fr.fromU64(scalar.get_ui());
}

/* Scalar to/from field element array conversion */

//#define LOG_FEA2SCALAR_ERRORS

inline void fea2scalar (Goldilocks &fr, mpz_class &scalar, const Goldilocks::Element (&fea)[4])
{
    scalar = fr.toU64(fea[3]);
    scalar <<= 64;
    scalar += fr.toU64(fea[2]);
    scalar <<= 64;
    scalar += fr.toU64(fea[1]);
    scalar <<= 64;
    scalar += fr.toU64(fea[0]);
}

inline void fea2scalar (Goldilocks &fr, mpz_class &scalar, const Goldilocks::Element &fe0, const Goldilocks::Element &fe1, const Goldilocks::Element &fe2, const Goldilocks::Element &fe3, const Goldilocks::Element &fe4, const Goldilocks::Element &fe5, const Goldilocks::Element &fe6, const Goldilocks::Element &fe7)
{
    // Add field element 7
    uint64_t aux = fr.toU64(fe7);
    if (aux >= 0x100000000)
    {
#ifdef LOG_FEA2SCALAR_ERRORS
        cerr << "fea2scalar() found element 7 has a too high value=" << fr.toString(fe7, 16) << endl;
#endif
        exitProcess();
    }
    scalar = aux << 32;
    //scalar <<= 32;

    // Add field element 6
    aux = fr.toU64(fe6);
    if (aux >= 0x100000000)
    {
#ifdef LOG_FEA2SCALAR_ERRORS
        cerr << "fea2scalar() found element 6 has a too high value=" << fr.toString(fe6, 16) << endl;
#endif
        exitProcess();
    }
    scalar += aux;
    scalar <<= 32;

    // Add field element 5
    aux = fr.toU64(fe5);
    if (aux >= 0x100000000)
    {
#ifdef LOG_FEA2SCALAR_ERRORS
        cerr << "fea2scalar() found element 5 has a too high value=" << fr.toString(fe5, 16) << endl;
#endif
        exitProcess();
    }
    scalar += aux;
    scalar <<= 32;

    // Add field element 4
    aux = fr.toU64(fe4);
    if (aux >= 0x100000000)
    {
#ifdef LOG_FEA2SCALAR_ERRORS
        cerr << "fea2scalar() found element 4 has a too high value=" << fr.toString(fe4, 16) << endl;
#endif
        exitProcess();
    }
    scalar += aux;
    scalar <<= 32;

    // Add field element 3
    aux = fr.toU64(fe3);
    if (aux >= 0x100000000)
    {
#ifdef LOG_FEA2SCALAR_ERRORS
        cerr << "fea2scalar() found element 3 has a too high value=" << fr.toString(fe3, 16) << endl;
#endif
        exitProcess();
    }
    scalar += aux;
    scalar <<= 32;

    // Add field element 2
    aux = fr.toU64(fe2);
    if (aux >= 0x100000000)
    {
#ifdef LOG_FEA2SCALAR_ERRORS
        cerr << "fea2scalar() found element 2 has a too high value=" << fr.toString(fe2, 16) << endl;
#endif
        exitProcess();
    }
    scalar += aux;
    scalar <<= 32;

    // Add field element 1
    aux = fr.toU64(fe1);
    if (aux >= 0x100000000)
    {
#ifdef LOG_FEA2SCALAR_ERRORS
        cerr << "fea2scalar() found element 1 has a too high value=" << fr.toString(fe1, 16) << endl;
#endif
        exitProcess();
    }
    scalar += aux;
    scalar <<= 32;

    // Add field element 0
    aux = fr.toU64(fe0);
    if (aux >= 0x100000000)
    {
#ifdef LOG_FEA2SCALAR_ERRORS
        cerr << "fea2scalar() found element 0 has a too high value=" << fr.toString(fe0, 16) << endl;
#endif
        exitProcess();
    }
    scalar += aux;
}

inline void fea2scalar (Goldilocks &fr, mpz_class &scalar, const Goldilocks::Element (&fea)[8])
{
   fea2scalar(fr, scalar, fea[0], fea[1], fea[2], fea[3], fea[4], fea[5], fea[6], fea[7]);
}

//#define LOG_SCALAR2FEA_ERRORS

inline void scalar2fea (Goldilocks &fr, const mpz_class &scalar, Goldilocks::Element (&fea)[4])
{
    mpz_class aux = scalar & ScalarMask64;
    if (aux >= ScalarGoldilocksPrime)
    {
#ifdef LOG_SCALAR2FEA_ERRORS
        cerr << "Error: scalar2fea() found value higher than prime: " << aux.get_str(16) << endl;
#endif
        exitProcess();
    }
    fea[0] = fr.fromU64(aux.get_ui());

    aux = scalar>>64 & ScalarMask64;
    if (aux >= ScalarGoldilocksPrime)
    {
#ifdef LOG_SCALAR2FEA_ERRORS
        cerr << "Error: scalar2fea() found value higher than prime: " << aux.get_str(16) << endl;
#endif
        exitProcess();
    }
    fea[1] = fr.fromU64(aux.get_ui());

    aux = scalar>>128 & ScalarMask64;
    if (aux >= ScalarGoldilocksPrime)
    {
#ifdef LOG_SCALAR2FEA_ERRORS
        cerr << "Error: scalar2fea() found value higher than prime: " << aux.get_str(16) << endl;
#endif
        exitProcess();
    }
    fea[2] = fr.fromU64(aux.get_ui());

    aux = scalar>>192 & ScalarMask64;
    if (aux >= ScalarGoldilocksPrime)
    {
#ifdef LOG_SCALAR2FEA_ERRORS
        cerr << "Error: scalar2fea() found value higher than prime: " << aux.get_str(16) << endl;
#endif
        exitProcess();
    }
    fea[3] = fr.fromU64(aux.get_ui());
}

inline void scalar2fea (Goldilocks &fr, const mpz_class &scalar, Goldilocks::Element &fe0, Goldilocks::Element &fe1, Goldilocks::Element &fe2, Goldilocks::Element &fe3, Goldilocks::Element &fe4, Goldilocks::Element &fe5, Goldilocks::Element &fe6, Goldilocks::Element &fe7)
{
    mpz_class aux;
    aux = scalar & ScalarMask32;
    fe0 = fr.fromU64(aux.get_ui());
    aux = scalar>>32 & ScalarMask32;
    fe1 = fr.fromU64(aux.get_ui());
    aux = scalar>>64 & ScalarMask32;
    fe2 = fr.fromU64(aux.get_ui());
    aux = scalar>>96 & ScalarMask32;
    fe3 = fr.fromU64(aux.get_ui());
    aux = scalar>>128 & ScalarMask32;
    fe4 = fr.fromU64(aux.get_ui());
    aux = scalar>>160 & ScalarMask32;
    fe5 = fr.fromU64(aux.get_ui());
    aux = scalar>>192 & ScalarMask32;
    fe6 = fr.fromU64(aux.get_ui());
    aux = scalar>>224 & ScalarMask32;
    fe7 = fr.fromU64(aux.get_ui());
}

inline void scalar2fea (Goldilocks &fr, const mpz_class &scalar, Goldilocks::Element (&fea)[8])
{
    scalar2fea(fr, scalar, fea[0], fea[1], fea[2], fea[3], fea[4], fea[5], fea[6], fea[7]);
}

/* Scalar to/from a Sparse Merkle Tree key conversion, interleaving bits */
void scalar2key (Goldilocks &fr, mpz_class &s, Goldilocks::Element (&key)[4]);

/* Hexa string to/from field element (array) conversion */
void string2fe  (Goldilocks &fr, const string &s, Goldilocks::Element &fe);
string fea2string (Goldilocks &fr, const Goldilocks::Element(&fea)[4]);
string fea2string (Goldilocks &fr, const Goldilocks::Element &fea0, const Goldilocks::Element &fea1, const Goldilocks::Element &fea2, const Goldilocks::Element &fea3);

/* Normalized strings */
string Remove0xIfPresent      (const string &s);
string Add0xIfMissing         (const string &s);
string PrependZeros           (const string &s, uint64_t n);
string NormalizeTo0xNFormat   (const string &s, uint64_t n);
string NormalizeToNFormat     (const string &s, uint64_t n);
string stringToLower          (const string &s);

/* Keccak */
void   keccak256 (const uint8_t *pInputData, uint64_t inputDataSize, uint8_t *pOutputData, uint64_t outputDataSize);
string keccak256 (const uint8_t *pInputData, uint64_t inputDataSize);
string keccak256 (const vector<uint8_t> &input);
string keccak256 (const string &inputString);

/* Byte to/from char conversion */
uint8_t char2byte (char c);
char    byte2char (uint8_t b);
string  byte2string(uint8_t b);

/* Strint to/from byte array conversion
   s must be even sized, and must not include the leading "0x"
   pData buffer must be big enough to store converted data */
uint64_t string2ba (const string &s, uint8_t *pData, uint64_t &dataSize);
void     string2ba (const string &textString, string &baString);
string   string2ba (const string &textString);
void     ba2string (string &s, const uint8_t *pData, uint64_t dataSize);
string   ba2string (const uint8_t *pData, uint64_t dataSize);
void     ba2string (const string &baString, string &textString);
string   ba2string (const string &baString);

/* Byte array of exactly 2 bytes conversion */
void ba2u16(const uint8_t *pData, uint16_t &n);
void ba2scalar(const uint8_t *pData, uint64_t dataSize, mpz_class &s);

/* Scalar to byte array conversion (up to dataSize bytes) */
void scalar2ba(uint8_t *pData, uint64_t &dataSize, mpz_class s);
void scalar2ba16(uint64_t *pData, uint64_t &dataSize, mpz_class s);
void scalar2bytes(mpz_class &s, uint8_t (&bytes)[32]);

/* Converts a scalar to a vector of bits of the scalar, with value 1 or 0; bits[0] is least significant bit */
void scalar2bits(mpz_class s, vector<uint8_t> &bits);

/* Byte to/from bits array conversion, with value 1 or 0; bits[0] is the least significant bit */
void byte2bits(uint8_t byte, uint8_t *pBits);
void bits2byte(const uint8_t *pBits, uint8_t &byte);

/* 8 fe to/from 4 fe conversion */
void sr8to4 ( Goldilocks &fr,
              Goldilocks::Element a0,
              Goldilocks::Element a1,
              Goldilocks::Element a2,
              Goldilocks::Element a3,
              Goldilocks::Element a4,
              Goldilocks::Element a5,
              Goldilocks::Element a6,
              Goldilocks::Element a7,
              Goldilocks::Element &r0,
              Goldilocks::Element &r1,
              Goldilocks::Element &r2,
              Goldilocks::Element &r3 );
void sr4to8 ( Goldilocks &fr,
              Goldilocks::Element a0,
              Goldilocks::Element a1,
              Goldilocks::Element a2,
              Goldilocks::Element a3,
              Goldilocks::Element &r0,
              Goldilocks::Element &r1,
              Goldilocks::Element &r2,
              Goldilocks::Element &r3,
              Goldilocks::Element &r4,
              Goldilocks::Element &r5,
              Goldilocks::Element &r6,
              Goldilocks::Element &r7 );

/* Scalar to/from fec conversion */
void fec2scalar(RawFec &fec, const RawFec::Element &fe, mpz_class &s);
void scalar2fec(RawFec &fec, RawFec::Element &fe, const mpz_class &s);

#endif