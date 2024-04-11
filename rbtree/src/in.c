int* f3(void) {
  int *px;
  px = (int *) malloc(sizeof(int));
  *px = 10;
  return px;
}
void main(){
    f3();
}