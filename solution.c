#include "common.h"

int removeElement(int* nums, int numsSize, int val)
{
  UNUSED(nums);
  UNUSED(numsSize);
  UNUSED(val);
  return 0;
}

typedef struct Testcase {
  int* nums;
  int numsSize;
  int val;
  int expected;
} Testcase;

int main(void)
{
  Testcase testcases[] = {
    {NULL, 0, 0, 0}
  };

  for (size_t i = 0; i < ARR_LEN(testcases); i++) {
    NL();

    int* nums = testcases[i].nums;
    LOGN(nums);

    int numsSize = testcases[i].numsSize;
    LOGN(numsSize);

    int val = testcases[i].val;
    LOGN(val);

    int exp = testcases[i].expected;
    int res = removeElement(nums, numsSize, val);

    ASSERT(exp, res);
  }

  return 0;
}
