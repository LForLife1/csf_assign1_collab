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
  // start at end of the string (not including null terminator) - so read right to left
  const char * currentHex = hex;
  while (*(currentHex + 1) != '\0') {
    currentHex++;
  }
  // scan 64 characters max (or until reaches start of string)
  char *buffer = malloc(sizeof(char) * 9);
  for (int i = 0; i < 8; i++) {
    strcpy(buffer, "xxxxxxxx"); // reset to invalid hex characters
    for (int j = 0; j < 8 && currentHex != hex-1; j++, currentHex--) {
      buffer[j] = *currentHex;
    }

    result.data[i] = strtoul(buffer, NULL, 16);
  }
  free(buffer);
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = calloc(65, sizeof(char)); // 64 hex digits max + null terminator (all initialized to null terminators)

  // copy bits to hex in chunks of 8 bytes
  char *buffer = malloc(sizeof(char) * 9);
  for (int i = 7; i >= 0; i--){
    uint32_t value = val.data[i];
    sprintf(buffer, "%08x", value);
    strcat(hex + ((7 - i) * 8), buffer);
  }
  free(buffer);

  // trim leading zeros
  char *leadingHex = hex;
  while (*leadingHex == '0' && *(leadingHex + 1) != '\0') {
    leadingHex++;
  }
  char *trimmedHex = malloc(65 - (leadingHex - hex)); // alloc amount of space for bits after leadingHex
  strcpy(trimmedHex, leadingHex); // copy bits after leadingHex
  free(hex);

  return trimmedHex;
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
  uint64_t tempSum = 0U;
  uint32_t overflow = 0U;

  for (int i = 0; i <= 7; i++)
  {
    tempSum = (uint64_t) left.data[i] + right.data[i] + overflow;
    sum.data[i] = (uint32_t) tempSum;  //bottom 32 bits
    tempSum >>= 32;                    //shift top 32 bits down to bottom
    overflow = (uint32_t) tempSum;     //top 32 bits (now bottom)
  }
  return sum;
}


// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  result =  uint256_add(left, uint256_negate(right));
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  UInt256 one = uint256_create_from_u32(1);
  for (int i = 0; i <= 7; i++)        //each index of data
  {
    result.data[i] = ~(val.data[i]);  //invert all bits
  }
  result = uint256_add(result, one);
  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  int u32Shift = nbits / 8;
  int bitShift = nbits % 32;
  UInt256 result;
  for (int u32 = 0; u32 < 8; u32++) {
    int lookupU32 = (u32 - u32Shift + 8) % 8;
    int lookupU32Overflow = (lookupU32 - 1 + 8) % 8;
    int mask = 0xffffffff >> bitShift;
    result.data[u32] = (val.data[lookupU32] & mask) << bitShift;
    result.data[u32] |= (val.data[lookupU32Overflow] & ~mask) >> (32 - bitShift);
  }
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  int u32Shift = nbits / 8;
  int bitShift = nbits % 32;
  UInt256 result;
  for (int u32 = 0; u32 < 8; u32++) {
    int lookupU32 = (u32 + u32Shift) % 8;
    int lookupU32Overflow = (lookupU32 + 1) % 8;
    int mask = 0xffffffff << bitShift;
    result.data[u32] = (val.data[lookupU32] & mask) >> bitShift;
    result.data[u32] |= (val.data[lookupU32Overflow] & ~mask) << (32 - bitShift);
  }
  return result;
}
