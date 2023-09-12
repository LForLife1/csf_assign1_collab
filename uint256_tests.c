#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
  UInt256 zero; // the value equal to 0
  UInt256 one;  // the value equal to 1
  UInt256 max;  // the value equal to (2^256)-1
  UInt256 one_below_max; //the value equal to (2^256) - 2
  UInt256 msb_set; // the value equal to 2^255
  UInt256 rot; // value used to test rotations
} TestObjs;

// Helper functions for implementing tests
void set_all(UInt256 *val, uint32_t wordval);

#define ASSERT_SAME(expected, actual) \
do { \
  ASSERT(expected.data[0] == actual.data[0]); \
  ASSERT(expected.data[1] == actual.data[1]); \
  ASSERT(expected.data[2] == actual.data[2]); \
  ASSERT(expected.data[3] == actual.data[3]); \
  ASSERT(expected.data[4] == actual.data[4]); \
  ASSERT(expected.data[5] == actual.data[5]); \
  ASSERT(expected.data[6] == actual.data[6]); \
  ASSERT(expected.data[7] == actual.data[7]); \
} while (0)

#define INIT_FROM_ARR(val, arr) \
do { \
  for (unsigned i = 0; i < 8; ++i) \
    val.data[i] = arr[i]; \
} while (0)

// Functions to create and cleanup the test fixture object
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// Declarations of test functions
void test_get_bits(TestObjs *objs);
void test_create_from_u32(TestObjs *objs);
void test_create(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_add(TestObjs *objs);
void test_add_genfact();
void test_add_genfact2();
void test_sub(TestObjs *objs);
void test_subtract_genfact();
void test_negate(TestObjs *objs);
void test_rotate_left(TestObjs *objs);
void test_rotate_right(TestObjs *objs);

int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_get_bits);
  TEST(test_create_from_u32);
  TEST(test_create);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_add);
  TEST(test_add_genfact);
  TEST(test_add_genfact2);
  TEST(test_sub);
  TEST(test_subtract_genfact);
  TEST(test_negate);
  TEST(test_rotate_left);
  TEST(test_rotate_right);

  TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all(UInt256 *val, uint32_t wordval) {
  for (unsigned i = 0; i < 8; ++i) {
    val->data[i] = wordval;
  }
}

TestObjs *setup(void) {
  TestObjs *objs = (TestObjs *) malloc(sizeof(TestObjs));

  // initialize several UInt256 values "manually"
  set_all(&objs->zero, 0);
  set_all(&objs->one, 0);
  objs->one.data[0] = 1U;
  set_all(&objs->max, 0xFFFFFFFFU);

  set_all(&objs->one_below_max, 0xFFFFFFFFU);
  objs->one_below_max.data[0] = 0xFFFFFFFEU;

  // create a value with only the most-significant bit set
  uint32_t msb_set_data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U };
  INIT_FROM_ARR(objs->msb_set, msb_set_data);

  // value with nonzero values in least significant and most significant words
  // Note that the hex representation of this value is
  //   CD000000 00000000 00000000 00000000 00000000 00000000 00000000 000000AB
  uint32_t rot_data[8] = { 0x000000ABU, 0U, 0U, 0U, 0U, 0U, 0U, 0xCD000000U };
  INIT_FROM_ARR(objs->rot, rot_data);

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_get_bits(TestObjs *objs) {
  ASSERT(0U == uint256_get_bits(objs->zero, 0));
  ASSERT(0U == uint256_get_bits(objs->zero, 1));
  ASSERT(0U == uint256_get_bits(objs->zero, 2));
  ASSERT(0U == uint256_get_bits(objs->zero, 3));
  ASSERT(0U == uint256_get_bits(objs->zero, 4));
  ASSERT(0U == uint256_get_bits(objs->zero, 5));
  ASSERT(0U == uint256_get_bits(objs->zero, 6));
  ASSERT(0U == uint256_get_bits(objs->zero, 7));

  ASSERT(1U == uint256_get_bits(objs->one, 0));
  ASSERT(0U == uint256_get_bits(objs->one, 1));
  ASSERT(0U == uint256_get_bits(objs->one, 2));
  ASSERT(0U == uint256_get_bits(objs->one, 3));
  ASSERT(0U == uint256_get_bits(objs->one, 4));
  ASSERT(0U == uint256_get_bits(objs->one, 5));
  ASSERT(0U == uint256_get_bits(objs->one, 6));
  ASSERT(0U == uint256_get_bits(objs->one, 7));

  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 0));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 1));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 2));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 3));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 4));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 5));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 6));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 7));
}

