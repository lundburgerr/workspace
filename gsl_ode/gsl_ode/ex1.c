#include <gsl/gsl_vector.h>

/* silly program that does nothing useful */
int main(){ 
  size_t n = 4; 
  gsl_vector * a = gsl_vector_alloc (n) ; 
  gsl_vector * b = gsl_vector_alloc (n) ; 

  for (int i = 0; i<n; ++i) { 
    gsl_vector_set (a, i, (double )i+1); 
    gsl_vector_set (b, i, (double )i+1); 
  }
  gsl_vector_free(a);
  gsl_vector_free(b);
  return 0;
}
