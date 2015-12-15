/* extern int funcA(int arg); */
/* extern int funcB(int arg1, int arg2); */
/* extern int funcC(int arg1, int arg2, int arg3); */
/* extern int funcD(int arg1, int arg2, int arg3, int arg4); */
/* extern int funcE(int arg1, int arg2, int arg3, int arg4, int arg5); */
int MyFn(int a, int b) {
  int result = a + b;
  int result2 = result + 2;
  printf("uh oh\n");
  return result2;
}

int MyFn2(int a, int b) {
  int result = a - b;
  int result2 = result + 5;
  return result;
}

int main(int argc, char **argv)
{
  printf("%d\n", MyFn2(1, 2));
  return 0x12345;
}