void test_create_from_u32(TestObjs *objs) {
  UInt256 zero = uint256_create_from_u32(0U);
  UInt256 one = uint256_create_from_u32(1U);

  ASSERT_SAME(objs->zero, zero);
  ASSERT_SAME(objs->one, one);
}

void test_create(TestObjs *objs) {
  (void) objs;

  uint32_t data1[8] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U };
  UInt256 val1 = uint256_create(data1);
  ASSERT(1U == val1.data[0]);
  ASSERT(2U == val1.data[1]);
  ASSERT(3U == val1.data[2]);
  ASSERT(4U == val1.data[3]);
  ASSERT(5U == val1.data[4]);
  ASSERT(6U == val1.data[5]);
  ASSERT(7U == val1.data[6]);
  ASSERT(8U == val1.data[7]);

  uint32_t zerosdata[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  UInt256 val2 = uint256_create(zerosdata);
  ASSERT(0U == val2.data[0]);
  ASSERT(0U == val2.data[1]);
  ASSERT(0U == val2.data[2]);
  ASSERT(0U == val2.data[3]);
  ASSERT(0U == val2.data[4]);
  ASSERT(0U == val2.data[5]);
  ASSERT(0U == val2.data[6]);
  ASSERT(0U == val2.data[7]);
}

void test_create_from_hex(TestObjs *objs) {
  UInt256 zero = uint256_create_from_hex("0");
  ASSERT_SAME(objs->zero, zero);

  UInt256 one = uint256_create_from_hex("1");
  ASSERT_SAME(objs->one, one);

  UInt256 max = uint256_create_from_hex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  ASSERT_SAME(objs->max, max);
}

void test_format_as_hex(TestObjs *objs) {
  char *s;

  s = uint256_format_as_hex(objs->zero);
  ASSERT(0 == strcmp("0", s));
  free(s);

  s = uint256_format_as_hex(objs->one);
  ASSERT(0 == strcmp("1", s));
  free(s);

  s = uint256_format_as_hex(objs->max);
  ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s));
  free(s);
}

void test_add(TestObjs *objs) {
  UInt256 result;

  result = uint256_add(objs->zero, objs->zero);  // 0 + 0 (zeroes)
  ASSERT_SAME(objs->zero, result);

  result = uint256_add(objs->zero, objs->one);   // 0 + 1
  ASSERT_SAME(objs->one, result);

  result = uint256_add(objs->one, objs->zero);   // 1 + 0 (commutative)
  ASSERT_SAME(objs->one, result);

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  result = uint256_add(objs->one, objs->one);    // 1 + 1 (symmetrical non-zero)
  ASSERT_SAME(two, result);

  result = uint256_add(objs->zero, objs->max);   // 0 + MAX
  ASSERT_SAME(objs->max, result);

  result = uint256_add(objs->max, objs->one);    // MAX + 1 = 0 (overflow)
  ASSERT_SAME(objs->zero, result);

  result = uint256_add(objs->one, objs->max);    // MAX + 1 = 0 (commutative overflow)
  ASSERT_SAME(objs->zero, result);

  result = uint256_add(objs->max, two);          // MAX + 2 = 1
  ASSERT_SAME(objs->one, result);

  result = uint256_add(objs->max, objs->max);    // MAX + MAX = MAX - 1
  ASSERT_SAME(objs->one_below_max, result);
}

void test_add_genfact() {          //used genfact addition fact
  UInt256 left, right, result;
  left.data[0] = 0xe1a96ad2U;
  left.data[1] = 0xe6499b1eU;
  left.data[2] = 0x136e9ef3U;
  left.data[3] = 0x03d900deU;
  left.data[4] = 0x3d4e8c93U;
  left.data[5] = 0x2a98dab1U;
  left.data[6] = 0xb0d2941dU;
  left.data[7] = 0x0ec8a915U;
  right.data[0] = 0xc4337d6dU;
  right.data[1] = 0x954bb345U;
  right.data[2] = 0x8b33686fU;
  right.data[3] = 0x2ac4e8aeU;
  right.data[4] = 0x132e93dfU;
  right.data[5] = 0x8f6f05d8U;
  right.data[6] = 0x9c92d508U;
  right.data[7] = 0x02531c80U;
  result = uint256_add(left, right);
  ASSERT(0xa5dce83fU == result.data[0]);
  ASSERT(0x7b954e64U == result.data[1]);
  ASSERT(0x9ea20763U == result.data[2]);
  ASSERT(0x2e9de98cU == result.data[3]);
  ASSERT(0x507d2072U == result.data[4]);
  ASSERT(0xba07e089U == result.data[5]);
  ASSERT(0x4d656925U == result.data[6]);
  ASSERT(0x111bc596U == result.data[7]);
}

