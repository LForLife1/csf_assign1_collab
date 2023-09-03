#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result;
  result.data[0] = val;
  for (int i = 1; i <= 7; i++)
  {
    result.data[i] = 0;
  }
  return result;
}

// Create a UInt256 value from an array of 8 uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 7 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;
  for (int i = 0; i <= 7; i++)
  {
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result;
  int index = strlen(hex) - 1;
  // uint32_t hexNum;
  // for (int i = 0; i <= 7; i++) 
  // {
  //   const char *buffer = hex;
  //   hexNum = strtoul (buffer, NULL, 16);
  //   result.data[i] = hexNum;
  // }
    return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = NULL;
  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 7 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  // TODO: implement
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  for (int i = 0; i < 7; i++) //each index of data
  {
    result.data[i] = ~val.data[i]; //invert all bits
  }
  //TODO:                       // and add one
  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  int u32Shift = nbits / 8;
  int bitShift = nbits % 8;
  UInt256 result;
  printf("\n");
  for (int i = 0; i < 8; i++) {
    printf("%d\n", val.data[i]);
  }
  printf("\n");
  for (int u32 = 0; u32 < 8; u32++) {
    int lookupU32 = (u32 + u32Shift) % 8;
    int lookupU32Overflow = (lookupU32 + 1) % 8;
    int overflowMask = ~(0xffffffff >> bitShift);
    result.data[u32] = val.data[lookupU32] << bitShift;
    result.data[u32] &= overflowMask & (val.data[lookupU32Overflow] << bitShift);
    printf("%d\n", result.data[u32]);
  }
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  int u32Shift = (nbits / 8) % 8; // no point in going past 8, then loops
  int bitShift = nbits % 8;
  UInt256 result;
  printf("\n");
  for (int i = 0; i < 8; i++) {
    printf("%d\n", val.data[i]);
  }
  printf("\n");
  for (int u32 = 0; u32 < 8; u32++) {
    uint32_t dataBlock = 0;
    int lookupU32 = (u32 + 8 - u32Shift) % 8; // add 8 to avoid negative before modulo
    int lookupU32Overflow = (lookupU32 + 8 - 1) % 8; // add 8 to avoid negative before modulo
    int overflowMask = ~(0xffffffff << bitShift);
    for (int bit = 0; bit < 32; bit++) {
      int lookupDataBlock = ((u32 + 8 - u32Shift) + (bit - bitShift) / 32) % 8;
      dataBlock &= 1 & (val.data[lookupDataBlock] >> (bit - bitShift));
    }
    result.data[u32] = dataBlock;
    printf("%d\n", result.data[u32]);
  }
  return result;
}
