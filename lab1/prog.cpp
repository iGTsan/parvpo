#include <iostream>
#include <omp.h>

using namespace std;

void stress(long long int n) {
  for (long long int i = 0; i < n; i++) {
    i = i;
  }
}

int main() {
  long long int level = 5e10;
  double start = omp_get_wtime();
  #pragma omp parallel num_threads(12)
  {
    stress(level);
  }
  cout << omp_get_wtime() - start << endl;
}