void test_add_genfact2() {
  UInt256 left, right, result;
  left.data[0] = 0xbd3f2275U;
  left.data[1] = 0xdaade3feU;
  left.data[2] = 0x8f8991d5U;
  left.data[3] = 0x4b5feaa9U;
  left.data[4] = 0x19448805U;
  left.data[5] = 0x525c1526U;
  left.data[6] = 0x4719744bU;
  left.data[7] = 0x50a3b0bcU;
  right.data[0] = 0x90066acbU;
  right.data[1] = 0xf3e33d14U;
  right.data[2] = 0x39edfa60U;
  right.data[3] = 0xb3da172dU;
  right.data[4] = 0x2b92cedeU;
  right.data[5] = 0x2594beccU;
  right.data[6] = 0x0d571731U;
  right.data[7] = 0x996fc7bdU;
  result = uint256_add(left, right);
  ASSERT(0x4d458d40U == result.data[0]);
  ASSERT(0xce912113U == result.data[1]);
  ASSERT(0xc9778c36U == result.data[2]);
  ASSERT(0xff3a01d6U == result.data[3]);
  ASSERT(0x44d756e3U == result.data[4]);
  ASSERT(0x77f0d3f2U == result.data[5]);
  ASSERT(0x54708b7cU == result.data[6]);
  ASSERT(0xea137879U == result.data[7]);

}

void test_sub(TestObjs *objs) {
  UInt256 result;

  result = uint256_sub(objs->zero, objs->zero);   // 0 - 0
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->one, objs->zero);    // 1 - 0
  ASSERT_SAME(objs->one, result);

  result = uint256_sub(objs->one, objs->one);     // 1 - 1
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->max, objs->zero);    // MAX - 0
  ASSERT_SAME(objs->max, result);

  result = uint256_sub(objs->max, objs->one);     // MAX - 1
  ASSERT_SAME(objs->one_below_max, result);

  result = uint256_sub(objs->max, objs->max);     // MAX - MAX
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->zero, objs->one);    // 0 - 1 (underflow)
  ASSERT_SAME(objs->max, result);

  result = uint256_sub(objs->zero, objs->max);    // 0 - MAX (underflow)
  ASSERT_SAME(objs->one, result);
}

void test_subtract_genfact() {          //used genfact subtraction fact
  UInt256 left, right, result;
  left.data[0] = 0x7f3d84b6U;
  left.data[1] = 0x84dd9c17U;
  left.data[2] = 0x6fff7b15U;
  left.data[3] = 0x31db746cU;
  left.data[4] = 0x79e25608U;
  left.data[5] = 0x8b63dbedU;
  left.data[6] = 0x64816a25U;
  left.data[7] = 0x089695f5U;
  right.data[0] = 0xe5b85b3dU;
  right.data[1] = 0xb5d9d04dU;
  right.data[2] = 0xb56fbd77U;
  right.data[3] = 0xd376c846U;
  right.data[4] = 0x8ab7830dU;
  right.data[5] = 0x819c4ceaU;
  right.data[6] = 0xc5b28bf6U;
  right.data[7] = 0x00bf0534U;
  result = uint256_sub(left, right);
  ASSERT(0x99852979U == result.data[0]);
  ASSERT(0xcf03cbc9U == result.data[1]);
  ASSERT(0xba8fbd9dU == result.data[2]);
  ASSERT(0x5e64ac25U == result.data[3]);
  ASSERT(0xef2ad2faU == result.data[4]);
  ASSERT(0x09c78f02U == result.data[5]);
  ASSERT(0x9ecede2fU == result.data[6]);
  ASSERT(0x07d790c0U == result.data[7]);
}

void test_negate(TestObjs *objs) {
  UInt256 result;

  result = uint256_negate(objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_negate(objs->one);
  ASSERT_SAME(objs->max, result);

  result = uint256_negate(objs->max);
  ASSERT_SAME(objs->one, result);

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  result = uint256_negate(two);    
  ASSERT_SAME(objs->one_below_max, result);

  result = uint256_negate(objs->one_below_max);    
  ASSERT_SAME(two, result);
}

void test_rotate_left(TestObjs *objs) {
  UInt256 result;

  result = uint256_rotate_left(objs->zero, 0);  //check not rotating on zero changes nothing
  ASSERT_SAME(objs->zero, result);

  result = uint256_rotate_left(objs->zero, 8);  //check rotation zero changes nothing
  ASSERT_SAME(objs->zero, result);

  result = uint256_rotate_left(objs->max, 0);  //check no rotation max changes nothing
  ASSERT_SAME(objs->max, result);

  result = uint256_rotate_left(objs->max, 8);  //check rotation max changes nothing
  ASSERT_SAME(objs->max, result);

  result = uint256_rotate_left(objs->one, 0);  //check not rotating on non-zero changes nothing
  ASSERT_SAME(objs->one, result);

  result = uint256_rotate_left(objs->zero, 256);  //check rotation by 256 changes nothing
  ASSERT_SAME(objs->zero, result);

  result = uint256_rotate_left(objs->max, 256);  
  ASSERT_SAME(objs->max, result);

  // rotating the value with just the most significant bit set
  // one position to the left should result in the value equal to 1
  // (i.e., the value with only the least significant bit set)
  result = uint256_rotate_left(objs->msb_set, 1);
  ASSERT_SAME(objs->one, result);

  //ROT VALUES: CD000000 00000000 00000000 00000000 00000000 00000000 00000000 000000AB

  // after rotating the "rot" value left by 4 bits, the resulting value should be
  //   D0000000 00000000 00000000 00000000 00000000 00000000 00000000 00000ABC
  result = uint256_rotate_left(objs->rot, 4);
  ASSERT(0x00000ABCU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xD0000000U == result.data[7]);

  // after rotating the "rot" value left by 4 + 256 bits, the resulting value should still be
  //   D0000000 00000000 00000000 00000000 00000000 00000000 00000000 00000ABC
  result = uint256_rotate_left(objs->rot, 260);
  ASSERT(0x00000ABCU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xD0000000U == result.data[7]);

  // after rotating the "rot" value left by 8 bits, the resulting value should be
  //   00000000 00000000 00000000 00000000 00000000 00000000 00000000 0000ABCD
  //CURRENTLY GETTING:
  //   00000000 00000000 00000000 00000000 00000000 00000000 0000ABCD 00000000 
  result = uint256_rotate_left(objs->rot, 8);
  ASSERT(0x0000ABCDU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[7]);

    // after rotating the "rot" value left by 9 bits, the resulting value should be
  //   00000000 00000000 00000000 00000000 00000000 00000000 00000000 0001579A
  //CURRENTLY GETTING:
  //   UNKNOWN
  result = uint256_rotate_left(objs->rot, 9);
  ASSERT(0x0001579AU == result.data[0]); //FIX WHAT THIS SHOULD BE
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[7]);


  // after rotating the "rot" value left by 56 bits, the resulting value should be
  // D0000000 00000000 00000000 00000000 00000000 00000000 ABCD0000 00000000
  //CURRENTLY GETTING:
  //   UNKNOWN
  result = uint256_rotate_left(objs->rot, 56);
  ASSERT(0U == result.data[0]);
  ASSERT(0xABCD0000U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[7]);
}

void test_rotate_right(TestObjs *objs) {
  UInt256 result;

  result = uint256_rotate_right(objs->zero, 0);  //check not rotating on zero changes nothing
  ASSERT_SAME(objs->zero, result);

  result = uint256_rotate_right(objs->zero, 8);  //check rotation zero changes nothing
  ASSERT_SAME(objs->zero, result);

  result = uint256_rotate_right(objs->max, 0);  //check no rotation max changes nothing
  ASSERT_SAME(objs->max, result);

  result = uint256_rotate_right(objs->max, 8);  //check rotation max changes nothing
  ASSERT_SAME(objs->max, result);

  result = uint256_rotate_right(objs->one, 0);  //check not rotating on non-zero changes nothing
  ASSERT_SAME(objs->one, result);

  result = uint256_rotate_right(objs->zero, 256);  //check rotation by 256 changes nothing
  ASSERT_SAME(objs->zero, result);

  result = uint256_rotate_right(objs->max, 256);  
  ASSERT_SAME(objs->max, result);

  // rotating 1 right by 1 position should result in a value with just
  // the most-significant bit set
  result = uint256_rotate_right(objs->one, 1);
  ASSERT_SAME(objs->msb_set, result);

  // after rotating the "rot" value right by 4 bits, the resulting value should be
  //   BCD00000 00000000 00000000 00000000 00000000 00000000 00000000 0000000A
  result = uint256_rotate_right(objs->rot, 4);
  ASSERT(0x0000000AU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xBCD00000U == result.data[7]);

  // after rotating the "rot" value right by 4 + 256 bits, the resulting value should still be
  //   BCD00000 00000000 00000000 00000000 00000000 00000000 00000000 0000000A
  result = uint256_rotate_right(objs->rot, 260);
  ASSERT(0x0000000AU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xBCD00000U == result.data[7]);
}
